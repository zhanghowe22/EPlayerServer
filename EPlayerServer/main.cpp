#include <cstdio>
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
class CFunction: public CFunctionBase
{
public:
    CFunction(_FUNCTION_ func, _ARGS_... args)
    {
    }

    virtual ~CFunction() {}

    virtual int operator()() {
        return m_binder;
    }

    // m_binder 是一个可调用对象，封装了指定的函数（_FUNCTION_）和参数（_ARGS_...），并返回 int 类型的结果。
    std::_Bindres_helper<int, _FUNCTION_, _ARGS_...>::type m_binder; //指定函数对象及其参数，std::bind会返回一个可调用对象
};

// 管理进程的创建和入口函数的设置
class CProcess
{
public:
    CProcess() {
        m_func = NULL;
        m_pid = -1;
    }

    ~CProcess() {
        if (m_func != NULL) {
            delete m_func;
            m_func = NULL;
        }
    }

    // 设置入口函数
    template<typename _FUNCTION_, typename... _ARGS_> // _FUNCTION_：函数， _ARGS_：参数
    int SetEntryFunction(_FUNCTION_ func, _ARGS_... args) 
    {
        m_func = new CFunction(func, args...);
        return 0;
    }

    // 创建子进程
    int CreateSubProcess() {
        if (m_func == NULL) return -1;
        pid_t pid = fork();
        // fork返回值为-1时代表失败，等于0时代表是子进程，其他情况表示是父进程
        if (pid == -1) return -2;
        if (pid == 0) { // 子进程
            return (*m_func)();
        }
        m_pid = pid; // 主进程的pid
        return 0;
    }

private:
    CFunctionBase* m_func;
    pid_t m_pid;
};

int CreateLogServer(CProcess* proc)
{
    return 0;
}

int CreateClientServer(CProcess* proc)
{
    return 0;
}

int main()
{
    CProcess proclog, procclients;

    proclog.SetEntryFunction(CreateLogServer, &proclog);
    int ret = proclog.CreateSubProcess();
    procclients.SetEntryFunction(CreateClientServer, &procclients);
    ret = procclients.CreateSubProcess();

    return 0;
}