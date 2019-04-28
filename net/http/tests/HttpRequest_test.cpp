
#include "../../buffer.h"
#include "../HttpContext.h"
#include "../../../base/logging.h"

using std::string;
using net::Buffer;
using net::HttpContext;
using net::HttpRequest;

int main(void)
{
	{
		HttpContext context;
		Buffer input;
		input.append("GET /index.html?{\"userid\":\"10010\"} HTTP/1.1\r\n"
			"Host: www.chenshuo.com\r\n"
			"\r\n");
		LOG_INFO << context.parseRequest(&input, Timestamp::now()) << "::::" << true;
		LOG_INFO << context.gotAll() << "::::" << true;

		const HttpRequest& request = context.request();
		LOG_INFO << request.method() << "::::" << HttpRequest::kGet;
		LOG_INFO << request.path() << "::::" << string("/index.html");
		LOG_INFO << request.getVersion() << "::::" << HttpRequest::kHttp11;
		LOG_INFO << request.getHeader("Host") << "::::" << string("www.chenshuo.com");
		LOG_INFO << request.getHeader("User-Agent") << "::::" << string("");
		LOG_INFO << request.query();
	}

// 	{
// 	string all("GET /index.html HTTP/1.1\r\n"
// 		"Host: www.chenshuo.com\r\n"
// 		"\r\n");
// 	for (size_t sz1 = 0; sz1 < all.size(); ++sz1)
// 	{
// 		HttpContext context;
// 		Buffer input;
// 		input.append(all.c_str(), sz1);
// 
// 		LOG_INFO << context.parseRequest(&input, Timestamp::now()) << "::::" << true;
// 		LOG_INFO << context.gotAll() << "::::" << false;
// 
// 		size_t sz2 = all.size() - sz1;
// 		input.append(all.c_str() + sz1, sz2);
// 		LOG_INFO << context.parseRequest(&input, Timestamp::now()) << "::::" << true;
// 		LOG_INFO << context.gotAll() << "::::" << true;
// 		const HttpRequest& request = context.request();
// 		LOG_INFO << request.method() << "::::" << HttpRequest::kGet;
// 		LOG_INFO << request.path() << "::::" << string("/index.html");
// 		LOG_INFO << request.getVersion() << "::::" << HttpRequest::kHttp11;
// 		LOG_INFO << request.getHeader("Host") << "::::" << string("www.chenshuo.com");
// 		LOG_INFO << request.getHeader("User-Agent") << "::::" << string("");
// 	}
// }

	{
		HttpContext context;
		Buffer input;
		input.append("POST /index.html HTTP/1.1\r\n"
			"Host: www.chenshuo.com\r\n"
			"User-Agent:\r\n"
			"Accept-Encoding: \r\n"
			"content-length:22\r\n"
			"\r\n"
			"{\"userid\":\"10010\"}");

		LOG_INFO << context.parseRequest(&input, Timestamp::now()) << "::::" << true;
		LOG_INFO << context.gotAll() << "::::" << true;

		const HttpRequest& request = context.request();
		LOG_INFO << request.method() << "::::" << HttpRequest::kGet;
		LOG_INFO << request.path() << "::::" << string("/index.html");
		LOG_INFO << request.getVersion() << "::::" << HttpRequest::kHttp11;
		LOG_INFO << request.getHeader("Host") << "::::" << string("www.chenshuo.com");
		LOG_INFO << request.getHeader("User-Agent") << "::::" << string("");
		LOG_INFO << request.getHeader("Accept-Encoding") << "::::" << string("");
		LOG_INFO << request.getHeader("content-length") << "::::" << string("22");
		LOG_INFO << request.query();
	}
}