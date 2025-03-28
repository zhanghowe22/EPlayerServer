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
	SOCK_ISSERVER = 1,//�Ƿ������ 1��ʾ�� 0��ʾ�ͻ���
	SOCK_ISNONBLOCK = 2,//�Ƿ����� 1��ʾ������ 0��ʾ����
	SOCK_ISUDP = 4,//�Ƿ�ΪUDP 1��ʾudp 0��ʾtcp
	SOCK_ISIP = 8,//�Ƿ�ΪIPЭ�� 1��ʾIPЭ�� 0��ʾ�����׽���
};