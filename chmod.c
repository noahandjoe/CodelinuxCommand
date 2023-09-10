#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

void usage(char* name){
    printf("Usage: %s mode file\n", name);
    printf("  mode - the new mode of the file, using format 'ugoa+-rwx'\n");
    printf("  file - the file to modify\n");
}

int main(int argc, char *argv[])
{
    if(argc != 3){
        usage(argv[0]);
        return -1;
    }

    char* mode_str = argv[1];
    char* filename = argv[2];

    //解析权限掩码
    __mode_t mode = 0;
    for(int i = 0; i<strlen(mode_str); i++){
        if(mode_str[i] == 'u'){
            mode |= S_IRUSR | S_IWUSR | S_IXUSR;
        }else if(mode_str[i] == 'g'){
            mode |= S_IRGRP | S_IWGRP | S_IXGRP;
        }else if(mode_str[i] == 'o'){
            mode |= S_IROTH | S_IWOTH | S_IXOTH;
        }else if(mode_str[i] == 'a'){
            mode |= S_IRUSR | S_IWUSR | S_IXUSR |
                    S_IRGRP | S_IWGRP | S_IXGRP |
                    S_IROTH | S_IWOTH | S_IXOTH;
        }else if(mode_str[i] == '+'){
            i++;
            while(i<strlen(mode_str)){
                if(mode_str[i] == 'r'){
                    mode |= S_IRUSR | S_IRGRP | S_IROTH;
                }else if(mode_str[i] == 'w'){
                    mode |= S_IWUSR | S_IWGRP | S_IWOTH;
                }else if(mode_str[i] == 'x'){
                    mode |= S_IXUSR | S_IXGRP | S_IXOTH;
                }else{
                    printf("Error: invalid permission '%c'\n", mode_str[i]);
                    return -1;
                }
                i++;
            }
        }else if(mode_str[i] == '-'){
            i++;
            while(i<strlen(mode_str)){
                if(mode_str[i] == 'r'){
                    mode &= ~(S_IRUSR | S_IRGRP | S_IROTH);
                }else if(mode_str[i] == 'w'){
                    mode &= ~(S_IWUSR | S_IWGRP | S_IWOTH);
                }else if(mode_str[i] == 'x'){
                    mode &= ~(S_IXUSR | S_IXGRP | S_IXOTH);
                }else{
                    printf("Error: invalid permission '%c'\n", mode_str[i]);
                    return -1;
                }
                i++;
            }
        }
    }

    if(chmod(filename, mode) == -1){
        perror("chmod");
        return -1;
    }

    return 0;
}
