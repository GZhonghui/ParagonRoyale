// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <queue>
#include <chrono>

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"

#include "Sockets.h"
#include "IPAddress.h"
#include "SocketTypes.h"
#include "SocketSubsystem.h"

// Remember This:
// #pragma warning(disable:4125)
#include "../PB/GameMessage.pb.h"

// Unreal C++ VS Protobuf
#undef  check
#define check(expr) UE_CHECK_IMPL(expr)

#include "MainGameInstance.generated.h"

// Could be faster
#define GI Cast<UMainGameInstance>(GetGameInstance())

inline float F(int X)
{
    return X / 255.999f;
};

inline void Print(const FString& Content)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, *Content);
	}
}

inline void PrintBytes(uint8_t* Buffer, int Count)
{
    for (int i = 0; i < Count; i += 1)
    {
        Print(FString::Printf(TEXT("Recv : %d"), (int8_t) * (Buffer + i)));
    }
}

USTRUCT()
struct FClientStep
{
    GENERATED_BODY()

    FClientStep() {}
    FClientStep(const PClientStep& Step)
    {
        StepBuffer.SetNum(Step.ByteSizeLong());
        Step.SerializeToArray(StepBuffer.GetData(), Step.ByteSizeLong());
    }

    TArray<uint8_t> StepBuffer;
};

// Single
DECLARE_DELEGATE(ServerMessageSingleDelegate)
DECLARE_DELEGATE_TwoParams(ServerMessageTwoIntSingleDelegate, int, int)

// Multi
DECLARE_MULTICAST_DELEGATE_ThreeParams(ServerMessageOneIntTwoFloatMultiDelegate, int, float, float)

// Multi, Not Server Message
DECLARE_MULTICAST_DELEGATE_OneParam(FrameNotice, int) // Not in use
DECLARE_MULTICAST_DELEGATE_ThreeParams(ClientStepNotice, int, int, FClientStep) // PlayerID, IsEmpty

/*
enum MType
{
    RegisterRequest = 1,
    RegisterRespond,

    LoginRequest,
    LoginRespond,

    MatchRequest,
    CancelMatchRequest,
    MatchFoundRespond,
    MapLoadedRequest,
    AllPlayerLoadedRespond,

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
    QuitRequest,

    ClientStep,
    ServerStep,

    Heart,

    NextFrame,

    GetPlayerInfoRequest,
    GetPlayerInfoRespond,
    SetPlayerInfoRequest
};
*/

// Request 请求类型，都是从客户端发出的
// Respond 回应类型，都是从服务器发出的
enum MType
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

class SingleMessageData
{
public:
	uint8_t MessageType;
	uint8_t MessageLen;

	TArray<uint8_t> Buffer;
};

class NetworkSendThread : public FRunnable
{
public:
    NetworkSendThread() = default;
    NetworkSendThread(FSocket* Server) : Server(Server) {}
    virtual ~NetworkSendThread() = default;

private:
    FSocket* Server;

    std::queue<SingleMessageData> WillSend;
    FCriticalSection QueueMutex;

protected:
    FCriticalSection stop_mutex;
    bool _should_stop;
    bool ShouldStop(bool flag = false)
    {
        FScopeLock Lock(&stop_mutex);

        if (flag) _should_stop = true;
        return _should_stop;
    }

public:
    virtual uint32 Run() override
    {
        uint8_t Buffer[40960];
        uint8_t Index = 0, Size = 0;

        while (true)
        {
            if (!ShouldStop())
            {
                if (!Server) break;

                if (Size)
                {
                    int Sended;
                    Server->Send(Buffer + Index, Size - Index, Sended);
                    Index += Sended;
                    if (Index >= Size)
                    {
                        Index = Size = 0;

                        // Print(FString::Printf(TEXT("Send a %u"), *Buffer));
                    }
                }
                else
                {
                    FScopeLock Lock(&QueueMutex);

                    if (!WillSend.empty())
                    {
                        const SingleMessageData& Message = WillSend.front();
                        memcpy(Buffer + 0, &Message.MessageType, 1);
                        memcpy(Buffer + 1, &Message.MessageLen, 1);
                        if (Message.MessageLen)
                        {
                            memcpy(Buffer + 2, Message.Buffer.GetData(), Message.MessageLen);
                        }

                        Index = 0;
                        Size = 2 + Message.MessageLen;

                        WillSend.pop();
                    }
                }
            }
            else break;
        }
        return (uint32_t)0;
    }

