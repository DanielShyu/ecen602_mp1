#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
const int MAXLEN = 5 ; 
// Function prototypes
ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t readline(int fd, void *vptr, size_t maxlen);

// The writen function writes exactly 'n' bytes to a socket.
ssize_t writen(int fd, const void *vptr, size_t n){
    ssize_t nleft = n; // number of bytes left to write
    ssize_t nwritten; // number of bytes written in the last write() call
    char *ptr = (char *)vptr;

    while (nleft>0){
        nwritten = write(fd, ptr, nleft);
        if(nwritten < 0){
            if(errno == EINTR)
                nwritten = 0; // call write() again
            else{
                printf("Error writing to socket: %s\n", strerror(errno));
                return -1;
            }
        } else if (nwritten == 0) {
            break;
        }
        ptr += nwritten;
        nleft -= nwritten;
    }
    return n - nleft; // return the number of bytes written (excluding the null terminator)

}

// The readline function reads a line from a socket, up to 'maxlen' bytes.
ssize_t readline(int fd, void *vptr, size_t maxlen){
    ssize_t n, rc; //n is the number of bytes read, rc is the return code from read()
    char c, *ptr; //c is the character read, ptr is a pointer to the buffer where the line will be stored
    ptr = (char *)vptr;
    
    for (n = 1; n < maxlen ; n++){
        rc = read(fd, &c, 1);
        if (rc==1){
            *ptr++ = c;
            if(c == '\n'){
                break; // newline is stored,
            }
        }
        else if (rc == 0){
            *ptr = 0;
            return n - 1;// End of file, n - 1 bytes were read
        }
        else{
            printf("Error reading from socket: %s\n", strerror(errno));
            return -1;
        }
    }
    *ptr = 0; // null-terminate the string
    
    if (n == (ssize_t)maxlen) {
        return n - 1; // buffer full, but no '\n' us found，thus minus one
    }
    return n ; // return the number of bytes read
}

int main(int argc, char **argv) {
    int listenfd, connfd;

    // The server is started with the command line: echos <port>[cite: 1].
    if (argc != 2) {
        fprintf(stderr, "Usage: echos <port>\n");
        return 1;
    }

    // Convert the port number from string to integer.
    int port = atoi(argv[1]);
    if (port <= 0) {
        fprintf(stderr, "Invalid port number\n");
        return 1;
    }
    
    //create socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("socket");
        return 1;
    }
    
    //setup server address structure
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    //bind socket to the specified port
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind");
        close(listenfd);
        return 1;
    }

    //listen for incoming connections
    if (listen(listenfd, MAXLEN) < 0) {
        perror("listen");
        close(listenfd);
        return 1;
    }

    //accept incoming connections and handle them
    while (1) {
        connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
        if (connfd < 0) {
            perror("accept");
            continue;
        }else{
            printf("Accepted connection\n");
        }

        //fork a child process to handle the connection
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }

        // In the child process, handle the connection and exit
        if (pid == 0) {
            // Close the listening socket in the child process
            close(listenfd);
            //use readline and writen functions to read from and write to the socket
            ssize_t n; // number of bytes read
            char buffer[MAXLEN];
            // Read lines from the client and echo them back
            while (1) {
                n = readline(connfd, buffer, sizeof(buffer));

                if (n < 0) {
                    break;
                }
                if (n == 0) {
                    break;
                }
                if (writen(connfd, buffer, n) < 0) {
                    break;
                }
            }
            close(connfd); // Close the connected socket in the child process
            printf("Connection handled and closed\n");
            exit(0);// Exit the child process after handling the connection
        }
        
        //parent process closes the connection socket and continues to accept new connections
        close(connfd);
    }
    
}