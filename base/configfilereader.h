#ifndef __CONFIG_FILE_READER_H__
#define __CONFIG_FILE_READER_H__

#include <map>
#include <string>

class CConfigFileReader
{
public:
	CConfigFileReader(const char* fileName);
	~CConfigFileReader();

	char * getConfigName(const char* name);
	int setConfigName(const char* name, const char* value);

private:
	void _loadFile(const char* fileName);
	int _writeFile(const char* fileName = NULL);
	void _parseLine(char* line);
	char* _trimSpace(char* name);

private:
	bool m_loadOk;
	std::map<std::string, std::string> m_configMap;
	std::string m_configFile;
};






#endif

