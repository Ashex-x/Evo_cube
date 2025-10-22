# Lanch my server of FunASR and deepseek
# Deepseek API key: sk-f7efbc5143204001aa97633dd04310e2
# Set powershell if you are using WSL2: netsh interface portproxy add v4tov4 listenport=13579 listenaddress=0.0.0.0 connectport=13579 connectaddress=<hostname -I>

import socket
import struct
import torch
import numpy as np
from funasr import AutoModel

class Server:
    def __init__(self, host='0.0.0.0', port=13579):
        self.host = host
        self.port = port
        self.sample_rate = 16000
        
    def lanchServer(self):
        # Init server
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server.bind((self.host, self.port))
        self.server.listen(1) # max clients
        print(f"Lisening: {self.host}:{self.port}")
        
        return self.server
        

class FunASR:
    
    def __init__(self, server):
        self.server = server

        # init FunASR
        print("Loading FunASR model...")

        is_cuda = torch.cuda.is_available()
        print(f"CUDA Available to PyTorch: {is_cuda}")
        self.model = AutoModel(
            model="paraformer-en",                      # English model
            vad_model=None,
            # vad_model="fsmn-vad",                     # Voice activity detection
            punc_model="ct-punc",                       # Punctuation
            streaming=True,
            device="cuda:0",
            disable_update=True
        )
        print("Model loaded successfully!")
        
    def start_receive(self):
        
        # Wait for client to connect
        print("Waiting for client.")
        client, addr = self.server.accept()
        print(f"Connected by {addr}")
        # Greet client
        client.send("\nWelcome to Ashex's island".encode('utf-8'))

        while True:
            
            self.handle_client(client)
    
    def handle_client(self, client):
        asr_cache = {}

        try:
            # Receive PCM header
            header_data = client.recv(16)
            if len(header_data) == 16:
                magic, sample_rate, channels, bits_per_sample, datasize = \
                    struct.unpack('<IIHHI', header_data)
                
                if magic == 0x50434D31:  # Verify .magic
                    if sample_rate != 16000 or bits_per_sample != 16 or channels != 1:
                        print("Error: Model requires 16k Hz, 16-bit, mono audio.")
                        client.send("Error: Invalid audio format. Requires 16k/16bit/mono.".encode('utf-8'))
                        return
                    
                    print(f"Audio stream: {sample_rate}Hz, {channels}ch, {bits_per_sample}bit")
            
            # Audio buffer
            audio_buffer = bytearray()
            process_chunk_size = 96000

            while True:
                data = client.recv(2048)  # Receive data
                if not data:
                    if len(audio_buffer) > 0:
                        # Convert bytes to a 1D NumPy array of 16-bit signed integers
                        audio_np = np.frombuffer(audio_buffer, dtype=np.int16) 
                        self._process_chunk(audio_np, asr_cache, client, is_final=True)
                    else:
                        # If the buffer is empty, just send the final signal
                        self._process_chunk(np.array([], dtype=np.int16), asr_cache, client, is_final=True)

                    break
                
                audio_buffer.extend(data)

                # Process the audio in fixed-size chunks
                while len(audio_buffer) >= process_chunk_size:
                    # Extract a chunk for processing (ensure it's an even number of bytes for int16)
                    chunk_bytes = audio_buffer[:process_chunk_size]
                    audio_buffer = audio_buffer[process_chunk_size:]
                    
                    # Convert bytes to a 1D NumPy array of 16-bit signed integers
                    audio_np = np.frombuffer(chunk_bytes, dtype=np.int16) 
                    
                    # Process the chunk as a non-final segment
                    self._process_chunk(audio_np, asr_cache, client, is_final=False)

                """
                if len(audio_buffer) >= process_chunk_size:
                    # Convert to bytes
                    audio_data = bytes(audio_buffer)
                    
                    # print(f"Recieve: {audio_data}")
                    
                    # Start FunASR
                    result = self.model.generate(input=audio_data, language="en")
                    
                    if result and len(result) > 0:
                        text = result[0].get('text', '').strip()
                        if text:
                            print(f"Result: {text}")
                    
                            
                    # consistent recognition
                    keep_size = 8000  # 0.25s context
                    audio_buffer = audio_buffer[-keep_size:] if len(audio_buffer) > keep_size else bytearray()
                """
                    
        except Exception as e:
            print(f"Client handling error: {e}")
        finally:
            client.close()
            print("Client disconnected")

    def _process_chunk(self, audio_np, cache, client, is_final):
        
        if audio_np.size > 0:
        # Scale the 16-bit integers (range -32768 to 32767) to a float range (-1.0 to 1.0)
            audio_np = audio_np.astype(np.float32) / 32768.0

        # Call generate with the NumPy array input, the cache, and the is_final flag
        # The result will contain the text transcription up to this point.
        result = self.model.generate(
            input=audio_np, 
            cache=cache, 
            is_final=is_final, 
            language="en"
        )
        
        # FunASR streaming returns 'text' in the result dictionary.
        if result and len(result) > 0:
            text = result[0].get('text', '').strip()
            # In streaming mode, 'text' often contains the partial result.
            if text:
                # You might only want to print/send the text if it's the final result 
                # or if you want to show real-time partial results.
                # For simplicity, we print any received text.
                print(f"[{'FINAL' if is_final else 'PARTIAL'}] Result: {text}")

                # Optional: Send the result back to the client
                client.send(f"ASR:{text}\n".encode('utf-8'))

if __name__ == "__main__":
    server = Server()
    island = server.lanchServer()
    funasr_model = FunASR(island)
    funasr_model.start_receive()