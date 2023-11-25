#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <queue>
#include <list>

#include "GameMessage.pb.h"
#include "PRGameDefine.h"
#include "PRRoomClass.h"

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

const int MaxRoomCount = 512;
const int MaxEvents = 10;
GateBuffer gateBuffer;
uint8_t SendBuffer[512];

int GateFd = -1, DBFd = -1;

GameRoom gameRoom[MaxRoomCount];
queue<int> qIdleRoom;
list<int> listInGameRoom;
int iWaitRoom;
unordered_map<int,int> fdToGameRoom;
unordered_map<int,int> fdToClientHeart;
unordered_map<int,int> fdState;//0不在，1匹配中，2游戏中
void init()
{
    for(int i=1;i<MaxRoomCount;i++)
        qIdleRoom.push(i);
}
void BeginGame()
{
    if(gameRoom[iWaitRoom].getPlayerCount()<GamePlayerCount) return;
    if(gameRoom[iWaitRoom].isBegin)
    {
        std::cout<<"出错\n";
        return;
    }

    PMatchFoundRespond matchFoundRespond;
    Player* players = gameRoom[iWaitRoom].getPlayer();
    for(int i=0;i<GamePlayerCount;i++)
    {
        PMatchFoundRespond::POtherPlayerInfo* playerInfo = matchFoundRespond.add_otherplayers();
        playerInfo->set_playerid(players[i].PlayerID);
        playerInfo->set_heroid(players[i].HeroId);
        playerInfo->set_playername(players[i].UserName);
        playerInfo->set_skinid(players[i].SkinID);
        fdState[players[i].fd] = 2;
    }
    //随机数种子
    matchFoundRespond.set_randomseed(rand()%100);

    FrameMessageNode *pMessageNode = FRAMEMESSAGENODEINSTANCE->popBufferObjectPool();
    uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::MatchFoundRespond);
    uint8_t SingleMessageLen = static_cast<uint8_t>(matchFoundRespond.ByteSizeLong());
    memcpy(pMessageNode->pMessage + 0, &SingleMessageType, 1);
    memcpy(pMessageNode->pMessage + 1, &SingleMessageLen, 1);
    matchFoundRespond.SerializeToArray(pMessageNode->pMessage + 2, SingleMessageLen);
    pMessageNode->len = SingleMessageLen + 2;
    gameRoom[iWaitRoom].addMessage(pMessageNode);

    gameRoom[iWaitRoom].isBegin = true;
    listInGameRoom.push_back(iWaitRoom);
    printf("room %d Begin\n",iWaitRoom);
    iWaitRoom = -1;
    if(!qIdleRoom.empty())
    {
        iWaitRoom = qIdleRoom.front();
        qIdleRoom.pop();
        gameRoom[iWaitRoom].init();
    }    
}

inline void MatchRoom(Player &player)
{
    if(iWaitRoom==-1)
    {
        printf("房间满了\n");
        return;
    }
    //房间没满且客户端不在房间里面
    if(gameRoom[iWaitRoom].getPlayerCount()<GamePlayerCount&&(!fdState.count(player.fd) || fdState[player.fd]==0))
    {
        gameRoom[iWaitRoom].addPlayer(player);
        fdToGameRoom[player.fd] = iWaitRoom;
        fdState[player.fd]=1;

        printf("%d add %d player count:%d\n",iWaitRoom,player.fd,gameRoom[iWaitRoom].getPlayerCount());
        BeginGame();
    }
}
//回收所有结束游戏了的房间号
inline void RecycleGameRoom()
{
    for(list<int>::iterator it = listInGameRoom.begin();it != listInGameRoom.end();)
    {
        if(gameRoom[*it].getLivePlayer()==0)
        {   
            Player* players = gameRoom[*it].getPlayer();
            for(int i=0;i<GamePlayerCount;i++)
                fdState[players[i].fd] = 0;
            gameRoom[*it].endGame();
            qIdleRoom.push(*it);
            gameRoom[*it].init();
            it = listInGameRoom.erase(it);
        }
        else
            it++;
    }
}