    virtual bool Init() override
    {
        _should_stop = false;
        return true;
    }

    virtual void Stop() override
    {
        ShouldStop(true);
    }

    virtual void Exit() override
    {

    }

public:
    void AddMessage(const SingleMessageData& Message)
    {
        FScopeLock Lock(&QueueMutex);

        WillSend.push(Message);
    }
};

class NetworkRecvThread : public FRunnable
{
public:
    NetworkRecvThread() = default;
    NetworkRecvThread(FSocket* Server) : Server(Server) {}
    virtual ~NetworkRecvThread() = default;

private:
    FSocket* Server;

    std::queue<SingleMessageData> HaveRecved;
    FCriticalSection QueueMutex;

protected:
    FCriticalSection stop_mutex;
    bool _should_stop;
    bool ShouldStop(bool flag = false)
    {
        FScopeLock Lock(&stop_mutex);

        if (flag) _should_stop = true;
        return _should_stop;
    }

public:
    virtual uint32 Run() override
    {
        uint8_t Buffer[40960];
        uint8_t Size = 0;

        uint8_t MessageType = 0, MessageLen = 0;

        while (true)
        {
            if (!ShouldStop())
            {
                if (!Server) break;

                if (!MessageType || !MessageLen)
                {
                    int Readed;
                    Server->Recv(Buffer + Size, 2 - Size, Readed);
                    // PrintBytes(Buffer + Size, Readed);

                    Size += Readed;
                    if (Size >= 2)
                    {
                        memcpy(&MessageType, Buffer + 0, 1);
                        memcpy(&MessageLen, Buffer + 1, 1);

                        Size = 0;

                        if (!MessageLen) // No Data Message
                        {
                            SingleMessageData ComplateMessage;
                            ComplateMessage.MessageType = MessageType;
                            ComplateMessage.MessageLen = MessageLen;
                            ComplateMessage.Buffer.SetNum(MessageLen); // 0

                            {
                                FScopeLock Lock(&QueueMutex);
                                HaveRecved.push(ComplateMessage);
                            }

                            MessageType = 0;
                            MessageLen = 0;

                            continue;
                        }
                    }
                }
                else
                {
                    int Readed;
                    Server->Recv(Buffer + Size, MessageLen - Size, Readed);
                    // PrintBytes(Buffer + Size, Readed);

                    Size += Readed;
                    if (Size >= MessageLen)
                    {
                        SingleMessageData ComplateMessage;
                        ComplateMessage.MessageType = MessageType;
                        ComplateMessage.MessageLen = MessageLen;
                        ComplateMessage.Buffer.SetNum(MessageLen);
                        memcpy(ComplateMessage.Buffer.GetData(), Buffer, MessageLen);

                        {
                            FScopeLock Lock(&QueueMutex);
                            HaveRecved.push(ComplateMessage);
                        }

                        MessageType = 0;
                        MessageLen = 0;

                        Size = 0;
                    }
                }
            }
            else break;
        }
        return (uint32)0;
    }

    virtual bool Init() override
    {
        _should_stop = false;
        return true;
    }

    virtual void Stop() override
    {
        ShouldStop(true);
    }

    virtual void Exit() override
    {

    }

public:
    SingleMessageData NextMessage()
    {
        FScopeLock Lock(&QueueMutex);

        if (HaveRecved.empty())
        {
            SingleMessageData Empty;
            Empty.MessageType = 0;
            return Empty;
        }

        return HaveRecved.front();
    }

    void PopMessage()
    {
        FScopeLock Lock(&QueueMutex);

        HaveRecved.pop();
    }
};

