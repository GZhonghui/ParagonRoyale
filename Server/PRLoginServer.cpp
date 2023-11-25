#include <iostream>
#include <unordered_map>

#include "SQLTool.h"
#include "GameMessage.pb.h"
#include "PRLoginDefine.h"

#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include <netinet/tcp.h>
#include <netdb.h>
using namespace std;

const int MaxClient = 1024;
const int MaxEvents = 10;
SQLTool mySQL;
int GateFd = -1;
GateBuffer gateBuffer;
uint8_t SendBuffer[256];
unordered_map<string,int>  iHeart;
unordered_map<int,string> fdToName;
unordered_map<string,int> nameToFd;
//心跳检测
inline void CheckHeart()
{
    for(auto x:iHeart)
    {
        if(iHeart[x.first]>=0)
            iHeart[x.first]--;
        
        if(iHeart[x.first]<0&&nameToFd.count(x.first)&&fdToName.count(nameToFd[x.first]))
        {
            fdToName.erase(nameToFd[x.first]);
            nameToFd.erase(x.first);
        }
    }
}

inline void DealMessage()
{
    switch(static_cast<MessageType>(gateBuffer.iMessageType))
    {
    case MessageType::RegisterRequest:
    {
        PRegisterRequest RegisteredRequest;
        RegisteredRequest.ParseFromArray(gateBuffer.pMessage, gateBuffer.iMessageLen);
        PRegisterRespond RegisteredRespond;
        RegisteredRespond.set_success(0);//0成功1失败
        if(mySQL.addUser(RegisteredRequest.username(),RegisteredRequest.password()))
        {
             RegisteredRespond.set_success(1);
        }
        else
        {
            printf("Registered Fail (%d)\n", gateBuffer.iTargetFd);
        }
        uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::RegisterRespond);
        uint8_t SingleMessageLen = static_cast<uint8_t>(RegisteredRespond.ByteSizeLong());
        
        memcpy(SendBuffer + 0, &gateBuffer.iTargetFd, 4);
        memcpy(SendBuffer + 4, &SingleMessageType, 1);
        memcpy(SendBuffer + 5, &SingleMessageLen, 1);
        RegisteredRespond.SerializeToArray(SendBuffer + 6, SingleMessageLen);
        uint32_t sendLen = 0;
        uint32_t sumLen = SingleMessageLen + 6;
        while(sendLen < sumLen)
        {
            sendLen += send(GateFd, SendBuffer + sendLen, sumLen - sendLen, 0);
        }
        printf("deal message 1:%d\n",gateBuffer.iTargetFd);

    }break;
    case MessageType::LoginRequest:
    {
        PLoginRequest LoginRequest;
        LoginRequest.ParseFromArray(gateBuffer.pMessage, gateBuffer.iMessageLen);
        PLoginRespond LoginRespond;
        LoginRespond.set_success(1);
        LoginRespond.set_errorcode(0);
        string realPassword = "";
        fdToName[gateBuffer.iTargetFd] = LoginRequest.username();
        nameToFd[LoginRequest.username()] = gateBuffer.iTargetFd;
        if(!mySQL.getPassWord(LoginRequest.username(),realPassword))
        {
            LoginRespond.set_success(0);
            LoginRespond.set_errorcode(1);
                printf("no Player\n");
        }
        else if(realPassword != LoginRequest.password())
        {
            LoginRespond.set_success(0);
            LoginRespond.set_errorcode(2);
                printf("Wrong PassWord\n");
        }
        else
        {
            if(iHeart.count(LoginRequest.username())&&iHeart[LoginRequest.username()]>=0)
            {
                LoginRespond.set_success(0);
                LoginRespond.set_errorcode(3);
                printf("Logining\n");
            }
            else
            {
                iHeart[LoginRequest.username()] = 2;
                printf("Successfil\n");
            }
            //int id = 0;
            //if(mySQL.getUserID(LoginRequest.username(),id))
            //    pClient->iUserID = id;
        }
        uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::LoginRespond);
        uint8_t SingleMessageLen = static_cast<uint8_t>(LoginRespond.ByteSizeLong());
        memcpy(SendBuffer + 0, &gateBuffer.iTargetFd, 4);
        memcpy(SendBuffer + 4, &SingleMessageType, 1);
        memcpy(SendBuffer + 5, &SingleMessageLen, 1);
        LoginRespond.SerializeToArray(SendBuffer + 6, SingleMessageLen);
        uint32_t sendLen = 0;
        uint32_t sumLen = SingleMessageLen + 6;
        while(sendLen < sumLen)
        {
            sendLen += send(GateFd, SendBuffer + sendLen, sumLen - sendLen, 0);
        }
        printf("deal Login message :%d\n",gateBuffer.iTargetFd);

    }break;
    case MessageType::Heart:
    {
        if(fdToName.count(gateBuffer.iTargetFd)&&nameToFd.count(fdToName[gateBuffer.iTargetFd])&&gateBuffer.iTargetFd==nameToFd[fdToName[gateBuffer.iTargetFd]]&&iHeart.count(fdToName[gateBuffer.iTargetFd]))
            iHeart[fdToName[gateBuffer.iTargetFd]] = 2;
    }break;
    default:
    {
        printf("Error: Message Type: %u\n", gateBuffer.iTargetFd);
    }break;
    }
}