bool sendAMessageToAll(int roomID, FrameMessageNode* messageNode)
{
    Player *players = gameRoom[roomID].getPlayer();
    
    for(int i=0;i<GamePlayerCount;i++)
    {
        if(players[i].fd<0||!players[i].isKeepTCP||!players[i].isInRoom) 
            continue;
        memcpy(SendBuffer + 0, &players[i].fd, 4);
        memcpy(SendBuffer + 4, messageNode->pMessage, messageNode->len);
        uint32_t sendLen = 0, x = 0;
        uint32_t sumLen = (int)messageNode->len + 4;
        while(sendLen < sumLen)
        {
            x = send(GateFd, SendBuffer + sendLen, sumLen - sendLen, 0);
            if(x < 0)
            {
                std::cout<<"send 错误room\n";
                return false; 
            }
            sendLen += x;
        }
        
        //printf("send to %d\n",players[i].fd);
    }
    return true;
}

inline void SendAllFrameMessages()
{
    
    for(list<int>::iterator it = listInGameRoom.begin();it != listInGameRoom.end();it++)
    {
        
        if(MessageIPv == 1)
        {
            //先发驱动帧再发其他
            FrameMessageNode* messagenode = FRAMEMESSAGENODEINSTANCE->popBufferObjectPool();
            uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::NextFrame);
            uint8_t SingleMessageLen = static_cast<uint8_t>(0);
            memcpy(messagenode->pMessage + 0, &SingleMessageType, 1);
            memcpy(messagenode->pMessage + 1, &SingleMessageLen, 1);
            messagenode->len = 2;
            sendAMessageToAll(*it,messagenode);
            FRAMEMESSAGENODEINSTANCE->pushBufferObjectPool(messagenode);
        }

        queue<FrameMessageNode*>* messqueue = gameRoom[*it].getFrameMessageQueue();
        while(!messqueue->empty())
        {
            
            FrameMessageNode* messagenode = FRAMEMESSAGENODEINSTANCE->popBufferObjectPool();
            messagenode = messqueue->front();
            messqueue->pop();
            sendAMessageToAll(*it,messagenode);
            FRAMEMESSAGENODEINSTANCE->pushBufferObjectPool(messagenode);
        }

        //地图加载完毕后发这个
        if(gameRoom[*it].bSendAllLoaded==false)
        {
            if(gameRoom[*it].isAllLoaded())
            {
                FrameMessageNode* messagenode = FRAMEMESSAGENODEINSTANCE->popBufferObjectPool();
                uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::AllPlayerLoadedRespond);
                uint8_t SingleMessageLen = static_cast<uint8_t>(0);
                memcpy(messagenode->pMessage + 0, &SingleMessageType, 1);
                memcpy(messagenode->pMessage + 1, &SingleMessageLen, 1);
                messagenode->len = 2;
                sendAMessageToAll(*it,messagenode);
                printf("the room loaded\n");
                FRAMEMESSAGENODEINSTANCE->pushBufferObjectPool(messagenode);
                gameRoom[*it].bSendAllLoaded = true;
            }
        }
        
        if(gameRoom[*it].bSendAllLoaded&&MessageIPv == 2)
        {
 
            if(gameRoom[*it].p_iFrameCount>=MessageDelayTimes)
            {
                Player* players = gameRoom[*it].getPlayer();
                PServerStep ServerStep;
                ServerStep.set_stepindex(gameRoom[*it].p_iFrameCount);
                for(int i=0;i<GamePlayerCount;i++)
                {
                    PServerStep::Client* AClient = ServerStep.add_allclients();
                    AClient->set_playerid(players[i].PlayerID);
                    if(!gameRoom[*it].p_qPlayerMessage[i].empty())
                    {
                        FrameMessageNode* AMessageNode =  gameRoom[*it].p_qPlayerMessage[i].front();
                        gameRoom[*it].p_qPlayerMessage[i].pop();
                        PClientStep *clientStep = new PClientStep();
                        clientStep->ParseFromArray(AMessageNode->pMessage,AMessageNode->len);
                        AClient->set_allocated_step(clientStep);
                        FRAMEMESSAGENODEINSTANCE->pushBufferObjectPool(AMessageNode);
                        AClient->set_isempty(0);
                    }
                    else
                    {
                        AClient->set_isempty(1);
                    }
                }
                FrameMessageNode* messagenode = FRAMEMESSAGENODEINSTANCE->popBufferObjectPool();
                uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::ServerStep);
                uint8_t SingleMessageLen = static_cast<uint8_t>(ServerStep.ByteSizeLong());
                memcpy(messagenode->pMessage + 0, &SingleMessageType, 1);
                memcpy(messagenode->pMessage + 1, &SingleMessageLen, 1);
                ServerStep.SerializeToArray(messagenode->pMessage + 2, SingleMessageLen);
                messagenode->len = SingleMessageLen + 2;
                sendAMessageToAll(*it,messagenode);
                FRAMEMESSAGENODEINSTANCE->pushBufferObjectPool(messagenode);
            }
            
            gameRoom[*it].p_iFrameCount++;
            
            //printf("frame:---%d\n",gameRoom[*it].p_iFrameCount);
        }
    }
}

