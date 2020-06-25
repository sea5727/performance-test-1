#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <sys/epoll.h>

 
#define BUF_LEN 8196
 
int main(int argc, char *argv[])
{
        int s, n;
        char *haddr;
        struct sockaddr_in server_addr;
        //struct sockaddr_in server_addr : 서버의 소켓주소 구조체
        char read_buf[BUF_LEN+1] = { 0, };
        char write_buf[BUF_LEN+1] = { 0, };
        strcpy(write_buf, "start");
        if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("can't create socket\n");
            return -1;
        }
 
        memset((char *)&server_addr, 0x00, sizeof(server_addr));
        //서버의 소켓주소 구조체 server_addr을 NULL로 초기화
 
        server_addr.sin_family = AF_INET;
        //주소 체계를 AF_INET 로 선택
        server_addr.sin_addr.s_addr = inet_addr("192.168.0.34");
        //32비트의 IP주소로 변환
        server_addr.sin_port = htons(30000);
        //daytime 서비스 포트 번호
 
        if(connect(s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        {//서버로 연결요청
            printf("can't connect.\n");
            exit(0);
        }
        int write_len = 0;
        int read_len = 0;
        while(1)
        {
            write_len = write(s, write_buf, BUF_LEN);
            if(write_len <= 0){
                std::cout << "write.. len : " << write_len << std::endl;
                return -1;
            }
            std::cout << "write : " << write_buf << " LEN : " << BUF_LEN << std::endl;

            read_len = read(s, read_buf, BUF_LEN);
            if(read_len <= 0){
                std::cout << "read.. len : " << read_len << std::endl;
                return -1;
            }
            std::cout << "read : " << read_buf << " LEN : " << BUF_LEN << std::endl;
        }
 
        close(s);
        //사용이 완료된 소켓을 닫기
}