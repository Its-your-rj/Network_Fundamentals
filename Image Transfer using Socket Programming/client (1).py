import socket
import os

def send_file(client_socket, file_path, buffer_size=4096):
    # Send the file name to the server
    file_name = os.path.basename(file_path)
    client_socket.send(file_name.encode())

    # Check for server response about file type verification
    server_response = client_socket.recv(buffer_size).decode()
    if server_response != "File type supported":
        print(server_response)
        client_socket.close()
        return

    # Send the file data
    print(f"Sending {file_name} to the server...")
    with open(file_path, 'rb') as file:
        while (chunk := file.read(buffer_size)):
            client_socket.send(chunk)

    print(f"File {file_name} sent successfully.")
    client_socket.close()

# Client setup
def start_client(host='127.0.0.1', port=5001, file_path='path/to/your/image.jpg'):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((host, port))
    send_file(client_socket, file_path)

if __name__ == "__main__":
    start_client(file_path='/home/raj/Pictures/Screenshot_2024-08-09_22-18-45.png')
