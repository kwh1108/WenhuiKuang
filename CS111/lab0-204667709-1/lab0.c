#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <getopt.h>
#include <stdlib.h>
#include<signal.h>

#define required_argument 1
#define no_argument 0
void usage()
{
  printf("Please select options: --input, --output, --segfault, or --catch\n");
}

void signalHandler(int signum)
{
    fprintf(stderr,"Caught signal %d\n", signum);
    exit(3);
}

int main(int argc, char ** argv)
{
    int segfault_flag = 0;
    int catch_flag = 0;
    char *input = NULL;
    char *output = NULL;
    int fd0, fd1;
    char buffer[1];
    const struct option longopts[] =
    {
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {"segfault", no_argument, &segfault_flag , 1},
        {"catch", no_argument, &catch_flag, 1},
    };
    int iarg;
    while ((iarg = getopt_long(argc, argv,"i:o:sc",longopts,NULL)) != -1){
        switch (iarg){
            case 'i':
                input = optarg;
                break;
            case 'o':
                output = optarg;
                break;
            case 's':
                segfault_flag = 1;
                break;
            case 'c':
                catch_flag = 1;
                break;
        }
   }
    if ((argc == 1) || (argc == 2 && catch_flag == 1))
    {
        signal(SIGSEGV,signalHandler);
        while (read(0,buffer,1) > 0)
        {
            write(1,buffer,1);
        }
    }
    if (catch_flag == 1)
    {
        signal(SIGSEGV,signalHandler);
    }
    if (segfault_flag == 1)
    {
        int *foo = NULL;
        *foo = 1;
    }
    if (input != NULL && output != NULL){
        fd0 = open(input,O_RDONLY);
        if (fd0 == -1)
        {
            fprintf(stderr,"Cannot open file %s.\n",input);
            perror("Error: ");
            exit(1);
        }
        close(0);
        dup(fd0);
        fd1 = open(output,O_CREAT | O_WRONLY, 0600);
        if (fd1 == -1)
        {
            fprintf(stderr,"Cannot create file %s.\n",output);
            perror("Error: \n");
            exit(2);
        }
        close(1);
        dup(fd1);
        while (read(fd0,buffer,1) > 0)
        {
            write(fd1,buffer,1);
        }
        close(fd0);
        close(fd1);
    }
    else if(input != NULL){
        fd0 = open(input,O_RDONLY);
        if (fd0 == -1)
        {
            fprintf(stderr,"Cannot open file %s.\n",input);
            perror("Error: ");
            exit(1);
        }
        close(0);
        dup(fd0);
        while (read(fd0,buffer,1) > 0)
        {
            write(1,buffer,1);
        }
        close(fd0);
    }
    else if(output != NULL){
        fd1 = open(output,O_CREAT | O_WRONLY, 0600);
        if (fd1 == -1)
        {
            fprintf(stderr,"Cannot create file %s.\n",output);
            perror("Error: \n");
            exit(2);
        }
        close(1);
        dup(fd1);
        while (read(0,buffer,1) > 0)
        {
            write(fd1,buffer,1);
        }
    }
    return 0;
}



