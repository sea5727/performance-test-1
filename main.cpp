#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <sys/epoll.h>

int arrSock[1024] = { 0, };
int main(int, char**) {
    std::cout << "Hello, world!\n";

    std::string result = "init";
    int nSocket   = -1;

    nSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(nSocket < 0) {    
        result = "socket create error";
        std::cout << result << std::endl;
        return -1;
    }

    sockaddr_in server_addr, client_addr;
    size_t clientAddrSize = sizeof(int);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(30000);


    if(bind(nSocket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0 ){
        result = "bind fail";
        std::cout << result << std::endl;
        return -1;
    }



    if(listen(nSocket, 5) < 0){
        result = "listen fail";
        std::cout << result << std::endl;
        return -1;
    }


    char szBuf[8192] = "";


    int nEpollFD = epoll_create(1000);

    fd_set set;
    struct timeval timeout;
    int rv;

    while(1){
            
        FD_ZERO(&set); /* clear the set */
        FD_SET(nSocket, &set); /* add our file descriptor to the set */

        timeout.tv_sec = 0;
        timeout.tv_usec = 10 * 1000;

        if( (rv = select(nSocket + 1, &set, NULL, NULL, &timeout)) <= 0) continue;
 
        if(FD_ISSET(nSocket, &set)){
            std::cout << "select!! FD_ISSET" << std::endl;
            size_t clientAddrSize = sizeof(int);
            int nClientSocket = accept(nSocket, (struct sockaddr *) &client_addr, (socklen_t *) &clientAddrSize);
            std::cout << "Client accepted.. " << inet_ntoa(client_addr.sin_addr) << " : " << ntohs(client_addr.sin_port) << std::endl;
            struct epoll_event stEvent;
            stEvent.events = EPOLLIN;
            stEvent.data.fd = nClientSocket;
            int nEpollRet = epoll_ctl(nEpollFD, EPOLL_CTL_ADD, nClientSocket, &stEvent);
            if(nEpollRet != 0){
                std::cout << "epoll ctl ADD error" << std::endl;
            }
            struct epoll_event stEvents[100];
            int nEpoll_EventCnt = epoll_wait(nEpollFD, stEvents, 100, 30);
        }
  
    //         //     read_len = recv(s, szBuf, sizeof(szBuf), 0);
    //         //     if(read_len <= 0){
    //         //         std::cout << "client read fail.." << std::endl;
    //         //         close(s);
    //         //         break;
    //         //     }

    //         //     write_len= write(nClientSocket, szBuf, read_len);
    //         //     if(write_len <= 0){
    //         //         std::cout << "client write fail.." << std::endl;
    //         //         close(s);
    //         //         break;
    //         //     }
                
    }
}
