#pragma once
#include <iostream>
#include <queue>
#include <unordered_map>

#include "PRGameDefine.h"
using namespace std;

struct Player
{
    int fd;
    int PlayerID;
    int HeroId;
    int SkinID;
    int UserID;
    bool isKeepTCP;
    bool isLive;
    bool isInRoom;
    bool isLoaded;
    string UserName;
};

struct FrameMessageNode
{
    unsigned char *pMessage;
    unsigned char len;
    FrameMessageNode(){
        pMessage = new unsigned char[MaxMessageLen];
        len = 0;
    }
    ~FrameMessageNode(){
        if(pMessage!=NULL)
        {
            delete pMessage;
            pMessage = NULL;
        }
    }
};

//未加锁的单例
const int InitMessageNodePoolSize = 4e5;
class FrameMessageNodeManager
{
public:  
    static FrameMessageNodeManager* getInstance();
private:
    static FrameMessageNodeManager* Instance;

public:
    FrameMessageNodeManager()
    {
        for(int i=0;i<InitMessageNodePoolSize;i++)
        {
            m_qBufferObjectPool.push(new FrameMessageNode());
        }
    }
    ~FrameMessageNodeManager()
    {
        while(!m_qBufferObjectPool.empty())
        {
            FrameMessageNode *object = m_qBufferObjectPool.front();
            m_qBufferObjectPool.pop();
            delete object;
        }
    }
    //回收
    void pushBufferObjectPool(FrameMessageNode* pNode)
    { 
        pNode->len = 0;
        m_qBufferObjectPool.push(pNode); 
    }
    //分配
    FrameMessageNode* popBufferObjectPool()
    { 
        FrameMessageNode* object = NULL;
        if(!m_qBufferObjectPool.empty())
        {
            object = m_qBufferObjectPool.front();
            m_qBufferObjectPool.pop();
        }
        else
        {
            return new FrameMessageNode();
        }
        return object; 
    }

private:
    std::queue<FrameMessageNode*> m_qBufferObjectPool;
};
FrameMessageNodeManager* FrameMessageNodeManager::Instance = NULL;
FrameMessageNodeManager* FrameMessageNodeManager::getInstance()
{
    if(Instance == NULL)
    {
        Instance = new FrameMessageNodeManager();
    }
    return Instance;
}

#define FRAMEMESSAGENODEINSTANCE FrameMessageNodeManager::getInstance()


//一个房间玩家数量
const int GamePlayerCount = 4;


class GameRoom
{
public:
    GameRoom()
    { 
        init();
    }

    ~GameRoom()
    {
        while(!m_qMessage.empty())
        {
            FRAMEMESSAGENODEINSTANCE->pushBufferObjectPool(m_qMessage.front());
            m_qMessage.pop();
        }
    }
    void init()
    {
        for(int i=0;i<GamePlayerCount;i++)
        {
            m_vPlayer[i].isInRoom = false;
            m_vPlayer[i].isLoaded = false;
            m_vPlayer[i].isLive = true;
        }
        m_iPlayerCount = 0;
        p_iFrameCount = 0;
        isBegin = false;
        m_bAllLoaded = false;
        bSendAllLoaded = false;
        memset(MonsterLive,false,sizeof(MonsterLive));
    }
    void addMessage(FrameMessageNode* node){ m_qMessage.push(node); }
    queue<FrameMessageNode*>* getFrameMessageQueue(){ return &m_qMessage; }
    bool addPlayer(Player &player)
    {
        if(m_iPlayerCount>=GamePlayerCount)
        { 
            printf("renmanle %d\n",m_iPlayerCount);
            return false;
        }
        if(player.fd<0) return false;
        int playerID = 0;
        for(int i=0;i<GamePlayerCount;i++)
            if(m_vPlayer[i].isInRoom==false)
            {
                playerID = i;
                break;    
            }
        m_vPlayer[playerID].fd = player.fd;
        m_vPlayer[playerID].HeroId = player.HeroId;
        m_vPlayer[playerID].SkinID = player.SkinID;
        m_vPlayer[playerID].isKeepTCP = player.isKeepTCP;
        m_vPlayer[playerID].isLive = player.isLive;
        m_vPlayer[playerID].PlayerID = playerID + 1;//发给客户端1~4
        m_vPlayer[playerID].UserID = player.UserID;
        m_vPlayer[playerID].UserName = player.UserName;
        m_vPlayer[playerID].isInRoom = player.isInRoom;
        m_iPlayerCount++;
        return true;
    }
    
