#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std; 
bool TEST = false; 
const int MAXLEN = 5; 
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

int readline(int fd, void *buf, size_t maxlen){
    char *ptr = (char*)(buf);
    size_t total_read = 0;
    while (total_read<maxlen-1) {
        char c;
        int n = read(fd, &c, 1); // 1 byte a time
        if (n == 1) {//successfully read
            ptr[total_read] = c ; //store the read character in buffer
            total_read +=1 ;    //update counter
            if(c=='\n'){ //if \n is read, end this 
                ptr[total_read]= '\0' ;
                return total_read;
            } 
        } else if (n == 0) {
            ptr[total_read] = '\0'; 
            return total_read ;  
        } else { 
            if(errno!=EINTR){
                return -1 ;
            }
        }
    }
    ptr[total_read] = '\0'; 
    return total_read;
}

int main(int argc, char* argv[]) {
    if(argc!=3){
        cout<<"Usage Message"<<endl; 
        return 0; 
    }
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
    char buffer[MAXLEN]; //IO buffer 
    while(fgets(buffer, sizeof(buffer), stdin)!=NULL){
        size_t len = strlen(buffer);
        //clear io buffer that exceed maximum length
        if (len > 0 && buffer[len-1] != '\n') {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            // do nothing, simply flusing the standard io
            }
        }
        int sent = written(sockId, buffer, strlen(buffer));
        //test case 4, close after sent
        
        //cout<<"Disconnected"<<endl; 
        //close(sockId);
        //return 0;
        
        //if sent 
        if(sent==-1){
            cerr<< strerror(errno)<<endl; 
            close(sockId);
            return -1;
        }
        
        int n = readline(sockId,buffer,MAXLEN); 
        if(n==0){
            cout<<"connection end"<<endl;
            close(sockId);
            return 0 ; 
        }
        if(n==-1){
            cerr<< strerror(errno)<<endl ; 
            close(sockId);
            return -1 ; 
        }
        fwrite(buffer, 1, n, stdout);
        if (buffer[n-1] != '\n') {
            cout << endl;
        }
    }
    cout<<"Disconnected"<<endl; 
    close(sockId);
    return 0;
}