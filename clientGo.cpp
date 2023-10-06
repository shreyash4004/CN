#include<iostream>
#include<stdio.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<string.h>
#define cls() printf("\033[H\033[J")

struct frame {
    int packet[40];
};

struct ack {
    int acknowledge[40];
};

int main() {
    int clientsocket;
    sockaddr_in serveraddr;
    socklen_t len;
    hostent* server;
    frame f1;
    int windowsize, totalpackets, totalframes, i = 0, j = 0, framesreceived = 0, k, buffer, l;
    ack acknowledgement;
    char req[50];

    clientsocket = socket(AF_INET, SOCK_DGRAM, 0);

    memset((char*)&serveraddr, 0, sizeof(serveraddr)); // Added this line
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(5018);
    server = gethostbyname("127.0.0.1");
    memcpy((char*)&serveraddr.sin_addr.s_addr, (char*)server->h_addr, server->h_length); // Added this line

    printf("\nSending request to the client.\n");
    sendto(clientsocket, "HI I AM CLIENT.", sizeof("HI I AM CLIENT."), 0, (sockaddr*)&serveraddr, sizeof(serveraddr));

    printf("\nWaiting for reply.\n");
    recvfrom(clientsocket, req, sizeof(req), 0, (sockaddr*)&serveraddr, &len);
    printf("\nThe server has send:\t%s\n", req);

    printf("\nEnter the window size:\t");
    scanf("%d", &windowsize);

    printf("\n\nSending the window size.\n");
    sendto(clientsocket, (char*)&windowsize, sizeof(windowsize), 0, (sockaddr*)&serveraddr, sizeof(serveraddr));
    cls();

    printf("\nWaiting for the server response.\n");
    recvfrom(clientsocket, (char*)&totalpackets, sizeof(totalpackets), 0, (sockaddr*)&serveraddr, &len);
    printf("\nThe total packets are:\t%d\n", totalpackets);
    sendto(clientsocket, "RECEIVED.", sizeof("RECEIVED."), 0, (sockaddr*)&serveraddr, sizeof(serveraddr));

    recvfrom(clientsocket, (char*)&totalframes, sizeof(totalframes), 0, (sockaddr*)&serveraddr, &len);
    printf("\nThe total frames/windows are:\t%d\n", totalframes);
    sendto(clientsocket, "RECEIVED.", sizeof("RECEIVED."), 0, (sockaddr*)&serveraddr, sizeof(serveraddr));

    printf("\nStarting the process of receiving.\n");

    while (i < totalpackets) {
        printf("\nInitialising the receive buffer.\n");
        printf("\nThe expected frame is %d with packets:  ", framesreceived);
        j = 0;
        buffer = i;

        while (j < windowsize && i < totalpackets) {
            printf("%d  ", i);
            i++;
            j++;
        }

        printf("\n\nWaiting for the frame.\n");

        recvfrom(clientsocket, (char*)&f1, sizeof(f1), 0, (sockaddr*)&serveraddr, &len);
        printf("\nReceived frame %d\n\nEnter -1 to send negative acknowledgement for the following packets.\n", framesreceived);

        j = 0;
        l = buffer;
        k = 0;

        while (j < windowsize && l < totalpackets) {
            printf("\nPacket: %d\n", f1.packet[j]);
            scanf("%d", &acknowledgement.acknowledge[j]);

            if (acknowledgement.acknowledge[j] == -1) {
                if (k == 0) {
                    i = f1.packet[j];
                    k = 1;
                }
            }

            j++;
            l++;
        }

        framesreceived++;
        sendto(clientsocket, (char*)&acknowledgement, sizeof(acknowledgement), 0, (sockaddr*)&serveraddr, sizeof(serveraddr));
        cls();
    }

    printf("\nAll frames received successfully.\n\nClosing connection with the server.\n");
    close(clientsocket);

    return 0;
}