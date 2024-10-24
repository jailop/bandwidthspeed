import sys
import os
import socket
import time

PORT = 9999
ONE_MEGA = 1024 * 1024  # 1 MB
BUFFER_SIZE = 4096
REPORT_INTERVAL = 5
REPORT_NAME = 'report.txt'

def client_program():
    # Ask the user for the server IP address
    server_ip = input("Enter the server IP address: ")
    # Ask the user for the duration of the test in seconds
    duration = float(input("Enter the duration of the test in seconds: "))
    print()

    pprint('-' * 70)
    pprint(f"Bandwidth test with server {server_ip}:{PORT}")
    pprint('-' * 70)

    # Connecting to the server
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect((server_ip, PORT))

    # Follow up variables
    total_received = 0
    start_time = time.time()
    progress = Progress(start_time)

    try:
        while time.time() - start_time < duration:
            # Receive data from the server
            data = client.recv(BUFFER_SIZE)
            if not data:
                break
            data_size = len(data) / ONE_MEGA
            total_received += data_size
            progress.update(total_received)
    except socket.error:
        pass
    finally:
        end_time = time.time()
        client.close()

    # Calculate results
    total_mb = total_received  # MB
    time_taken = end_time - start_time        # sec
    bandwidth = total_mb * 8 / time_taken     # Mbps
    timestamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
    # Prepare result string
    result = (
        f"Server IP     : {server_ip}\n"
        f"Test Duration : {duration:.1f} seconds\n"
        f"Data received : {total_mb:.1f} MB\n"
        f"Time Taken    : {time_taken:.1f} seconds\n"
        f"Bandwidth     : {bandwidth:.1f} Mbps\n"
        f"Test Time     : {timestamp}\n"
    )

    # Print results to console
    pprint('-' * 70)
    pprint(result)
    pprint('-' * 70)

    # Save results to a file
    with open('network_test_results.txt', 'w') as file:
        file.write(result)

class Progress:
    seq = 1
    init_time = 0
    start_time: int
    step_transfer = 0
    def __init__(self, start_time):
        self.start_time = start_time
    def update(self, total_received):
        step_time = time.time() - self.start_time
        interval = step_time - self.init_time
        transfer = total_received - self.step_transfer
        bandwidth = transfer * 8 / interval
        if interval >= REPORT_INTERVAL:
            if self.seq == 1:
                pprint("[  ID] %-22s %-22s %-22s"
                    % ('Interval', 'Transfer', 'Bandwidth'))
            _interval = f'{self.init_time:.1f}-{step_time:.1f} sec'
            _transfer = f'{transfer:.1f} MB'
            _bandwidth = f'{bandwidth:.1f} Mbps'
            pprint(f'[{self.seq:4d}] {_interval:22s} ' +
                f'{_transfer:22s} {_bandwidth:22s}')
            self.init_time = step_time
            self.step_transfer = total_received
            self.seq += 1

def pprint(*args, **kargs):
    output = " ".join(map(str, args))
    print(output, **kargs)
    with open(REPORT_NAME, 'a', encoding='utf-8') as fd:
        output += '\n' if not 'end' in kargs else kargs['end']
        fd.write(output)

if __name__ == "__main__":
    if os.path.exists(REPORT_NAME):
        os.remove(REPORT_NAME)
    client_program()
