/**
*  管理所有的用户信息，初始信息从数据库中加载, UserManager.h
*  zhangyl 2017.03.15
**/
#include <memory>
#include <sstream>
#include <stdio.h>
#include "../database/databasemysql.h"
#include "../base/logging.h"
#include "../jsoncpp-0.5.0/json.h"
#include "usermanager.h"

UserManager::UserManager()
{

}

UserManager::~UserManager()
{

}

bool UserManager::Init(const char* dbServer, const char* dbUserName, const char* dbPassword, const char* dbName)
{
	m_strDbServer = dbServer;
	m_strDbUserName = dbUserName;
	if (dbPassword != NULL)
		m_strDbPassword = dbPassword;
	m_strDbName = dbName;

	//从数据库中加载所有用户信息
	if (!LoadUsersFromDb())
		return false;

	for (auto& iter : m_allCachedUsers)
	{
		if (!LoadRelationshipFromDb(iter.userid, iter.friends))
		{
			LOG_ERROR << "Load relationship from db error, userid=" << iter.userid;
			continue;
		}
	}

	return true;
}

bool UserManager::GetUserInfoByUserId(int32_t userid, User& u)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	for (const auto& iter : m_allCachedUsers)
	{
		if (iter.userid == userid)
		{
			u = iter;
			return true;
		}
	}

	return false;
}

bool UserManager::GetUserInfoByUserId(int32_t userid, User*& u)
{
	std::lock_guard<std::mutex> guard(m_mutex);
	for (auto& iter : m_allCachedUsers)
	{
		if (iter.userid == userid)
		{
			u = &iter;
			return true;
		}
	}

	return false;
}

bool UserManager::LoadUsersFromDb()
{
	std::unique_ptr<CDatabaseMysql> pConn;
	pConn.reset(new CDatabaseMysql());
	if (!pConn->Initialize(m_strDbServer, m_strDbUserName, m_strDbPassword, m_strDbName))
	{
		LOG_FATAL << "UserManager::LoadUsersFromDb failed, please check params: dbserver=" << m_strDbServer
			<< ", dbusername=" << m_strDbUserName << ", dbpassword" << m_strDbPassword
			<< ", dbname=" << m_strDbName;
		return false;
	}

	//TODO: 到底是空数据集还是出错，需要修改下返回类型
	std::shared_ptr<QueryResult> pResult = pConn->Query("SELECT f_user_id, f_username, f_nickname, f_password,  f_facetype, f_customface, f_gender, f_birthday, f_signature, f_address, f_phonenumber, f_mail, f_teaminfo FROM t_user ORDER BY  f_user_id DESC");
	if (NULL == pResult)
	{
		LOG_INFO << "UserManager::_Query error, dbname=" << m_strDbName;
		return false;
	}

	string teaminfo;
	while (true)
	{
		Field* pRow = pResult->Fetch();
		if (pRow == NULL)
			break;

		User u;
		u.userid = pRow[0].GetInt32();
		u.username = pRow[1].GetString();
		u.nickname = pRow[2].GetString();
		u.password = pRow[3].GetString();
		u.facetype = pRow[4].GetInt32();
		u.customface = pRow[5].GetString();
		u.gender = pRow[6].GetInt32();
		u.birthday = pRow[7].GetInt32();
		u.signature = pRow[8].GetString();
		u.address = pRow[9].GetString();
		u.phonenumber = pRow[10].GetString();
		u.mail = pRow[11].GetString();
		u.teaminfo = pRow[12].GetString();
		m_allCachedUsers.push_back(u);

		LOG_INFO << "userid: " << u.userid << ", username: " << u.username << ", password: " << u.password << ", nickname: " << u.nickname << ", signature: " << u.signature;

		//计算当前最大userid
		if (u.userid < GROUPID_BOUBDARY && u.userid > m_baseUserId)
			m_baseUserId = u.userid;

		//计算当前最大群组id
		if (u.userid > GROUPID_BOUBDARY && u.userid > m_baseGroupId)
			m_baseGroupId = u.userid;

		if (!pResult->NextRow())
		{
			break;
		}
	}

	LOG_INFO << "current base userid: " << m_baseUserId << ", current base group id: " << m_baseGroupId;

	pResult->EndQuery();

	return true;
}

bool UserManager::LoadRelationshipFromDb(int32_t userid, std::list<FriendInfo>& r)
{
	std::unique_ptr<CDatabaseMysql> pConn;
	pConn.reset(new CDatabaseMysql());
	if (!pConn->Initialize(m_strDbServer, m_strDbUserName, m_strDbPassword, m_strDbName))
	{
		LOG_FATAL << "UserManager::LoadRelationhipFromDb failed, please check params";
		return false;
	}

	char sql[256] = { 0 };
	snprintf(sql, 256, "SELECT f_user_id1, f_user_id2, f_user1_markname, f_user2_markname, f_user1_teamname, f_user2_teamname FROM t_user_relationship WHERE f_user_id1 = %d OR f_user_id2 = %d ", userid, userid);
	std::shared_ptr<QueryResult> pResult = pConn->Query(sql);
	if (NULL == pResult)
	{
		LOG_INFO << "UserManager::Query error, db=" << m_strDbName;
		return false;
	}

	while (true)
	{
		Field* pRow = pResult->Fetch();
		if (pRow == NULL)
			break;

		int32_t friendid1 = pRow[0].GetInt32();
		int32_t friendid2 = pRow[1].GetInt32();
		string markname1 = pRow[2].GetCppString();
		string markname2 = pRow[3].GetCppString();
		string teamname1 = pRow[4].GetCppString();
		string teamname2 = pRow[5].GetCppString();
		if (teamname1.empty())
			teamname1 = DEFAULT_TEAMNAME;
		if (teamname2.empty())
			teamname2 = DEFAULT_TEAMNAME;
		FriendInfo fi;
		if (friendid1 == userid)
		{
			fi.friendid = friendid2;
			fi.markname = markname1;
			fi.teamname = teamname1;
			r.emplace_back(fi);
			//LOG_INFO << "userid=" << userid << ", friendid=" << friendid2;
		}
		else
		{
			fi.friendid = friendid1;
			fi.markname = markname2;
			fi.teamname = teamname2;
			r.emplace_back(fi);
			//LOG_INFO << "userid=" << userid << ", friendid=" << friendid1;
		}

		if (!pResult->NextRow())
		{
			break;
		}
	}

	pResult->EndQuery();

	return true;
}