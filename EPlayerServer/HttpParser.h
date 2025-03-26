#pragma once
#include <map>
#include "Socket.h"
#include "http_parser.h"
#include <cstddef>

// Http解析 将C库封装为C++类
class CHttpParser
{
private:
	http_parser m_parser;
	http_parser_settings m_settings;
	std::map<Buffer, Buffer> m_HeaderValues;
	Buffer m_status;
	Buffer m_url;
	Buffer m_body;
	bool m_complete;
	Buffer m_lastField;

public:
	CHttpParser();
	~CHttpParser();
	CHttpParser(const CHttpParser& http);
	CHttpParser& operator=(const CHttpParser& http);

public:
	size_t Parser(const Buffer& data);

	// GET POST HEAD PUT DELETE... 参考http_parser.h HTTP_METHOD_MAP宏
	unsigned Method() const { return m_parser.method; }

	const std::map<Buffer, Buffer>& Headers() { return m_HeaderValues; }

	const Buffer& Status() const { return m_status; }

	const Buffer& Url() const { return m_url; }

	const Buffer& Body() const { return m_body; }

	unsigned Errno() const { m_parser.http_errno; }

protected:
	// 这种设计模式是 C 风格回调与 C++ 类对象交互 的常见解决方案
	// http_parser 是一个 C 语言库，它的回调函数要求是 普通函数 或 静态成员函数（因为 C 语言没有类的概念）。但 CHttpParser 是一个 C++ 类，需要访问成员变量（如 m_url、m_HeaderValues 等）
	// 使用static函数作为C风格回调，使用普通成员函数处理实际逻辑，访问类的成员变量

	static int OnMessageBegin(http_parser* parser);
	static int OnUrl(http_parser* parser, const char* at, size_t length);
	static int OnStatus(http_parser* parser, const char* at, size_t length);
	static int OnHeaderField(http_parser* parser, const char* at, size_t length);
	static int OnHeaderValue(http_parser* parser, const char* at, size_t length);
	static int OnHeadersComplete(http_parser* parser);
	static int OnBody(http_parser* parser, const char* at, size_t length);
	static int OnMessageComplete(http_parser* parser);

	int OnMessageBegin();
	int OnUrl(const char* at, size_t length);
	int OnStatus(const char* at, size_t length);
	int OnHeaderField(const char* at, size_t length);
	int OnHeaderValue(const char* at, size_t length);
	int OnHeadersComplete();
	int OnBody(const char* at, size_t length);
	int OnMessageComplete();

};

class UrlParser {
public:
	UrlParser(const Buffer& url);
	~UrlParser() {}
	int Parser();
	Buffer operator[](const Buffer& name) const;
	Buffer Protocol() const { return m_protocol; }
	Buffer Host() const { return m_host; }
	// 默认返回80
	int Port() const { return m_port; }
	// 设置URL
	void SetUrl(const Buffer& url);

private:
	Buffer m_url;
	Buffer m_protocol;
	Buffer m_host;
	Buffer m_uri;
	int m_port;
	std::map<Buffer, Buffer> m_values; // url的映射
};

