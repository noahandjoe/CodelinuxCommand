#include <iostream>
#include <string>
#include <map>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>

using namespace std;

map<string, string> aliases;

void add_alias(const char* name, const char* command){
    string key(name), value(command);
    aliases[key] = value;
}

void print_aliases(){
    for(auto it = aliases.begin(); it!=aliases.end(); ++it){
        cout << it->first << "=\"" << it->second << "\"" << endl;
        // key="value"
    }
}

void execute_command(const char* command){
    string cmd(command);
    auto it = aliases.find(cmd);
    if(it!=aliases.end()){
        //找到了该命令的别名,使用 system 函数执行该别名对应的命令
        //c_str()函数将std::string类型的字符串转换为C风格的字符串
        system(it->second.c_str());
    }else{
        //没有找到该命令的别名,执行该命令字符串
        system(command);
    }
}

int main(){
    char buf[1024];
    while(true){
        printf("myshell> ");
        fgets(buf, sizeof(buf), stdin);
        buf[strlen(buf) - 1] = '\0'; //去掉换行符
        if(strncmp(buf, "alias ", 6) == 0){
            const char* name = strtok(&buf[6],"="); //分割后的别名
            const char* command = strtok(NULL, ""); //剩下的是对应命令
            //先存到别名表里
            add_alias(name, command);
        }else if(strcmp(buf, "$alias") == 0){
            //列出所有别名
            print_aliases();
        }else if(strcmp(buf, "exit") == 0){
            //退出
            break;
        }else{
            //执行命令
            execute_command(buf);
        }
    }
    return 0;
}