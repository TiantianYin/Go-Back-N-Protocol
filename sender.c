#include "gbn.h"

int main(int argc, char *argv[]) {
    int sockfd;              /* socket file descriptor of the client            */
    int numRead;
    socklen_t socklen;       /* length of the socket structure sockaddr         */
    char buf[DATALEN * N];   /* buffer to send packets                       */
    struct hostent *he;      /* structure for resolving names into IP addresses */
    FILE *inputFile;         /* input file pointer                              */
    struct sockaddr_in server;

    socklen = sizeof(struct sockaddr);

    /*----- Checking arguments -----*/
    if (argc != 4){
        fprintf(stderr, "usage: sender <hostname> <port> <filename>\n");
        exit(-1);
    }
    
    /*----- Opening the input file -----*/
    if ((inputFile = fopen(argv[3], "rb")) == NULL){
        perror("fopen");
        exit(-1);
    }

    /*----- Resolving hostname to the respective IP address -----*/
    if ((he = gethostbyname(argv[1])) == NULL){
        perror("gethostbyname");
        exit(-1);
    }
    
    /*----- Opening the socket -----*/
    if ((sockfd = gbn_socket()) == -1){
        perror("gbn_socket");
        exit(-1);
    }
    
    struct sockaddr_in si_me;
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(28000 + rand()%1000);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    gbn_bind(sockfd, &si_me, sizeof(si_me));

    /*--- Setting the server's parameters -----*/
    memset(&server, 0, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_addr   = *(struct in_addr *)he->h_addr;
    server.sin_port   = htons(atoi(argv[2]));
    
    /*----- Connecting to the server -----*/
    if (gbn_connect(sockfd, (struct sockaddr *)&server, socklen) < 0){
        printf("gbn_connect\n");
        exit(-1);
    }

    while ((numRead = fread(buf, 1, DATALEN * N, inputFile)) > 0) {
        printf("sending...\n");
        gbn_send(sockfd, buf, numRead, 0);
    }
    
    /*----- Closing the socket -----*/
    if(gbn_close(sockfd) == -1) {
        perror("gbn_close");
        exit(-1);
    }
    
    /*----- Closing the file -----*/
    if(fclose(inputFile) == EOF) {
        perror("fclose");
        exit(-1);
    }

    return(0);
}


