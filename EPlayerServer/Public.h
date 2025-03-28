#pragma once
#include <string>
class Buffer :public std::string
{
public:
	Buffer() :std::string() {}
	Buffer(size_t size) :std::string() { resize(size); }
	Buffer(const std::string& str) :std::string(str) {}
	Buffer(const char* str) :std::string(str) {}
	Buffer(const char* str, size_t length) :std::string() {
		resize(length);
		memcpy((char*)c_str(), str, length);
	}
	Buffer(const char* begin, const char* end) :std::string() {
		long int len = end - begin;
		if (len > 0) {
			resize(len);
			memcpy((char*)c_str(), begin, len);
		}
	}
	operator char* () { return (char*)c_str(); }
	operator char* () const { return (char*)c_str(); }
	operator const char* () const { return c_str(); }
};

enum SockAttr {
	SOCK_ISSERVER = 1,//是否服务器 1表示是 0表示客户端
	SOCK_ISNONBLOCK = 2,//是否阻塞 1表示非阻塞 0表示阻塞
	SOCK_ISUDP = 4,//是否为UDP 1表示udp 0表示tcp
	SOCK_ISIP = 8,//是否为IP协议 1表示IP协议 0表示本地套接字
};