inline void DealMessage()
{
    switch(static_cast<MessageType>(gateBuffer.iMessageType))
    {
    case MessageType::MatchRequest:
    {
        PMatchRequest matchRequst;
        matchRequst.ParseFromArray(gateBuffer.pMessage, gateBuffer.iMessageLen);
        Player player;
        player.fd = gateBuffer.iTargetFd;
        player.HeroId = matchRequst.heroid();
        player.SkinID = matchRequst.skinid();
        player.UserName = matchRequst.username();
        //player.UserID = pClient->iUserID;
        player.isLive = true;
        player.isKeepTCP = true;
        player.isInRoom = true;
        MatchRoom(player);
        
        printf("匹配 (%d)\n", gateBuffer.iTargetFd);
    }break;
    case MessageType::CancelMatchRequest:
    {
        //取消匹配
        if(fdToGameRoom.count(gateBuffer.iTargetFd) && fdState.count(gateBuffer.iTargetFd) &&fdState[gateBuffer.iTargetFd] == 1)
        {
            gameRoom[fdToGameRoom[gateBuffer.iTargetFd]].deletePlayer(gateBuffer.iTargetFd);
            fdState[gateBuffer.iTargetFd] = 0;
            printf("取消匹配 (%d)\n", gateBuffer.iTargetFd);
        }
    }break;
    case MessageType::MapLoadedRequest:
    {   
        //加载完毕
        if(fdToGameRoom.count(gateBuffer.iTargetFd)&& fdState.count(gateBuffer.iTargetFd) && fdState[gateBuffer.iTargetFd] == 2)
        {
            if(gameRoom[fdToGameRoom[gateBuffer.iTargetFd]].setLoaded(gateBuffer.iTargetFd))
                printf("Loaded %d\n",gateBuffer.iTargetFd);
        }
       
    }break;
    case MessageType::PlayerMoveRequest:
    {
        PPlayerMoveRequest playerMoveRequest;
        playerMoveRequest.ParseFromArray(gateBuffer.pMessage, gateBuffer.iMessageLen);
        int roomID = fdToGameRoom[gateBuffer.iTargetFd];
        int playerID = gameRoom[roomID].getPlayerID(gateBuffer.iTargetFd);
        if(playerID <= 0)
        {
           cout<<"错误\n";
           return;
        }
        PPlayerMoveRespond playerMoveRespond;
        playerMoveRespond.set_playerid(playerID);
        playerMoveRespond.set_x(playerMoveRequest.x());
        playerMoveRespond.set_y(playerMoveRequest.y());
        //printf("%d--Player Pos:%.4f,%.4f\n",playerID,playerMoveRequest.x(),playerMoveRequest.y());
        uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::PlayerMoveRespond);
        uint8_t SingleMessageLen = static_cast<uint8_t>(playerMoveRespond.ByteSizeLong());
        
        FrameMessageNode *pMessageNode = FRAMEMESSAGENODEINSTANCE->popBufferObjectPool();
        memcpy(pMessageNode->pMessage + 0, &SingleMessageType, 1);
        memcpy(pMessageNode->pMessage + 1, &SingleMessageLen, 1);
        playerMoveRespond.SerializeToArray(pMessageNode->pMessage + 2, SingleMessageLen);
        pMessageNode->len = SingleMessageLen + 2;
        gameRoom[roomID].addMessage(pMessageNode);
    }break;
    case MessageType::PlayerTurnRequest:
    {
        PPlayerTurnRequest playerTurnRequest;
        playerTurnRequest.ParseFromArray(gateBuffer.pMessage, gateBuffer.iMessageLen);
        int roomID = fdToGameRoom[gateBuffer.iTargetFd];
        int playerID = gameRoom[roomID].getPlayerID(gateBuffer.iTargetFd);
        if(playerID <= 0)
        {
           cout<<"错误\n";
           return;
        }
        PPlayerTurnRespond playerTurnRespond;
        playerTurnRespond.set_playerid(playerID);
        playerTurnRespond.set_x(playerTurnRequest.x());
        playerTurnRespond.set_y(playerTurnRequest.y());
        
        uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::PlayerTurnRespond);
        uint8_t SingleMessageLen = static_cast<uint8_t>(playerTurnRespond.ByteSizeLong());
        printf("---------%.2f,%.2f\n",playerTurnRequest.x(),playerTurnRequest.y());
        FrameMessageNode *pMessageNode = FRAMEMESSAGENODEINSTANCE->popBufferObjectPool();
        memcpy(pMessageNode->pMessage + 0, &SingleMessageType, 1);
        memcpy(pMessageNode->pMessage + 1, &SingleMessageLen, 1);
        playerTurnRespond.SerializeToArray(pMessageNode->pMessage + 2, SingleMessageLen);
        pMessageNode->len = SingleMessageLen + 2;
        gameRoom[roomID].addMessage(pMessageNode);
    }break;
    case MessageType::PlayerHealthRequest:
    {
        PPlayerHealthRequest playerHealthRequest;
        playerHealthRequest.ParseFromArray(gateBuffer.pMessage, gateBuffer.iMessageLen);
        int roomID = fdToGameRoom[gateBuffer.iTargetFd];
        int playerID = gameRoom[roomID].getPlayerID(gateBuffer.iTargetFd);
        if(playerID < 0)
        {
           cout<<"错误\n";
           return;
        }
        PPlayerHealthRespond playerHealthRespond;
        playerHealthRespond.set_playerid(playerID);
        playerHealthRespond.set_healthmax(playerHealthRequest.healthmax());
        playerHealthRespond.set_healthremain(playerHealthRequest.healthremain());
        
        uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::PlayerHealthRespond);
        uint8_t SingleMessageLen = static_cast<uint8_t>(playerHealthRespond.ByteSizeLong());

        FrameMessageNode *pMessageNode = FRAMEMESSAGENODEINSTANCE->popBufferObjectPool();
        memcpy(pMessageNode->pMessage + 0, &SingleMessageType, 1);
        memcpy(pMessageNode->pMessage + 1, &SingleMessageLen, 1);
        playerHealthRespond.SerializeToArray(pMessageNode->pMessage + 2, SingleMessageLen);
        pMessageNode->len = SingleMessageLen + 2;

        gameRoom[roomID].addMessage(pMessageNode);
    }break;
    case MessageType::PlayerActionRequest:
    {
        PPlayerActionRequest playerActionRequest;
        playerActionRequest.ParseFromArray(gateBuffer.pMessage, gateBuffer.iMessageLen);
        int roomID = fdToGameRoom[gateBuffer.iTargetFd];
        int playerID = gameRoom[roomID].getPlayerID(gateBuffer.iTargetFd);
        if(playerID < 0)
        {
           cout<<"错误\n";
           return;
        }
        PPlayerActionRespond playerActionRespond;
        playerActionRespond.set_playerid(playerID);
        playerActionRespond.set_locationx(playerActionRequest.locationx());
        playerActionRespond.set_locationy(playerActionRequest.locationy());
        playerActionRespond.set_targetx(playerActionRequest.targetx());
        playerActionRespond.set_targety(playerActionRequest.targety());
        playerActionRespond.set_actionid(playerActionRequest.actionid());

        uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::PlayerActionRespond);
        uint8_t SingleMessageLen = static_cast<uint8_t>(playerActionRespond.ByteSizeLong());
        
        FrameMessageNode *pMessageNode = FRAMEMESSAGENODEINSTANCE->popBufferObjectPool();
        memcpy(pMessageNode->pMessage + 0, &SingleMessageType, 1);
        memcpy(pMessageNode->pMessage + 1, &SingleMessageLen, 1);
        playerActionRespond.SerializeToArray(pMessageNode->pMessage + 2, SingleMessageLen);
        pMessageNode->len = SingleMessageLen + 2;

        gameRoom[roomID].addMessage(pMessageNode);
    }break;
    case MessageType::PlayerBuffRequest:
    {
        PPlayerBuffRequest playerBuffRequest;
        playerBuffRequest.ParseFromArray(gateBuffer.pMessage, gateBuffer.iMessageLen);
        int roomID = fdToGameRoom[gateBuffer.iTargetFd];
        int playerID = gameRoom[roomID].getPlayerID(gateBuffer.iTargetFd);
        if(playerID < 0)
        {
           cout<<"错误\n";
           return;
        }
        PPlayerBuffRespond playerBuffRespond;
        playerBuffRespond.set_playerid(playerID);
        playerBuffRespond.set_scale(playerBuffRequest.scale());

        uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::PlayerBuffRespond);
        uint8_t SingleMessageLen = static_cast<uint8_t>(playerBuffRespond.ByteSizeLong());
        
        FrameMessageNode *pMessageNode = FRAMEMESSAGENODEINSTANCE->popBufferObjectPool();
        memcpy(pMessageNode->pMessage + 0, &SingleMessageType, 1);
        memcpy(pMessageNode->pMessage + 1, &SingleMessageLen, 1);
        playerBuffRespond.SerializeToArray(pMessageNode->pMessage + 2, SingleMessageLen);
        pMessageNode->len = SingleMessageLen + 2;
        
        gameRoom[roomID].addMessage(pMessageNode);

    }break;
    case MessageType::MonsterKilledRequest:
    {
        PMonsterKilledRequest monsterKilledRequest;
        monsterKilledRequest.ParseFromArray(gateBuffer.pMessage, gateBuffer.iMessageLen);
        int roomID = fdToGameRoom[gateBuffer.iTargetFd];
        int playerID = gameRoom[roomID].getPlayerID(gateBuffer.iTargetFd);
        if(playerID < 0)
        {
           cout<<"错误\n";
           return;
        }
        //判重
        if(gameRoom[roomID].MonsterLive[monsterKilledRequest.monsterid()])
        {
            return;
        }
        gameRoom[roomID].MonsterLive[monsterKilledRequest.monsterid()]=true;
        PMonsterKilledRespond monsterKilledRespond;
        monsterKilledRespond.set_playerid(playerID);
        monsterKilledRespond.set_monsterid(monsterKilledRequest.monsterid());

        uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::MonsterKilledRespond);
        uint8_t SingleMessageLen = static_cast<uint8_t>(monsterKilledRespond.ByteSizeLong());
        
        FrameMessageNode *pMessageNode = FRAMEMESSAGENODEINSTANCE->popBufferObjectPool();
        memcpy(pMessageNode->pMessage + 0, &SingleMessageType, 1);
        memcpy(pMessageNode->pMessage + 1, &SingleMessageLen, 1);
        monsterKilledRespond.SerializeToArray(pMessageNode->pMessage + 2, SingleMessageLen);
        pMessageNode->len = SingleMessageLen + 2;
        
        gameRoom[roomID].addMessage(pMessageNode);

    }break;
    case MessageType::KilledRequest:
    {
        PKilledRequest killedRequest;
        killedRequest.ParseFromArray(gateBuffer.pMessage, gateBuffer.iMessageLen);
        int roomID = fdToGameRoom[gateBuffer.iTargetFd];
        int playerID = gameRoom[roomID].getPlayerID(gateBuffer.iTargetFd);
        if(playerID < 0)
        {
           cout<<"错误\n";
           return;
        }
        PKilledRespond killedRespond;
        killedRespond.set_diedplayerid(playerID);
        killedRespond.set_killerid(killedRequest.otherplayerid());
        killedRespond.set_rank(gameRoom[roomID].getPlayerCount());
        gameRoom[roomID].playerDead(playerID);

        uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::KilledRespond);
        uint8_t SingleMessageLen = static_cast<uint8_t>(killedRespond.ByteSizeLong());
        
        FrameMessageNode *pMessageNode = FRAMEMESSAGENODEINSTANCE->popBufferObjectPool();
        memcpy(pMessageNode->pMessage + 0, &SingleMessageType, 1);
        memcpy(pMessageNode->pMessage + 1, &SingleMessageLen, 1);
        killedRespond.SerializeToArray(pMessageNode->pMessage + 2, SingleMessageLen);
        pMessageNode->len = SingleMessageLen + 2;
        
        gameRoom[roomID].addMessage(pMessageNode);
        if(gameRoom[roomID].getLivePlayer()==1)
        {
            gameRoom[roomID].endGame();
        }

    }break;
    case MessageType::QuitRequest:
    {
        if(fdToGameRoom.count(gateBuffer.iTargetFd))
        {
            int roomID = fdToGameRoom[gateBuffer.iTargetFd];
            gameRoom[roomID].deletePlayer(gateBuffer.iTargetFd);
            fdState[gateBuffer.iTargetFd] = 0;
        }
    }break;
    case MessageType::Heart:
    {
        fdToClientHeart[gateBuffer.iTargetFd] = 2;
    }break;
    case MessageType::ClientStep:
    {   
        PClientStep clientStep;
        clientStep.ParseFromArray(gateBuffer.pMessage, gateBuffer.iMessageLen);
        int roomID = fdToGameRoom[gateBuffer.iTargetFd];
        int playerID = gameRoom[roomID].getPlayerID(gateBuffer.iTargetFd);
        if(playerID <= 0)
        {
           cout<<"错误\n";
           return;
        }
       
        int nowFarme = gameRoom[roomID].p_iFrameCount;
        if( nowFarme - MessageDelayTimes < clientStep.stepindex())
        {
            FrameMessageNode *pMessageNode = FRAMEMESSAGENODEINSTANCE->popBufferObjectPool();
            pMessageNode->len = static_cast<uint8_t>(clientStep.ByteSizeLong());
            clientStep.SerializeToArray(pMessageNode->pMessage, pMessageNode->len);
            gameRoom[roomID].p_qPlayerMessage[playerID - 1].push(pMessageNode);
        }else
        {
        }

    }break;
    case MessageType::Reconnect:
    {

    }break;
    default:
    {
        printf("Error: Message Type: %u\n", gateBuffer.iMessageType);
    }break;
    }
    
}

