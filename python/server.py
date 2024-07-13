# Band speed server

# Python built-in libraries
import sys
import re
import signal
import socket
import threading
import subprocess

PORT = 9999
SERVER_ADDRESS = '0.0.0.0'
ONE_MEGA = 1024 * 1024  # 1 MB
server = None

def signal_handler(sig, frame):
    if not server is None:
        server.close()
        print("All connections closed.")
    sys.exit(0)

def handle_client(client_socket):
    try:
        # Send a large amount of data to the client
        data = b'x' * ONE_MEGA
        while True:
            client_socket.send(data)
    except socket.error:
        pass
    finally:
        client_socket.close()

def server_program():
    global server
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((SERVER_ADDRESS, PORT))
    server.listen(5)
    print('-' * 40)
    print(f"Server listening at:")
    for ip4 in server_address():
        print(f'  {ip4}:{PORT}')
    print(f"TCP window size: {ONE_MEGA / 1024:.0f} KByte")
    print('-' * 40)
    print('Press Ctrl-C to exit')

    while True:
        client_sock, address = server.accept()
        print(f"Accepted connection from {address}")
        client_handler = threading.Thread(
            target=handle_client,
            args=(client_sock,)
        )
        client_handler.start()

def server_address():
    """To obtain host ip4 addresses"""
    # Executing the command ifconfig
    output = subprocess.run('ifconfig', capture_output=True)
    # Getting ifconfig output
    text = output.stdout.decode()
    # Regular expressions to extract ip4 addresses
    reexpr = r"\d+\.\d+\.\d+\.\d+"
    # Obtaining ip4 address from ifconfig output
    ip4_list = re.findall(reexpr, text)
    # Excluding ip4 ending in 0 or 255
    ip4_list = [
        ip 
        for ip in ip4_list
        if not ip.split('.')[-1] in ['0', '255']
    ]
    return ip4_list

if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)
    server_program()
