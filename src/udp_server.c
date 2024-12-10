#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define MAXLINE 1024

int sockfd;
struct sockaddr_in servaddr, cliaddr;
socklen_t len;

void *receive_messages(void *arg) {
    char buffer[MAXLINE];
    while (1) {
        int n = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';
        printf("\nClient: %s\nYou: ", buffer);
        fflush(stdout);
    }
    return NULL;
}

void *send_messages(void *arg) {
    char buffer[MAXLINE];
    while (1) {
        printf("You: ");
        fgets(buffer, MAXLINE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
        sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM, (struct sockaddr *)&cliaddr, len);
    }
    return NULL;
}

int main() {
    pthread_t send_thread, recv_thread;

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Fill server information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Chat Server listening on port %d...\n", PORT);

    len = sizeof(cliaddr);

    // Threads for sending and receiving messages
    pthread_create(&recv_thread, NULL, receive_messages, NULL);
    pthread_create(&send_thread, NULL, send_messages, NULL);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);

    close(sockfd);
    return 0;
}
