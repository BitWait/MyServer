#ifndef __DATABASE_MYSQL_H__
#define __DATABASE_MYSQL_H__

#include <stdint.h>
#include <mysql/mysql.h>
#include <mysql/errmsg.h>
#include "queryresult.h"
#include <memory>

#define MAX_QUERY_LEN   1024

class CDatabaseMysql
{
public:
	struct DatabaseInfo
	{
		string strHost;
		string strUser;
		string strPwd;
		string strDBName;
	};

	CDatabaseMysql(void);
	~CDatabaseMysql(void);

	bool Initialize(const string& host, const string& user, const string& pwd, const string& dbname);
	//QueryResult* Query(const char *sql);
	std::shared_ptr<QueryResult> Query(const char *sql);

	std::shared_ptr<QueryResult> Query(const string& sql)
	{
		return Query(sql.c_str());
	}

	std::shared_ptr<QueryResult> PQuery(const char *format, ...);
	bool Execute(const char* sql);
	bool Execute(const char* sql, uint32_t& uAffectedCount, int& nErrno);
	bool PExecute(const char *format, ...);

	uint32_t GetInsertID();

	void ClearStoredResults();

	int32_t EscapeString(char* szDst, const char* szSrc, uint32_t uSize);
private:
	DatabaseInfo m_DBInfo;
	MYSQL *m_Mysql;
	bool m_bInit;
};


#endif
