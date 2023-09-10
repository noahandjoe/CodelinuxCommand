#include <utmp.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


//终端注销 
//将指定终端的utmp记录的类型修改为DEAD_PROCESS，并更新时间戳
//返回-1 on err, 0 on succ
int logout_tty(char* line){
    int fd; 
    struct utmp rec;
    int len = sizeof(struct utmp);
    int retval = -1;

    struct utmp* utbufp;
    struct utmp* utmp_next();

    //打开utmp文件
    if((fd = utmp_open(UTMP_FILE)) == -1){
        perror(UTMP_FILE);
        exit(EXIT_FAILURE);
    }

    while((utbufp = utmp_next()) != (struct utmp*)NULL){

            //如果读取到的记录的设备名称与传入的参数line一致，则表示找到了指定的终端。
            if(strncmp(utbufp->ut_line, line, sizeof(rec.ut_line)) == 0){
                rec.ut_type = DEAD_PROCESS;
                time_t timer;
                time(&timer);
                rec.ut_tv.tv_sec = timer;
                strcpy(rec.ut_user, "");
                strcpy(rec.ut_host, "");
                strcpy(rec.ut_line, line);

                if(utmp_seek() != -1){

                    if(write(fd, &rec, len) == len){
                    retval = 0;
                        break;
                    }else{
                        perror("write");
                        exit(EXIT_FAILURE);
                    }
                    
                }

                retval = -1;
                break;
            }

    }

    utmp_close();
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
