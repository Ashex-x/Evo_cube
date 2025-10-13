import socket
import struct
import threading
from funasr import AutoModel

class Server:
    def __init__(self, host='0.0.0.0', port=13579):
        self.host = host
        self.port = port
        self.sample_rate = 16000

        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server.bind((self.host, self.port))
        self.server.listen(1) # max clients
        print(f"Lisening: {self.host}:{self.port}")
        return self.server
        
    def lanchServer(self):
        client, addr = self.server.accept()
        print(f"connect to {str(addr)}")

        """
        while True:
            data = client.recv(1024).decode('uft-8')
            if not data:
                break
            print(f"receive: {data.strip()}")
        """

        while True:
            client.send("Welcome to Ashex's island".encode('utf-8'))

        




class FunASR:
    
    def __init__(self, server):
        self.server = server

        # init FunASR
        print("Loading FunASR model...")
        self.model = AutoModel(
            model="iic/speech_paraformer-large_asr_nat-zh-cn-16k-common-vocab8404-pytorch",
            vad_model="iic/speech_fsmn_vad_zh-cn-16k-common-pytorch",
            punc_model="iic/punc_ct-transformer_zh-cn-common-vocab272727-pytorch",
            device="CUDA:0"
        )
        print("Model loaded successfully!")
        
    def start_receive(self):
        while True:
            client, addr = self.server.accept()
            print(f"Connected by {addr}")
            
            # New process for every client
            client_thread = threading.Thread(
                target=self.handle_client, 
                args=(client,)
            )
            client_thread.daemon = True
            client_thread.start()
    
    def handle_client(self, client):
        try:
            # Receive PCM header
            header_data = client.recv(16)
            if len(header_data) == 16:
                magic, sample_rate, channels, bits_per_sample, datasize = \
                    struct.unpack('<IIHHI', header_data)
                
                if magic == 0x50434D31:  # Verify .magic
                    print(f"Audio stream: {sample_rate}Hz, {channels}ch, {bits_per_sample}bit")
            
            # Audio buffer
            audio_buffer = bytearray()
            min_buffer_size = 32000  # 1s data
            
            while True:
                data = client.recv(2048)  # Receive data
                if not data:
                    break
                
                audio_buffer.extend(data)
                
                # Start recongnition
                if len(audio_buffer) >= min_buffer_size:
                    # Convert to bytes
                    audio_data = bytes(audio_buffer)
                    
                    # Start FunASR
                    result = self.model.generate(input=audio_data)
                    
                    if result and len(result) > 0:
                        text = result[0].get('text', '').strip()
                        if text:
                            print(f"Result: {text}")
                    
                    # consistent recognition
                    keep_size = 8000  # 0.25s context
                    audio_buffer = audio_buffer[-keep_size:] if len(audio_buffer) > keep_size else bytearray()
                    
        except Exception as e:
            print(f"Client handling error: {e}")
        finally:
            client.close()
            print("Client disconnected")


if __name__ == "__main__":
    server = Server()
    island = server.lanchServer()
    # funasr_model = FunASR(island)
    # funasr_model.start_receive()