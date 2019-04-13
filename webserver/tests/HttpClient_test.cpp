#include "../HttpClient.h"
#include <string>
#include <iostream>


using namespace std;

int main(void)
{
	CHttpClient httpclient;
	string strURL = "https://api.weixin.qq.com/sns/jscode2session?appid=<AppId>&secret=<AppSecret>&js_code=<code>&grant_type=authorization_code";
	string strResp;
	httpclient.Get(strURL, strResp);
	cout << strResp << endl;
}