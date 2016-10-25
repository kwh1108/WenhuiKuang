//
//  main.c
//  lab1a
//
//  Created by wenhui kuang on 4/6/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define no_argument 0
int pid;
struct termios config;

void signalHandler(int signum)
{
   // signal(signum, SIG_IGN);
    fprintf(stderr,"Caught signal %d\n", signum);
    exit(2);
}

void* threadFunction(void *fd)
{
    char buffer[1];
    int size = 0;
    int file = *(int *)fd;
    while ((size = read(file, buffer, 1)) > 0)
    {
        
        if (buffer[0] == 0x04)
        {
            exit(1);
        }
        else
        {
            write(STDOUT_FILENO, buffer, 1);
        }
    }
    return NULL;
}
void restore()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &config);
}
void restore2()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &config);
    int status;
    if ( waitpid(pid, &status, 0) == -1 ) {
        perror("waitpid failed");
        exit(0);
    }
    printf("exit status was %d\n", WEXITSTATUS(status));
}

int main(int argc, char ** argv) {
    struct termios config2;
    char buffer[1];
    int size = 0;
    int shell_flag = 0, no_arg = 1;
    const struct option longopts[] =
    {
        {"shell", no_argument, NULL, 's'},
    };
    int iarg;
    while ((iarg = getopt_long(argc,argv,"s",longopts,NULL)) != -1){
        switch (iarg){
            case 's':
                shell_flag = 1;
                no_arg = 0;
                break;
        }
    }
    if ((tcgetattr(STDIN_FILENO, &config) < 0) || (tcgetattr(STDIN_FILENO, &config2) < 0))
    {
        fprintf(stderr, "Can not get the attributes.\n");
        exit(0);
    }
    atexit(restore);
    config2.c_lflag &= ~(ICANON | ECHO);
    config2.c_cc[VMIN] = 1;
    config2.c_cc[VTIME] = 0;
    if(tcsetattr(STDIN_FILENO, TCSANOW, &config2) < 0)
    {
        fprintf(stderr, "Can not set the attributes.\n");
        exit(0);
    }
    if (no_arg == 1)
    {
        while ((size = read(STDIN_FILENO, buffer, 1) > 0))
        {
            if (buffer[0] == 0x04)
            {
                exit(0);
            }
            else if (buffer[0] == 0x0D || buffer[0] == 0x0A) {
                buffer[0] = 0x0D;
                write(STDOUT_FILENO, buffer, 1);
                buffer[0] = 0x0A;
                write(STDOUT_FILENO, buffer, 1);
            }
            else
            {
                write(STDOUT_FILENO, buffer, 1);
            }
        }
    }
    if (shell_flag == 1)
    {
        int pipe_from_shell[2];
        int pipe_to_shell[2];
        if (pipe(pipe_from_shell) != 0)
        {
            fprintf(stderr, "Pipe function fails.\n");
        }
        if (pipe(pipe_to_shell) != 0)
        {
            fprintf(stderr, "Pipe function fails.\n");
        }
        pid = fork();
        if (pid == 0) {
            close(pipe_from_shell[0]);
            dup2(pipe_from_shell[1], 1);
            close(pipe_to_shell[1]);
            dup2(pipe_to_shell[0], 0);
            execl("/bin/bash", "/bin/bash",NULL);
        }
        else
        {
            atexit(restore2);
            char buffer[0];
            int size = 0;
            close(pipe_to_shell[0]);
            close(pipe_from_shell[1]);
            pthread_t tid;
            int ret = pthread_create(&tid, NULL, threadFunction, (void *)&pipe_from_shell[0]);
            if (ret) {
                printf("Error creating thread. Error code is %d\n", ret);
                exit(1);
            }
            while ((size = read(STDIN_FILENO, buffer, 1)) > 0)
            {
                signal(SIGPIPE, signalHandler);
                signal(SIGINT, signalHandler);
                signal(SIGHUP, signalHandler);
                if (buffer[0] == 0x03)
                {
                    kill(pid, SIGINT);
                }
                else if (buffer[0] == 0x04)
                {
		    close(pipe_from_shell[0]);
                    close(pipe_to_shell[1]);
                    kill(pid, SIGHUP);
                    exit(0);
                }
                else if (buffer[0] == 0x0D || buffer[0] == 0x0A)
                {
                    buffer[0] = 0x0D;
                    write(STDOUT_FILENO, buffer, 1);
                    buffer[0] = 0x0A;
                    write(STDOUT_FILENO, buffer, 1);
                    write(pipe_to_shell[1], buffer, 1);
                }
                else
                {
                    write(STDOUT_FILENO, buffer, 1);
                    write(pipe_to_shell[1], buffer, 1);
                }
            }
        }
    }
    
       
}
