import socket
import struct
import threading
from funasr import AutoModel

def initServer():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    host = '0.0.0.0'
    port = 13579

    server.bind((host, port))
    server.listen(1) # max clients
    print(f"Lisening: {host}:{port}")
    client, addr = server.accept()
    print(f"connect to {str(addr)}")

    client.send("Welcome to Ashex's island".encode('utf-8'))

    while True:
        data = client.recv(1024).decode('uft-8')
        if not data:
            break
        print(f"receive: {data.strip()}")


if (__name__ == "__main__"):
    print("Launching server.")
    initServer()

class PCMReceiver:
    def __init__(self, host='0.0.0.0', port=8080):
        self.host = host
        self.port = port
        self.sample_rate = 16000
        
        # 初始化FunASR模型
        print("Loading FunASR model...")
        self.model = AutoModel(
            model="iic/speech_paraformer-large_asr_nat-zh-cn-16k-common-vocab8404-pytorch",
            vad_model="iic/speech_fsmn_vad_zh-cn-16k-common-pytorch",
            punc_model="iic/punc_ct-transformer_zh-cn-common-vocab272727-pytorch"
        )
        print("Model loaded successfully!")
        
    def start_receive(self):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((self.host, self.port))
        server_socket.listen(1)
        
        print(f"PCM receiver listening on {self.host}:{self.port}")
        
        while True:
            client_socket, addr = server_socket.accept()
            print(f"Connected by {addr}")
            
            # 为每个连接创建新线程
            client_thread = threading.Thread(
                target=self.handle_client, 
                args=(client_socket,)
            )
            client_thread.daemon = True
            client_thread.start()
    
    def handle_client(self, client_socket):
        try:
            # 接收PCM头
            header_data = client_socket.recv(16)
            if len(header_data) == 16:
                magic, sample_rate, channels, bits_per_sample, data_size = \
                    struct.unpack('<IIHHI', header_data)
                
                if magic == 0x50434D31:  # 验证PCM1魔术字
                    print(f"Audio stream: {sample_rate}Hz, {channels}ch, {bits_per_sample}bit")
            
            # 音频数据缓冲区
            audio_buffer = bytearray()
            min_buffer_size = 32000  # 约1秒的数据
            
            while True:
                data = client_socket.recv(2048)  # 接收数据
                if not data:
                    break
                
                audio_buffer.extend(data)
                
                # 当积累足够数据时进行识别
                if len(audio_buffer) >= min_buffer_size:
                    # 转换为bytes进行识别
                    audio_data = bytes(audio_buffer)
                    
                    # 使用FunASR识别
                    result = self.model.generate(input=audio_data)
                    
                    if result and len(result) > 0:
                        text = result[0].get('text', '').strip()
                        if text:
                            print(f"识别结果: {text}")
                    
                    # 保留部分数据用于连续识别
                    keep_size = 8000  # 保留0.25秒作为上下文
                    audio_buffer = audio_buffer[-keep_size:] if len(audio_buffer) > keep_size else bytearray()
                    
        except Exception as e:
            print(f"Client handling error: {e}")
        finally:
            client_socket.close()
            print("Client disconnected")

if __name__ == "__main__":
    receiver = PCMReceiver()
    receiver.start_receive()