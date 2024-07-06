#include "headers.h"

int prefix(const char *pre, const char *str){
    int pi = 0;
    int si = 0;

    if (pre == NULL || str == NULL){
        return 1;
    }

    while(pre[pi] != '\0'){
        if(str[si] == '\0'){ return 1; }
        if(str[si] != pre[pi]){ return 1; }
        pi++; si++;
    }

    return 0;
}

int prompt(char* home, int len) {
    // Do not hardcode the prmopt
    // printf("<Everything is a file> ");  
    // char* user = getenv("USERNAME");
    struct passwd* user = getpwuid(getuid());
    if(user == NULL){
        perror(RED"username"RESET);
        return 1;
    }

    struct utsname device;
    if(uname(&device) == -1){
        perror(RED"device name"RESET);
        return 1;
    }

    printf("<\033[1;32m%s@%s\033[0m:", user->pw_name, device.nodename);

    char* curr_dir = getcwd(NULL, 0); // cwd becomes ~
    if(curr_dir == NULL){
        printf("\033[1;31mCouldn't get current directory.\033[0m\n");
        return 1;
    }

    int tilde = prefix(home, curr_dir);
    if(tilde == 0){
        printf("\033[1;34m~%s\033[0m", curr_dir+len);
    }
    else{
        printf("\033[1;34m%s\033[0m", curr_dir);
    }

    int difftime = (int)((end_time.tv_sec - start_time.tv_sec)+(end_time.tv_usec - start_time.tv_usec)/1000000);
    if(difftime > 2){
        printf(" %s : %ds>", commandtoprint, difftime);

    }
    else{
        printf(">");
    }

    if(curr_dir != NULL){
        free(curr_dir);
    }

    start_time.tv_sec = 0;
    start_time.tv_usec = 0;
    end_time.tv_sec = 0;
    end_time.tv_usec = 0;
    strcpy(commandtoprint, "\0");

    return 0;
  
}
