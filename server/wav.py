import socket
import struct
import numpy as np
import wave
import os
import torch
from datetime import datetime
import threading
import whisper
import io
import tempfile

class WhisperAudioServer:
    def __init__(self, host='0.0.0.0', port=13579, output_dir="recorded_audio", gain=8.0, model_size="base"):
        self.host = host
        self.port = port
        self.output_dir = output_dir
        self.gain = gain
        self.running = False
        
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        
        # 初始化Whisper模型
        print(f"Loading Whisper {model_size} model...")
        self.model = whisper.load_model(model_size)
        print(f"Whisper model loaded successfully!")
        
    def start_server(self):
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server.bind((self.host, self.port))
        self.server.listen(5)
        self.server.settimeout(1.0)
        
        self.running = True
        print(f"Whisper Audio Server started on {self.host}:{self.port}")
        print(f"Audio gain: {self.gain}x")
        print(f"Recordings will be saved to: {os.path.abspath(self.output_dir)}")
        
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
    
    def stop_server(self):
        self.running = False
        if hasattr(self, 'server'):
            self.server.close()
    
    def apply_gain(self, audio_data, gain):
        if len(audio_data) == 0:
            return audio_data
            
        audio_float = audio_data.astype(np.float32)
        audio_float = audio_float * gain
        
        max_val = np.max(np.abs(audio_float))
        if max_val > 32767:
            audio_float = audio_float * (32767 / max_val)
            print(f"  Applied compression to prevent clipping")
        
        return np.clip(audio_float, -32768, 32767).astype(np.int16)
    
    def handle_client(self, client_socket, client_addr, client_id):
        wav_file = None
        enhanced_wav_file = None
        byte_buffer = bytearray()
        enhanced_audio_buffer = np.array([], dtype=np.int16)
        
        # 处理参数 - 使用更大的块以提高准确性
        process_chunk_size = 48000  # 3秒的音频
        total_samples = 0
        start_time = datetime.now()
        
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
            
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            
            # 创建原始音频文件
            original_filename = f"client{client_id}_{timestamp}_{client_addr[0]}_original.wav"
            original_filepath = os.path.join(self.output_dir, original_filename)
            
            # 创建增强音频文件
            enhanced_filename = f"client{client_id}_{timestamp}_{client_addr[0]}_enhanced_{self.gain}x.wav"
            enhanced_filepath = os.path.join(self.output_dir, enhanced_filename)
            
            wav_file = wave.open(original_filepath, 'wb')
            wav_file.setnchannels(1)
            wav_file.setsampwidth(2)
            wav_file.setframerate(16000)
            
            enhanced_wav_file = wave.open(enhanced_filepath, 'wb')
            enhanced_wav_file.setnchannels(1)
            enhanced_wav_file.setsampwidth(2)
            enhanced_wav_file.setframerate(16000)
            
            print(f"Client #{client_id}: Started recording and ASR")
            print(f"Original: {original_filename}")
            print(f"Enhanced: {enhanced_filename} (gain: {self.gain}x)")
            
            client_socket.send(b"OK: Audio recording and Whisper ASR started\n")
            
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
                        
                        chunk_bytes = bytes(byte_buffer[:processable_bytes])
                        audio_data = np.frombuffer(chunk_bytes, dtype=np.int16)
                        
                        # 写入原始音频文件
                        wav_file.writeframes(chunk_bytes)
                        
                        # 应用增益并写入增强文件
                        enhanced_audio = self.apply_gain(audio_data, self.gain)
                        enhanced_bytes = enhanced_audio.tobytes()
                        enhanced_wav_file.writeframes(enhanced_bytes)
                        
                        # 添加到音频缓冲区用于ASR
                        enhanced_audio_buffer = np.concatenate([enhanced_audio_buffer, enhanced_audio])
                        
                        total_samples += len(audio_data)
                        byte_buffer = byte_buffer[processable_bytes:]
                        
                        # 处理完整的音频块进行ASR
                        while len(enhanced_audio_buffer) >= process_chunk_size:
                            asr_chunk = enhanced_audio_buffer[:process_chunk_size]
                            enhanced_audio_buffer = enhanced_audio_buffer[process_chunk_size:]
                            
                            # 使用Whisper进行语音识别
                            self.process_with_whisper(asr_chunk, client_socket)
                        
                        # 每5秒打印一次进度
                        if total_samples % (16000 * 5) < 1600:
                            duration = total_samples / 16000
                            stats = self.get_audio_stats(audio_data)
                            enhanced_stats = self.get_audio_stats(enhanced_audio)
                            print(f"Client #{client_id}: {duration:.1f}s recorded")
                            print(f"  Original: {stats}")
                            print(f"  Enhanced: {enhanced_stats}")
                    
                except socket.timeout:
                    client_socket.settimeout(1.0)
                    continue
                except Exception as e:
                    print(f"Client #{client_id} receive error: {e}")
                    break
            
            # 处理剩余的音频数据
            if len(enhanced_audio_buffer) > 0:
                print(f"Client #{client_id}: Processing final {len(enhanced_audio_buffer)} enhanced samples")
                self.process_with_whisper(enhanced_audio_buffer, client_socket, is_final=True)
        
        except Exception as e:
            print(f"Client #{client_id} handling error: {e}")
        
        finally:
            if wav_file:
                wav_file.close()
            if enhanced_wav_file:
                enhanced_wav_file.close()
                
            if wav_file:
                original_size = os.path.getsize(original_filepath)
                enhanced_size = os.path.getsize(enhanced_filepath)
                
                end_time = datetime.now()
                recorded_duration = total_samples / 16000
                
                print(f"Client #{client_id}: Recording completed")
                print(f"  Duration: {recorded_duration:.2f}s")
                print(f"  Original: {original_filepath} ({original_size} bytes)")
                print(f"  Enhanced: {enhanced_filepath} ({enhanced_size} bytes)")
            
            try:
                client_socket.close()
            except:
                pass
            
            print(f"Client #{client_id} disconnected")
    
    def process_with_whisper(self, audio_data, client_socket, is_final=False):
        """使用Whisper处理音频数据进行语音识别"""
        try:
            if audio_data.size > 0:
                # 打印音频统计信息
                max_amp = np.abs(audio_data).max()
                print(f"\nWhisper processing: {len(audio_data)} samples, max amplitude: {max_amp}")
                
                # 创建临时WAV文件
                with tempfile.NamedTemporaryFile(suffix='.wav', delete=False) as temp_file:
                    temp_filename = temp_file.name
                
                # 将音频保存为WAV文件
                with wave.open(temp_filename, 'wb') as wav_file:
                    wav_file.setnchannels(1)
                    wav_file.setsampwidth(2)
                    wav_file.setframerate(16000)
                    wav_file.writeframes(audio_data.tobytes())
                
                try:
                    # 使用Whisper进行识别
                    result = self.model.transcribe(
                        temp_filename,
                        language="en",
                        fp16=False  # 如果使用CPU，设置为False
                    )
                    
                    text = result["text"].strip()
                    
                    if text:
                        status = "FINAL" if is_final else "PARTIAL"
                        print(f"Whisper [{status}]: {text}")
                        
                        # 发送识别结果给客户端
                        try:
                            client_socket.send(f"ASR:{text}\n".encode('utf-8'))
                        except:
                            print("Failed to send ASR result to client")
                    else:
                        print("Whisper returned empty text")
                        
                finally:
                    # 删除临时文件
                    try:
                        os.unlink(temp_filename)
                    except:
                        pass
        
        except Exception as e:
            print(f"Whisper processing error: {e}")
            import traceback
            traceback.print_exc()
    
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
    
    def get_audio_stats(self, audio_data):
        if len(audio_data) == 0:
            return "No audio data"
        
        max_amplitude = np.abs(audio_data).max()
        rms = np.sqrt(np.mean(audio_data.astype(np.float64)**2))
        
        stats = {
            'min': int(audio_data.min()),
            'max': int(audio_data.max()),
            'rms': int(rms)
        }
        
        if max_amplitude < 1000:
            quality = "VERY_QUIET"
        elif max_amplitude < 5000:
            quality = "QUIET"
        elif max_amplitude < 15000:
            quality = "NORMAL"
        elif max_amplitude < 25000:
            quality = "LOUD"
        else:
            quality = "VERY_LOUD"
        
        return f"{quality} | min:{stats['min']} max:{stats['max']} rms:{stats['rms']}"


def main():
    # 模型大小选项: "tiny", "base", "small", "medium", "large"
    # 对于实时应用，建议使用 "base" 或 "small"
    server = WhisperAudioServer(
        host='0.0.0.0',
        port=13579,
        output_dir="recorded_audio",
        gain=8.0,
        model_size="base"  # 可以改为 "small" 以提高准确性
    )
    
    try:
        server.start_server()
    except KeyboardInterrupt:
        print("Shutting down server...")
        server.stop_server()


if __name__ == "__main__":
    main()