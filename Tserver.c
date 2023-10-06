#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 12345
#define BUFSIZE 1024

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Failed to initialize Winsock");
        return 1;
    }

    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int addrSize = sizeof(struct sockaddr_in);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        perror("Error in socket creation");
        return 1;
    }

    // Set up server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind socket to address
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("Error in binding");
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        perror("Error in listening");
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept a connection
    clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrSize);
    if (clientSocket == INVALID_SOCKET) {
        perror("Error in accepting connection");
        return 1;
    }

    // File transfer
    FILE* file;
    char buffer[BUFSIZE];
    int bytesReceived;

    file = fopen("receive.txt", "wb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    while ((bytesReceived = recv(clientSocket, buffer, BUFSIZE, 0)) > 0) {
        fwrite(buffer, 1, bytesReceived, file);
    }

    fclose(file);
    printf("File received successfully.\n");

    // Close sockets
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
