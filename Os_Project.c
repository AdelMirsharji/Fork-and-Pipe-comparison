#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>

#define Line_Size 128 
#define Max_Number_of_lines 30

int main(void) 
{
    char line[Max_Number_of_lines][Line_Size];
    FILE *fptr = NULL; 
    fptr = fopen("Commands.txt", "r");

    int i = 0;
    while(fgets(line[i], Line_Size, fptr)) 
	{
        line[i][strlen(line[i]) - 1] = '\0';
        i++;
    }

    int number_of_commands = i;
    int number_of_first_forks = line[0][0]-'0';
    int number_of_second_forks = line[0][2]-'0';    

    printf("\n number of first forks = %d \n number of second forks = %d \n",number_of_first_forks,number_of_second_forks);
	printf("\n The commands are: \n");    
    for(i = 1; i <= number_of_first_forks*number_of_second_forks ; ++i)
    {
        printf(" %s\n", line[i]);
    }
    printf("\n");

    int p1[2],p2[2],p3[2];
    char pipe_number_one_buffer[1000];
    char pipe_number_two_buffer[1000];
    char pipe_number_three_buffer[1000];

    if(pipe(p1) < 0){
        printf("First pipe failed ... exiting!\n");
        return 0;
    }

    if(pipe(p2) < 0){
        printf("Second pipe failed ... exiting!\n");
        return 0;
    }

    if(pipe(p3) < 0){
        printf("Second pipe failed ... exiting!\n");
        return 0;
    }

    for(i = 1 ; i < number_of_commands ; ++i)
        write(p1[1],line[i],sizeof(line[i]));

    pid_t pid = 1, wait_pid;
    int status = 0;
    int p_counter_of_commands_write = 1;
    int c_counter_of_commands_write = 1;
    char commands[Max_Number_of_lines][Line_Size];
    char msg[] = {"Second stage child successfully executed command!"};
    int n = number_of_first_forks * number_of_second_forks;
    int m = number_of_second_forks;

    printf("***************************************************************************\n");
    for(int j = 1 ; j < number_of_commands ; ++j)
        write(p1[1],line[j],sizeof(line[j]));

    for(i = 0 ; i < number_of_first_forks ; ++i){
        if(pid != 0)
            pid = fork();
    }

    if(pid != 0 ){
        struct timeval start, end;
        gettimeofday(&start, NULL);
        while ( n > 0) {
            wait_pid = wait(&status);
            read(p2[0],pipe_number_two_buffer,sizeof(msg));
            printf("%s\n",pipe_number_two_buffer);
            --n;  
        }
        gettimeofday(&end, NULL);
        double time_taken = end.tv_sec + end.tv_usec / 1e3 - start.tv_sec - start.tv_usec / 1e3;
        printf("***************************************************************************\n");
        printf("\nTime taken to to execute %d commands using %d first stage %d second stage fork is = %lf milliseconds\n\n",(number_of_first_forks*number_of_second_forks),number_of_first_forks,number_of_second_forks,time_taken);
    }

    if(pid == 0){

        for(int j = 0 ; j < number_of_second_forks ; ++j){
            read(p1[0],pipe_number_one_buffer,sizeof(line[1]));
            for(int k = 0 ; k < sizeof(pipe_number_one_buffer) ; ++k)
                commands[j][k] = pipe_number_one_buffer[k];
        }

        pid = 1;
        int temp = -1;
        for(int j = 0 ; j < number_of_second_forks ; ++j){
            if(pid != 0){
                temp++;
                pid = fork();
            }
        }

        if(pid != 0){
            while ( m > 0) {
                wait_pid = wait(&status);
                read(p3[0],pipe_number_three_buffer,sizeof(msg));
                write(p2[1],pipe_number_three_buffer,sizeof(msg));
                --m;
            }
        }
            
        if(pid == 0){
            char command[10];
            int l = 0;
            for(int k = 6 ; k < sizeof(commands[temp]) ; ++k){
                command[l] = commands[temp][k];
                l++;
            }
            command[l] = '\0';

            int write_end_of_third_pipe = p3[1];
            char file_descriptor[20];
            sprintf(file_descriptor, "%d", write_end_of_third_pipe);
            char *args[] = {"./EXEC.out",file_descriptor,command,NULL};
            execv(args[0],args);
        }
        exit(0);
    } 

    return 0;
}
