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

// ģ����
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

	// m_binder ��һ���ɵ��ö��󣬷�װ��ָ���ĺ�����_FUNCTION_���Ͳ�����_ARGS_...���������� int ���͵Ľ����
	typename std::_Bindres_helper<int, _FUNCTION_, _ARGS_...>::type m_binder; //ָ�����������������std::bind�᷵��һ���ɵ��ö���
};