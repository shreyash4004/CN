#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h> // Add this for 'close'

#define cls() printf("\033[H\033[J") // Corrected escape sequence

struct frame {
    int packet[40];
};

struct ack {
    int acknowledge[40];
};

int main() {
    int serversocket;
    sockaddr_in serveraddr, clientaddr;
    socklen_t len;
    int windowsize, totalpackets, totalframes, framessend = 0, i = 0, j = 0, k, buffer, l;
    ack acknowledgement;
    frame f1;
    char req[50];

    serversocket = socket(AF_INET, SOCK_DGRAM, 0);

    memset((char *)&serveraddr, 0, sizeof(serveraddr)); // Use memset to initialize
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(5018);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    bind(serversocket, (sockaddr *)&serveraddr, sizeof(serveraddr));

    bzero((char *)&clientaddr, sizeof(clientaddr));
    len = sizeof(clientaddr);

    printf("\nWaiting for client connection.\n");
    recvfrom(serversocket, req, sizeof(req), 0, (sockaddr *)&clientaddr, &len);
    printf("\nThe client connection obtained.\t%s\n", req);

    printf("\nSending request for window size.\n");
    sendto(serversocket, "REQUEST FOR WINDOWSIZE.", sizeof("REQUEST FOR WINDOWSIZE."), 0, (sockaddr *)&clientaddr, sizeof(clientaddr));

    printf("\nWaiting for the windowsize.\n");
    recvfrom(serversocket, (char *)&windowsize, sizeof(windowsize), 0, (sockaddr *)&clientaddr, &len);
    cls();
    printf("\nThe windowsize obtained as:\t%d\n", windowsize);

    printf("\nObtaining packets from network layer.\n");
    printf("\nTotal packets obtained:\t%d\n", (totalpackets = windowsize * 5));
    printf("\nTotal frames or windows to be transmitted:\t%d\n", (totalframes = 5));

    printf("\nSending total number of packets.\n");
    sendto(serversocket, (char *)&totalpackets, sizeof(totalpackets), 0, (sockaddr *)&clientaddr, sizeof(clientaddr));
    recvfrom(serversocket, req, sizeof(req), 0, (sockaddr *)&clientaddr, &len);

    printf("\nSending total number of frames.\n");
    sendto(serversocket, (char *)&totalframes, sizeof(totalframes), 0, (sockaddr *)&clientaddr, sizeof(clientaddr));
    recvfrom(serversocket, req, sizeof(req), 0, (sockaddr *)&clientaddr, &len);

    printf("\nPRESS ENTER TO START THE PROCESS.\n");
    fgets(req, 2, stdin);
    cls();

    while (i < totalpackets) {
        bzero((char *)&f1, sizeof(f1));
        printf("\nInitialising the transmit buffer.\n");
        printf("\nThe frame to be send is %d with packets:\t", framessend);
        buffer = i;
        j = 0;

        while (j < windowsize && i < totalpackets) {
            printf("%d  ", i);
            f1.packet[j] = i;
            i++;
            j++;
        }

        printf("\nSending frame %d\n", framessend);

        sendto(serversocket, (char *)&f1, sizeof(f1), 0, (sockaddr *)&clientaddr, sizeof(clientaddr));
        printf("\nWaiting for the acknowledgement.\n");
        recvfrom(serversocket, (char *)&acknowledgement, sizeof(acknowledgement), 0, (sockaddr *)&clientaddr, &len);
        cls();

        j = 0;
        k = 0;
        l = buffer;

        while (j < windowsize && l < totalpackets) {
            if (acknowledgement.acknowledge[j] == -1) {
                printf("\nNegative acknowledgement received for packet: %d\n", f1.packet[j]);
                printf("\nRetransmitting from packet: %d.\n", f1.packet[j]);
                i = f1.packet[j];
                k = 1;
                break;
            }
            j++;
            l++;
        }

        if (k == 0) {
            printf("\nPositive acknowledgement received for all packets within the frame: %d\n", framessend);
        }

        framessend++;
        printf("\nPRESS ENTER TO PROCEED……\n");
        fgets(req, 2, stdin);
        cls();
    }

    printf("\nAll frames send successfully.\n\nClosing connection with the client.\n");
    close(serversocket);

    return 0;
}