//
//  main.c
//  lab1b
//
//  Created by wenhui kuang on 4/15/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//

#include <fcntl.h>
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
int pid,fd1 = 0,log_flag = 0,encrypt_flag = 0;
struct termios config;
MCRYPT td;
int i;
char *key;
char *IV;
int keysize=16; /* 128 bits */

void signalHandler(int signum)
{
    //fprintf(stderr,"Caught signal %d\n", signum);
    exit(2);
}

void* threadFunction(void *fd) // Read from the socket and write to the stdout.
{
    char buffer[1024];
    int size = 0;
    memset(buffer, 0, sizeof(buffer));
    while ((size = read(*(int*)fd, buffer, sizeof(buffer))) > 0)
    {
        
        if (buffer[0] == '\004')
        {
	    close(*(int*)fd);
            exit(0);
        }
        if (log_flag == 1) {
            dprintf(fd1, "RECEIVED %d bytes: %s\n",size, buffer);
        }
        if (encrypt_flag == 1)
        {
            if(mdecrypt_generic (td, &buffer, sizeof(buffer)) != 0){
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
        memset(buffer, 0, sizeof(buffer));
    }
    if (size <= 0) {
        close(*(int*)fd);
        exit(1);
    }
    return NULL;
}
void restore()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &config);
}

int main(int argc, char ** argv) {
    struct termios config2;
    int size = 0;
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char *output = NULL;
    
    if (argc < 2)
    {
        fprintf(stderr,"Number of Arguments is at least two.");
    }
    
    const struct option longopts[] =
    {
        {"log", required_argument, NULL, 'l'},
        {"encryption", no_argument, NULL, 'e'},
        {"port", required_argument, NULL, 'p'},
    };
    int iarg;
    while ((iarg = getopt_long(argc,argv,"el:p:",longopts,NULL)) != -1){
        switch (iarg){
            case 'l':
                log_flag = 1;
                output = optarg;
                break;
            case 'e':
                encrypt_flag = 1;
                break;
            case 'p':
                portno = atoi(optarg);
                break;
        }
    }
    if (log_flag == 1)
    {
        fd1 = open(output,O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fd1 == -1)
        {
            fprintf(stderr,"Cannot create file %s.\n",output);
            perror("Error: \n");
            exit(2);
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
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("ERROR opening socket");
    server = gethostbyname("localhost");
    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr,(char *)server->h_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        perror("ERROR connecting");

    pthread_t tid;
    int ret = pthread_create(&tid, NULL, threadFunction, &sockfd);
    if (ret) {
        printf("Error creating thread. Error code is %d\n", ret);
        exit(1);
    }
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
        for (i=0; i< mcrypt_enc_get_iv_size( td); i++) {
            IV[i]=rand();
        }
        i=mcrypt_generic_init( td, key, keysize, IV);
        if (i<0) {
            mcrypt_perror(i);
            return 1;
        }
    }
    
    char buffer[1];
    memset(buffer, 0, sizeof(buffer));
    while ((size = read(0, buffer, 1)) > 0)    // read from terminal to socket
    {
        if (buffer[0] == '\004')
        {
            close(sockfd);
            exit(0);
        }
        else if (buffer[0] == '\r' || buffer[0] == '\n')
        {
            buffer[0] = '\r';
            write(1, buffer, 1);
            buffer[0] = '\n';
            write(1, buffer, 1);
        }
        else
        {
            write(1, buffer, 1);
        }
        if (encrypt_flag == 1)
        {
            if(mcrypt_generic (td, &buffer, 1) != 0){
                perror("failed to encrypt");
                exit(1);
            }
            i=mcrypt_generic_init(td, key, keysize, IV);
            if (i<0) {
                mcrypt_perror(i);
                return 1;
            }
        }
        if (log_flag == 1)
        {
            dprintf(fd1, "SENT %d bytes: %s\n",size,buffer);
        }
        
        write(sockfd, buffer, 1);
        memset(buffer, 0, sizeof(buffer));
    }
    mcrypt_generic_end(td);
}

