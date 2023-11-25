#include <iostream>
#include <unordered_map>

#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/tcp.h>

#include "GameMessage.pb.h"
#include "PRGateDefine.h"
using namespace std;

const int MaxClient = 1024;
const int MaxEvents = 2048;

uint8_t SendBuffer[256];

unordered_map<int, int> fdToGateClientIndex;
queue<int> qIdleClientIndex;
GateClient gateClient[MaxClient];
ServerBuffer theLoginServer, theGameServer;
int LoginServerFd = -1;
int GameServerFd = -1;
inline void init()
{
    for (int i = 0; i < MaxClient; i++)
        qIdleClientIndex.push(i);
}
void addClient(int fd)
{
    fdToGateClientIndex[fd] = qIdleClientIndex.front();
    qIdleClientIndex.pop();
    gateClient[fdToGateClientIndex[fd]].init();
    gateClient[fdToGateClientIndex[fd]].fd = fd;
    gateClient[fdToGateClientIndex[fd]].isLogin = false;
    gateClient[fdToGateClientIndex[fd]].isInGame = false;
    gateClient[fdToGateClientIndex[fd]].iHeart = 2;
}

void ProcessEvent(GateClient *client)
{
    int iMessageType = static_cast<MessageType>(client->pBuff.iMessageType);
    if (iMessageType == MessageType::ThisIsLoginServer)//LoginServer
    {
        printf("Login Server connect\n");
        LoginServerFd = client->fd;
        qIdleClientIndex.push(fdToGateClientIndex[client->fd]);
        client->fd = -1;
    }
    else if (iMessageType == MessageType::ThisIsGameServer)//GameServer
    {
        printf("Game Server connect\n");
        GameServerFd = client->fd;
        qIdleClientIndex.push(fdToGateClientIndex[client->fd]);
        client->fd = -1;
    }
    else if (iMessageType > MessageType::LoginMessageTypeBgein && iMessageType < MessageType::GameMessageTypeEnd) // Send to LoginServer OR GameServer
    { 
        if(iMessageType!=MessageType::ClientStep)
            printf("get a Message %d from %d\n",iMessageType,client->fd);
        uint8_t SingleMessageType = client->pBuff.iMessageType;
        uint8_t SingleMessageLen = client->pBuff.iMessageLen;
        uint32_t clientFd = client->fd;
        
        memcpy(SendBuffer + 0, &clientFd, 4);
        memcpy(SendBuffer + 4, &SingleMessageType, 1);
        memcpy(SendBuffer + 5, &SingleMessageLen, 1);
        memcpy(SendBuffer + 6, client->pBuff.pMessage, SingleMessageLen);
        int TargetServer;
        if(iMessageType < MessageType::LoginMessageTypeEnd)
        {
            if(LoginServerFd < 0)
            {
                cout<<"Gate: LoginServer NoRespond\n";
                return;
            }
            TargetServer = LoginServerFd;
            //printf("Gate send a Message To LoginServer\n");
        }
        else
        {
            if(GameServerFd < 0)
            {
                cout<<"Gate: GameServer NoRespond\n";
                return;
            }
            TargetServer = GameServerFd;
            //printf("Gate send a Message To GameServer\n");
        }
        uint32_t sendLen = 0, x = 0;
        uint32_t sumLen = SingleMessageLen + 6;
        while(sendLen < sumLen)
        {
            x = send(TargetServer, SendBuffer + sendLen, sumLen - sendLen, 0);
            if(x < 0)
            {
                cout<<"Gate: send 错误\n";
                return;
            }
            sendLen += x;
        }
        //printf("send over\n");
    }
    else if(iMessageType == MessageType::Heart)
    {
        uint8_t SingleMessageType = iMessageType;
        uint8_t SingleMessageLen = client->pBuff.iMessageLen;
        uint32_t clientFd = client->fd;
        gateClient[fdToGateClientIndex[client->fd]].iHeart = 2;
        memcpy(SendBuffer + 0, &clientFd, 4);
        memcpy(SendBuffer + 4, &SingleMessageType, 1);
        memcpy(SendBuffer + 5, &SingleMessageLen, 1);
        memcpy(SendBuffer + 6, client->pBuff.pMessage, SingleMessageLen);
        int TargetServer;
        if(LoginServerFd < 0)
        {
            cout<<"Gate: LoginServer NoRespond\n";
            return;
        }
        TargetServer = LoginServerFd;
        //printf("Gate send a Message To LoginServer\n");
       
        
        uint32_t sendLen = 0, x = 0;
        uint32_t sumLen = SingleMessageLen + 6;
        while(sendLen < sumLen)
        {
            x = send(TargetServer, SendBuffer + sendLen, sumLen - sendLen, 0);
            if(x < 0)
            {
                cout<<"Gate: send 错误\n";
                return;
            }
            sendLen += x;
        }

        if(GameServerFd < 0)
        {
            cout<<"Gate: GameServer NoRespond\n";
            return;
        }
        TargetServer = GameServerFd;
        //printf("Gate send a Message To GameServer\n");
        sendLen = 0;
        x = 0;
        sumLen = SingleMessageLen + 6;
        while(sendLen < sumLen)
        {
            x = send(TargetServer, SendBuffer + sendLen, sumLen - sendLen, 0);
            if(x < 0)
            {
                cout<<"Gate: send 错误\n";
                return;
            }
            sendLen += x;
        }
    }
    else
    {
        printf("Gate Error: Message Type: %d\n", client->pBuff.iMessageType);
    }
}
void DealServerMessage(ServerBuffer *pServer)
{
    uint8_t SingleMessageType = pServer->iMessageType;
    uint8_t SingleMessageLen = pServer->iMessageLen;
    uint32_t clientFd = pServer->iTargetFd;
    memcpy(SendBuffer + 0, &SingleMessageType, 1);
    memcpy(SendBuffer + 1, &SingleMessageLen, 1);
    memcpy(SendBuffer + 2, pServer->pMessage, SingleMessageLen);
    uint32_t sendLen = 0, x = 0;
    uint32_t sumLen = SingleMessageLen + 2;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long BeginSendTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    long long nowTime;
    while(sendLen < sumLen)
    {
        gettimeofday(&tv, NULL);
        nowTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        if( nowTime - BeginSendTime > 1)return; 
        x = send(clientFd, SendBuffer + sendLen, sumLen - sendLen, 0);
        if(x < 0)
        {
            cout<<"Gate: send 错误\n";
            return;
        }
        sendLen += x;
    }
    //printf("send a Message To Client\n");
}
int main()
{
    init();
    struct epoll_event event;
    struct epoll_event wait_event[MaxEvents];

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Gate Server: error in creating socket\n");
        return -1;
    }
    int on = 1;
    if (setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) < 0)
    {
        printf("Gate Server: error in set TCP_NODELAY\n");
        return -1;
    }

    struct sockaddr_in my_addr;
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(iPort);
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0)
    {
        printf("Gate Server: error in socket bind\n");
        return -1;
    }
    if (listen(sockfd, 10) < 0)
    {
        printf("Gate Server: error in socket listen\n");
        return -1;
    }
    printf("Gate Server Start\n");
    int epfd = epoll_create(MaxEvents);
    if (epfd < 0)
    {
        printf("Gate Server: error in creating epfd\n");
        return -1;
    }
    event.data.fd = sockfd;
    event.events = EPOLLIN;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event);
    if (ret < 0)
    {
        printf("Gate Server: error in epoll adding server\n");
        return -1;
    }

    //时间戳用于心跳检测
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long long lastFrameSendTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    while (true)
    {
        ret = epoll_wait(epfd, wait_event, MaxEvents, 1000);
        // 5秒一次心跳检测
        gettimeofday(&tv, NULL);
        if (tv.tv_sec * 1000 + tv.tv_usec / 1000 - lastFrameSendTime >= 5000)
        {
            lastFrameSendTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
            for (int i = 0; i < MaxClient; i++)
            {
                if (gateClient[i].fd < 0 ||  gateClient[i].fd==LoginServerFd ||gateClient[i].fd==LoginServerFd)
                    continue;
                gateClient[i].iHeart--;
                if (gateClient[i].iHeart < 0)
                {
                    if (epoll_ctl(epfd, EPOLL_CTL_DEL, gateClient[i].fd, &event) < 0)
                    {
                        printf("Gate EPOLL_CTL_DEL error\n");
                        return -1;
                    }
                    if (close(gateClient[i].fd) < 0)
                    {
                        printf("Gate Close fd error\n");
                        return -1;
                    }
                    qIdleClientIndex.push(i);
                    gateClient[i].init();
                }
            }
        }
        switch (ret)
        {
        case -1:
            printf("Gate Server: epoll_wait Error\n");
            exit(2);
        default:
        {
            for (int i = 0; i < ret; i++)
            {
                if ((sockfd == wait_event[i].data.fd) && (wait_event[i].events & EPOLLIN))
                {
                    
                    printf("new connect\n");
                    //连接数量达到最大
                    if (!qIdleClientIndex.empty())
                    {
                        struct sockaddr_in cli_addr;
                        unsigned int clilen = sizeof(cli_addr);
                        int connfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
                        //注册客户端号与fd！！！
                        event.data.fd = connfd;
                        event.events = EPOLLIN;
                        ret = epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &event);
                        if (-1 == ret)
                        {
                            printf("Gate Server: error in adding client\n");
                            return -1;
                        }
                        else
                        {
                            addClient(connfd);
                            printf("new connect %d \n",connfd);
                        }
                    }
                }
                else if ((wait_event[i].events & EPOLLIN) && (wait_event[i].data.fd == LoginServerFd || wait_event[i].data.fd == GameServerFd))//来自服务器的消息
                {
                    
                    int len = 0;
                    ServerBuffer *pServer = NULL;
                    if(wait_event[i].data.fd == LoginServerFd)
                    {
                        pServer = &theLoginServer;
                    }
                    else
                    {
                        pServer = &theGameServer;
                    }
                    if (!pServer->iMessageType || !pServer->iMessageLen || pServer->iTargetFd==-1)
                    {
                        len = recv(wait_event[i].data.fd, pServer->pMessage + pServer->iSize, 6 - pServer->iSize, 0);
                        if(len < 0)
                        {
                            printf("326Read error disconnect%d\n",wait_event[i].data.fd);
                            pServer->iMessageType = 0;
                            pServer->iMessageLen = 0;
                            pServer->iTargetFd = -1;
                            pServer->iSize = 0;
                            if (epoll_ctl(epfd, EPOLL_CTL_DEL, wait_event[i].data.fd, &event) < 0)
                            {
                                printf("Gate EPOLL_CTL_DEL error\n");
                                return -1;
                            }
                            if (close(wait_event[i].data.fd) < 0)
                            {
                                printf("Gate Close fd error\n");
                                return -1;
                            }
                        }
                        pServer->iSize += len;
                        if (pServer->iSize >= 6)
                        {
                            memcpy(&pServer->iTargetFd, pServer->pMessage + 0, 4);
                            memcpy(&pServer->iMessageType, pServer->pMessage + 4, 1);
                            memcpy(&pServer->iMessageLen, pServer->pMessage + 5, 1);
                            pServer->iSize = 0;
                            if (!pServer->iMessageLen) // No Data Message
                            {
                                DealServerMessage(pServer);
                                pServer->iMessageType = 0;
                                pServer->iMessageLen = 0;
                                pServer->iTargetFd = -1;
                                pServer->iSize = 0;
                            }
                        }
                    }
                    else
                    {
                        len = recv(wait_event[i].data.fd, pServer->pMessage + pServer->iSize, pServer->iMessageLen - pServer->iSize, 0);
                        if(len < 0)
                        {
                            printf("364Read error disconnect%d\n",wait_event[i].data.fd);
                            pServer->iMessageType = 0;
                            pServer->iMessageLen = 0;
                            pServer->iTargetFd = -1;
                            pServer->iSize = 0;
                            if (epoll_ctl(epfd, EPOLL_CTL_DEL, wait_event[i].data.fd, &event) < 0)
                            {
                                printf("Gate EPOLL_CTL_DEL error\n");
                                return -1;
                            }
                            if (close(wait_event[i].data.fd) < 0)
                            {
                                printf("Gate Close fd error\n");
                                return -1;
                            }
                        }
                        pServer->iSize += len;
                        if (pServer->iSize >= pServer->iMessageLen)
                        {
                            DealServerMessage(pServer);
                            pServer->iMessageType = 0;
                            pServer->iMessageLen = 0;
                            pServer->iTargetFd = -1;
                            pServer->iSize = 0;
                        }
                    }
                }
                else if (wait_event[i].events & EPOLLIN)
                {
                    //printf("read %d\n",wait_event[i].data.fd);
                    int len = 0;
                    GateClient *pClient = gateClient + fdToGateClientIndex[wait_event[i].data.fd];
                    if (!pClient->pBuff.iMessageType || !pClient->pBuff.iMessageLen)
                    {
                        len = recv(wait_event[i].data.fd, pClient->pBuff.pMessage + pClient->pBuff.iSize, 2 - pClient->pBuff.iSize, 0);
                        if(len < 0)
                        {
                            printf("401Read error disconnect%d\n",wait_event[i].data.fd);
                            
                            qIdleClientIndex.push(fdToGateClientIndex[wait_event[i].data.fd]);
                            gateClient[fdToGateClientIndex[wait_event[i].data.fd]].init();
                            fdToGateClientIndex[wait_event[i].data.fd] = -1;
                            if (epoll_ctl(epfd, EPOLL_CTL_DEL, wait_event[i].data.fd, &event) < 0)
                            {
                                printf("Gate EPOLL_CTL_DEL error\n");
                                return -1;
                            }
                            if (close(wait_event[i].data.fd) < 0)
                            {
                                printf("Gate Close fd error\n");
                                return -1;
                            }
                        }
                        pClient->pBuff.iSize += len;
                        if (pClient->pBuff.iSize >= 2)
                        {
                            memcpy(&pClient->pBuff.iMessageType, pClient->pBuff.pMessage + 0, 1);
                            memcpy(&pClient->pBuff.iMessageLen, pClient->pBuff.pMessage + 1, 1);
                            pClient->pBuff.iSize = 0;
                            if (!pClient->pBuff.iMessageLen) // No Data Message
                            {
                                ProcessEvent(pClient);
                                pClient->pBuff.iMessageType = 0;
                                pClient->pBuff.iMessageLen = 0;
                                pClient->pBuff.iSize = 0;
                            }
                        }
                    }
                    else
                    {
                        len = recv(wait_event[i].data.fd, pClient->pBuff.pMessage + pClient->pBuff.iSize, pClient->pBuff.iMessageLen - pClient->pBuff.iSize, 0);
                        if(len < 0)
                        {
                            printf("Read error disconnect%d\n",wait_event[i].data.fd);
                            
                            qIdleClientIndex.push(fdToGateClientIndex[wait_event[i].data.fd]);
                            gateClient[fdToGateClientIndex[wait_event[i].data.fd]].init();
                            fdToGateClientIndex[wait_event[i].data.fd] = -1;
                            if (epoll_ctl(epfd, EPOLL_CTL_DEL, wait_event[i].data.fd, &event) < 0)
                            {
                                printf("Gate EPOLL_CTL_DEL error\n");
                                return -1;
                            }
                            if (close(wait_event[i].data.fd) < 0)
                            {
                                printf("Gate Close fd error\n");
                                return -1;
                            }
                        }
                        pClient->pBuff.iSize += len;
                        if (pClient->pBuff.iSize >= pClient->pBuff.iMessageLen)
                        {
                            ProcessEvent(pClient);
                            pClient->pBuff.iMessageType = 0;
                            pClient->pBuff.iMessageLen = 0;
                            pClient->pBuff.iSize = 0;
                        }
                    }
                }
            }
        }
        }
    }
    return 0;
}