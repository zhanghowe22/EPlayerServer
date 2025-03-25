#pragma once
#include "Socket.h"
#include "Epoll.h"
#include "ThreadPool.h"
#include "Process.h"

/* 回调包装类
* 将用户指定的函数（含参数）封装为统一接口 CFunctionBase，用于回调执行。
*/
template<typename _FUNCTION_, typename... _ARGS_>
class CConnectedFunction :public CFunctionBase
{
public:
	CConnectedFunction(_FUNCTION_ func, _ARGS_... args)
		:m_binder(std::forward<_FUNCTION_>(func), std::forward<_ARGS_>(args)...)
	{}
	virtual ~CConnectedFunction() {}
	virtual int operator()(CSocketBase* pClient) {
		return m_binder(pClient);
	}
	typename std::_Bindres_helper<int, _FUNCTION_, _ARGS_...>::type m_binder;
};

template<typename _FUNCTION_, typename... _ARGS_>
class CReceivedFunction :public CFunctionBase
{
public:
	CReceivedFunction(_FUNCTION_ func, _ARGS_... args)
		:m_binder(std::forward<_FUNCTION_>(func), std::forward<_ARGS_>(args)...)
	{}
	virtual ~CReceivedFunction() {}
	virtual int operator()(CSocketBase* pClient, const Buffer& data) {
		return m_binder(pClient, data);
	}
	typename std::_Bindres_helper<int, _FUNCTION_, _ARGS_...>::type m_binder;
};

/* 业务逻辑抽象基类
* 定义业务处理接口，支持设置两类回调：
* 连接回调（m_connectedcallback）：客户端连接时触发。
* 数据接收回调（m_recvcallback）：收到客户端数据时触发
* 通过模板方法 setConnectedCallback 和 setRecvCallback 绑定任意函数
*/
class CBusiness
{
public:
	CBusiness()
		:m_connectedcallback(NULL), m_recvcallback(NULL)
	{}
	virtual int BusinessProcess(CProcess* proc) = 0;
	template<typename _FUNCTION_, typename... _ARGS_>
	int setConnectedCallback(_FUNCTION_ func, _ARGS_... args) {
		m_connectedcallback = new CConnectedFunction< _FUNCTION_, _ARGS_...>(func, args...);
		if (m_connectedcallback == NULL)return -1;
		return 0;
	}
	template<typename _FUNCTION_, typename... _ARGS_>
	int setRecvCallback(_FUNCTION_ func, _ARGS_... args) {
		m_recvcallback = new CReceivedFunction< _FUNCTION_, _ARGS_...>(func, args...);
		if (m_recvcallback == NULL)return -1;
		return 0;
	}
protected:
	CFunctionBase* m_connectedcallback;
	CFunctionBase* m_recvcallback;
};

// 管理服务端Socket、Epoll事件循环、线程池和子进程
class CServer
{
public:
	CServer();
	~CServer() { Close(); }
	CServer(const CServer&) = delete;
	CServer& operator=(const CServer&) = delete;
public:
	int Init(CBusiness* business, const Buffer& ip = "127.0.0.1", short port = 9999);
	int Run();
	int Close();
private:
	int ThreadFunc();
private:
	CThreadPool m_pool; // 线程池，处理连接和任务
	CSocketBase* m_server; // 服务端Socket，监听客户端连接
	CEpoll m_epoll; // Epoll实例，监听Socket事件
	CProcess m_process; // 子进程管理器，用于业务处理
	CBusiness* m_business;// 业务逻辑回调接口
};

