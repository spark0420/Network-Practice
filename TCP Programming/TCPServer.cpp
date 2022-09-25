#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main(){
    int sock = 0, new_socket, nBytes, port = 0, opt = 1;
    char buffer[1024] = {0};
    struct sockaddr_in serverAddr;
    int addrlen = sizeof serverAddr;

    //Because it is TCP, we need a fild handler
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        cout << "socket creation error!" << endl;
        exit(EXIT_FAILURE);
    }

    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        cout << "socket setsockopt error!" << endl;
        exit(EXIT_FAILURE);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    cout << "Please enter a listening port: ";
    cin.getline(buffer, 6, '\n');
    port = atoi(buffer);
    serverAddr.sin_port = htons(port);

    if(bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
        cout << "Bind failed!" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "Listening ..." << endl;

    if(listen(sock, 3) < 0){
        cout << "Listening failed!" << endl;
        exit(EXIT_FAILURE);
    }

    if((new_socket = accept(sock, (struct sockaddr *)&serverAddr, (socklen_t *)&addrlen)) < 0){
        cout << "Accept Failed!" << endl;
        exit(EXIT_FAILURE);
    }

    do{
        nBytes = read(new_socket, buffer, 1024);
        buffer[nBytes] = 0;
        cout << "I recieved: " << buffer << endl;
        send(new_socket, buffer, strlen(buffer), 0);
        if(strcmp(buffer, "Quit!") == 0){
            close(new_socket);
        }
        memset(buffer, '\0', sizeof(buffer));
    }while(strcmp(buffer, "Quit!") != 0);
    cout << "Exit...\n";
    close(sock);
    return 0;
}