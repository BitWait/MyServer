#include "../buffer.h"
#include "../../base/logging.h"
#include <iostream>
using namespace std;
using namespace net;

void output(Buffer&& buf, const void* inner)
{
	Buffer newbuf(std::move(buf));
	// printf("New Buffer at %p, inner %p\n", &newbuf, newbuf.peek());
	LOG_INFO << inner << ' ' << newbuf.peek();
}

int main(void)
{
	{
		cout << "-----------------testmove--------------------\n";
		Buffer buf;
		buf.append("hello", 5);
		const void* inner = buf.peek();
		// printf("Buffer at %p, inner %p\n", &buf, inner);
		output(std::move(buf), inner);
	}

	{
		cout << "-----------------testBufferFindEOL--------------------\n";
		Buffer buf;
		buf.append(string(100000, 'x'));
		const char* null = NULL;
		LOG_INFO << buf.findEOL() << ' ' << null;
		LOG_INFO << buf.findEOL(buf.peek() + 90000) << ' ' << null;
	}

	{
		cout << "-----------------testBufferAppendRetrieve--------------------\n";
		Buffer buf;

		LOG_INFO << buf.readableBytes() << ' ' << 0;
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize;
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend;

		const string str(200, 'x');
		buf.append(str);

		LOG_INFO << buf.readableBytes() << ' ' << str.size();
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize - str.size();
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend;


		const string str2 = buf.retrieveAsString(50);

		LOG_INFO << str2.size() << ' ' << 50;
		LOG_INFO << buf.readableBytes() << ' ' << str.size();
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize - str.size();
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend;
		LOG_INFO << str2.size() << ' ' << 50;

		buf.append(str);


		LOG_INFO << buf.readableBytes() << ' ' << 2 * str.size() - str2.size();
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize - 2 * str.size();
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend + str2.size();

		const string str3 = buf.retrieveAllAsString();

		LOG_INFO << str3.size() << ' ' << 350;
		LOG_INFO << buf.readableBytes() << ' ' << 0;
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize;
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend;
	}

	{
		cout << "-----------------testbuffergrow--------------------\n";
		Buffer buf;
		buf.append(string(400, 'y'));

		LOG_INFO << buf.readableBytes() << ' ' << 400;
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize - 400;

		buf.retrieve(50);

		LOG_INFO << buf.readableBytes() << ' ' << 350;
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize - 400;
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend + 50;


		buf.append(string(1000, 'z'));

		LOG_INFO << buf.readableBytes() << ' ' << 1350;
		LOG_INFO << buf.writableBytes() << ' ' << 0;
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend + 50;

		buf.retrieveAll();
		LOG_INFO << buf.readableBytes() << ' ' << 0;
		LOG_INFO << buf.writableBytes() << ' ' << 1400;
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend;
	}

	{
		cout << "-----------------testBufferInsideGrow--------------------\n";

		Buffer buf;
		buf.append(string(800, 'y'));


		LOG_INFO << buf.readableBytes() << ' ' << 800;
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize - 800;

		buf.retrieve(500);

		LOG_INFO << buf.readableBytes() << ' ' << 300;
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize - 800;
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend + 500;

		buf.append(string(300, 'z'));

		LOG_INFO << buf.readableBytes() << ' ' << 600;
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize - 600;
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend;
	}
	{
		cout << "-----------------testBuffershrink--------------------\n";
		Buffer buf;
		buf.append(string(2000, 'y'));
		LOG_INFO << buf.readableBytes() << ' ' << 2000;
		LOG_INFO << buf.writableBytes() << ' ' << 0;
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend;

		buf.retrieve(1500);
		LOG_INFO << buf.readableBytes() << ' ' << 500;
		LOG_INFO << buf.writableBytes() << ' ' << 0;
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend + 1500;

		buf.shrink(0);
		LOG_INFO << buf.readableBytes() << ' ' << 500;
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize - 500;
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend;
	}
	{
		cout << "-----------------testBufferPrepend--------------------\n";
		Buffer buf;
		buf.append(string(200, 'y'));
		LOG_INFO << buf.readableBytes() << ' ' << 200;
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize - 200;
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend;

		int x = 0;
		buf.prepend(&x, sizeof x);
		LOG_INFO << buf.readableBytes() << ' ' << 204;
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize - 200;
		LOG_INFO << buf.prependableBytes() << ' ' << Buffer::kCheapPrepend - 4;
	}
	{
		cout << "-----------------testBufferReadInt--------------------\n";
		Buffer buf;
		buf.append("HTTP");

		LOG_INFO << buf.readableBytes() << ' ' << 4;
		LOG_INFO << buf.peekInt8() << ' ' << 'H';


		int top16 = buf.peekInt16();
		LOG_INFO << top16 << ' ' << 'H' * 256 + 'T';
		LOG_INFO << buf.peekInt32() << ' ' << top16 * 65536 + 'T' * 256 + 'P';

		LOG_INFO << buf.readInt8() << ' ' << 'H';
		LOG_INFO << buf.readInt16() << ' ' << 'T' * 256 + 'T';
		LOG_INFO << buf.readInt8() << ' ' << 'P';
		LOG_INFO << buf.readableBytes() << ' ' << 0;
		LOG_INFO << buf.writableBytes() << ' ' << Buffer::kInitialSize;

		buf.appendInt8(-1);
		buf.appendInt16(-2);
		buf.appendInt32(-3);

		LOG_INFO << buf.readableBytes() << ' ' << 7;
		LOG_INFO << buf.readInt8() << ' ' << -1;
		LOG_INFO << buf.readInt16() << ' ' << -2;
		LOG_INFO << buf.readInt32() << ' ' << -3;
	}
}