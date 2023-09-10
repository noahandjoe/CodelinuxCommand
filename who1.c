//open, read UTMP file, and show results

#include <stdio.h>
#include <utmp.h>       //utmp struct
#include <fcntl.h>      //open
#include <unistd.h>     //read
#include <stdlib.h>     //exit
#include <time.h>       //ctime

#define SHOWHOST

void show_info(struct utmp* utbufp);

int main(int argc, char const *argv[])
{
    struct utmp current_recond; //utmp格式变量用来记录用户登录信息
    //utmp 记录了当前登录系统的用户信息、登出信息、系统启动和关闭的时间信息等。
    int utmpfd;
    int reclen = sizeof(current_recond);

    // UTMP_FILE用于表示系统中utmp文件的路径, 指的是 "/var/run/utmp"
    // 这里打开的就是utmp文件
    if((utmpfd = open(UTMP_FILE, O_RDONLY)) == -1){
        perror(UTMP_FILE);  
        exit(1);
    }

    //循环读取 utmp 文件中的记录，每次读取一个 current_recond 的结构体变量
    while(read(utmpfd, &current_recond, reclen) == reclen)
        show_info(&current_recond);

    return 0;
}

void show_info(struct utmp* utbufp){
    // -是左对齐，8 是指定输出字符串占用的宽度，
    //.8 是指截取字符串的长度为8，
    //最后的 s 是字符串类型。
    printf("%-8.8s ", utbufp->ut_user);
    printf("%-8.8s ", utbufp->ut_line);
    //注意: 在 Linux 中，ut_time 已经被废弃了。
    printf("%10ld ", (long int)utbufp->ut_tv.tv_sec);

    #ifdef SHOWHOST
    printf("(%s)", utbufp->ut_host);
    #endif
    printf("\n");
}

//目前代码还存在的问题
//消除空白记录
//正确显示时间



/*
struct utmp 结构体中包含了以下成员：

    ut_type：登录类型，包括如 INIT_PROCESS、LOGIN_PROCESS、USER_PROCESS 和 DEAD_PROCESS 等。
    ut_pid：登录进程的进程 ID。
    ut_line：登录用户所使用的设备名，例如 tty1、pts/1 等。
    ut_id：登录进程的 ID，通常指在 /etc/inittab 文件中定义的 id。
    ut_user：登录用户的用户名。
    ut_host：远程登录的主机名。
    ut_exit：记录进程退出状态的结构体，只有 ut_type 为 DEAD_PROCESS 时才有意义。
    ut_session：会话 ID，用于窗口管理。
    ut_tv：记录记录登录或注销时间的时间结构体，包括秒数和微秒数。
    ut_addr_v6：远程主机的 IPv6 地址。
    __glibc_reserved：保留字段，目前没有使用。

utmp 数据库通常可以在 /var/run/utmp 或者 /var/log/wtmp 文件中找到。
使用 utmp 数据库可以监控用户登录和注销信息，以及查看当前系统中登录的用户信息。

--------------------------------------------
ctime 用于将时间戳转换为本地时间的字符串表示:
char* ctime(const time_t* timep);
    time_t* : 就是long int
    timep: 要转换的时间戳（秒）

返回: 格式为 www mmm dd hh:mm:ss yyyy\n 的时间字符串指针
    www是星期几的缩写，如“Mon”、“Tue”等。
    mmm是月份的缩写，如“Jan”、“Feb”等。
    dd表示一个月中的第几天。
    hh表示小时（24小时制）。
    mm表示分钟。
    ss表示秒。
    yyyy表示年份。

*/