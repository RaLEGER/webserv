import socket

def send_string_over_socket(host, port, message):
    # Create a socket object
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            # Connect to the server
            s.connect((host, port))
            
            # Send the message (string) over the socket
            s.sendall(message.encode())
            print(f"Sent: {message}")

            # Receive response (if needed)
            response = s.recv(1024)
            print("Received:", response.decode())
            
        except Exception as e:
            print(f"Error: {e}")

if __name__ == "__main__":
    # Define the host and port
    host = 'localhost'
    port = 8081
    
    # Define the message you want to send
    # message = "GET / HTTP/1\r\nHost: localhost:8081\r\nThis is a test message."
    message = "GET / HTTP/1"

    # Call the function to send the message
    send_string_over_socket(host, port, message)
