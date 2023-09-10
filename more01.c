/*************************************************************************
    > File Name: more01.c
    > Author: Joe
    > Mail: Joe@xxx.com
    > Created Time: Friday, March 24, 2023 PM09:44:20 HKT
 ************************************************************************/

//实现一个more指令
//读取打印24行，然后暂停执行一些指令
#include <stdio.h>
#include <stdlib.h>

#define PAGELEN 24 		//行数
#define LINELEN 512		//每行最大长度

void do_more(FILE*);
int see_more();

int main(int ac, char* av[]){
	FILE* fp;
	//判断命令行参数数量
	if(ac == 1)		
		do_more(stdin); //从管道的输出读取进输入流
	else
		while(--ac){
			//依次读取每个参数指定的文件
			if((fp = fopen(*++av, "r")) != NULL){
				//显示文件内容
				do_more(fp);	
				fclose(fp); //关闭文件
			}else
				exit(1);
		}

	return 0;	
}

void do_more(FILE* fp){
	//读取一页的内容，然后执行see_more()读取进一步说明
	char line[LINELEN];
	int num_of_lines = 0;
	int see_more(), reply;

	//读取每一行
	while(fgets(line, LINELEN, fp)){	
		//如果读满了一页
		if(num_of_lines == PAGELEN){	
			reply = see_more();
			if(reply == 0)
				break;
			//如果读取一页，num_of_lines又变成0了，新的一页开始
			//如果读取一行，则下次循环又满足 num_of_lines == PAGELEN 条件 ，继续输入指令
			num_of_lines -= reply;
		}		
		//输出一行
		if(fputs(line, stdout) == EOF)
			exit(1);
		num_of_lines++;
	}
}

int see_more(){
	//等待响应 q-退出，空格翻页，回车显示一行
	int c;
	// \033[7m 是设置反显（即黑底白字）的控制码，\033[m 则是关闭反显的控制码
	printf("\033[7m more? \033[m");
	while((c = getchar()) != EOF){
		if(c=='q')
			return 0;
		if(c==' ')
			return PAGELEN;
		if(c=='\n')
			return 1;
	}
	return 0;
}

//目前代码的问题: 
//more也会随着文字上滚
//输入指令后必须要按回车
//以输入流的形式读取会一次性全部显示