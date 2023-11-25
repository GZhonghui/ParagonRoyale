#pragma once
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
//消息最大长度
const int MaxMessageLen = 256;
//消息延迟帧数
const int MessageDelayTimes = 5;
//服务器同步版本
const int MessageIPv = 2;

const int iPort = 3020;
struct GateBuffer
{
    //head
    __uint8_t iMessageType;
    __uint8_t iMessageLen;
    __uint32_t iTargetFd;
    //body
    __uint8_t  pMessage[MaxMessageLen];
    //已读字节<=1+1+iMessageLen
    __uint32_t iSize;
};

// Request 请求类型，都是从客户端发出的
// Respond 回应类型，都是从服务器发出的
enum MessageType
{
    LoginMessageTypeBgein = 1,
    // 注册
    RegisterRequest,
    RegisterRespond,
    // 登录
    LoginRequest,
    LoginRespond,
    LoginMessageTypeEnd,

    GameMessageTypeBegin,
    // 匹配
    MatchRequest,
    CancelMatchRequest, // 没有数据
    MatchFoundRespond,
    MapLoadedRequest, // 没有数据
    AllPlayerLoadedRespond, // 没有数据
    // 游戏
    PlayerMoveRequest,
    PlayerMoveRespond,
    PlayerTurnRequest,
    PlayerTurnRespond,
    PlayerHealthRequest,
    PlayerHealthRespond,
    PlayerActionRequest,
    PlayerActionRespond,
    PlayerBuffRequest,
    PlayerBuffRespond,
    MonsterKilledRequest,
    MonsterKilledRespond,
    KilledRequest,
    KilledRespond,
    QuitRequest, // 没有数据
    // 帧
    ClientStep,
    ServerStep,
    // 驱动帧
    NextFrame,
    //重新链接
    Reconnect,
    GameMessageTypeEnd,

    
    // 心跳检测
    Heart, // 没有数据
    //标明身份
    ThisIsLoginServer,
    ThisIsGameServer,

    // 玩家数据
    GetPlayerInfoRequest, // 没有数据
    GetPlayerInfoRespond, // 后面跟一个PPlayerInfo，表示从服务器返回的数值
    SetPlayerInfoRequest, // 后面跟一个PPlayerInfo，更新给服务器
};