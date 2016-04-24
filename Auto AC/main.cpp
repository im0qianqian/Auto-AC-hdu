#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <regex>
#include <vector>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;
vector <string> blogUrl;
string allHtml;
string CodeHtml;
string ResCode;
SOCKET sock;
char host[200];
char othPath[300];
char buf[1024];
int ProblemID;
char s[200];
string StateAns;
string StateSapce;
string StateTime;
void SendRequst(char *host, char *othPath)              ///请求建立连接
{
	WSADATA wd;
	WSAStartup(MAKEWORD(2, 2), &wd);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cout << "建立socket失败！ 错误码： " << WSAGetLastError() << endl;
		return;
	}
	sockaddr_in sa = { AF_INET };
	int n = bind(sock, (sockaddr*)&sa, sizeof(sa));
	if (n == SOCKET_ERROR)
	{
		cout << "bind函数失败！ 错误码： " << WSAGetLastError() << endl;
		return;
	}
	struct hostent  *p = gethostbyname(host);
	if (p == NULL)
	{
		cout << "主机解析错误! 错误码： " << WSAGetLastError() << endl;
		return;
	}
	sa.sin_port = htons(80);
	memcpy(&sa.sin_addr, p->h_addr, 4);
	n = connect(sock, (sockaddr*)&sa, sizeof(sa));
	if (n == SOCKET_ERROR)
	{
		cout << "connect函数失败！ 错误码： " << WSAGetLastError() << endl;
		return;
	}
	string  reqInfo = "GET " + (string)othPath + " HTTP/1.1\r\nHost: " + (string)host + "\r\nConnection:Close\r\n\r\n";
	if (SOCKET_ERROR == send(sock, reqInfo.c_str(), reqInfo.size(), 0))
	{
		cout << "send error! 错误码： " << WSAGetLastError() << endl;
		closesocket(sock);
	}
}
void SendCode(char *host, char *othPath, string &Code, string PHPSESSID, string CNZZDATA)       ///发送最终的Code至服务器
{
	WSADATA wd;
	WSAStartup(MAKEWORD(2, 2), &wd);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cout << "建立socket失败！ 错误码： " << WSAGetLastError() << endl;
		return;
	}
	sockaddr_in sa = { AF_INET };
	int n = bind(sock, (sockaddr*)&sa, sizeof(sa));
	if (n == SOCKET_ERROR)
	{
		cout << "bind函数失败！ 错误码： " << WSAGetLastError() << endl;
		return;
	}
	struct hostent  *p = gethostbyname(host);
	if (p == NULL)
	{
		cout << "主机解析错误! 错误码： " << WSAGetLastError() << endl;
		return;
	}
	sa.sin_port = htons(80);
	memcpy(&sa.sin_addr, p->h_addr, 4);
	n = connect(sock, (sockaddr*)&sa, sizeof(sa));
	if (n == SOCKET_ERROR)
	{
		cout << "connect函数失败！ 错误码： " << WSAGetLastError() << endl;
		return;
	}
	string  Typee = "\r\nContent-Type: application/x-www-form-urlencoded";
	string ConLen = "\r\nContent-Length: ";
	_itoa(ProblemID, s, 10);
	string ElseInfo = "\r\nCache-Control: max-age=0\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8O\r\nOrigin: http://acm.hdu.edu.cn\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/42.0.2311.135 Safari/537.36\r\nReferer: http://acm.hdu.edu.cn/submit.php?pid=";
	ElseInfo = ElseInfo + (string)s + "\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.8";

	string HeaderP = "check=0&problemid=" + (string)s;
	HeaderP += "&language=2&usercode=";

	ResCode = HeaderP + ResCode;
	char s[300];
	_itoa(ResCode.length(), s, 10);
	string Cookie = "exesubmitlang=2; PHPSESSID=" + PHPSESSID + "; CNZZDATA1254072405=" + CNZZDATA;
	string  reqInfo = "POST " + (string)othPath + " HTTP/1.1\r\nHost: " + (string)host + ElseInfo + Typee + ConLen + (string)s + "\r\nCookie: " + Cookie + "\r\nConnection:Close\r\n\r\n" + ResCode;
	if (SOCKET_ERROR == send(sock, reqInfo.c_str(), reqInfo.size(), 0))
	{
		cout << "send error! 错误码： " << WSAGetLastError() << endl;
		closesocket(sock);
	}
	//cout << reqInfo;
}
void GetCSDNurl(string &allHtml)                ///提取网页中的csdn博客网址
{
	blogUrl.clear();
	smatch mat;
	regex pattern("href=\"(http://blog.csdn[^\\s\"]+)\"");
	string::const_iterator start = allHtml.begin();
	string::const_iterator end = allHtml.end();
	while (regex_search(start, end, mat, pattern))
	{
		string msg(mat[1].first, mat[1].second);
		blogUrl.push_back(msg);
		start = mat[0].second;
	}
}
bool analyUrl(char *url)                    ///仅支持http协议,解析出主机和IP地址
{
	char *pos = strstr(url, "http://");
	if (pos == NULL)return false;
	else pos += 7;
	sscanf(pos, "%[^/]%s", host, othPath);      //http:// 后一直到/之前的是主机名
	cout << "host: " << host << "   repath:" << othPath << endl;
	return true;
}

