#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string>
#include <sys/ioctl.h>
#include <vector>
#include <iterator>

using namespace std;

void *respond(void *arg);
string readRequest(char message[]);

int thread_count = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char const *argv[]){
    int serverSocket, new_socket, opt = 1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int port = 12345;
    pthread_t tid[100];

    if((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        cout << "socket creation error!" << endl;
        exit(EXIT_FAILURE);
    }

    if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        cout << "socket setsocketopt error!" << endl;
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if(bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0){
        exit(EXIT_FAILURE);
    }

    cout << "Listen to new tcp request in port 12345..." << endl;

    if(listen(serverSocket, 10) < 0){
        exit(EXIT_FAILURE);
    }
    while(1){
        if((new_socket = accept(serverSocket, (struct sockaddr *) & address, (socklen_t *)&addrlen)) < 0){
            cout << "Accept Failed!" << endl;
            exit(EXIT_FAILURE);
        }else{
            cout << "\nNew Connection!" << endl;
            pthread_create(&tid[thread_count], NULL, respond, &new_socket);
            pthread_detach(tid[thread_count]);
            pthread_mutex_lock(&mutex1);
            thread_count++;
            cout << "Number of connections: " << thread_count << endl;
            pthread_mutex_unlock(&mutex1);
        }
        while(thread_count > 99){ 
            sleep(1); 
        }
    }
    
    return 0;
}

void *respond(void *arg){
    int new_socket;
    char message[1024] = {0};
    char clientip[20];
    string requestedFile;
    int nBytes;

    new_socket = *((int *)arg);
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);

    strcpy(clientip, inet_ntoa(addr.sin_addr));
    cout << "Client IP: " << clientip << endl;

    //creating a stringstream for ouput from server
    ostringstream serverOutput;
    serverOutput << "HTTP/1.1 200 OK\r\n";
    serverOutput << "Content-Type: text/html; charset=UTF-8\r\n\r\n";


    memset(message, '\0', sizeof(message));
    nBytes = 0;
    
    nBytes = read(new_socket, message, 1024);

    if(nBytes != 0){
        int error_code;
        unsigned int error_code_size = sizeof(error_code);
        if(getsockopt(new_socket, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size) == 0){
            if(error_code == 0){
                cout << "\n========= Listening to the clients' request========\n";
                cout << message << endl;
                requestedFile = readRequest(message);
                ifstream f(requestedFile);
                    if(f.good()){
                        string str((istreambuf_iterator<char>(f)), istreambuf_iterator<char>());
                        serverOutput << str;
                        send(new_socket, serverOutput.str().c_str(), serverOutput.str().size() + 1, 0);//sending back to the user
                        f.close();
                    }else{
                        ifstream f2("notFound.html");
                        if(f2.good()){
                            string str((istreambuf_iterator<char>(f2)), istreambuf_iterator<char>());
                            serverOutput << str;
                            send(new_socket, serverOutput.str().c_str(), serverOutput.str().size() + 1, 0);//sending back to the user
                            f2.close();
                        }
                        else{
                            exit(EXIT_FAILURE);
                        }
                    }
            }else{
                exit(EXIT_FAILURE);
            }
        }else{
            exit(EXIT_FAILURE);
        }
    }

    cout << "\033[1;31mClient disconnected!\033[0m" << endl;
    pthread_mutex_lock(&mutex1);
    thread_count--;
    cout << "\033[1;34mNumber of connections: " << thread_count << "\033[0m" << endl;
    pthread_mutex_unlock(&mutex1);
    close(new_socket);
    pthread_exit(NULL);
}

string readRequest(char message[]){
    string fileName = "";

    istringstream iter(message);
    vector<string> parsed((istream_iterator<string>(iter)),istream_iterator<string>());

    if(parsed.size() >= 3 && parsed[0] == "GET"){
        fileName = parsed[1];
        int strLen = fileName.length();
        //to remove / in the file name
        fileName = fileName.substr(1, strLen-1);
        return fileName;
    }else{
        return fileName;
    }

}