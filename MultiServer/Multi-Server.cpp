#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string>
#include <sys/ioctl.h>

using namespace std;

void *respond(void *arg);
//thread_count = for counting the connections between the server and the client
//thread_count = for counting how many requests are sent from the client
int thread_count = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[]){
    int serverSocket, new_socket, port, opt = 1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    //we are accepting 100 connections from the client
    pthread_t tid[100];

    //creating socket file descriptor
    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        cout << "socket creation error!" << endl;
        exit(EXIT_FAILURE);
    }

    //Forcefully serverSocket socket to the port 8080
    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        cout << "socket setsocketopt error!" << endl;
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    cout << "Please enter listening port: ";
    cin.getline(buffer, 7, '\n');
    address.sin_port = htons(atoi(buffer));
    //binding the socket and the port address
    if(bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0){
        exit(EXIT_FAILURE);
    }

    cout << "Listen to new tcp request..." << endl;

    // when the server is handling the current request, there are many requests coming in at the same time
    // the system will automatically set the incoming requests in the queue,
    // However, if there are too many, the system will run out of memory
    // Therefore, we set maximum 10 connections to put in a queue 
    // If there are more than 10, the system will reject the requests
    if(listen(serverSocket, 10) < 0){
        exit(EXIT_FAILURE);
    }
    while(1){//run forever
        if((new_socket = accept(serverSocket, (struct sockaddr *) & address, (socklen_t *)&addrlen)) < 0){
            cout << "Accept Failed!" << endl;
            exit(EXIT_FAILURE);
        }else{
            cout << "New Connection!" << endl;
            //passing new_socket to a thread as an argument
            pthread_create(&tid[thread_count], NULL, respond, &new_socket);
            //pthread_detach(tid) : is used to indicate to your application that storage forthe
            //thread tid can be reclaimed when the thread terminates
            pthread_detach(tid[thread_count]);
            pthread_mutex_lock(&mutex1);
            thread_count++;
            cout << "Number of connections: " << thread_count << endl;
            pthread_mutex_unlock(&mutex1);
        }

        while(thread_count > 99){ // if there are 100 threads
            sleep(1); //wait for a thread to end
        }
    }
    
    return 0;
}

void *respond(void *arg){
    int new_socket;
    char message[1024] = {0};
    char clientip[20];
    int nBytes;
    char *temp, *t;
    bool done = false;
    int size;
    char c[2];

    bool failure = false;

    //the argument is from pthread_create
    //argument is a void pointer, so we need to cast it to int pointer
    new_socket = *((int *)arg);
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);

    //In NAT environment, you only get router IP
    strcpy(clientip, inet_ntoa(addr.sin_addr));
    cout << "Client IP: " << clientip << endl;

    do{
        memset(message, '\0', sizeof(message));
        size = 0;

        do{
            //read 1 character at a time
            //it is because the TCP connection sends 1 byte at a time
            nBytes = read(new_socket, c, 1);
            if(nBytes == 0){
                if(c[0] != 0){
                    failure = true;
                    break; // somthing wrong happened..
                }
            }
            message[size] = c[0];
            size++;
        }while((c[0] != 0) && (!failure)); // it ends when it reaches 0

        if(!failure){//if the network is fine, and we reached the end of the string
            //then we start printing the message
            //In order to resue the socket to send message back to the client,
            // the 3 lines below here are needed
            int error_code;
            unsigned int error_code_size = sizeof(error_code);
            if(getsockopt(new_socket, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size) == 0){
                if(error_code == 0){
                    cout << "Received from " << clientip << ": " << message << endl;
                    if(strcmp(message, "Quit!") != 0){
                        send(new_socket, message, strlen(message) + 1, 0);//sending back to the user
                    }
                }else{
                    failure = true;
                    break;
                }
            }else{
                failure = true;
                break;
            }
        }else{
            break;
        }
    }while(strcmp(message, "Quit!") != 0);

    cout << "\033[1;31mClient disconnected!\033[0m" << endl;
    pthread_mutex_lock(&mutex1);
    thread_count--;
    cout << "\033[1;34mNumber of connections: " << thread_count << "\033[0m" << endl;
    pthread_mutex_unlock(&mutex1);
    close(new_socket);
    pthread_exit(NULL);
}