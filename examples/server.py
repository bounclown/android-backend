import socket

def run_server():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('127.0.0.1', 8080))
    server_socket.listen(5)
    print("Сервер запущен и ждет подключения на 127.0.0.1:8080...")

    while True:
        client_sock, address = server_socket.accept()
        print(f"Подключен клиент: {address}")
        data = client_sock.recv(1024).decode('utf-8')
        print(f"Получено от клиента: {data}")
        client_sock.send("Hello from Python Server!".encode('utf-8'))
        client_sock.close()

if __name__ == "__main__":
    run_server()