inline void CallGateThisGameServer(int sockfd)
{
    uint8_t SingleMessageType = static_cast<uint8_t>(MessageType::ThisIsGameServer);
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
            cout<<"Game: send 错误\n";
            return;
        }
        sendLen += x;
    }
}
int main()
{
    srand(time(0));
    init();
    
    printf("Game Server Start\n");
    struct epoll_event event;
    struct epoll_event wait_event[MaxEvents];

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("error in creating socket\n");
        return -1;
    }
    int on = 1;
    //开启TCP_NODELAY
    if(setsockopt(sockfd, IPPROTO_TCP,TCP_NODELAY, &on, sizeof(on))<0)
    {
        printf("error in set TCP_NODELAY\n");
        return -1;
    }
    struct sockaddr_in my_addr;
    struct hostent* h;
    bzero(&my_addr, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(iPort);
    if( (h = gethostbyname("10.0.128.199")) == 0)
	{
		printf("gethostbyname failed,\n");
		close(sockfd);
        return -1;
	}
    memcpy(&my_addr.sin_addr, h->h_addr, h->h_length);
    if(connect(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) != 0)
	{
		perror("connect");
		close(sockfd);
        return -1;
	}
    printf("Game Server Content Gate Successful\n");
    //告诉网关这是游戏服务器
    CallGateThisGameServer(sockfd);
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
    long long heartCount = 0, frameCount = 0;
    long long beginTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    while(true)
    {
        ret = epoll_wait(epfd, wait_event, MaxEvents, 1);
        if(ret<0)
        {
            printf("epoll_wait error\n");
            return -1;
        }
        gettimeofday(&tv, NULL);
       
        if ((tv.tv_sec * 1000 + tv.tv_usec / 1000 - beginTime)/30 > frameCount)
        {
            frameCount++;
            //推进游戏帧
            SendAllFrameMessages();
            //回收结束游戏了的房间号
            RecycleGameRoom();
        }

        //连续两次心跳都没检测到的视为掉线
        if((tv.tv_sec * 1000 + tv.tv_usec / 1000 - beginTime)/5000 > heartCount)
        {
            //这种时钟会累积误差
            //lastHeartCheckTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
            heartCount++;
            //cout<<iWaitRoom<<"房间人数："<<gameRoom[iWaitRoom].getPlayerCount()<<"\n";
            for(auto x : fdToClientHeart)
            {
                if(fdToClientHeart[x.first]>=0)
                    fdToClientHeart[x.first]--;
                if(fdToClientHeart[x.first]<0&&fdToGameRoom[x.first]>=0)
                {
                    fdState[x.first] = 0;
                    gameRoom[fdToGameRoom[x.first]].NOTCP(x.first);
                    //if(gameRoom[fdToGameRoom[x.first]].isBegin==false)
                    //{
                    gameRoom[fdToGameRoom[x.first]].deletePlayer(x.first);
                    fdToGameRoom.erase(x.first);
                    //}
                    fdToGameRoom[x.first] = -1;
                }
            }
        }

        for(int i=0; i<ret; i+=1)
        {
            const epoll_event& e = wait_event[i];

            if((GateFd == e.data.fd) && (e.events & EPOLLIN))
            {
                int len = 0;
                if (!gateBuffer.iMessageType || !gateBuffer.iMessageLen || !gateBuffer.iTargetFd)
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

    return 0;
}