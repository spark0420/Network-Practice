#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main(){
    int sock = 0, nBytes, port = 0;
    char buffer[1024] = {0};
    struct sockaddr_in serverAddr;
    int addrlen = sizeof serverAddr;

    //Because it is TCP, we need a fild handler
    //As long as it is something other than 0, it is okay
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
        cout << "socket creation error!" << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Please enter the server IP: ";
    cin.getline(buffer, 17, '\n');

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(buffer);
    cout << "Please enter server listening port: ";
    cin.getline(buffer, 6, '\n');
    port = atoi(buffer);
    serverAddr.sin_port = htons(port);

    if(connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
        cout << "Connection Failed!" << endl;
        exit(EXIT_FAILURE);
    }

    do{
        cout << "Type a Message: ";
        cin.getline(buffer, 1024, '\n');
        if(send(sock, buffer, strlen(buffer), 0) == -1){
            cout << "Send Failed!" << endl;
            exit(EXIT_FAILURE);
            break;
        }
    }while(strcmp(buffer, "Quit!") != 0);
    cout << "Exit...\n";
    close(sock);

    return 0;
}