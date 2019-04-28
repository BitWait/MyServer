// Copyright 2010, Shuo Chen.  All rights reserved.
// http://code.google.com/p/muduo/
//
// Use of this source code is governed by a BSD-style license
// that can be found in the License file.

// Author: Shuo Chen (chenshuo at chenshuo dot com)
//


#include "../buffer.h"
#include "HttpContext.h"
#include <iostream>
#include <sstream>

using namespace net;

bool HttpContext::processRequestLine(const char* begin, const char* end)
{
	bool succeed = false;
	const char* start = begin;
	const char* space = std::find(start, end, ' ');
	if (space != end && request_.setMethod(start, space))
	{
		start = space + 1;
		space = std::find(start, end, ' ');
		if (space != end)
		{
			const char* question = std::find(start, space, '?');
			if (question != space)
			{
				request_.setPath(start, question);
				request_.setQuery(question + 1, space);
			}
			else
			{
				request_.setPath(start, space);
			}
			start = space + 1;
			succeed = end - start == 8 && std::equal(start, end - 1, "HTTP/1.");
			if (succeed)
			{
				if (*(end - 1) == '1')
				{
					request_.setVersion(HttpRequest::kHttp11);
				}
				else if (*(end - 1) == '0')
				{
					request_.setVersion(HttpRequest::kHttp10);
				}
				else
				{
					succeed = false;
				}
			}
		}
	}
	return succeed;
}

// return false if any error
bool HttpContext::parseRequest(Buffer* buf, Timestamp receiveTime)
{
	bool ok = true;
	bool hasMore = true;
	while (hasMore)
	{
		if (state_ == kExpectRequestLine)
		{
			const char* crlf = buf->findCRLF();
			if (crlf)
			{
				ok = processRequestLine(buf->peek(), crlf);
				if (ok)
				{
					request_.setReceiveTime(receiveTime);
					buf->retrieveUntil(crlf + 2);
					state_ = kExpectHeaders;
				}
				else
				{
					hasMore = false;
				}
			}
			else
			{
				hasMore = false;
			}
		}
		else if (state_ == kExpectHeaders)
		{
			const char* crlf = buf->findCRLF();
			if (crlf)
			{
				const char* colon = std::find(buf->peek(), crlf, ':');
				if (colon != crlf)
				{
					request_.addHeader(buf->peek(), colon, crlf);
				}
				else
				{
					// empty line, end of header
					// FIXME:
					// 					state_ = kGotAll;
					// 					hasMore = false;
					state_ = kExpectBody;
				}
				buf->retrieveUntil(crlf + 2);
			}
			else
			{
				hasMore = false;
			}
		}
		else if (state_ == kExpectBody)
		{
			stringstream ss;
			int32_t length = 0;
			ss << request_.getHeader("Content-Length");
			ss >> length;
			std::cout << length;
			if (processRequestBody(buf->peek(), buf->peek() + length))
			{
				state_ = kGotAll;
			}
			hasMore = false;
		}
	}
	return ok;
}

bool HttpContext::processRequestBody(const char* begin, const char* end)
{
	bool nRet = false;
	do 
	{
		if (end - begin)
		{
			request_.setQuery(begin, end);
			nRet = true;
			break;
		}

		if (request_.method() == HttpRequest::kGet)
		{
			nRet = true;
			break;
		}
	} while (false);

	return nRet;
}