#include "../../base/configfilereader.h"
#include "../../base/logging.h"
#include "../../base/singleton.h"
#include "../mysqlmanager.h"
#include <string>

int main(void)
{
	CConfigFileReader config("mysqltest.conf");
	//初始化数据库配置
	const char* dbserver = config.getConfigName("dbserver");
	const char* dbuser = config.getConfigName("dbuser");
	const char* dbpassword = config.getConfigName("dbpassword");
	const char* dbname = config.getConfigName("dbname");
	if (!Singleton<CMysqlManager>::Instance().Init(dbserver, dbuser, dbpassword, dbname))
	{
		LOG_FATAL << "Init mysql failed, please check your database config..............";
	}

}