#pragma once
#include<iostream>
#include<cstring>
#include<vector>
#include<queue>
#include<list>
#include<unordered_map>

#include"PRAllDefine.h"

using namespace std;
struct GateClientMessageBuffer
{
    //head
    __uint8_t  iMessageType;
    __uint8_t  iMessageLen;
    //body
    __uint8_t  pMessage[MaxMessageLen];
    //已读字节<=1+4+iMessageLen
    __uint32_t iSize;
};
class GateClient
{
public:
    int fd;
    int iHeart;
    bool isLogin;
    bool isInGame;
    GateClientMessageBuffer pBuff;
    GateClient()
    {
        init();
    }    
    void init()
    {   
        fd = -1;
        iHeart = 0;
        isLogin = false;
        isInGame = false;
        memset(&pBuff,0,sizeof(GateClientMessageBuffer));
    }
};
struct ServerBuffer
{
    //head
    __uint8_t  iMessageType;
    __uint8_t iMessageLen;
    __uint32_t iTargetFd;
    //body
    __uint8_t  pMessage[MaxMessageLen];
    //已读字节<=1+4+iMessageLen
    __uint32_t iSize;
};