void GetCode(string &allHtml)               //提取代码部分
{
	CodeHtml = "";
	int pos = allHtml.find("#include");
	if (pos != string::npos)
	{
		for (int i = pos; i < (int)allHtml.length(); i++)
		{
			if ((allHtml[i] == '<'&&allHtml[i + 1] == '/'&&allHtml[i + 2] == 't'&&allHtml[i + 3] == 'e'&&allHtml[i + 4] == 'x'&&allHtml[i + 5] == 't'))return;
			else if (allHtml[i] == '<'&&allHtml[i + 1] == '/'&&allHtml[i + 2] == 'p'&&allHtml[i + 3] == 'r'&&allHtml[i + 4] == 'e'&&allHtml[i + 5] == '>')return;
			CodeHtml += allHtml[i];
		}
	}
	else
	{
		cout << "未找到合适的代码！" << endl;
		return;
	}
}

string HTMLTOC(string &CodeHtml)                     ///HTML转义字符转义处理
{
	string ans;
	for (int i = 0; i < (int)CodeHtml.length(); i++)
	{
		if (CodeHtml[i] == '&'&&CodeHtml[i + 1] == 'l'&&CodeHtml[i + 2] == 't'&&CodeHtml[i + 3] == ';')     ///&lt;   <
		{
			ans += '<';
			i += 3;
		}
		else if (CodeHtml[i] == '&'&&CodeHtml[i + 1] == 'g'&&CodeHtml[i + 2] == 't'&&CodeHtml[i + 3] == ';')     ///&gt;   >
		{
			ans += '>';
			i += 3;
		}
		else if (CodeHtml[i] == '/'&&CodeHtml[i + 1] == 'n')     /// /n;   \\n
		{
			ans += "\\n";
			i += 1;
		}
		else if (CodeHtml[i] == '&'&&CodeHtml[i + 1] == 'a'&&CodeHtml[i + 2] == 'm'&&CodeHtml[i + 3] == 'p'&&CodeHtml[i + 4] == ';')    ///&amp;    &
		{
			ans += '&';
			i += 4;
		}
		else if (CodeHtml[i] == '&'&&CodeHtml[i + 1] == 'q'&&CodeHtml[i + 2] == 'u'&&CodeHtml[i + 3] == 'o'&&CodeHtml[i + 4] == 't'&&CodeHtml[i + 5] == ';')    ///&quot;    \"
		{
			ans += '\"';
			i += 5;
		}
		else if (CodeHtml[i] == '&'&&CodeHtml[i + 1] == 'n'&&CodeHtml[i + 2] == 'b'&&CodeHtml[i + 3] == 's'&&CodeHtml[i + 4] == 'p'&&CodeHtml[i + 5] == ';')    ///&nbsp;    ' '
		{
			ans += ' ';
			i += 5;
		}
		else if (CodeHtml[i] == '&'&&CodeHtml[i + 1] == '#'&&CodeHtml[i + 2] == '4'&&CodeHtml[i + 3] == '3'&&CodeHtml[i + 4] == ';')    ///&#43;    +
		{
			ans += '+';
			i += 4;
		}
		else if (CodeHtml[i] == '&'&&CodeHtml[i + 1] == '#'&&CodeHtml[i + 2] == '3'&&CodeHtml[i + 3] == '9'&&CodeHtml[i + 4] == ';')    ///&#39;    '\'
		{
			ans += '\'';
			i += 4;
		}
		else ans += CodeHtml[i];
	}
	return ans;
}
string ASCtoHex(int num)                    ///十进制转换成十六进制
{
	char str[] = "0123456789ABCDEF";
	int temp = num;
	string ans;
	while (temp)
	{
		ans += str[temp % 16];
		temp /= 16;
	}
	ans += '%';
	reverse(ans.begin(), ans.end());
	return ans;
}
string GetRescode(string &CodeHtml)             ///转换为HTTP编码
{
	ResCode = "";
	for (int i = 0; i < (int)CodeHtml.length(); i++)
	{
		if ((CodeHtml[i] >= 0 && CodeHtml[i] < 48) || (CodeHtml[i]>57 && CodeHtml[i]<65) || (CodeHtml[i]>90 && CodeHtml[i]<97) || (CodeHtml[i]>122 && CodeHtml[i] <= 127))
		{
			if (CodeHtml[i] == '\n')ResCode += "%0D%0A";
			else if (CodeHtml[i] == '.' || CodeHtml[i] == '-' || CodeHtml[i] == '*')ResCode += CodeHtml[i];
			else
			{
				string cur = ASCtoHex(CodeHtml[i]);
				if (cur == "%9")ResCode += "++++";
				else if (cur == "%20")ResCode += '+';
				else if (cur == "%D")ResCode += "++";
				else ResCode += cur;
			}
		}
		else ResCode += CodeHtml[i];
	}
	return ResCode;
}

