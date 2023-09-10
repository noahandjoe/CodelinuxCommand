#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[])
{
    if(argc < 2){
        cout<<"Usage: "<<argv[0]<<" directory_path"<<endl;
        return 1;
    }

    // 获取当前工作目录
    char currentDir[1024];
    if(getcwd(currentDir, sizeof(currentDir)) == NULL){
        cout<<"Failed to get current working directory."<<endl;
        return 1;
    }

    // 判断目录是否存在
    struct stat dirStat;
    if(stat(argv[1], &dirStat) != 0){
        cout<<"The directory does not exist."<<endl;
        return 1;
    }

    // 判断是否是目录
    if(!S_ISDIR(dirStat.st_mode)){
        cout<<"The path is not a directory."<<endl;
        return 1;
    }

    // 切换目录
    if(chdir(argv[1])!=0){
        cout<<"Failed to change directory."<<endl;
        return 1;
    }
    // system("ls -la");

    // 获取新的工作目录
    char newDir[1024];
    if(getcwd(newDir, sizeof(newDir)) == NULL){
        cout<<"Failed to get current working directory."<<endl;
        return 1;
    }

    cout<<"cd "<<newDir<<endl;

    // cout<<"Changed directory from "<<currentDir<<" to "<<newDir<<endl;

    return 0;
}
