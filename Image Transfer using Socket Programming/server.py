import socket

def receive_file(server_socket, buffer_size=4096):
    conn, addr = server_socket.accept()
    print(f"Connection from {addr} has been established.")

    # Receive the file name and confirm it’s an image file
    file_name = conn.recv(buffer_size).decode()
    if not file_name.lower().endswith(('.png', '.jpg', '.jpeg', '.bmp', '.gif')):
        print("The received file is not an image.")
        conn.send("File type not supported".encode())
        conn.close()
        return

    conn.send("File type supported".encode())

    # Open the file to write the incoming data
    with open(file_name, 'wb') as file:
        print(f"Receiving {file_name}...")
        while True:
            data = conn.recv(buffer_size)
            if not data:
                break
            file.write(data)

    print(f"File {file_name} received successfully.")
    conn.close()

# Server setup
def start_server(host='0.0.0.0', port=5001):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(1)
    print(f"Server is listening on {host}:{port}")
    receive_file(server_socket)
    server_socket.close()

if __name__ == "__main__":
    start_server()
