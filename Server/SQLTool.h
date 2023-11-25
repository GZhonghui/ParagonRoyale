#pragma once
#include <hiredis/hiredis.h>
#include <mysql.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <pthread.h>
using namespace std;
class SQLTool
{
public:
    SQLTool();

    bool init();

    //暂时不提供修改地址、数据库账户密码的接口
    void setPort(int32_t iPort) { m_iPort = iPort; }
    int32_t getPort() { return m_iPort; }

    void setHost(string strHost) { m_strHost = strHost; }
    string getHost() { return m_strHost; }

    void setTable(string strTableName) { m_strTableName = strTableName; }
    string getTable() { return m_strTableName; }
    //数据库操作接口(根据表的类型来增加接口)
    bool getPassWord(string strUsername,string &strPassWord);
    bool setPassWord(string strUsername, string strNewPassWord);
    bool addUser(string strUserName, string strPassWord);
    bool deleteUser(string strUserName);
    bool getUserID(string strUsername,int &UserID);
    
    //关闭数据库连接
    void closeSql(){
        mysql_close(&m_sqlHandler);
    }
private:

    int32_t m_iPort;
    string m_strUsername; //库的name
    string m_strPassword;
    string m_strHost;
    string m_strDBName;
    string m_strTableName;
    MYSQL m_sqlHandler;
};
SQLTool::SQLTool()
{
    m_iPort = 3306;
    m_strUsername = "game";
    m_strPassword = "game123u";
    m_strHost = "10.0.128.199";
    //m_strHost = "127.0.0.1";
    m_strDBName = "DB1";
    m_strTableName = "UserInfo";
    init();
}
bool SQLTool::init()
{
    
    //pthread_mutex_init(&mutex_SQL, NULL);
    uint32_t client_flag = 0;

    //初始化连接
    mysql_init(&m_sqlHandler);
    mysql_options(&m_sqlHandler, MYSQL_OPT_RECONNECT, "1");
    mysql_options(&m_sqlHandler, MYSQL_SET_CHARSET_NAME, "utf8");
    MYSQL* connect_result = mysql_real_connect(&m_sqlHandler,  m_strHost.c_str(),  m_strUsername.c_str(), m_strPassword.c_str(), NULL, m_iPort, NULL, client_flag);
    if (NULL == connect_result)
    {
        printf("mysql_real_connect failed with code [%d] msg[%s]\n", mysql_errno(&m_sqlHandler), mysql_error(&m_sqlHandler));
        return false;
    }

    if (mysql_ping(&m_sqlHandler))
    {
        printf("mysql ping failed\n");
        return false;
    }

    if (mysql_select_db(&m_sqlHandler, m_strDBName.c_str()))
    {
        printf("select db failed\n");
        return false;
    }

    if (mysql_ping(&m_sqlHandler))
    {
        printf("mysql_ping failed\n");
        return false;
    }
    return true;
}
bool SQLTool::getPassWord(string strUsername,string &strPassWord)
{
    string sql = "SELECT PassWord FROM " + m_strTableName + " WHERE UserName=\"" + strUsername +"\"";
    
    //pthread_mutex_lock(&mutex_SQL);
    if(mysql_real_query(&m_sqlHandler, sql.c_str(), sql.size()))
    {
        printf("mysql_real_query failed with code [%d] msg[%s]\n", mysql_errno(&m_sqlHandler), mysql_error(&m_sqlHandler));
        return false;
    }
    MYSQL_RES* mysql_result = mysql_use_result(&m_sqlHandler);
    if(mysql_result == nullptr && (mysql_field_count(&m_sqlHandler) > 0))
    {
        printf("mysql_use_result failed with code [%d] msg[%s]\n", mysql_errno(&m_sqlHandler), mysql_error(&m_sqlHandler));
        return false;
    }
    MYSQL_ROW m_current_row = mysql_fetch_row(mysql_result);
    if(!m_current_row)
    {
        return false;
    }
    uint64_t *m_fields_length = mysql_fetch_lengths(mysql_result); // 每一列得数据长度
    strPassWord = std::string(m_current_row[0], (size_t)m_fields_length[0]);
    mysql_free_result(mysql_result);
    
    //pthread_mutex_unlock(&mutex_SQL);
    return true;
}
bool SQLTool::setPassWord(string strUsername, string strNewPassWord)
{
    string sql = "UPDATE " + m_strTableName + " SET PassWord=\"" + strNewPassWord +"\" WHERE UserName=\"" + strUsername + "\"";
    if(mysql_real_query(&m_sqlHandler, sql.c_str(), sql.size()))
    {
        printf("mysql_real_query failed with code [%d] msg[%s]\n", mysql_errno(&m_sqlHandler), mysql_error(&m_sqlHandler));
        return false;
    }
    return true;
}
bool SQLTool::addUser(string strUserName, string strPassWord)
{
    if(getPassWord(strUserName,strPassWord))
    {
        return false;
    }
    string sql = "INSERT INTO " + m_strTableName + " (UserName,PassWord) VALUES (\"" + strUserName + "\",\"" + strPassWord + "\")";
    if(mysql_real_query(&m_sqlHandler, sql.c_str(), sql.size()))
    {
        printf("mysql_real_query failed with code [%d] msg[%s]\n", mysql_errno(&m_sqlHandler), mysql_error(&m_sqlHandler));
        return false;
    }
    return true;
}
bool SQLTool::deleteUser(string strUserName)
{
    return true;
}
int stringToInt(string s)
{
    int res = 0;
    int p=0;
    while(p<s.size()){
        res*=10;
        res += s[p] - '0';
        p++;
    }
    return res;
}
bool SQLTool::getUserID(string strUsername,int &UserID)
{
    string sql = "SELECT UserID FROM UserNameToID WHERE UserName=\"" + strUsername +"\"";

    if(mysql_real_query(&m_sqlHandler, sql.c_str(), sql.size()))
    {
        printf("mysql_real_query failed with code [%d] msg[%s]\n", mysql_errno(&m_sqlHandler), mysql_error(&m_sqlHandler));
        return false;
    }
    MYSQL_RES* mysql_result = mysql_use_result(&m_sqlHandler);
    if(mysql_result == nullptr && (mysql_field_count(&m_sqlHandler) > 0))
    {
        printf("mysql_use_result failed with code [%d] msg[%s]\n", mysql_errno(&m_sqlHandler), mysql_error(&m_sqlHandler));
        return false;
    }
    MYSQL_ROW m_current_row = mysql_fetch_row(mysql_result);
    if(!m_current_row)
    {
        return false;
    }
    uint64_t *m_fields_length = mysql_fetch_lengths(mysql_result); // 每一列得数据长度
    UserID = stringToInt( std::string(m_current_row[0], (size_t)m_fields_length[0]) );
    mysql_free_result(mysql_result);

    return true;
}
/* TEST
    string usename = "XXX";
    string password = "xxx";
    if(SQLTool::getInstance()->getPassWord(usename ,password))
        cout<<password<<"\n";
    else 
    {
        cout<<"账户不存在，cin password\n";
        cin>>password;
        if(SQLTool::getInstance()->addUser(usename ,password)){
            cout<<"注册 成功\n";
        }
    }
    if(SQLTool::getInstance()->getPassWord(usename ,password)){
        cout<<"登录成功，你的用户名和密码是："<<usename<<"---";
        cout<<password<<"\n";
    }
*/
/*
class RedisTool{
public:
    
    RedisTool();

    bool init();

    bool RedisToolConnect();
    int handleReply();
    int setValue(const string &key,const string &value);
    int getValue(const string &key,string &value);
    int delKey(const string &key);
private:
    int m_iNum;
    int m_iRoom;
    redisContext* m_pm_rct;
    redisReply* m_pm_rr;
};

RedisTool::RedisTool()
{
    init();
}
bool  RedisTool::init()
{
    
    m_iNum=1;
    m_iRoom=1;
    m_pm_rct=nullptr;
    m_pm_rr = nullptr;
    if(RedisToolConnect())
    {
        return true;
    }
    return false;
}
bool RedisTool::RedisToolConnect()
{
    m_pm_rct = redisConnect("127.0.0.1",6379);
    if(m_pm_rct->err)
	{
		cout<<"redis connect error"<<"\n";
        return false;
	}
    return true;
}
int RedisTool::handleReply()
{
	return m_pm_rct->err;
}

int RedisTool::setValue(const string &key,const string &value)
{
    string cmd="set "+key+" "+value;
	m_pm_rr=(redisReply*)redisCommand(m_pm_rct,cmd.c_str());
	return handleReply();
}
int RedisTool::getValue(const string &key,string &value)
{
	string cmd="get "+key;
	m_pm_rr=(redisReply*)redisCommand(m_pm_rct,cmd.c_str());
    if(m_pm_rr->str)
    {
	    value=string(m_pm_rr->str);
        return 1;
    }
    return 0;
	return handleReply();
}
int RedisTool::delKey(const string &key)
{
	string cmd="del "+key;
	m_pm_rr=(redisReply*)redisCommand(m_pm_rct,cmd.c_str());
	return handleReply();
}*/