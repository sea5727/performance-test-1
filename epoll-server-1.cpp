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


    struct epoll_event stEvent;
    stEvent.events = EPOLLIN;
    stEvent.data.fd = nSocket;
    int nEpollRet = epoll_ctl(nEpollFD, EPOLL_CTL_ADD, nSocket, &stEvent);
    if(nEpollRet != 0){
        std::cout << "epoll ctl ADD error" << std::endl;
    }
    struct epoll_event stEvents[100];
    

    int nEpoll_EventCnt = -1;
    int read_len = -1;
    int write_len = -1;
    while(1){
        nEpoll_EventCnt = epoll_wait(nEpollFD, stEvents, 100, 30);
        if(nEpoll_EventCnt < 0)
            continue;
        
        for(int i = 0 ; i < nEpoll_EventCnt ; i++){
            int nCliSessIdx = stEvents[i].data.fd;
            std::cout << "epoll!!! sock : " << nCliSessIdx << std::endl;
            if(stEvents[i].data.fd == nSocket){
                int nClientSocket = accept(nSocket, (struct sockaddr *) &client_addr, (socklen_t *) &clientAddrSize);
                std::cout << "Client accepted.. " << inet_ntoa(client_addr.sin_addr) << " : " << ntohs(client_addr.sin_port) << std::endl;
                struct epoll_event stEvent;
                stEvent.events = EPOLLIN;
                stEvent.data.fd = nClientSocket;
                int nEpollRet = epoll_ctl(nEpollFD, EPOLL_CTL_ADD, nClientSocket, &stEvent);
                if(nEpollRet != 0){
                    std::cout << "epoll ctl ADD error" << std::endl;
                }
            }
            else {
                read_len = recv(nCliSessIdx, szBuf, sizeof(szBuf), 0);
                if(read_len <= 0){
                    std::cout << "client read fail.." << std::endl;
                    nEpollRet = epoll_ctl(nEpollFD, EPOLL_CTL_DEL, nCliSessIdx, NULL);
                    if(nEpollRet != 0)
                        std::cout << "epoll fail.." << nEpollRet << " read_len : " << read_len << std::endl;
                    close(nCliSessIdx);
                    
        
                    break;
                }

                write_len= write(nCliSessIdx, szBuf, read_len);
                if(write_len <= 0){
                    std::cout << "client write fail.." << std::endl;
                    int nEpollRet = epoll_ctl(nEpollFD, EPOLL_CTL_DEL, nCliSessIdx, NULL);
                    
                    nEpollRet = epoll_ctl(nEpollFD, EPOLL_CTL_DEL, nCliSessIdx, NULL);
                    if(nEpollRet != 0)
                        std::cout << "epoll fail.." << nEpollRet << " read_len : " << read_len << std::endl;
                    close(nCliSessIdx);
                    break;
                }
            }

        }

  
                
    }
}
