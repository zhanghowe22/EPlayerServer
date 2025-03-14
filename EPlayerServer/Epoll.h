#pragma once
#include <unistd.h>
#include <sys/epoll.h>
#include <vector>

class EpollData
{
public:
	EpollData() { m_data.u64 = 0; }
	EpollData(void* ptr) { m_data.ptr = ptr; }
	explicit EpollData(int fd) { m_data.fd = fd; }
	explicit EpollData(uint32_t u32) { m_data.u32 = u32; }
	explicit EpollData(uint64_t u64) { m_data.u64 = u64; }
private:
	epoll_data_t m_data;
};

using EPEvents = std::vector<epoll_event>;

class CEpoll
{
public:
	CEpoll();

	~CEpoll();

	CEpoll(const CEpoll&) = delete;

	CEpoll& operator=(const CEpoll&) = delete;

	operator int()const { return m_epoll; }

	int Create(unsigned count);

	// 小于0表示错误 等于0表示没有事情发生 大于0代表成功拿到事件
	ssize_t WaitEvents(EPEvents& events, int timeout = 10);

	int Add(int fd, const EpollData& data = EpollData((void*)0), uint32_t events = EPOLLIN);

	int Modify(int fd, uint32_t events, const EpollData& data = EpollData((void*)0));

	int Del(int fd);

	void Close();

private:
	int m_epoll;
};

