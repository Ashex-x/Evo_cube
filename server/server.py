# Lanch my server of OpenAI_whisper and deepseek
# Deepseek API key: sk-f7efbc5143204001aa97633dd04310e2 (Don't use my API key without permission)
# Set powershell if you are using WSL2: 
# netsh interface portproxy add v4tov4 listenport=13579 listenaddress=0.0.0.0 connectport=13579 connectaddress=<hostname -I>

import socket
import struct
import numpy as np
import whisper
import json
import threading
import wave
import tempfile
import os
import torch

import dpsk_api


class Whisper:
    def __init__(self, host, port, gain, model_size, chunk_size, LLM):
        self.host = host
        self.port = port
        self.gain = gain
        self.chunk_size = chunk_size
        self.running = False
        self.is_cuda = torch.cuda.is_available()
        self.LLM = LLM

        print(f"Loading Whisper {model_size} model...")
        self.model = whisper.load_model(model_size)
        print(f"Whisper model loaded successfully!")

    def launch_server(self):
        # Initialize socket
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server.bind((self.host, self.port))
        self.server.listen(5)
        self.server.settimeout(1.0)

        self.running = True
        print(f"Whisper Audio Server started on {self.host}:{self.port}")
        print(f"Audio gain: {self.gain}x")

        client_counter = 0
        
        while self.running:
            try:
                client_socket, client_addr = self.server.accept()
                client_counter += 1
                
                print(f"Client #{client_counter} connected from {client_addr}")
                
                client_thread = threading.Thread(
                    target=self.handle_client,
                    args=(client_socket, client_addr, client_counter)
                )
                client_thread.daemon = True
                client_thread.start()
                
            except socket.timeout:
                continue
            except Exception as e:
                if self.running:
                    print(f"Server error: {e}")
        
        print("Server stopped")

    def handle_client(self, client_socket, client_addr, client_id):
        print(f"\nConnected by [addr]: {client_addr}")
        process_chunk_size = self.chunk_size

        try:
            header_data = self.recv_exact(client_socket, 16)
            if not header_data or len(header_data) != 16:
                print(f"Client #{client_id}: Invalid or missing header")
                return
            
            magic, sample_rate, channels, bits_per_sample, datasize = \
                struct.unpack('<IIHHI', header_data)
            
            print(f"Client #{client_id} PCM Header:")
            print(f"  Magic: 0x{magic:08X}")
            print(f"  Sample Rate: {sample_rate} Hz")
            print(f"  Channels: {channels}")
            print(f"  Bits per Sample: {bits_per_sample}")
            print(f"  Data Size: {datasize} bytes")

            if magic != 0x50434D31:
                print(f"Client #{client_id}: Invalid magic number")
                client_socket.send(b"ERROR: Invalid PCM header magic")
                return
            
            if sample_rate != 16000 or channels != 1 or bits_per_sample != 16:
                print(f"Client #{client_id}: Unsupported audio format")
                error_msg = f"ERROR: Requires 16kHz, mono, 16-bit. Got {sample_rate}Hz, {channels}ch, {bits_per_sample}bit"
                client_socket.send(error_msg.encode('utf-8'))
                return
            
            byte_buffer = bytearray()
            audio_buffer = np.array([], dtype=np.int16)

            while self.running:
                try:
                    data = client_socket.recv(4096)
                    if not data:
                        break
                    
                    byte_buffer.extend(data)

                    while len(byte_buffer) >= 2:
                        processable_bytes = len(byte_buffer) - (len(byte_buffer) % 2)
                        if processable_bytes < 2:
                            break

                        byte_chunk = bytes(byte_buffer[:processable_bytes])
                        audio_chunk = np.frombuffer(byte_chunk, dtype=np.int16)

                        enhanced_audio = self.apply_gain(audio_chunk)
                        audio_buffer = np.concatenate([audio_buffer, enhanced_audio])

                        byte_buffer = byte_buffer[processable_bytes:]

                        while len(audio_buffer) >= process_chunk_size:
                            whisper_chunk = audio_buffer[:process_chunk_size]
                            audio_buffer = audio_buffer[process_chunk_size:]
                            
                            # Apply Whisper
                            self.apply_whisper(whisper_chunk, client_socket)

                except socket.timeout:
                    client_socket.settimeout(1.0)
                    continue
                except Exception as e:
                    print(f"Client #{client_id} receive error: {e}")
                    break
        
            """
            if len(audio_buffer) > 0:
                print(f"Client #{client_id}: Processing final {len(audio_buffer)} enhanced samples")
                self.apply_whisper(audio_buffer, client_socket, is_final=True)
            """
        except Exception as e:
            print(f"Client #{client_id} handling error: {e}")

    def recv_exact(self, sock, size):
        data = b''
        while len(data) < size:
            try:
                chunk = sock.recv(size - len(data))
                if not chunk:
                    return None
                data += chunk
            except socket.timeout:
                continue
            except:
                return None
        return data
    
    def apply_gain(self, audio):
        if len(audio) == 0:
            return audio
            
        audio_float = audio.astype(np.float32)
        audio_float = audio_float * self.gain

        max_val = np.max(np.abs(audio_float))
        if max_val > 32767:
            audio_float = audio_float * (32767 / max_val)
            print(f"  Applied compression to prevent clipping")
        
        return np.clip(audio_float, -32768, 32767).astype(np.int16)
        # Enhance audio


    def apply_whisper(self, audio, socket, is_final=False):

        try:
            if audio.size > 0:
                # Print info
                max_amp = np.abs(audio).max()
                print(f"\nWhisper processing: {len(audio)} samples, max amplitude: {max_amp}")
                
                # Create temp wav file
                with tempfile.NamedTemporaryFile(suffix='.wav', delete=False) as temp_file:
                    temp_filename = temp_file.name
                
                # Save wav file
                with wave.open(temp_filename, 'wb') as wav_file:
                    wav_file.setnchannels(1) 
                    wav_file.setsampwidth(2) # Int16 for 2 bytes
                    wav_file.setframerate(16000)
                    wav_file.writeframes(audio.tobytes())
                
                try:
                    # Use Whisper to recongnize
                    result = self.model.transcribe(
                        temp_filename,
                        language="en",
                        fp16=True if self.is_cuda else False  # Set False if use CPU
                    )
                    
                    text = result["text"].strip()
                    
                    if text:
                        status = "FINAL" if is_final else "PARTIAL"
                        print(f"Whisper [{status}]: {text}")

                        # Send result
                        emoji = self.LLM.launch_Deepseek(text)
                        print(emoji)

                        try:
                            socket.send(f"Island:{emoji}\n".encode('utf-8'))
                        except:
                            print("Failed to send ASR/LLM result to client")
                        
                finally:
                    # Delete temp file
                    try:
                        os.unlink(temp_filename)
                    except:
                        pass

        except Exception as e:
            print(f"Whisper processing error: {e}")
            import traceback
            traceback.print_exc()

    def stop_server(self):
        self.running = False
        if hasattr(self, 'server'):
            self.server.close()

    def fetch(self):
        return self.emoji


if __name__ == "__main__":
    config_path = os.path.join(os.path.dirname(__file__), 'config.json')

    with open(config_path, 'r', encoding='utf-8') as config_json:
        config = json.load(config_json)

    host = config['Whisper']['host']
    port = config['Whisper']['port']
    gain = config['Whisper']['gain']
    model_size = config['Whisper']['model_size']
    chunk_size = config['Whisper']['chunk_size']

    dpsk = dpsk_api.Deepseek_api()
    Island = Whisper(host=host, port=port, gain=gain, model_size=model_size, chunk_size=chunk_size, LLM=dpsk)

    try:
        Island.launch_server()

        while True:
            print("11")
            if Island.content:
                print("\nContent sended")
                emoji = dpsk.launch_Deepseek(Island.content)

                
    except KeyboardInterrupt:
        print("Shutting down server...")
        Island.stop_server()
    
    

            