inline void CallGateThisLoginServer(int sockfd)
{
    uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::ThisIsLoginServer);
    uint8_t SingleMessageLen = 0;
    memcpy(SendBuffer + 0, &SingleMessageType, 1);
    memcpy(SendBuffer + 1, &SingleMessageLen, 1);
    uint32_t sendLen = 0, x = 0;
    uint32_t sumLen = SingleMessageLen + 2;
    while(sendLen < sumLen)
    {
        x = send(sockfd, SendBuffer + sendLen, sumLen - sendLen, 0);
        if(x < 0)
        {
            cout<<"Login: send 错误\n";
            return;
        }
        sendLen += x;
    }
}
int main()
{
    struct epoll_event event;
    struct epoll_event wait_event[MaxEvents];

    printf("Login Server Start\n");

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Login error in creating socket\n");
        return -1;
    }
    int on = 1;
    //开启TCP_NODELAY
    if(setsockopt(sockfd, IPPROTO_TCP,TCP_NODELAY, &on, sizeof(on))<0)
    {
        printf("Login error in set TCP_NODELAY\n");
        return -1;
    }
    struct sockaddr_in my_addr;
    struct hostent* h;
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(iPort);
    if( (h = gethostbyname("10.0.128.199")) == 0)
	{
		printf("Login gethostbyname failed,\n");
		close(sockfd);
        return -1; 
	}
    memcpy(&my_addr.sin_addr, h->h_addr, h->h_length);
    if(connect(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) != 0)
	{
		printf("Login connect error\n");
		close(sockfd);
        return -1; 
	}
    printf("Login Server Content Gate Successful\n");
    //告诉网关这是游戏服务器
    CallGateThisLoginServer(sockfd);
    GateFd = sockfd;
    //创建epoll
    int epfd = epoll_create(MaxEvents);
    if (epfd<0)
    {
        printf("error in creating epfd\n");
        return -1;
    }
    event.data.fd = sockfd;
    event.events = EPOLLIN;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);
    if (ret<0)
    {
        printf("error in epoll adding server\n");
        return -1;
    }

    //时间戳用于帧同步、检测是否匹配成功
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long heartCount = 0;
    long long beginTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    while (true)
    {
        ret = epoll_wait(epfd, wait_event, MaxEvents, 1);
        //5秒一次心跳检测
        gettimeofday(&tv, NULL);
        if((tv.tv_sec * 1000 + tv.tv_usec / 1000 - beginTime)/5000 > heartCount)
        {
            heartCount++;
            CheckHeart();
        }
        switch (ret)
        {
        case -1:
            printf("epoll_wait Erro\n");
            exit(2);
        default:
        {
            for (int i = 0; i < ret; i++)
            {
                //最新的消息
                if ((GateFd == wait_event[i].data.fd) && (wait_event[i].events & EPOLLIN))
                {
                    int len = 0;
                    if (!gateBuffer.iMessageType || !gateBuffer.iMessageLen || gateBuffer.iTargetFd==-1)
                    {
                        len = recv(wait_event[i].data.fd, gateBuffer.pMessage + gateBuffer.iSize, 6 - gateBuffer.iSize, 0);
                        gateBuffer.iSize += len;
                        if (gateBuffer.iSize >= 6)
                        {
                            memcpy(&gateBuffer.iTargetFd, gateBuffer.pMessage + 0, 4);
                            memcpy(&gateBuffer.iMessageType, gateBuffer.pMessage + 4, 1);
                            memcpy(&gateBuffer.iMessageLen, gateBuffer.pMessage + 5, 1);
                            gateBuffer.iSize = 0;
                            if (!gateBuffer.iMessageLen) // No Data Message
                            {
                                DealMessage();
                                gateBuffer.iMessageType = 0;
                                gateBuffer.iMessageLen = 0;
                                gateBuffer.iTargetFd = -1;
                                gateBuffer.iSize = 0;
                            }
                        }
                    }
                    else
                    {
                        len = recv(wait_event[i].data.fd, gateBuffer.pMessage + gateBuffer.iSize, gateBuffer.iMessageLen - gateBuffer.iSize, 0);
                        gateBuffer.iSize += len;
                        if (gateBuffer.iSize >= gateBuffer.iMessageLen)
                        {
                            DealMessage();
                            gateBuffer.iMessageType = 0;
                            gateBuffer.iMessageLen = 0;
                            gateBuffer.iTargetFd = -1;
                            gateBuffer.iSize = 0;
                        }
                    }
                }
            }
        }
        }
    }
    return 0;
}