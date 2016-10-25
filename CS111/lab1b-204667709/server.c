//
//  server.c
//  lab1b
//
//  Created by wenhui kuang on 4/16/16.
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
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <mcrypt.h>

#define no_argument 0
#define required_argument 1
int pid,sockfd,encrypt_flag = 0;
int newsockfd;

MCRYPT td;
int i;
char *key;
char *IV;
int keysize=16; /* 128 bits */

void signalHandler(int signum)
{
    // signal(signum, SIG_IGN);
    //  fprintf(stderr,"Caught signal %d\n", signum);
    exit(2);
}

void* threadFunction(void *fd)
{
    char buffer[1024];
    int size = 0;
    int file = *(int *)fd;
    while ((size = read(file, buffer, sizeof(buffer))) > 0)
    {
      
        if (buffer[0] == '\004')
        {
            close(sockfd);
            kill(pid, SIGKILL);
            exit(2);
        }
        if (encrypt_flag == 1)
        {
            if(mcrypt_generic (td, &buffer, sizeof(buffer)) != 0){
                perror("failed to encrypt");
                exit(1);
            }
            i=mcrypt_generic_init( td, key, keysize, IV);
            if (i<0) {
                mcrypt_perror(i);
                return (void *)1;
            }
        }
        write(1, buffer, size);
    }
    return NULL;
}

int main(int argc, char ** argv) {
    int portno;
    if (argc < 2)
    {
        fprintf(stderr,"Number of Arguments is at least two.");
    }
    const struct option longopts[] =
    {
        {"encryption", no_argument, NULL, 'e'},
        {"port", required_argument, NULL, 'p'},
    };
    int iarg;
    while ((iarg = getopt_long(argc,argv,"p:e",longopts,NULL)) != -1){
        switch (iarg){
            case 'p':
                portno = atoi(optarg);
                break;
            case 'e':
                encrypt_flag = 1;
                break;
        }
    }

    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("ERROR opening socket");
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        perror("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    
    if (encrypt_flag == 1)
    {
        int c,n = 0;
        FILE *file = fopen("my.key","r");
        if (file == NULL) {
            fprintf(stderr,"Can not open spencified file.\n");
        }
        key = (char*)malloc(keysize*sizeof(char));
        while ((c = fgetc(file)) != EOF) {
            key[n++] = (char)c;
        }
        key[n] = '\0';
        
        td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
        if (td==MCRYPT_FAILED) {
            return 1;
        }
        IV = malloc(mcrypt_enc_get_iv_size(td));
        /* Put random data in IV. Note these are not real random data,
         * consider using /dev/random or /dev/urandom.
         */
        srand(1);
        for (i = 0; i< mcrypt_enc_get_iv_size( td); i++) {
            IV[i]=rand();
        }
        i=mcrypt_generic_init( td, key, keysize, IV);
        if (i < 0) {
            mcrypt_perror(i);
            return 1;
        }
        

    }
    if (newsockfd < 0)
        perror("ERROR on accept");
    
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
        close(sockfd);
        close(newsockfd);
        close(pipe_from_shell[0]);
        close(pipe_to_shell[1]);
        dup2(pipe_from_shell[1], 2);
        dup2(pipe_from_shell[1], 1);
        dup2(pipe_to_shell[0], 0);
        execl("/bin/bash", "/bin/bash",NULL);
    }
    else
    {
        char buffer1[1];
        int size = 0;
        close(pipe_to_shell[0]);
        close(pipe_from_shell[1]);
        dup2(newsockfd,0);
        dup2(newsockfd,1);
        dup2(newsockfd,2);
        close(newsockfd);
        pthread_t tid;
        int ret = pthread_create(&tid, NULL, threadFunction, &pipe_from_shell[0]);
        if (ret) {
            printf("Error creating thread. Error code is %d\n", ret);
            exit(1);
        }
        while ((size = read(0, buffer1, 1)) > 0)
        {
            signal(SIGPIPE, signalHandler);
            if (encrypt_flag == 1)
            {
                if(mdecrypt_generic (td, &buffer1, 1) != 0){
                    perror("failed to encrypt");
                    exit(1);
                }
                i=mcrypt_generic_init( td, key, keysize, IV);
                if (i<0) {
                    mcrypt_perror(i);
                    return 1;
                }
            }
            write(pipe_to_shell[1], buffer1, 1);
        }
        if (size <= 0)
        {
            close(sockfd);
            kill(pid, SIGKILL);
            close(pipe_from_shell[0]);
            close(pipe_to_shell[1]);
            exit(1);
        }
    }

    
    
}

