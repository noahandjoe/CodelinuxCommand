#include <cstdio>
#include <unistd.h>

using namespace std;

int main(){
    char buf[256];
    if(getcwd(buf, sizeof(buf))!=NULL){
        printf("%s\n", buf);
    }else{
        perror("getcwd() error");
        return -1;
    }
    return 0;
}