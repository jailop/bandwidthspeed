/* Bandwidth Speed Tester - Server */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <signal.h>

#define PORT 9999
#define WINDOW_SIZE 16 * 1024

int sockfd = -1;  // Socket descriptor

/* Clean up function when user press Ctrl+C
 * If the server socket is open, it is closed
 */
void sigint_handler(int signum) {
        if (sockfd != -1) close(sockfd); 
        printf("\nAll connections are closed\n");
        exit(0);
}

/* Utility to identify up network connections
 * in the host.
 */
void server_addresses(void) {
    // Getting the list of devices/addresses
    struct ifaddrs *ifap;
    int res = getifaddrs(&ifap);
    if (res != 0) {
        fprintf(stderr, "error on getting list of devices/addresses (%d)\n",
            errno);
        return;
    }
    // Looping address by address
    struct ifaddrs *curr = ifap;
    printf("Server listening at:\n");
    while (curr != NULL) {
        if (curr->ifa_addr && curr->ifa_addr->sa_family == AF_INET) {
            // This is an INET address, so let's process it
            struct sockaddr_in *haddr = (struct sockaddr_in *) curr->ifa_addr;
            printf("  %s: %s\n", curr->ifa_name, inet_ntoa(haddr->sin_addr));
        }
        curr = curr->ifa_next;
    }
    // Cleaning
    freeifaddrs(ifap);
}

int main(int argc, char *argv[]) {
        // Registering signal handler for Ctrl+C
        signal(SIGINT, sigint_handler);
        // Obtaining the server socket
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
                fprintf(stderr, "Failed to create socket\n");
                exit(1);
        }
        // Socket initialization
        struct sockaddr_in srv;
        bzero(&srv, sizeof(srv));
        srv.sin_family = AF_INET; 
        srv.sin_addr.s_addr = htonl(INADDR_ANY); 
        srv.sin_port = htons(PORT); 

        // Binding the connection
        if (bind(sockfd, (const struct sockaddr *) &srv,
                        sizeof(srv)) != 0) {
                fprintf(stderr, "Failed to bind server\n");
                exit(1);
        }

        // Start listening for client connections
        if ((listen(sockfd, 8)) != 0) {
                fprintf(stderr, "Server failed to start listening\n");
                exit(1);
        }
      
        // General information
        printf("Bandwidth Speed Tester\n");
        server_addresses();
        printf("TCP window size: %d KB\n", WINDOW_SIZE / 1024);
        printf("Press Ctrl+C to exit\n\n");

        struct sockaddr cli;  // Client address
        unsigned int len;
        long connfd;          // Client sream identifier
        pid_t childpid;
        while (1) {
                // Waiting to accept a new connection
                connfd = accept(sockfd, (struct sockaddr *) &cli, &len);
                // Priting client address
                char *client_address = inet_ntoa(((struct sockaddr_in *)
                        &cli)->sin_addr);
                printf("Connection accepted from %s\n", client_address);
                if ((childpid = fork()) == 0) {
                        // Forked to attend in a new process
                        // dumping data to the client
                        char buf[4096];
                        while (send(connfd, buf, 4096, 0) != -1);
                        close(connfd);
                }
        }

        // Clean up
        close(sockfd);
        return 0;
}
