import socket

def initServer():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    host = '0.0.0.0'
    port = 12345

    server.bind((host, port))
    server.listen(1) # max clients
    print(f"Lisening: {host}:{port}")
    client, addr = server.accept()
    print(f"connect to {str(addr)}")

    client.send("Welcome to Ashex's island".encode('utf-8'))

    while True:
        print("Waiting for data")
        data = client.recv(1024).decode('uft-8')
        if not data:
            break
        print(f"receive: {data}")


if (__name__ == "__main__"):
    print("Launching server.")
    initServer()
