#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std; 
bool TEST = false; 
int MAXLEN = 1024; 
/*
usage: safely write n characters in the buffer to the socket
writen(fd,buf,n)
fd: socketed id 
buffer: message buffer
n: size of buffer
*/
int written(int fd, const void *buf, size_t n) {
    const char *ptr = static_cast<const char*>(buf); 
    size_t total_sent = 0;
    while (total_sent < n) {
        int sent = write(fd, ptr + total_sent, n - total_sent);
        // error handle
        if(sent==-1){
            cerr<< strerror(errno) ; 
            if(errno!=EINTR){
                return -1; 
            }
            //if connection interrupted, attempt writing again
            sent = 0 ; 
        }
        //update sent counter
        total_sent += sent; 
    }
    return total_sent;
}

int readlineSlow(); 

int main(int argc, char* argv[]) {
    string ip = argv[1];
    string port = argv[2]; 
    //creat socket
    int sockId = socket(AF_INET,SOCK_STREAM,0); //socket descritor: (IPv4,TCP, automatied protocol)
    //define connection destination
    sockaddr_in serverAddr{};  //Internet address structure
    serverAddr.sin_family = AF_INET;  //IPv4
    int portNumber = stoi(port) ; 
    serverAddr.sin_port = htons(portNumber); //set server port number for connection, hton convert integer to internet format
    inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);  //set server ip
    //establish connection
    int connectStatus = connect(
        sockId,
        reinterpret_cast<sockaddr*>(&serverAddr),
        sizeof(serverAddr)
    );
    if (connectStatus < 0 ){
        cerr<< "connection fail"; 
        return 0 ;
    } //connection fail 
    //connection success
    char buffer[1024]; //IO buffer 
    while(fgets(buffer, sizeof(buffer), stdin)!=NULL){
        written(sockId, buffer, strlen(buffer));
        int n = read(sockId, buffer, sizeof(buffer));
        fwrite(buffer, 1, n, stdout);
    }
    cout<<"Disconnected"<<endl; 
    close(sockId);
    return 0;
}