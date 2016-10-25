/*	
 *	Demonstration TLS client
 *
 *       Compile with
 *
 *       gcc -Wall -o tls_client tls_client.c -L/usr/lib -lssl -lcrypto
 *
 *       Execute with
 *
 *       ./tls_client <server_INET_address> <port> <client message string>
 *
 *       Generate certificate with 
 *
 *       openssl req -x509 -nodes -days 365 -newkey rsa:1024 -keyout tls_demonstration_cert.pem -out tls_demonstration_cert.pem
 *
 *	 Developed for Intel Edison IoT Curriculum by UCLA WHI
 */
#include "tls_header.h"
#include <pthread.h>

int rate,receive_length;
SSL *ssl;
FILE *fd;

void* thread_fun (void *arg)
{
    char buffer[BUFSIZE];
    while (1) {
        receive_length = SSL_read(ssl, buffer, sizeof(buffer));
        if(strstr(buffer, "new_rate: ") != NULL)
        {
            sscanf(buffer, "Heart rate of patient %*s is %*f new_rate: %d", &rate);
            
            rate = rate;
            printf("New rate %d received from server.\n", rate);
            fprintf(fd,"New rate %d received from server.\n", rate);
            fflush(fd);
        }
        
        printf("Received message '%s' from server.\n\n", buffer);
        fprintf(fd,"Received message '%s' from server.\n", buffer);
        fflush(fd);
        memset(buffer,0,sizeof(buffer));
    }
    return NULL;
}
int main(int args, char *argv[])
{
    int port, range;
    int server;
    int line_length;
    char ip_addr[BUFSIZE];
	char *my_ip_addr;
    char *line = NULL;
    char buf[BUFSIZE];
    double heart_rate;
    FILE *fp = NULL;
    //SSL *ssl;
    SSL_CTX *ctx;
	
    fd = fopen("log.txt","w");
    
	my_ip_addr = get_ip_addr();
    printf("My ip addr is: %s\n", my_ip_addr);
    fprintf(fd,"My ip addr is: %s\n", my_ip_addr);
    fflush(fd);
    /* READ INPUT FILE */
    fp = fopen("config_file", "r");
    if(fp == NULL){
	fprintf(stderr, "Error opening config file with name 'config_file'. Exiting.\n");
	exit(1);
    }
    printf("Reading input file...\n");
    fprintf(fd,"Connecting to: %s:%d\n", ip_addr, port);
    fflush(fd);
    while(getline(&line, &line_length, fp) > 0){
	if(strstr(line, "host_ip") != NULL){
	    sscanf(line, "host_ip: %s\n", ip_addr);
	}
	else if(strstr(line, "port") != NULL){
	    sscanf(line, "port: %d\n", &port);
	}
	else if(strstr(line, "range") != NULL){
	    sscanf(line, "range: %d\n", &range);
	}
	else if(strstr(line, "rate") != NULL){
	    sscanf(line, "rate: %d\n", &rate);
	}
	else{
	    fprintf(stderr, "Unrecognized line found: %s. Ignoring.\n", line);
	}
    }
    fclose(fp);
    /* FINISH READING INPUT FILE */

    printf("Connecting to: %s:%d\n", ip_addr, port);
    fprintf(fd,"Connecting to: %s:%d\n", ip_addr, port);
    fflush(fd);

    /* SET UP TLS COMMUNICATION */
    SSL_library_init();
    ctx = initialize_client_CTX();
    server = open_port(ip_addr, port);
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, server);
    /* FINISH SETUP OF TLS COMMUNICATION */

    /* SEND HEART RATE TO SERVER */
    if (SSL_connect(ssl) == -1){ //make sure connection is valid
	fprintf(stderr, "Error. TLS connection failure. Aborting.\n");
	ERR_print_errors_fp(stderr);
	exit(1);
    }
    else {
	printf("Client-Server connection complete with %s encryption\n", SSL_get_cipher(ssl));
    fprintf(fd,"Client-Server connection complete with %s encryption\n", SSL_get_cipher(ssl));
    fflush(fd);
    
	display_server_certificate(ssl);
    }
    
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, thread_fun, NULL);
    if (ret) {
        printf("Error creating thread. Error code is %d\n", ret);
        exit(1);
    }

    while(1){
	printf("Current settings: rate: %d, range: %d\n", rate, range);
    fprintf(fd,"Current settings: rate: %d, range: %d\n", rate, range);
    fflush(fd);
        
	heart_rate = 
	    generate_random_number(AVERAGE_HEART_RATE-(double)range, AVERAGE_HEART_RATE+(double)range);
	memset(buf,0,sizeof(buf)); //clear out the buffer

	//populate the buffer with information about the ip address of the client and the heart rate
	sprintf(buf, "Heart rate of patient %s is %4.2f", my_ip_addr, heart_rate);
	printf("Sending message '%s' to server...\n", buf);
    fprintf(fd,"Sending message '%s' to server...\n", buf);
    fflush(fd);

	SSL_write(ssl, buf, strlen(buf));

	memset(buf,0,sizeof(buf));
        
    sleep(rate);
    }
    /* FINISH HEART RATE TO SERVER */

    //clean up operations
    SSL_free(ssl);
    close(server);
    SSL_CTX_free(ctx);
    return 0;
}
