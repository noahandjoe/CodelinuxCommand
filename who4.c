//do with buffered reads

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <utmp.h>

#define SHOWHOST

void show_time(long int);
void show_time2(long int);
void show_info(struct utmp*);

int main(int argc, char const *argv[])
{
    struct utmp* utbufp;
    struct utmp* utmp_next();

    if(utmp_open(UTMP_FILE) == -1){
        perror(UTMP_FILE);
        exit(1);
    }

    while((utbufp = utmp_next()) != (struct utmp*)NULL)
        show_info(utbufp);
    
    utmp_close();

    return 0;
}

//displays the contents of the utmp struct
//in human readable form
//displays nothing if recond has no user name
void show_info(struct utmp* utbufp)
{
    //因为在utmplib2.c文件中把无用的结果过滤掉了,所以此处无需判断
    // if(utbufp->ut_type != USER_PROCESS)
    //     return;
    
    printf("%-8.8s", utbufp->ut_user);
    printf(" ");
    printf("%-12.8s", utbufp->ut_line);
    printf(" ");
    show_time2(utbufp->ut_tv.tv_sec);

    #ifdef SHOWHOST
    //只显示有地址的结果
    if(utbufp->ut_host[0] != '\0')
        printf("(%s)", utbufp->ut_host);
    #endif
    
    printf("\n");
    //joe      pts/0        2023-03-27 13:40 (192.168.111.1)
}

//old version
//format the time 
void show_time(long int timeval){
    char* cp = ctime(&timeval); //"Wed Jun 30 21:49:08 1993\n"
    printf("%12.12s", cp+4);  
    //Jun 30 21:49 
}

//new version
void show_time2(long int timeval){
    struct tm* time_info;
    char time_str[20];

    time_info = localtime(&timeval);
    // 将时间格式化为指定格式的字符串
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", time_info);
    //2023-03-27 13:40 

    printf("%s ",time_str);
}