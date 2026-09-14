#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std; 
bool TEST = false; 
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

    //TODO: send stdin and receive message and print on stdout

    return 0 ; 






}