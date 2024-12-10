#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 12345
#define MAXLINE 1024

int sockfd;
struct sockaddr_in servaddr;
socklen_t len;

void *receive_messages(void *arg) {
    char buffer[MAXLINE];
    while (1) {
        int n = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, NULL, NULL);
        buffer[n] = '\0';
        printf("\nServer: %s\nYou: ", buffer);
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
        sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM, (struct sockaddr *)&servaddr, len);
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
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    len = sizeof(servaddr);

    printf("UDP Chat Client started. Type your messages below:\n");

    // Threads for sending and receiving messages
    pthread_create(&recv_thread, NULL, receive_messages, NULL);
    pthread_create(&send_thread, NULL, send_messages, NULL);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);

    close(sockfd);
    return 0;
}
