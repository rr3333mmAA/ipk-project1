/*------------------------------------------------------ IPKCPC -----
|  Task: IPK Calculator Protocol
|  
|  File: ipkcpc.c
|
|  Author: Abayev Amirkhan
|  Login: xabaye00
|  Faculty: Information Technology, Brno University of Technology
|
|  Description: Client implementation 
|               for the IPK Calculator Protocol
|
|  Date: 21.3.2023
*-------------------------------------------------------------------*/

#include <stdio.h> 
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <arpa/inet.h>

/* Constants */
#define BUFSIZE_TCP 1024
#define BUFSIZE_UDP 512

/* Global variables and structure */
struct sockaddr_in servaddr;
socklen_t serverlen;
int sockfd;
char *mode;

/* Function declarations */
void handle_interrupt(int sig);
int setup_connection(char *host, char *port);
void send_request(char *msg);
void recv_response(void);

int main(int argc, char *argv[]){
    int opt;
    char *host, *port;

    /* Parse command line options */
    while ((opt = getopt(argc, argv, "h:p:m:")) != -1) { 
        switch (opt) { 
        case 'h':
            host = optarg;
            break;
        case 'p':
            port = optarg;
            break;
        case 'm':
            mode = optarg;
            break;
        default:
            fprintf(stderr, "Usage: %s -h <host> -p <port> -m <mode>\n", 
                    argv[0]); 
            exit(EXIT_FAILURE); 
        } 
    }

    /* Check if arguments is supplied */
    if (host == NULL || port == NULL || mode == NULL) {
        fprintf(stderr, "Error: configuration not specified\nUsage: %s -h <host> -p <port> -m <mode>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Set server address */
    serverlen = sizeof(servaddr);
    memset(&servaddr, 0, serverlen); 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(atoi(port)); 
    servaddr.sin_addr.s_addr = inet_addr(host);

    /* Setup connection */
    if (setup_connection(host, port) < 0) {
        exit(EXIT_FAILURE);
    }

    /* Set up signal handler for interrupt signal. Only for TCP mode */
    if (strcmp(mode, "tcp") == 0){
        if (signal(SIGINT, handle_interrupt) == SIG_ERR) { 
            perror("signal"); 
            exit(EXIT_FAILURE); 
        }
    }

    /* Read and process input from stdin */
    char line[BUFSIZE_TCP];
    while (fgets(line, BUFSIZE_TCP, stdin) != NULL) {
        send_request(line);
        recv_response();
    }

    /* Close connection */
    close(sockfd); 
    exit(EXIT_SUCCESS);

}

/* Signal handler for SIGINT */
void handle_interrupt(int sig) {
    send_request("BYE");
    printf("\nBYE\n"); 
    close(sockfd); 
    exit(EXIT_SUCCESS); 
}


/* Setup connection with server */
int setup_connection(char *host, char *port) {
    /* Create socket */
    if (strcmp(mode, "tcp") == 0) {
        /* TCP */
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
            perror("socket"); 
            return -1; 
        }
         
        /* Connect to server */
        if (connect(sockfd, (struct sockaddr *)&servaddr, serverlen) < 0) { 
            perror("connect");
            return -1; 
        }
    } else if (strcmp(mode, "udp") == 0) {
        /* UDP */
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
            perror("socket"); 
            return -1; 
        } 
    }

    return 0;
}

/* Send request to server */
void send_request(char *msg) {
    int bytes;

    if (strcmp(mode, "tcp") == 0) {
        /* Sending message */
        bytes = send(sockfd, msg, strlen(msg), 0);
        if (bytes < 0) {
            perror("send");
            exit(EXIT_FAILURE); 
        }
    } else if (strcmp(mode, "udp") == 0) {
        /* Request message format */
        const char* payload = msg;
        uint8_t opcode = 0;
        size_t payload_len = strlen(payload);
        uint8_t buffer[2 + payload_len];
        buffer[0] = opcode;
        buffer[1] = payload_len;
        memcpy(buffer + 2, payload, payload_len);

        /* Sending message */
        bytes = sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&servaddr, serverlen);
        if (bytes < 0) {
            perror("sendto");
            exit(EXIT_FAILURE); 
        }
    }
}

/* Receive response from server */
void recv_response(void) {
    ssize_t bytes_received; 

    if (strcmp(mode, "tcp") == 0) {
        /* TCP */

        /* Receiving message */
        char response[BUFSIZE_TCP];
        bytes_received = recv(sockfd, response, BUFSIZE_TCP, 0);
        if (bytes_received < 0) {
            perror("recv"); 
            exit(EXIT_FAILURE); 
        }

        response[bytes_received] = '\0'; 
        
        /* Print TCP response */
        if (strcmp(response, "BYE\n\0\n\0") == 0){  // if server send "BYE" -> terminate
            printf("%s", response);
            close(sockfd); 
            exit(EXIT_SUCCESS); 
        } else {
            printf("%s", response);
        }
    } else if (strcmp(mode, "udp") == 0) {
        /* UDP */

        uint8_t response_buffer[BUFSIZE_UDP];

        /* Set timeout for 10 seconds */
        struct timeval timeout={ 10, 0 }; 

        /* Set receive UDP message timeout */
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));

        /* Receiving message */
        bytes_received = recvfrom(sockfd, response_buffer, sizeof(response_buffer), 0, NULL, NULL);
        if (bytes_received < 0) {
            perror("recvfrom"); 
            exit(EXIT_FAILURE); 
        }

        /* Response message format */
        uint8_t response_opcode = response_buffer[0];
        uint8_t response_status_code = response_buffer[1];
        size_t response_payload_len = response_buffer[2];
        const char* response_payload = (const char*)(response_buffer + 3);

        /* Print UDP response */
        if (response_opcode == 1 && response_status_code == 0) {
            int result = atoi(response_payload);
            printf("OK:%d\n", result);
        } else {
            printf("ERR:%s\n", response_payload);
        }
    }
}

