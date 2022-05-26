#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>

int main(int argc, const char *argv[]){
    
    char msg[] = {"Second stage child successfully executed command!"};
    char file_descriptor = argv[1][0] - '0';
    int seconds_to_sleep = 0;
    sscanf(argv[2], "%d", &seconds_to_sleep);
    seconds_to_sleep = seconds_to_sleep * 1000;
    usleep(seconds_to_sleep);
    write(file_descriptor,"Second stage child successfully executed command!",sizeof(msg));
      
    return 0; 
}