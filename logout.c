#include <utmp.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>

//终端注销 
//将指定终端的utmp记录的类型修改为DEAD_PROCESS，并更新时间戳
//返回-1 on err, 0 on succ
int logout_tty(char* line){
    int fd; 
    struct utmp rec;
    int len = sizeof(struct utmp);
    int retval = -1;

    //打开utmp文件
    if((fd = open(UTMP_FILE, O_RDWR)) == -1)
        return -1;
    
    //循环读取UTMP_FILE文件中的utmp记录， 一次读取一条,寻找指定终端
    //读取的结果保存在rec变量中
    while(read(fd, &rec, len) == len){
        //如果读取到的记录的设备名称与传入的参数line一致，则表示找到了指定的终端。
        if(strncmp(rec.ut_line, line, sizeof(rec.ut_line)) == 0){
            //更新utmp记录的类型为DEAD_PROCESS
            rec.ut_type = DEAD_PROCESS;
            //使用time函数获取当前时间戳，并将其保存在utmp记录的ut_tv.tv_sec字段中。
            //time返回当前时间并将其放入*TIMER中
            if(time(&rec.ut_tv.tv_sec) != -1)
                //将文件位置指针移到当前记录的前一个记录的位置（-len）
                if(lseek(fd,-len,SEEK_CUR) != -1)
                    //将更新后的utmp记录写入文件中
                    if(write(fd, &rec, len) == len)
                        retval = 0;
            break;
        }
    }

    if(close(fd) == -1)
        retval = -1;
    return retval;
}

    
int main(int argc, char const *argv[])
{
    char* line = ttyname(STDIN_FILENO)+5;
    printf("logout: %s\n", line);

    int ret = logout_tty(line);
    if(ret == -1){
        perror("logout");
    }

    return 0;
}
