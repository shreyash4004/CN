#include<iostream>
#include<stdio.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<cstring> 

#ifdef _WIN32
#include<winsock2.h>
#else
#include<unistd.h>
#endif

#define cls() printf("33[H33[J")

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
    int windowsize,totalpackets,totalframes,i=0,j=0,framesreceived=0,k,l,m,repacket[40];
    ack acknowledgement;
    char req[50];

    #ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        exit(1);
    }
    #endif

    clientsocket = socket(AF_INET,SOCK_DGRAM,0);
    memset((char*)&serveraddr,sizeof(serveraddr), 0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(5018);
    server = gethostbyname("127.0.0.1");
    memcpy((char*)&serveraddr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);

    printf("\nSending request to the client.\n");
    sendto(clientsocket,"HI I AM CLIENT.",sizeof("HI I AM CLIENT."),0,(sockaddr*)&serveraddr,sizeof(serveraddr));

    printf("\nWaiting for reply.\n");
    recvfrom(clientsocket,req,sizeof(req),0,(sockaddr*)&serveraddr,&len);
    printf("\nThe server has send:\t%s\n",req);

    printf("\nEnter the window size:\t");
    scanf("%d",&windowsize);

    printf("\n\nSending the window size.\n");
    sendto(clientsocket,(char*)&windowsize,sizeof(windowsize),0,(sockaddr*)&serveraddr,sizeof(serveraddr));

    cls();

    printf("\nWaiting for the server response.\n");
    recvfrom(clientsocket,(char*)&totalpackets,sizeof(totalpackets),0,(sockaddr*)&serveraddr,&len);
    printf("\nThe total packets are:\t%d\n",totalpackets);
    sendto(clientsocket,"RECEIVED.",sizeof("RECEIVED."),0,(sockaddr*)&serveraddr,sizeof(serveraddr));

    recvfrom(clientsocket,(char*)&totalframes,sizeof(totalframes),0,(sockaddr*)&serveraddr,&len);
    printf("\nThe total frames/windows are:\t%d\n",totalframes);
    sendto(clientsocket,"RECEIVED.",sizeof("RECEIVED."),0,(sockaddr*)&serveraddr,sizeof(serveraddr));

    printf("\nStarting the process of receiving.\n");

    j = 0;
    l = 0;

    while(l<totalpackets) {
        printf("\nInitialising the receive buffer.\n");
        printf("\nThe expected frame is %d with packets:  ",framesreceived);

        for(m=0;m<j;m++) {
            printf("%d  ",repacket[m]);
        }

        while(j<windowsize && i<totalpackets) {
            printf("%d  ",i); 
            i++;
            j++; 
        }

        printf("\n\nWaiting for the frame.\n");

        recvfrom(clientsocket,(char*)&f1,sizeof(f1),0,(sockaddr*)&serveraddr,&len);

        printf("\nReceived frame %d\n\nEnter -1 to send negative acknowledgement for the following packets.\n",framesreceived);

        j = 0;
        m = 0;
        k = l;

        while(m<windowsize && k<totalpackets) {
            printf("\nPacket: %d\n",f1.packet[m]); 

            scanf("%d",&acknowledgement.acknowledge[m]);

            if(acknowledgement.acknowledge[m]==-1) {
                repacket[j]=f1.packet[m];
                j++;
            } else {
                l++;
            }

            m++;
            k++;
        }

        framesreceived++;

        sendto(clientsocket,(char*)&acknowledgement,sizeof(acknowledgement),0,(sockaddr*)&serveraddr,sizeof(serveraddr));

        cls();
    }

    printf("\nAll frames received successfully.\n\nClosing connection with the server.\n");

    #ifdef _WIN32
    closesocket(clientsocket);
    WSACleanup();
    #else
    shutdown(clientsocket, SHUT_RDWR);
    close(clientsocket);
    #endif

    return 0;
}