/*
/dev/tty: 键盘和显示器设备描述文件
向该文件写->显示在用户屏幕
向该文件读->从键盘获取用户输入

当more需要用户输入可以从/dev/tty得到数据
*/

#include <stdio.h>
#include <stdlib.h>

#define PAGELEN 24
#define LINELEN 512

void do_more(FILE*);
int see_more(FILE*);

/*
    main 函数根据命令行参数打开并读取文件，
    如果没有命令行参数则从标准输入读取。
    调用 do_more 函数来执行分页显示的操作。
*/
int main(int argc, char const *argv[])
{
    FILE* fp;
    if(argc == 1)
        do_more(stdin); // 没有参数时读取标准输入
    else
        while(--argc){
            // 逐个打开文件并读取
            if((fp = fopen(*++argv, "r")) != NULL){ 
                do_more(fp);
                fclose(fp);
            }else
                exit(1);
        }

    return 0;
}

/*
do_more 函数对于每个文件，从文件中读取内容并分页显示，
等待用户按下空格或回车以继续显示，或者按下 q 退出显示。
*/
void do_more(FILE* fp){
    char line[LINELEN];
    int num_of_lines = 0;   //已经显示的行数
    int see_more(FILE*), reply;
    FILE* fp_tty;   //指向 /dev/tty 设备文件的文件指针
    //该设备文件指向当前正在使用的终端设备，用来接收用户的输入。
    fp_tty = fopen("/dev/tty","r"); 

    if(fp_tty == NULL){
        exit(1);
    }

    while(fgets(line, LINELEN, fp)){
        //当显示行数达到 PAGELEN 定义的行数时就停止并调用 see_more 函数等待用户输入
        if(num_of_lines == PAGELEN){
            reply = see_more(fp_tty);
            if(reply == 0)
                break;
                
            num_of_lines -= reply;  //reset count
        }

        if(fputs(line, stdout) == EOF)
            exit(1);
        num_of_lines++;
    }
}

int see_more(FILE* cmd){
    int c;
    printf("\033[7m more? \033[m");
    //使用 getc 函数从 fp_tty 文件指针中获取用户的输入。
    while((c=getc(cmd)) != EOF){
        //按下 q 时，退出函数并返回 0，表示不再显示更多内容。
        if(c=='q')
            return 0;
        //按下空格时，返回 PAGELEN 定义的行数，表示继续显示下一页。
        if(c==' ')
            return PAGELEN;
        //按下回车时，返回 1，表示只显示下一行内容。
        if(c=='\n')
            return 1;
    }
    return 0;
}


/*

getc 和 getchar 有什么区别
相同:都是 C 语言中用于从输入流中读取一个字符的函数

不同:
- getchar 从标准输入流（通常是键盘）中读取一个字符。
    它是 C 标准库中的函数，可以不传递参数，也可以传递参数 stdin，
    表示从标准输入流中读取一个字符。
- getc 也可以从标准输入流中读取一个字符，但它还可以从其他流中读取一个字符。
    它需要传递一个 FILE 类型的指针参数，表示从哪个流中读取一个字符。
*/

//代码存在的问题
//more也会随着文字上滚
//输入指令后还要按回车键,而且输入的指令会显示
