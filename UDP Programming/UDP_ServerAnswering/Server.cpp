#include <iostream>
#include <string.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h> 

using namespace std;

int main()
{

    int udpSocket, nBytes;
    int portNum;
    char buffer[1024];
    char input_buffer[1024];
    struct sockaddr_in ServerAddr, ClientAddr;
    char *ClientIP;

    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);


    ServerAddr.sin_family = AF_INET;
    portNum = atoi("12345");
    
    ServerAddr.sin_port = htons(portNum);

    ServerAddr.sin_addr.s_addr = INADDR_ANY;

    memset(ServerAddr.sin_zero, '\0', sizeof(ServerAddr.sin_zero));

    bind(udpSocket, (struct sockaddr *) &ServerAddr, sizeof(ServerAddr));

    socklen_t addr_size = sizeof ClientAddr;
    do{
        nBytes = recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr *)&ClientAddr, &addr_size);
        buffer[nBytes] = '\0';
        ClientIP = inet_ntoa(ClientAddr.sin_addr);
        if(strcmp(buffer, "car") == 0){
            strcpy(input_buffer, "fast");
            nBytes = strlen(input_buffer) + 1;
        }else if((strcmp(buffer, "flower") == 0)){
            strcpy(input_buffer, "beautiful");
            nBytes = strlen(input_buffer) + 1;
        }else if(strcmp(buffer, "elephant") == 0){
            strcpy(input_buffer, "mouse");
            nBytes = strlen(input_buffer) + 1;
        }else if((strcmp(buffer, "monkey") == 0)){
            strcpy(input_buffer, "banana");
            nBytes = strlen(input_buffer) + 1;
        }else{
            strcpy(input_buffer, "what?");
            nBytes = strlen(input_buffer) + 1;
        }
        cout << ClientIP << "says: " << buffer << endl;
        sendto(udpSocket, input_buffer, nBytes, 0, (struct sockaddr *)&ClientAddr, addr_size);
        sleep(1);

    }while(strcmp(buffer, "Quit!") != 0);
    close(udpSocket);

    return 0; 
}