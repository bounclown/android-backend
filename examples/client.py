import socket

def run_client():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect(('127.0.0.1', 8080))
    client_socket.send("Hello from Python Client!".encode('utf-8'))
    response = client_socket.recv(1024).decode('utf-8')
    print(f"Ответ сервера: {response}")
    client_socket.close()

if __name__ == "__main__":
    run_client()