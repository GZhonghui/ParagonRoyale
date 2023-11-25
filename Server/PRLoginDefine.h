#pragma once

#include "PRAllDefine.h"

using namespace std;

struct ClientMessageBuffer{
    //head
    __uint8_t  iMessageType;
    __uint8_t  iMessageLen;
    //body
    __uint8_t  pMessage[1024];
    //已读字节<=1+4+iMessageLen
    __uint8_t iSize;
    ClientMessageBuffer()
    {
       init();
    }
    ~ClientMessageBuffer()
    {

    };
    void init()
    {
       memset(this,0,sizeof(ClientMessageBuffer));
    }
};

//未加锁的单例
const int InitBufferPoolSize = 1024;
class ClientBufferManager
{
public:  
    static ClientBufferManager* getInstance();
private:
    static ClientBufferManager* Instance;

public:
    ClientBufferManager()
    {
        for(int i=0;i<InitBufferPoolSize;i++)
        {
            m_qBufferObjectPool.push(new ClientMessageBuffer());
        }
    }
    ~ClientBufferManager()
    {
        while(!m_qBufferObjectPool.empty())
        {
            ClientMessageBuffer *object = m_qBufferObjectPool.front();
            m_qBufferObjectPool.pop();
            delete object;
        }
    }
    //玩家下线时回收
    void pushBufferObjectPool(ClientMessageBuffer* pBuff)
    { 
        pBuff->init();
        m_qBufferObjectPool.push(pBuff); 
    }
    //玩家上线时分配
    ClientMessageBuffer* popBufferObjectPool()
    { 
        ClientMessageBuffer* object = NULL;
        if(!m_qBufferObjectPool.empty())
        {
            object = m_qBufferObjectPool.front();
            m_qBufferObjectPool.pop();
        }
        else
        {
            return new ClientMessageBuffer();
        }
        return object; 
    }

private:
    std::queue<ClientMessageBuffer*> m_qBufferObjectPool;
};

ClientBufferManager* ClientBufferManager::Instance = NULL;
ClientBufferManager* ClientBufferManager::getInstance()
{
    if(Instance == NULL)
    {
        Instance = new ClientBufferManager();
    }
    return Instance;
}

class Client
{
public:
    int fd;
    ClientMessageBuffer *pBuff;
    int iUserID;
    bool isGame;
    bool isInRoom;
    Client()
    {
        fd = -1;
        pBuff = NULL;
        isGame = false;
        iUserID = 0;
        isInRoom = false;
    }    
};

class ClientsManager{
public:
    static ClientsManager* getInstance();
private:
    static ClientsManager* Instance;
public:
    ClientsManager(){ m_iClientsCount = 0; }
    Client* addOneClient(int fd)
    {
        if(fd==-1 || m_umapFd2Client.count(fd)) return NULL;

        Client *newClient = new Client;
        newClient->fd = fd;
        newClient->pBuff = ClientBufferManager::getInstance()->popBufferObjectPool();

        m_umapFd2Client[fd] = newClient;
        m_iClientsCount++;

        return newClient;
    }
    bool deleteOneClient(int fd)
    {
        if(fd==-1 || !m_umapFd2Client.count(fd)) return false;
        ClientBufferManager::getInstance()->pushBufferObjectPool(m_umapFd2Client[fd]->pBuff);
        delete m_umapFd2Client[fd];
        m_umapFd2Client.erase(fd);
        m_iClientsCount--;
        return true;
    }
    int getClientCount()
    {
        return m_iClientsCount;
    }
    Client *getClientByFd(int fd)
    {
        return m_umapFd2Client[fd];
    }
    
    unordered_map<int ,Client*>* getAllFd(){ return &m_umapFd2Client; }
    bool setUserID(int fd,int userID)
    {
        if(fd==-1 || m_umapFd2Client.count(fd)) return false;
        m_umapFd2Client[fd]->iUserID = userID;
        return true;
    }
    int getUserID(int fd)
    {
        if(fd==-1 || m_umapFd2Client.count(fd)) return -1;
        return m_umapFd2Client[fd]->iUserID;
    }
    bool BeginGame(int fd)
    {
        if(fd==-1 || m_umapFd2Client.count(fd) || m_umapFd2Client[fd]->isGame) return false;
        m_umapFd2Client[fd]->isGame = true;
        return true;
    }
    bool EndGame(int fd)
    {
        if(fd==-1 || m_umapFd2Client.count(fd) || m_umapFd2Client[fd]->isGame==false) return false;
        m_umapFd2Client[fd]->isGame = false;
        return true;
    }

private:
    int m_iClientsCount;
    unordered_map<int ,Client*> m_umapFd2Client;
};

ClientsManager* ClientsManager::Instance = NULL;
ClientsManager* ClientsManager::getInstance()
{
    if(Instance == NULL)
    {
        Instance = new  ClientsManager();
    }
    return Instance;
}

#define CLIENTSINSTANCE ClientsManager::getInstance()