void GetResult(string &allHtml, int Prob)       ///解析出state.php中的结果，空间，时间
{
	StateAns = "", StateSapce = "", StateTime = "";
	char d[200];
	_itoa(ProblemID, d, 10);
	strcat(d, "</a>");
	int pos = allHtml.find((string)d);
	int Mpos = pos;
	int Tpos;
	if (Mpos == string::npos)return;
	else
	{
		Mpos += 17;
		while (true)
		{
			if (allHtml[Mpos] == '<')
			{
				Tpos = Mpos;
				break;
			}
			StateSapce += allHtml[Mpos];
			Mpos++;
		}
		cout << "空间： " << StateSapce << endl;
	}
	Tpos += 9;
	while (true)
	{
		if (allHtml[Tpos] == '<')break;
		StateTime += allHtml[Tpos];
		Tpos++;
	}
	cout << "耗时： " << StateTime << endl;
	if (pos == string::npos)return;
	else
	{
		pos = pos - 52;
		int begin;
		while (true)
		{
			if (allHtml[pos] == '>')
			{
				begin = pos;
				break;
			}
			pos--;
		}
		for (int i = begin + 1; allHtml[i] != '<'; i++)StateAns += allHtml[i];
	}
	cout << "结果： " << "---------------::::::" << StateAns << endl;
}
char* U2G(const char* utf8)         ///UTF-8 to GB2312
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

char* G2U(const char* gb2312)       ///GB2312 TO UTF-8
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}
int main()
{
	int start, end, n;
	string PHPSESSID, CNZZDATA;
	cout << "请输入您的 'PHPSESSID'" << endl;
	cin >> PHPSESSID;
	cout << "请输入您的 'CNZZDATA'" << endl;
	cin >> CNZZDATA;
	cout << "请输入开始题号和结束题号" << endl;
	cin >> start >> end;
	for (ProblemID = start; ProblemID < end; ProblemID++)
	{
		allHtml = "";
		strcpy(host, "cn.bing.com");
		strcpy(othPath, "/search?q=hdu+");
		_itoa(ProblemID, s, 10);
		strcat(othPath, s);
		strcat(othPath, "+csdn");
		SendRequst(host, othPath);
		while ((n = recv(sock, buf, sizeof(buf) - 1, 0)) > 0)
		{
			buf[n] = 0;
			allHtml += buf;
		}
		closesocket(sock);
		GetCSDNurl(allHtml);  //提取博客网址
		//cout << allHtml << endl;   //输出整个网页
		for (int i = 0; i < (int)min(blogUrl.size(), 3); i++)        //最多提交3次
		{
			allHtml = "";
			cout << blogUrl[i] << endl;                     ///输出博客链接
			char tmp[400];
			strcpy(tmp, blogUrl[i].c_str());
			if (analyUrl(tmp) == false)
			{
				cout << "analyUrl函数错误！" << endl;
				return 0;
			}
			SendRequst(host, othPath);
			while ((n = recv(sock, buf, sizeof(buf) - 1, 0)) > 0)
			{
				buf[n] = 0;
				allHtml += buf;
			}
			closesocket(sock);
			GetCode(allHtml);
			CodeHtml = HTMLTOC(CodeHtml);
			char  *p = U2G(CodeHtml.c_str());               ///处理汉字，UTF-8编码转为GB
			CodeHtml = string(p);
			//cout << CodeHtml;						//输出当前题目的代码
			strcpy(tmp, "http://acm.hdu.edu.cn/submit.php?action=submit");
			if (analyUrl(tmp) == false)
			{
				cout << "analyUrl函数错误！" << endl;
				return 0;
			}
			ResCode = GetRescode((string)p);
			//cout << ResCode;
			SendCode(host, othPath, ResCode, PHPSESSID, CNZZDATA);
			while ((n = recv(sock, buf, sizeof(buf) - 1, 0)) > 0)
			{
				buf[n] = '\0';
				cout << buf;
			}
			char temp[400];
			strcpy(temp, "http://acm.hdu.edu.cn/status.php");
			if (analyUrl(temp) == false)
			{
				cout << "analyUrl函数错误！" << endl;
				return 0;
			}
			SendRequst(host, othPath);
			allHtml = "";
			while ((n = recv(sock, buf, sizeof(buf) - 1, 0)) > 0)
			{
				buf[n] = '\0';
				allHtml += buf;
			}
			GetResult(allHtml, ProblemID);
			closesocket(sock);
			Sleep(3000);
		}
	}
	return 0;
}
