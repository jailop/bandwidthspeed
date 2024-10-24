/* Bandwidth Speed Tester - client */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 9999
#define ONEMEGA (1024 * 1024)
#define INTERVAL 10

int sockfd = -1;   // Socket descriptor

/* Clean up function when user press Ctrl+C
 * If the client socket is open, it is closed
 */
void sigint_handler(int signum);
/* Print detailed error message when connect fails */
void connect_error(int err);

int main(int argc, char *argv[]) {
        // Checking if the user input the correct number of
        // arguments. Otherwise, a help message is displayed
        if (argc < 3) {
                fprintf(stderr, "Usage: %s ADDRESS TIME\n", argv[0]);
                exit(1);
        }
        // Registering signal handler for Ctrl+C
        signal(SIGINT, sigint_handler);
        // Socket initialization
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
                fprintf(stderr, "Failed to create socket\n");
                exit(1);
        }
        struct sockaddr_in srv;
        bzero(&srv, sizeof(srv));
        srv.sin_family = AF_INET; 
        srv.sin_addr.s_addr = inet_addr(argv[1]); 
        srv.sin_port = htons(PORT); 
        // Requiring connection
        if (connect(sockfd, (const struct sockaddr *) &srv,
                        sizeof(srv)) != 0) {
                connect_error(errno);
                fprintf(stderr, "Failed to connect\n");
                exit(1);
        }
        // Control variables
        char buf[ONEMEGA];
        ssize_t n = 0;
        ssize_t total = 0;
        ssize_t last_data = 0;
        int duration = atoi(argv[2]); 
        time_t start = time(NULL);
        time_t last_mark = start;
        time_t lap;
        int step = 1;
        double vol = 0.0;
        // Printing the header
        printf("%-4s %10s %15s %17s\n",
                "Step", "Duration", "Volumen", "Rate");
        // Looping with the given duration
        while ((last_mark - start) < duration) {
                // Cartching data from the server
                n = read(sockfd, buf, ONEMEGA);
                // When no more data is received, quit the loop
                if (n == 0) break;
                // Driving error on reading
                if (n == -1) {
                        fprintf(stderr, "Error reading data\n");
                        break;
                }
                total += n;   // bytes received
                // Step duration
                time_t current_time = time(NULL);
                lap = current_time - last_mark;
                if (lap >= INTERVAL) {
                        // Interval reached, time to report progress
                        vol = (total - last_data) * 1.0 / (float) ONEMEGA;
                        printf("%4d %8ld s %12.1f MB %12.1f Mbps\n",
                                step, lap, vol,
                                vol * 8 / (current_time - last_mark));
                        // Updating control variables
                        last_data = total;
                        last_mark = time(NULL);
                        step++;
                }
        }
        // Final report
        vol = total * 1.0 / (float) ONEMEGA;
        printf("Data received: %.1f MB\n", vol);
        printf("Transfer rate: %.1f Mbps\n", vol * 8 / (last_mark - start));
        // Clean up
        close(sockfd);
        return 0;
}

/* Clean up function when user press Ctrl+C
 * If the client socket is open, it is closed
 */
void sigint_handler(int signum) {
        if (sockfd != -1) close(sockfd); 
        printf("\n");
        exit(0);
}

void connect_error(int err) {
        switch (err) {
                case EADDRNOTAVAIL:
                        fprintf(stderr, "Address no available\n");
                        break;
                case EAFNOSUPPORT:
                        fprintf(stderr, "No valid address\n");
                        break;
                case EALREADY:
                        fprintf(stderr, "Conection in progress\n");
                        break;
                case EBADF:
                        fprintf(stderr, "No valid file descriptor\n");
                        break;
                case ECONNREFUSED:
                        fprintf(stderr, "Target not listening\n");
                        break;
                case EINPROGRESS:
                        fprintf(stderr, "Connectio shall be async\n");
                        break;
                case EINTR:
                        fprintf(stderr, "Signal interruption\n");
                        break;
                case EISCONN:
                        fprintf(stderr, "Already connected\n");
                        break;
                case ENETUNREACH:
                        fprintf(stderr, "No route to the network\n");
                        break;
                case ENOTSOCK:
                        fprintf(stderr, "Argument is not for a socket\n");
                        break;
                case EPROTOTYPE:
                        fprintf(stderr, "Incorrect type for address\n");
                        break;
                case ETIMEDOUT:
                        fprintf(stderr, "Timeout attemp\n");
                        break;
                case EACCES:
                        fprintf(stderr, "Permission denied\n");
                        break;
                case EADDRINUSE:
                        fprintf(stderr, "Address is in use\n");
                        break;
                case ECONNRESET:
                        fprintf(stderr, "Remote host reset the connection\n");
                        break;
                case EHOSTUNREACH:
                        fprintf(stderr, "Destination host cannot be reached\n");
                        break;
                case ENETDOWN:
                        fprintf(stderr, "Local network interface is down\n");
                        break;
                case ENOBUFS:
                        fprintf(stderr, "No buffer space availble\n");
                        break;
                case EOPNOTSUPP:
                        fprintf(stderr, "Socket is listening, "
                                "cannot be connected\n");
                        break;
                default:
                        fprintf(stderr, "Unkown reason\n");
        }
}