    void endGame()
    {
        for(int i=0;i<GamePlayerCount;i++)
        {
            while(!p_qPlayerMessage[i].empty())
            {
                FRAMEMESSAGENODEINSTANCE->pushBufferObjectPool(p_qPlayerMessage[i].front());
                p_qPlayerMessage[i].pop();
            }
            while(!p_qPlayerAllMessage[i].empty())
            {
                FRAMEMESSAGENODEINSTANCE->pushBufferObjectPool(p_qPlayerAllMessage[i].front());
                p_qPlayerAllMessage[i].pop();
            }
        }
        while (!m_qMessage.empty())
        {
            FRAMEMESSAGENODEINSTANCE->pushBufferObjectPool(m_qMessage.front());
            m_qMessage.pop();
        }
        
        init();
    }
    bool deletePlayer(int fd)
    {
        if(fd<0) return false;
        for(int i=0;i<GamePlayerCount;i++)
        {
            if(m_vPlayer[i].fd==fd)
            {
                m_vPlayer[i].isInRoom=false;
                m_vPlayer[i].isLive = false;
                m_iPlayerCount--;
                return true;
            }
        }
           return false;
    }
    void playerDead(int i)
    {
        m_vPlayer[i].isLive = false;
    }
    void NOTCP(int fd)
    {
        for(int i=0;i<GamePlayerCount;i++)
            if(m_vPlayer[i].fd==fd)
                m_vPlayer[i].isKeepTCP = false;
    }
    int getLivePlayer()
    {
        int liveCount = 0;
        for(int i=0;i<GamePlayerCount;i++)
            if( m_vPlayer[i].isLive)
                liveCount++;
        return liveCount;
    }
    int getPlayerCount(){ return m_iPlayerCount; }
    int getPlayerID(int fd)
    {
        for(int i=0;i<GamePlayerCount;i++)
            if(m_vPlayer[i].fd == fd)
                return m_vPlayer[i].PlayerID;//发给客户端1~4
        return -1; 
    }
    bool setLoaded(int fd)
    {
        for(int i=0;i<GamePlayerCount;i++)
            if(m_vPlayer[i].fd == fd)
            {
                m_vPlayer[i].isLoaded = true;
                return true;
            }
        return false;
    }
    bool isAllLoaded()
    {
        if(m_bAllLoaded) return true;
        for(int i=0;i<GamePlayerCount;i++)
            if(m_vPlayer[i].isLoaded==false)
                return false;
        m_bAllLoaded= true;
        return true;
    }
    Player* getPlayer(){ return m_vPlayer; }
    bool isBegin;
    bool bSendAllLoaded;
    //野怪
    bool MonsterLive[32];
    
    //玩家的操作队列
    queue<FrameMessageNode*> p_qPlayerMessage[GamePlayerCount];
    //玩家接收的所有消息
    queue<FrameMessageNode*> p_qPlayerAllMessage[GamePlayerCount];
    
    //第几帧
    int p_iFrameCount;

private:
    //4个玩家
    int m_iPlayerCount;
    Player m_vPlayer[GamePlayerCount];
    bool m_bAllLoaded;
    queue<FrameMessageNode*> m_qMessage;
};

/*
class GameRoomManager
{
public:  
    static GameRoomManager* getInstance();
private:
    static GameRoomManager* Instance;

public:
    GameRoomManager()
    {
       
    }
    ~GameRoomManager()
    {

    }
    void addGameRoom(GameRoom* gameRoom){ m_qGameRoom.push_back(gameRoom); }
    
private:
    vector<GameRoom*> m_qGameRoom;
};
GameRoomManager* GameRoomManager::Instance = NULL;
GameRoomManager* GameRoomManager::getInstance()
{
    if(Instance == NULL)
    {
        Instance = new GameRoomManager();
    }
    return Instance;
}
#define GAMEROOMINSTANCE FrameMessageNodeManager::getInstance()
*/