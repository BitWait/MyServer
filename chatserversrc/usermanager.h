/**
*  �������е��û���Ϣ����ʼ��Ϣ�����ݿ��м���, UserManager.h
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
//�û�����Ⱥ
struct User
{
	int32_t        userid;      //0x0FFFFFFF������Ⱥ�ţ���������ͨ�û�
	string         username;    //Ⱥ�˻���usernameҲ��Ⱥ��userid���ַ�����ʽ
	string         password;
	string         nickname;    //Ⱥ�˺�ΪȺ����
	int32_t        facetype;
	string         customface;
	string         customfacefmt;//�Զ���ͷ���ʽ
	int32_t        gender;
	int32_t        birthday;
	string         signature;
	string         address;
	string         phonenumber;
	string         mail;
	/*
	�����û����ѷ�����Ϣ������Ⱥ�˻���Ϊ�գ�����:
	[{"teamname": "�ҵĺ���"}, {"teamname": "�ҵ�ͬ��"}, {"teamname": "��ҵ�ͻ�"}]
	*/
	string             teaminfo;       //������ͨ�û���Ϊ������Ϣ������Ⱥ����Ϊ��
	int32_t            ownerid;        //����Ⱥ�˺ţ�ΪȺ��userid
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
	int                 m_baseUserId{ 0 };        //m_baseUserId, ȡ���ݿ�����userid���ֵ�������û�����������ϵ���
	int                 m_baseGroupId{ 0x0FFFFFFF };
	list<User>          m_allCachedUsers;
	mutex               m_mutex;

	string              m_strDbServer;
	string              m_strDbUserName;
	string              m_strDbPassword;
	string              m_strDbName;
};
