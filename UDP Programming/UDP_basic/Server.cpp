#include <iostream>
#include <string.h>
#include <sys/socket.h> //Because we are creating socket
#include <netinet/in.h> //Internet header
#include <arpa/inet.h> //Internet was first initialized from ARPA. So we need it for some reasons
#include <unistd.h> 

using namespace std;

int main()
{
    //We want to know how many bytes we are sending, so nBytes
    //buffer = allow us get information from input(recieved and sending messages)
    //socket address internet structure
    //one structure is for server address and the other is for client address
    int udpSocket, nBytes;
    char buffer[1024];
    struct sockaddr_in ServerAddr, ClientAddr;
    char *ClientIP;

    //To create a socket, you need socket API
    //the socket is going to store integer value in the UDP socket
    //socket API arguments(int domain, int type, int protocol)
    //PF_INET -> Because we are using Internet IPB4, we set
    //SOCK_DGRAM -> Type of the protocol
    //0> for protocol itself
    //this is how you subscirbe a socket
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

    //ServerAddr. -> Initialize our server address structure
    //arguments(source )
    //.sin_family -> source internet family 
    //AF_INET -> Because we are IPB4
    //Because we need a port number we are going to listen to, we ask the user the port number
    ServerAddr.sin_family = AF_INET;
    cout << "Pleaase enter listening port number: ";
    cin >> buffer;
    
    //You first set the family, now set the port number
    //portnumber from user is char, so to match server address structure
    //atoi -> char to int
    //htons -> some format that matches sin_port data type
    ServerAddr.sin_port = htons(atoi(buffer));

    //because we will allow any server to talk to us, we set INADDR_ANY
    //INADDR_ANY -> Internet address any
    ServerAddr.sin_addr.s_addr = INADDR_ANY;

    //memset -> memory set
    //sin_zero -> this field has to be zero
    //'\0', sizeof(ServerAddr.sin_zero) -> set zero as much as size of server address sin zero field
    memset(ServerAddr.sin_zero, '\0', sizeof(ServerAddr.sin_zero));

    //Because now server address is ready,
    //bind it to our application
    //we pass the pointer where the server address is
    //socket length is size of the structure itself
    bind(udpSocket, (struct sockaddr *) &ServerAddr, sizeof(ServerAddr));

    //start to recieve information, 
    //while(strcmp(buffer, "Quit") != 0) -> we continue until client sends "quit"
    //close(udpSocket); -> after then, we close our socket
    //recvfrom -> recieve from
    //recvfrom(udpSocket) -> from udpSocket
    //buffer -> store it into the buffer
    //1024 -> maximum byte we are going to allow per a socket
    //(struct sockaddr *)&ClientAddr -> client's address structure
    //&addr_size -> the size of the structure
    //socklen_t addr_size = sizeof ClientAddr -> assign client address size

    //buffer[nBytes] = '\0' -> buffer is a string, so we need to terminate with zero
    //ClientIP =  -> we want to know who's sending
    //inet_ntoa(ClientAddr.sin_addr) -> inet_ntoa -> network type to string
    //ClientAddr.sin_addr -> sender's address
    socklen_t addr_size = sizeof ClientAddr;
    do{
        nBytes = recvfrom(udpSocket, buffer, 1024, 0, (struct sockaddr *)&ClientAddr, &addr_size);
        buffer[nBytes] = '\0';
        ClientIP = inet_ntoa(ClientAddr.sin_addr);
        cout << ClientIP << "says: " << buffer << endl;
    }while(strcmp(buffer, "Quit!") != 0);
    close(udpSocket);

    // sine the main function is int, we need to return something
    return 0; 
    
}