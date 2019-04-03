#include "../protocolstream.h"
#include <iostream>
#include <string.h>
using namespace std;
using namespace balloon;

int main(void)
{
	std::string buf;
	char szData[256] = "{\"userid\":10001, \"type\":2, \"username\": \"helloworld\"}";

	balloon::BinaryWriteStream writeStream(&buf);
	writeStream.WriteInt32(1000);
	writeStream.WriteInt32(16);
	cout << strlen(szData) << endl;
	writeStream.WriteCString(szData, strlen(szData));
	writeStream.Flush();

	cout << buf.size();
	balloon::BinaryReadStream readStream(buf.c_str(), buf.size());
	int32_t cmd;
	int32_t seq;
	readStream.ReadInt32(cmd);
	cout << cmd << endl;
	readStream.ReadInt32(seq);
	cout << seq << endl;
	std::string data;
	size_t datalength;
	readStream.ReadString(&data, 0, datalength);
	cout << data << ": " << datalength << endl;
}
