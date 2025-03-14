#pragma once
#include <sys/types.h>
#include <unistd.h>
#include <functional>

class CFunctionBase
{
public:
	virtual ~CFunctionBase() {}
	virtual int operator()() = 0;
};

// 模板类
template<typename _FUNCTION_, typename... _ARGS_>
class CFunction : public CFunctionBase
{
public:
	CFunction(_FUNCTION_ func, _ARGS_... args)
		:m_binder(std::forward<_FUNCTION_>(func), std::forward<_ARGS_>(args)...)
	{
	}

	virtual ~CFunction() {}

	virtual int operator()() {
		return m_binder();
	}

	// m_binder 是一个可调用对象，封装了指定的函数（_FUNCTION_）和参数（_ARGS_...），并返回 int 类型的结果。
	typename std::_Bindres_helper<int, _FUNCTION_, _ARGS_...>::type m_binder; //指定函数对象及其参数，std::bind会返回一个可调用对象
};