USTRUCT()
struct FSkillInfo
{
    GENERATED_BODY()

    FString Name;
    FString IconPath;
    float CooldownTime;
    float ManaCost;
};

USTRUCT()
struct FItemInfo
{
    GENERATED_BODY()

    FString Name;
    FString IconPath;
    FString Description;
    FString Tip;
    int Cost;
};

USTRUCT()
struct FHeroInfo
{
    GENERATED_BODY()

    FString Name;
    int Skills[6];
    float HealthMax;
    float HealthRecover;
    float ManaMax;
    float ManaRecover;
    FString MiniImagePath;
};

UCLASS()
class PARAGONROYAL_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()

// Delegates of Messages from Server
public:
    // Single Delegate
    ServerMessageTwoIntSingleDelegate OnLoginRespond;
    ServerMessageTwoIntSingleDelegate OnSignupRespond;
    ServerMessageSingleDelegate OnMatchFound;

    // Multi Delegate
    ServerMessageOneIntTwoFloatMultiDelegate OnPlayerMove;
    ServerMessageOneIntTwoFloatMultiDelegate OnPlayerTurn;

    // Multi Delegate
    FrameNotice OnStepBegin;
    ClientStepNotice OnHaveClientStep;
	
public:
	virtual void Init() override;
	virtual void Shutdown() override;

private:
    FDelegateHandle TickStepDelegateHandle;
    FDelegateHandle TickMessageDelegateHandle;

private:
    float HeartCheckRemainTime = 0.0f;

public:
	bool UpdateStep(float Delta);
    bool UpdateMessage(float Delta);
    void Destroy();

public:
    void Send(MType SendType, uint8_t* Buffer, uint32_t DataLen);

protected:
    NetworkSendThread* SendWorker = nullptr;
    NetworkRecvThread* RecvWorker = nullptr;

    FRunnableThread* SendThread = nullptr;
    FRunnableThread* RecvThread = nullptr;

protected:
    FSocket* Server = nullptr;

public:
    TMap<int, FSkillInfo> SkillInfos;
    TMap<int, FItemInfo> ItemInfos;
    TMap<int, FHeroInfo> HeroInfos;

// About Server & Other Players
public:
    UPROPERTY(BlueprintReadOnly)
    int SelfID = 0; // ID in Game, Changes when Game Start

    PMatchFoundRespond MatchFoundRespondResult;
    std::string LoginUserName;

    ACharacter* HeroCharacter = nullptr;
    TArray<ACharacter*> AllCharacters;

    int LastPlayer;

// About Map
public:
    int32_t MapSeed = 996;
    float BlockWidthInMap = 140.0f;

    bool BlockVisiable[128][128];

    int LocalPlayerNowBlockX = 0;
    int LocalPlayerNowBlockY = 0;

    int PlayerInitLocationAsBlockX[5];
    int PlayerInitLocationAsBlockY[5];

    std::string MapName[4];
    int GameMap[4][61][61];
    int GameMapNumber = 3; // 1, 2, 3

// About Frame
private:
    int NowFrame = 0;

    std::chrono::steady_clock::time_point GameStartTime;
    double GameSumTime; // Begin from game start
    
    int LostPackageNumber = 0;
    double NetworkDelay = 0.0;
    double LastStepSendTime = 0.0;

    bool GameRunning = false;

public:
    const double FrameLong = 30; // 30 ms
    const int FrameDelay = 10; // 300 ms

    const int RoomSize = 4; // Must <= 4, could be 2,3,4

public:
    void GameStart();
    void GameStop();

// Get Game Info
public:
    double GetGameTime() const { return GameRunning ? (GameSumTime / 1e3) : 0.0; } // Seconds
    int GetStep() const { return NowFrame; }
    double GetDelay() const { return NetworkDelay; }
    double GetCheck() const { return 1.0 - (1.0 * LostPackageNumber / (NowFrame + 1)); }
    int GetLost() const { return LostPackageNumber; }
    int GetPlayerNumber() const { return LastPlayer; }

private:
    void InitConst();
};
