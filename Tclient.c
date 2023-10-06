#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFSIZE 1024

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return 1;
    }

    int clientSocket;
    struct sockaddr_in serverAddr;
    int addrSize = sizeof(struct sockaddr_in);

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        perror("Error in socket creation");
        return 1;
    }

    // Set up server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, addrSize) == SOCKET_ERROR) {
        perror("Error in connecting to the server");
        return 1;
    }

    // File transfer
    FILE* file;
    char buffer[BUFSIZE];
    int bytesRead;

    file = fopen("send.txt", "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    while ((bytesRead = fread(buffer, 1, BUFSIZE, file)) > 0) {
        send(clientSocket, buffer, bytesRead, 0);
    }

    fclose(file);
    printf("File sent successfully.\n");

    // Close the socket
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
