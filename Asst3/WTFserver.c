#include "header.h"
/**************************************/




/**************************************/
// server port

int main(int argc, char ** argv){
    signal(SIGINT, killSignal);

    if(argc != 2){
        err_n_die("Error: ./WTFserver <portnum>");
    }

    int listenfd, connfd;
    connection_t* connection;
    pthread_t t;
    //char* command = malloc(100*sizeof(char*));
    struct sockaddr_in servaddr;
    uint8_t buff[MAXLINE+1];
    uint8_t recvline[MAXLINE+1];
    int addr_size;
    SA_IN serv_addr, client_addr;

    if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        err_n_die("socket error");
    }

    //bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons((atoi(argv[1])));

    if((bind(listenfd, (struct sockaddr *) &servaddr, sizeof(struct sockaddr_in))) < 0){
        err_n_die("bind error.");
    }
    if((listen(listenfd, 10)) < 0){
        err_n_die("listen error.");
    }
    printf("waiting for connection on port %d\n", atoi(argv[1]));
    for( ; ; ){
        
        //fflush(stdout);
        connection = (connection_t *)malloc(sizeof(connection_t));
        connection->sock = accept(listenfd, &connection->address, &connection->addr_len);
        
        if(connection->sock <= 0){
        //if(connfd <= 0){
            free(connection);
            err_n_die("Error: was not able to connect\n.");
        }
        else{
            printf("Successfully Connected!!!!\n");
            printf("connection->sock in: %d\n", connection->sock);
            //readCommand((void*)connection);
            pthread_create(&t, 0, readCommand, (void *)connection);
            pthread_detach(t);
        }
        printf("connection->sock out: %d\n", connection->sock);
        //memset(recvline, 0, MAXLINE);

        /*pthread_t t;
        int *pclient = malloc(sizeof(int));
        *pclient = connfd;
        int z = 0;
        //printf("Hello\n");
        if(z = pthread_create(&t, NULL, readCommand, (void*) pclient) != 0){
            err_n_die("Failed to create thread: %d", z);
        }
        //readCommand(connfd);
        close(connfd);*/
    }
    return 0;
}