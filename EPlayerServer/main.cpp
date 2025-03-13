#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include <functional>
#include <memory.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

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

// 管理进程的创建和入口函数的设置
class CProcess
{
public:
    CProcess() {
        m_func = NULL;
        m_pid = -1;
        memset(pipes, 0, sizeof(pipes));
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
        m_func = new CFunction<_FUNCTION_, _ARGS_...>(func, args...);
        return 0;
    }

    // 创建子进程
    int CreateSubProcess() {
        if (m_func == NULL) return -1;
        //用于创建一对 互联的无名套接字（anonymous sockets）。这些套接字可以用于进程间通信（IPC），特别是父子进程之间的通信。
        int ret = socketpair(AF_LOCAL, SOCK_STREAM, 0, pipes);
        if (ret == -1) return -2;
        pid_t pid = fork();
        // fork返回值为-1时代表失败，等于0时代表是子进程，其他情况表示是父进程
        if (pid == -1) return -3;

        // 子进程
        if (pid == 0) {
            close(pipes[1]); // 子进程关闭掉写
            pipes[1] = 0;
            ret = (*m_func)();
            exit(0);
        }

        // 主进程
        close(pipes[0]); // 主进程关闭掉读
        pipes[0] = 0;
        m_pid = pid; // 主进程的pid
        return 0;
    }

    // 发送文件描述符
    int SendFD(int fd) { // 主进程完成
        struct msghdr msg {};
		iovec iov[2];
		char buf[2][10] = { "edoyun" , "jueding" };
		iov[0].iov_base = buf[0];
		iov[0].iov_len = sizeof(buf[0]);
		iov[1].iov_base = buf[1];
		iov[1].iov_len = sizeof(buf[1]);
		msg.msg_iov = iov;
		msg.msg_iovlen = 2;

        // 下面的数据才是我们需要传递的
        cmsghdr* cmsg = (cmsghdr*)calloc(1, CMSG_LEN(sizeof(int)));
        if (cmsg == NULL) return -1;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        *(int*)CMSG_DATA(cmsg) = fd; // 这里的fd才是真正想传过去的东西
        msg.msg_control = cmsg;
        msg.msg_controllen = cmsg->cmsg_len;

        ssize_t ret = sendmsg(pipes[1], &msg, 0);

        free(cmsg);

        if (ret == -1) {  
            return -2;
        }
        return 0;
    }

    int RecvFD(int& fd)
    {
        msghdr msg;
        iovec iov[2];
        char buf[][10] = { "", "" };
        iov[0].iov_base = buf[0];
        iov[0].iov_len = sizeof(buf[0]);
		iov[1].iov_base = buf[1];
		iov[1].iov_len = sizeof(buf[1]);
        msg.msg_iov = iov;
        msg.msg_iovlen = 2;

        cmsghdr* cmsg = (cmsghdr*)calloc(1, CMSG_LEN(sizeof(int)));
        if (cmsg == NULL) return -1;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        msg.msg_control = cmsg;
        msg.msg_controllen = CMSG_LEN(sizeof(int));
        ssize_t ret = recvmsg(pipes[0], &msg, 0);

        if (ret == -1) {
            free(cmsg);
            return -2;
        }

        fd = *(int*)CMSG_DATA(cmsg);
        free(cmsg);
        return 0;
    }

private:
    CFunctionBase* m_func; // 入口函数
    pid_t m_pid; // 进程id
    int pipes[2];
};

int CreateLogServer(CProcess* proc)
{
    printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    return 0;
}

int CreateClientServer(CProcess* proc)
{
    printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
	int fd = -1;
	int ret = proc->RecvFD(fd);
	printf("%s(%d):<%s> ret = %d\n", __FILE__, __LINE__, __FUNCTION__, ret);
	printf("%s(%d):<%s> fd = %d\n", __FILE__, __LINE__, __FUNCTION__, fd);
	sleep(1);
	char buf[10] = "";
    lseek(fd, 0, SEEK_SET);
	read(fd, buf, sizeof(buf));
	printf("%s(%d):<%s> buf is: %s\n", __FILE__, __LINE__, __FUNCTION__, buf);
	close(fd);
    return 0;
}

int main()
{
    CProcess proclog, procclients;
    printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    proclog.SetEntryFunction(CreateLogServer, &proclog);
    int ret = proclog.CreateSubProcess();
    if (ret != 0) {
        printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
        return -1;
    }
    printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    procclients.SetEntryFunction(CreateClientServer, &procclients);
    ret = procclients.CreateSubProcess();
    if (ret != 0) {
        printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
        return -2;
    }
    printf("%s(%d):<%s> pid = %d\n", __FILE__, __LINE__, __FUNCTION__, getpid());
    //usleep(100 * 000);
	int fd = open("./test.txt", O_RDWR | O_CREAT | O_APPEND);
	printf("%s(%d):<%s> fd = %d\n", __FILE__, __LINE__, __FUNCTION__, fd);
	if (fd == -1) return -3;
	ret = procclients.SendFD(fd);
	printf("%s(%d):<%s> ret = %d\n", __FILE__, __LINE__, __FUNCTION__, ret);
	write(fd, "edoyun", 6);
	close(fd);

    return 0;
}