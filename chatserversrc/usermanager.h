/**
*  管理所有的用户信息，初始信息从数据库中加载, UserManager.h
*  zhangyl 2017.03.15
**/

#pragma once
#include <stdint.h>
#include <string>
#include <list>
#include <mutex>
#include <set>

using namespace std;

#define GROUPID_BOUBDARY   0x0FFFFFFF 

#define DEFAULT_TEAMNAME  "My Friends"

enum FRIEND_OPERATION
{
	FRIEND_OPERATION_ADD,
	FRIEND_OPERATION_DELETE
};

struct FriendInfo
{
	int32_t friendid;
	string  markname;
	string  teamname;
};
//用户或者群
struct User
{
	int32_t        userid;      //0x0FFFFFFF以上是群号，以下是普通用户
	string         username;    //群账户的username也是群号userid的字符串形式
	string         password;
	string         nickname;    //群账号为群名称
	int32_t        facetype;
	string         customface;
	string         customfacefmt;//自定义头像格式
	int32_t        gender;
	int32_t        birthday;
	string         signature;
	string         address;
	string         phonenumber;
	string         mail;
	/*
	个人用户好友分组信息，对于群账户则为空，例如:
	[{"teamname": "我的好友"}, {"teamname": "我的同事"}, {"teamname": "企业客户"}]
	*/
	string             teaminfo;       //对于普通用户，为分组信息；对于群组则为空
	int32_t            ownerid;        //对于群账号，为群主userid
	list<FriendInfo>   friends;
};

class UserManager final
{
public:
	UserManager();
	~UserManager();

	bool Init(const char* dbServer, const char* dbUserName, const char* dbPassword, const char* dbName);

	UserManager(const UserManager& rhs) = delete;
	UserManager& operator=(const UserManager& rhs) = delete;
private:
	bool LoadUsersFromDb();
	bool LoadRelationshipFromDb(int32_t userid, std::list<FriendInfo>& r);

private:
	int                 m_baseUserId{ 0 };        //m_baseUserId, 取数据库里面userid最大值，新增用户在这个基础上递增
	int                 m_baseGroupId{ 0x0FFFFFFF };
	list<User>          m_allCachedUsers;
	mutex               m_mutex;

	string              m_strDbServer;
	string              m_strDbUserName;
	string              m_strDbPassword;
	string              m_strDbName;
};
