#include "configfilereader.h"

#include <stdio.h>  //for snprintf
#include <string.h>

CConfigFileReader::CConfigFileReader(const char* fileName)
{
	_loadFile(fileName);
}

CConfigFileReader::~CConfigFileReader()
{

}

char* CConfigFileReader::getConfigName(const char* name)
{
	if (!m_loadOk)
		return NULL;

	char* value = NULL;
	std::map<std::string, std::string>::iterator it = m_configMap.find(name);
	if (it != m_configMap.end())
	{
		value = (char*)it->second.c_str();
	}

	return value;
}

int CConfigFileReader::setConfigName(const char* name, const char* value)
{
	if (!m_loadOk)
		return -1;

	std::map<std::string, std::string>::iterator it = m_configMap.find(name);
	if (it != m_configMap.end())
	{
		it->second = value;
	}
	else
	{
		m_configMap.insert(std::make_pair(name, value));
	}

	return _writeFile();
}

int CConfigFileReader::_writeFile(const char* fileName)
{
	FILE* fp = NULL;
	
	if (fileName == NULL)
	{
		fopen(m_configFile.c_str(), "w");
	}
	else
	{
		fopen(fileName, "w");
	}

	if (!fp)
		return -1;

	char szPaire[128];
	std::map<std::string, std::string>::iterator it = m_configMap.begin();
	for (; it != m_configMap.end(); ++it)
	{
		memset(szPaire, 0, sizeof(szPaire));
		snprintf(szPaire, sizeof(szPaire), "%s=%s\n", it->first.c_str(), it->second.c_str());
		size_t ret = fwrite(szPaire, strlen(szPaire), 1, fp);
		if (ret != 1)
		{
			fclose(fp);
			return -1;
		}
	}
	fclose(fp);
	return 0;
}

void CConfigFileReader::_loadFile(const char* fileName)
{
	m_configFile.clear();
	m_configFile.append(fileName);
	FILE* fp = fopen(fileName, "r");
	if (!fp)
		return;
	char buf[256];
	for (;;)
	{
		char* p = fgets(buf, 256, fp);
		if (!p)
		{
			break;
		}
		size_t size = strlen(buf);

		if (buf[size - 1] == '\n')
			buf[size - 1] = 0;

		char* ch = strchr(buf, '#');
		if (ch)
			*ch = 0;

		if (strlen(buf) == 0)
			continue;
		_parseLine(buf);
	}
	fclose(fp);
	m_loadOk = true;
}

void CConfigFileReader::_parseLine(char* line)
{
	char* p = strchr(line, '=');
	if (!p)
		return;
	*p = 0;

	char* key = _trimSpace(line);
	char* value = _trimSpace(p + 1);

	if (key && value)
	{
		m_configMap.insert(std::make_pair(key, value));
	}

}

char* CConfigFileReader::_trimSpace(char* name)
{
	// remove starting space or tab
	char* start_pos = name;
	while ((*start_pos == ' ') || (*start_pos == '\t'))
	{
		start_pos++;
	}

	if (strlen(start_pos) == 0)
		return NULL;

	// remove ending space or tab
	char* end_pos = name + strlen(name) - 1;
	while ((*end_pos == ' ') || (*end_pos == '\t'))
	{
		*end_pos = 0;
		end_pos--;
	}

	int len = (int)(end_pos - start_pos) + 1;
	if (len <= 0)
		return NULL;

	return start_pos;
}