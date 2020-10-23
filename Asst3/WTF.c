#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <openssl/md5.h>
#include <limits.h>
#include <linux/limits.h>
#include <libgen.h>
#include <dirent.h>
//GLOBAL VAR
int checkWrite = 0;
char* portserver = NULL;
char* ip = NULL;
int count = 0;
 
struct node{
    char* versionNumber;
    char* name;
    char* hashcode;
    struct node * next;
};
 
//#define serverport 18000
#define MAXLINE 4096
#define SA struct sockaddr
//#define _XOPEN_SOURCE_EXTENDED 1
 
void errdie(const char*fmt, ...);
int argComp(char* string);
void configure(char* ipHost, char* portnum);
void readConfigure(int a);
unsigned char* hash(char* project, char* path);
void add(char* project, char* file_name);
int *remove_line(char* project, char* file_name);
void create(int sockfd, char* project, char* arg1);
void update(int sockfd, char* project, char* arg1);
void upgrade(int sockfd, char* project, char* arg1);
int checkMan(char* file_name, char* project);
void destroy(int sockfd, char* project, char* arg1);
void currVer(int sockfd, char *project, char *arg1);
void checkout(int sockfd, char *project, char *arg1);
char *recursive(char *basePath, char *fileName);
int checkFile(char *basePath);
void _mkdir(const char *dir);
void commitCom(struct node* head, struct node* head2, int connfd, char *project);
struct node* makeList(struct node* head, char* vNum, char* file, char* hash);
struct node* insertValues(struct node* head, char* path);
void commit(int sockfd, char *project, char *arg1);
void push(int sockfd, char *project, char *arg1);
void history(int sockfd, char *project, char *arg1);
void rollback(int sockfd, char *project, char *arg1);
void* upMan(char* project, char* ver);
 
 
int main(int argc, char**argv)
{
    int sockfd, n, sendbytes, file_size;
    char* project = NULL;
    char* arg1 = NULL;
    char* ver = NULL;
    char path [1000];
    struct sockaddr_in servaddr;
    char sendline[MAXLINE];
    char recline[MAXLINE];
    char buffer[BUFSIZ];
    if(argc > 4)
    {
        errdie("Too many arguments", argv[0]);
    }
    if(argc>1){
        if(argComp(argv[1]) == 1)//configure
        {
            if(argc != 4)
            {
                printf("unsufficient amount of args\n");
                exit(-1);
            }
            configure(argv[2],argv[3]);
            exit(0);
        }
        else if(argComp(argv[1]) == 2)//create project folder
        {
             if(argc != 3)
            {
                printf("unsufficient amount of args\n");
                exit(-1);
            }
            project = malloc(sizeof(char)*(100));
            arg1 = malloc(PATH_MAX);
            strcpy(arg1, argv[1]);
            strcat(arg1, " ");
            project = argv[2];
            memset(project + strlen(project),'\0',sizeof(project));
            strcat(arg1, project);
            strcat(arg1, "$");
        }
        else if(argComp(argv[1]) == 3)//add func
        {
             if(argc != 4)
            {
                printf("unsufficient amount of args\n");
                exit(-1);
            }
            add(argv[2], argv[3]);
            exit(0);
        }
        else if(argComp(argv[1]) == 4)//remove func
        {
             if(argc != 4)
            {
                printf("unsufficient amount of args\n");
                exit(-1);
            }
            remove_line(argv[2], argv[3]);
            exit(0); 
        }
         else if(argComp(argv[1]) == 5)//Update func
        {
             if(argc != 3)
            {
                printf("unsufficient amount of args\n");
                exit(-1);
            }
            project = malloc(sizeof(char)*(100));
            arg1 = malloc(PATH_MAX);
            strcpy(arg1, argv[1]);
            strcat(arg1, " ");
            project = argv[2];
            memset(project + strlen(project),'\0',sizeof(project));
            //printf("project:\n", project);
            //printf("arg1: %s\n", arg1);
        }
        else if(argComp(argv[1]) == 6)//Upgrade func
        {
             if(argc != 3)
            {
                printf("unsufficient amount of args\n");
                exit(-1);
            }
            project = malloc(sizeof(char)*(100));
            arg1 = malloc(PATH_MAX);
            strcpy(arg1, argv[1]);
            strcat(arg1, " ");
            project = argv[2];
            memset(project + strlen(project),'\0',sizeof(project));
            strcat(arg1, project);
            strcat(arg1, "$");
        }
        else if(argComp(argv[1]) == 7)//destroy func
        {
             if(argc != 3)
            {
                printf("unsufficient amount of args\n");
                exit(-1);
            }
            project = malloc(sizeof(char)*(100));
            arg1 = malloc(PATH_MAX);
            strcpy(arg1, argv[1]);
            strcat(arg1, " ");
            project = argv[2];
            memset(project + strlen(project),'\0',sizeof(project));
            strcat(arg1, project);
            strcat(arg1, "$");
        }
         else if(argComp(argv[1])==8)//checkout
        {
             if(argc != 3)
            {
                printf("unsufficient amount of args\n");
                exit(-1);
            }
            project = malloc(sizeof(char)*(100));
            arg1 = malloc(PATH_MAX);
            strcpy(arg1, argv[1]);
            strcat(arg1, " ");
            project = argv[2];
            memset(project + strlen(project),'\0',sizeof(project));
            strcat(arg1, project);
            strcat(arg1, "$");
        }
        else if(argComp(argv[1])==9)//currentversion
        {
             if(argc != 3)
            {
                printf("unsufficient amount of args\n");
                exit(-1);
            }
            project = malloc(sizeof(char)*(100));
            arg1 = malloc(PATH_MAX);
            strcpy(arg1, argv[1]);
            strcat(arg1, " ");
            project = argv[2];
            memset(project + strlen(project),'\0',sizeof(project));
            strcat(arg1, project);
            strcat(arg1, "$");
        }
        else if(argComp(argv[1]) == 10)//Commit func
        {
            if(argc != 3)
            {
                printf("unsufficient amount of args\n");
                exit(-1);
            }
            project = malloc(sizeof(char)*(100));
            arg1 = malloc(PATH_MAX);
            strcpy(arg1, argv[1]);
            strcat(arg1, " ");
            project = argv[2];
            memset(project + strlen(project),'\0',sizeof(project));
            strcat(arg1, project);
            strcat(arg1, "$");
        }
        else if(argComp(argv[1]) == 11)//Push func
        {
            if(argc != 3)
            {
                printf("unsufficient amount of args\n");
                exit(-1);
            }
            project = malloc(sizeof(char)*(100));
            arg1 = malloc(PATH_MAX);
            strcpy(arg1, argv[1]);
            strcat(arg1, " ");
            project = argv[2];
            memset(project + strlen(project),'\0',sizeof(project));
            strcat(arg1, project);
            strcat(arg1, "$");
        }
        else if(argComp(argv[1]) == 12)//history func
        {
             if(argc != 3)
            {
                printf("unsufficient amount of args\n");
                exit(-1);
            }
            project = malloc(sizeof(char)*(100));
            arg1 = malloc(PATH_MAX);
            strcpy(arg1, argv[1]);
            strcat(arg1, " ");
            project = argv[2];
            memset(project + strlen(project),'\0',sizeof(project));
            strcat(arg1, project);
            strcat(arg1, "$");
        }
        else if(argComp(argv[1]) == 13)//rollback func
        {
            if(argc!=4)
            {
                printf("insufficient amount of arguments\n");
                exit(-1);
            }
            project = malloc(sizeof(char)*(100));
            arg1 = malloc(PATH_MAX);
            ver = malloc(BUFSIZ);
            strcpy(arg1, argv[1]);
            strcat(arg1, " ");
            project = argv[2];
            strcat(arg1, project);
            strcat(arg1, " ");
            ver = argv[3];
            strcat(arg1, ver);
            strcat(arg1, "$");
        }
    }
 
    readConfigure(1);
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        errdie("ERROR could not create socket");
    }
 
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(portserver));
 
    if(inet_pton(AF_INET, ip, &servaddr.sin_addr) <= 0)
    {
        errdie("inte_pton error for %s", ip);
    }
 
    while(1){
        if(connect(sockfd, (SA*) &servaddr, sizeof(servaddr)) == 0)//keeps trying to connect till exited manually ctrl-C
        {
           //printf("connection successful\n");
           break;
        }
    }
 
    if(argComp(argv[1]) == 2)// send message to server create and receive
    {
       create(sockfd, project, arg1);
    }
    else if(argComp(argv[1])==5)
    {
        update(sockfd, project, arg1);
    }
    else if(argComp(argv[1]) == 6)//Upgrade func
        {
            upgrade(sockfd, argv[2], arg1);
        }
    else if(argComp(argv[1])==7)//Destroy
    {
        destroy(sockfd, project, arg1);
    }
    else if(argComp(argv[1])==8)//Checkout
    {
        //printf("goes into checkout\n");
        checkout(sockfd, project, arg1);
    }
    else if(argComp(argv[1])==9)//CurrVer
    {
        currVer(sockfd, project, arg1);
    }
    else if(argComp(argv[1])==10)//Commit
    {
        commit(sockfd, project, arg1);
    }
    else if(argComp(argv[1])==11)//Push
    {
        push(sockfd, project, arg1);
    }
    else if(argComp(argv[1])==12)//history
    {
        history(sockfd, project, arg1);
    }
    else if(argComp(argv[1])==13)//rollback
    {
        rollback(sockfd, project, arg1);
    }
    
    close(sockfd);
    exit(0); //ended successfully
 
}
void rollback(int sockfd, char *project, char *arg1)
{
    char buffer[BUFSIZ];
 
    if(send(sockfd, arg1, strlen(arg1), 0) == -1)
    {
        errdie("could not send command to server");
    }
    bzero(buffer, BUFSIZ);
    read(sockfd, buffer, 5);
 
    if(strcmp(buffer, "err1") == 0)
    {
        printf("Project doesn't exist on server side\n");
        exit(0);
    }
    else if(strcmp(buffer, "err2") == 0)
    {
        printf("Project with that version number doesn't exist\n");
        exit(0);
    }
    else
    {
        printf("success\n");
        exit(0);
    }
}
void history(int sockfd, char *project, char *arg1)
{
    char buffer[BUFSIZ], path[PATH_MAX];
    struct stat file_stat;
    char ready[10];
    int size, check;
    DIR* dir = opendir(project);
 
    bzero(path, PATH_MAX);
    strcpy(path, project);
    strcat(path, "/");
    strcat(path, ".history");
 
    if(send(sockfd, arg1, strlen(arg1), 0) == -1)
    {
        errdie("could not send command to server");
    }
    bzero(buffer, BUFSIZ);
    read(sockfd, buffer, 6);
 
    if(strcmp(buffer, "Error") == 0)
    {
        printf("Project doesn't exist on server side\n");
        exit(0);
    }
    
    if (dir) 
    {
        //printf("Project exists\n");
        write(sockfd, "ready", 6);
        closedir(dir);
    }
    else if (ENOENT == errno) 
    {
        printf("Project does not exist\n");
        write(sockfd, "Error", 6);
        exit(-1);
    } 
 
    bzero(buffer, BUFSIZ);
    if((check = read(sockfd, buffer, BUFSIZ) == -1))
    {
        errdie("failure to receive data for history\n");
    }
    size = atoi(buffer);
    char wr[size+1];
    bzero(wr, size);
    int fd = open (path, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0600);
    if (fd == -1) 
    {
        printf ("%s can't open history file.\n", path);
        exit(-1);
    }
             
    write(sockfd, "Ready", 6);
    if((check = read(sockfd, wr, size)) == -1)
    {
         errdie("failure to receive data for history\n");
    }
    memset(wr + size, '\0', sizeof(wr) - size);
    write(fd, wr, size);
    //printf("%s\n", wr);
    exit(0);       
}
void push(int sockfd, char *project, char *arg1)
{
    char buffer[BUFSIZ], path[PATH_MAX];
    struct stat file_stat;
    char fileSize[100];
    char ready[10];
    struct stat fileStat;
 
    bzero(path, PATH_MAX);
    strcpy(path, project);
    strcat(path, "/");
    strcat(path, ".Commit");
 
    if(send(sockfd, arg1, strlen(arg1), 0) == -1)
    {
        errdie("could not send command to server");
    }
    bzero(buffer, BUFSIZ);
    read(sockfd, buffer, 6);
    if(strcmp(buffer, "Error") == 0)
    {
        printf("Project doesn't exist on server side\n");
        exit(0);
    }
 
    int fd = open(path, O_RDONLY, 0);
    if(fstat(fd, &file_stat) < 0){
        errdie("could not get a file size\n");
    }
    char one[1];
    sprintf(buffer, "%d", file_stat.st_size);
    write(sockfd, ip, strlen(ip));
    read(sockfd, one, 2);
    if(strcmp(one, "0")==0)
    {
        printf("Server doesn't have commit file or commit files dont match. Must commit first before pushing!\n");
        exit(-1);
    }
    if(file_stat.st_size != 0 )
    {
        write(sockfd, buffer, file_stat.st_size);
    }
    else
    {
        write(sockfd, "0", 2);
    }
    char m[file_stat.st_size];
    read(fd, &m, file_stat.st_size);
    if(read(sockfd, one, 2))
    {
        if(strcmp(one, "?") == 0){
            printf("commit file is empty nothing to push\n");
            close(fd);
            remove(path);
            exit(0);
        }
        send(sockfd, &m, file_stat.st_size-1, 0);
    }
    
    bzero(buffer, BUFSIZ);
    read(sockfd, buffer, 6);
    if(strcmp(buffer, "Error")==0)
    {
        printf("Commit File does not match stored commit\n");
        exit(0);
    }
    //printf("buffer %s\n", buffer);
 
    bzero(buffer, BUFSIZ);
    read(sockfd, buffer, 6);
    if(strcmp(buffer, "ready")==0){
        write(sockfd, ip, strlen(ip));
    }
   ///////
    memset(fileSize, '\0', 100);
    bzero(buffer, BUFSIZ);
    while(1)
    {
        write(sockfd, "ready", 6);
        int n = read(sockfd, buffer, PATH_MAX);
        memset(buffer + n, '\0', BUFSIZ - n);
        if(n == 0 || n == -1){
            printf("Server had a issue\n");
            break;
        } 
        if(strcmp(buffer, "$") == 0) break;
        int fd2 = open(buffer, O_RDONLY, 0);
        if(!(fd2)) break;
        if(fstat(fd2, &fileStat) < 0){
            printf("Error: reading filesize.\n");
            exit(-1);
        }
        sprintf(fileSize, "%d", fileStat.st_size);
        write(sockfd, fileSize, strlen(fileSize));
 
        char c[fileStat.st_size];
        read(fd2, &c, fileStat.st_size);
        if(read(sockfd, ready, 6)){
            send(sockfd, &c, fileStat.st_size, 0);
        }
 
        memset(fileSize, '\0', 100);
        bzero(buffer, BUFSIZ);
 
        close(fd2);
    }
   /////
    close(fd);
    bzero(buffer, BUFSIZ);
    remove(path);
    printf("success\n");
    exit(0);
}
void commit(int sockfd, char *project, char *arg1)
{
    char buffer[BUFSIZ];
    char path[1000],  conPath[1000], manPath[1000];
    struct stat file_stat;
    char *mPath = NULL;
    int size, reader;
    struct node *listHead = NULL;
    struct node *serverHead = NULL;
    if(send(sockfd, arg1, strlen(arg1), 0) == -1)
    {
        errdie("could not send command to server");
    }
    bzero(buffer, BUFSIZ);
    read(sockfd, buffer, 6);
    if(strcmp(buffer, "Error") == 0)
    {
        printf("Project doesn't exist on server side\n");
        exit(0);
    }
    bzero(path, 1000);
    strcpy(path, project);
    strcat(path, "/");
    strcat(path, ".Update");
    int fd = open(path, O_RDONLY, 0);
    if (fd == -1) {
        //printf ("Continue...\n");
    }
    else
    {
        if (fstat(fd, &file_stat) < 0)
            {
                    errdie("could not get file size\n");
                    exit(EXIT_FAILURE);
            }
        if (file_stat.st_size != 0)
        {
            printf(".Update is not empty\n");
            close(fd);
            exit(0);
        }
    }
    close(fd);
    bzero(conPath, 1000);
    strcpy(conPath, project);
    strcat(conPath, "/");
    strcat(conPath, ".Conflict");
    int fd2 = open(conPath, O_RDONLY, 0);
    if (fd2 == -1) {
        //printf ("Continue...\n");
    }
    else
    {
        printf("There is a Conflict file in this project. Abort!\n");
        close(fd2);
        exit(0);
    }
    close(fd2);
    bzero(manPath, 1000);
    strcpy(manPath, project);
    strcat(manPath, "/");
    strcat(manPath, ".Manifest");
    mPath = malloc(strlen(manPath)+1);
    mPath = manPath;
    listHead = insertValues(listHead, mPath);
    write(sockfd, "ready", 6);
    bzero(buffer, BUFSIZ);
    reader = read(sockfd, buffer, BUFSIZ);
    memset(buffer + reader, '\0', BUFSIZ - reader);
    size = atoi(buffer);
    char wr[size+1];
    bzero(path, 1000);
    strcpy(path, project);
    strcat(path, "/");
    strcat(path, "serverManifest");
    int fd3 = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if(fd3==-1)
    {
        errdie("couldn't open file\n");
    }
    write(sockfd, "ready", 6);
    read(sockfd, wr, size);
    wr[size+1]='\0';
    write(fd3, wr, size);
    serverHead = insertValues(serverHead, path);
    close(fd3);
    commitCom(listHead, serverHead, sockfd, project);
    remove(path);
    exit(0);
}
void checkout(int sockfd, char *project, char *arg1)
{
    char dump[6];
    char *File = malloc(PATH_MAX);
    char *relPath = malloc(PATH_MAX);
    char path [PATH_MAX];
    char buffer[BUFSIZ];
    int reader, fd, fd2, size, fd3;
    DIR* dir = opendir(project);
    if (dir) 
    {
        printf("Project already exists\n");
        closedir(dir);
        exit(0);
    }
    else if (ENOENT == errno) 
    {
        //printf("Project does not exist\n");
        mkdir(project, 0700);
    } 
    if(send(sockfd, arg1, strlen(arg1), 0) == -1)
    {
        errdie("could not send command to server");
    }
    read(sockfd, dump, 6);
    if(strcmp(dump, "Error")==0)
    {
        printf("Project doesn't exist on server\n");
        exit(0);
    }
    while(1)
    {
        write(sockfd, "ready", 6);
        reader = read(sockfd, buffer, BUFSIZ);
        memset(buffer + reader, '\0', BUFSIZ - reader);
        bzero(path, sizeof(path));
        if(strcmp(buffer,"$")==0)
        {
            break;
        }
        strcpy(path, buffer);
        //printf("path: %s\n", path);
        File = basename(path);
        relPath = dirname(path);
        
        fd = open(buffer, O_WRONLY | O_CREAT, 0600);
        if (fd == -1)
        {
            _mkdir(relPath);
            fd2 = open(buffer, O_WRONLY | O_CREAT, 0600);
            if(fd2 == -1)
            {
                errdie("failed to create file\n");
            }
            bzero(buffer, BUFSIZ);
            write(sockfd, "ready", 6);
            reader = read(sockfd, buffer, BUFSIZ);
            size = atoi(buffer);
            char wr[size];
            bzero(wr, size);
            write(sockfd, "ready", 6);
            read(sockfd, wr , size);
            write(fd2, wr, size);
            close(fd2);
        }
        else
        {
            bzero(buffer, BUFSIZ);
            write(sockfd, "ready", 6);
            reader = read(sockfd, buffer, BUFSIZ);
            size = atoi(buffer);
            char wr[size];
            bzero(wr, size);
            write(sockfd, "ready", 6);
            read(sockfd, wr , size);
            write(fd, wr, size);
            close(fd);
        }
    }
    write(sockfd, "ready", 6);
    bzero(buffer, BUFSIZ);
    reader = read(sockfd, buffer, BUFSIZ);
    memset(buffer + reader, '\0', BUFSIZ - reader);
    fd3 = open(buffer, O_WRONLY | O_CREAT, 0600);
    if(fd3 == -1)
    {
        errdie("failed to create file\n");
    }
    bzero(buffer, BUFSIZ);
    write(sockfd, "ready", 6);
    reader = read(sockfd, buffer, BUFSIZ);
    size = atoi(buffer);
    char wr[size];
    bzero(wr, size);
    write(sockfd, "ready", 6);
    read(sockfd, wr , size);
    write(fd3, wr, size);
    close(fd3);
    exit(0);
}
void currVer(int sockfd, char *project, char *arg1)
{
    char buffer[BUFSIZ];
    int counter = 0;
    if(send(sockfd, arg1, strlen(arg1), 0) == -1)
    {
        errdie("could not send command to server");
    }
    else
    {
        while(1)
        {
            write(sockfd, "ready", 6);
            if(recv(sockfd, buffer, BUFSIZ,0) == -1)
            {
            errdie("failure to receive data for manifest\n");
            }
            else if(strcmp(buffer, "$")==0)
            {
                //printf("Current Version Successful\n");
                exit(0);
            }
            else if(strcmp(buffer, "?")==0)
            {
                printf("Server Manifest contains no files\n");
                exit(0);
            }
            else
            {
                if(counter == 0)
                {
                    printf("%s ", buffer);
                    counter = counter + 1;
                }
                else if(counter == 1)
                {
                     printf("%s\n", buffer);
                     counter = 0;
                }
                bzero(buffer, sizeof(buffer));
            }
        }
    }
    exit(1);
}
void _mkdir(const char *dir) {
        char temp[256];
        char *ptr = NULL;
        size_t len;
 
        snprintf(temp, sizeof(temp),"%s",dir);
        len = strlen(temp);
        if(temp[len - 1] == '/')
                temp[len - 1] = 0;
        for(ptr = temp + 1; *ptr; ptr++)
                if(*ptr == '/') {
                        *ptr = 0;
                        mkdir(temp, S_IRWXU);
                        *ptr = '/';
                }
        mkdir(temp, S_IRWXU);
}
void destroy(int sockfd, char* project, char* arg1)
{
    char buffer[BUFSIZ];
     DIR* dir = opendir(project);
    if (dir) 
    {
        printf("Project exists\n");
        closedir(dir);
    }
    else if (ENOENT == errno) 
    {
        printf("Project does not exist\n");
        exit(-1);
    } 
    else 
    {
        printf("failed cant open project");
        exit(-1);
    }
    if(send(sockfd, arg1, strlen(arg1), 0) == -1)
    {
        errdie("could not send command to server");
    }
    else
    {
        if(recv(sockfd, buffer, BUFSIZ,0) == -1)
        {
        errdie("failure to receive data for manifest\n");
        }
        else if(strcmp(buffer, "$")==0)
        {
            printf("Destroy Successful\n");
            exit(0);
        }
    }
    exit(-1);
}
void upgrade(int sockfd, char* project, char *arg1)
{
    char path[1000], command[3], fileName[FILENAME_MAX], hash[100], pathCom[1000];
    char checkConflict[1000], dump[100], filePath[PATH_MAX];
    char *sendFile; 
    int i = 0, reader, j, memFile, memCom, memHash, size, fd2, fd3, fd4;
    int bIndx = 0, counter = 0;
    char buffer[BUFSIZ], ver [BUFSIZ];
    struct stat file_stat;
    char token; 
 
    DIR* dir = opendir(project);
    if (dir) 
    {
        //printf("Project exists\n");
        closedir(dir);
    }
    else if (ENOENT == errno) 
    {
        printf("Project does not exist\n");
        exit(-1);
    } 
    else 
    {
        printf("failed cant open project");
        exit(-1);
    }
    strcpy(path, project);
    strcat(path,"/.Update");
 
    strcpy(checkConflict, project);
    strcat(checkConflict, "/.Conflict");
 
    if(send(sockfd, arg1, strlen(arg1), 0) == -1)
    {
        errdie("could not send command to server");
    }
 
    int check = open(checkConflict, O_RDONLY, 0);
    if (check >=0)
    {
        printf("Resolve all conflicts and Update\n");
        close(check);
        exit(-1);
    }
    int fd = open(path, O_RDONLY, 0);
    if (fd == -1) {
        printf ("No update file. Client must perform command update first.\n");
        exit(-1);
    }
    if (fstat(fd, &file_stat) < 0)
        {
                errdie("could not get file size\n");
                exit(EXIT_FAILURE);
        }
    if (file_stat.st_size == 0)
    {
        printf("The project is up to date\n");
        remove(path);
        exit(0);
    }
     while(i <= file_stat.st_size)
        {
            reader = read(fd, &token, 1);
            if(reader==0||isspace(token)!=0)
            {
                if(reader == 0 && isspace(token)!=0)
                {
                    break;
                }
                else if(counter == 0)
                {
                    for(j = 0; j < bIndx; j++)
                    {
                        command[j]=buffer[j];
                    }
                    memCom = bIndx;
                    counter ++;
                }
                else if(counter == 1)
                {
                     for(j = 0; j < bIndx; j++)
                    {
                        fileName[j]=buffer[j];
                    }
                    memFile = bIndx;
                    counter++;
                }
                else if(counter == 2)
                {
                     for(j = 0; j < bIndx; j++)
                    {
                        hash[j]=buffer[j];
                    }
                    memHash = bIndx;
                    memset(command + memCom, '\0', sizeof(command)-memCom);
                    memset(fileName + memFile, '\0', sizeof(fileName)-memFile);
                    memset(hash + memHash, '\0', sizeof(hash)-memHash);
                    //printf("bIndx: %d\n",bIndx );
                    if(strcmp(command,"D")==0)
                    {
                        bzero(pathCom, strlen(pathCom));
                        strcpy(pathCom, project);
                        strcat(pathCom, "/");
                        strcat(pathCom, fileName);
                        remove_line(project, fileName);
                        remove(fileName);
                    }
                    else if((strcmp(command,"A")==0) ||(strcmp(command,"M")==0))
                    {
                        recv(sockfd, dump, 6,0);
                        write(sockfd, fileName, strlen(fileName));
                        reader = recv(sockfd, buffer, BUFSIZ, 0);
                        memset(buffer + reader, '\0', BUFSIZ-reader);
                        size = atoi(buffer);
                        fd2 = open(fileName, O_WRONLY | O_TRUNC, 0600);
                        if (fd2 == -1)
                        {
                            fd3 = open(fileName, O_WRONLY | O_CREAT, 0600);
                            if(fd3==-1)
                            {
                                char *relPath = malloc(PATH_MAX);
                                bzero(path, sizeof(path));
                                strcpy(path, fileName);
                                relPath = dirname(path);
                                _mkdir(relPath);
                                fd4 = open(fileName, O_WRONLY | O_CREAT | O_APPEND| O_TRUNC, 0600);
                                if(fd4 == -1)
                                {
                                    errdie("failed to create file and path\n");
                                }
                                char wr[size];
                                bzero(wr, size);
                                write(sockfd, "ready", 6);
                                read(sockfd, wr , size);
                                write(fd4, wr, size);
                                add(project, fileName);
                                close(fd4);
                            }
                            else
                            {
                                char wr[size];
                                bzero(wr, size);
                                write(sockfd, "ready", 6);
                                read(sockfd, wr , size);
                                write(fd3, wr, size);
                                add(project, fileName);
                                close(fd3);
                            }
                        }
                        else
                        {
                            char wr[size];
                            bzero(wr, size);
                            write(sockfd, "ready", 6);
                            read(sockfd, wr , size);
                            write(fd2, wr, size);
                            add(project, fileName);
                            close(fd2);
                        }
                    }
                        counter++;
                        counter = 0;
                }
                bzero(buffer, BUFSIZ);
                bIndx = 0;
            }
            else
            {
                buffer[bIndx]= token;
                bIndx++;
            }
            i++;
        }
    write(sockfd,"$", 2);
    read(sockfd, buffer, BUFSIZ);
    write(sockfd, "ready", 6);
    reader = read(sockfd, ver, BUFSIZ);
    memset(ver + reader, '\0', sizeof(ver) - reader);
    upMan(project, ver);
    write(sockfd,"$", 2);
    close(fd);
    bzero(path, sizeof(path));
    strcpy(path, project);
    strcat(path,"/.Update");
    remove(path);
    exit(0);
}
void update(int sockfd, char* project, char* arg1)
{
    char path[1000];
    char pathUpdate[1000];
    unsigned char* hashcode = NULL;
    int check = 0;
    char buffer[BUFSIZ];
    char hashdump[MD5_DIGEST_LENGTH];
    struct stat file_stat;
    int i = 0, reader, size, update;
    int received = 0;
 
     DIR* dir = opendir(project);
    if (dir) 
    {
        //printf("Project exists\n");
        closedir(dir);
    }
    else if (ENOENT == errno) 
    {
        printf("Project does not exist\n");
        exit(-1);
    } 
    else 
    {
        printf("failed cant open project");
        exit(-1);
    }
 
    strcpy(path, project);
    strcat(path,"/.Manifest");
    int fd = open (path, O_RDONLY, 0);
    if (fd == -1) {
        printf ("%s can't be opened to Update.\n", path);
        exit(-1);
    }
    
    //find size of file to send first 
     if (fstat(fd, &file_stat) < 0)
        {
                errdie("could not get file size\n");
                exit(EXIT_FAILURE);
        }
     sprintf(buffer, "%d", file_stat.st_size);
     strcat(arg1, project);
     strcat(arg1, " ");
     strcat(arg1, path);
     strcat(arg1, " ");
     strcat(arg1, buffer);
     strcat(arg1, "$");
     int sender = 0;
     if((sender = send(sockfd, arg1, strlen(arg1), 0)) == -1)
        {
            errdie("could not send command to server");
        }
     else{
         
         char c[file_stat.st_size];
         reader = read(fd, &c, file_stat.st_size);
         sender = write(sockfd, &c, file_stat.st_size);
         
     }
     //modify
     bzero(buffer, BUFSIZ);
     i = 0;
     while (1)
        {
            if((check = recv(sockfd, buffer, BUFSIZ,0)) == -1)
            {
                errdie("failure to receive data for manifest\n");
            }
            else
            {
                if(strcmp(buffer, "1")==0)
                {
                    //printf("buffer: %s\n", buffer);
                    break;
                }
                else if(strcmp(buffer, "$")==0)
                {
                    printf("manifest version matches no update required!!!\n");
                    exit(0);
                }
                else if(strcmp(buffer, "?")==0)
                {
                    printf("Empty manifest no content in manifest. Abort!\n");
                    exit(-1);
                }
                else
                {
                    hashcode = malloc(MD5_DIGEST_LENGTH);
                    received = strlen(buffer);
                    memset(buffer+received, '\0', sizeof(buffer));
                    hashcode = hash(project, buffer);
                    for(i = 0; i < 2 * MD5_DIGEST_LENGTH; i++)
                    {
                        sprintf(hashdump+(i*2), "%02x", hashcode[i]);
                    }
                    write(sockfd, hashdump, 2 * MD5_DIGEST_LENGTH); 
                    //printf("hashdump: %s\n", hashdump);
                }
            }
            bzero(buffer, BUFSIZ);
        }
        if((check = recv(sockfd, buffer, BUFSIZ,0)) == -1)
        {
                errdie("failure to receive data for Update\n");
        }
        if(strcmp(buffer,"#")!=0){
            size = atoi(buffer);
            char wr[size+1];
            bzero(wr, size);
            strcpy(pathUpdate, project);
            strcat(pathUpdate, "/.Update");  
            update = open (pathUpdate, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0600);
            if (update == -1) 
            {
                printf ("%s can't open update file.\n", pathUpdate);
                exit(-1);
            }
            while(1)
            {
                write(sockfd, "1",2);
                if((check = read(sockfd, wr, size)) == -1)
                {
                    errdie("failure to receive data for manifest\n");
                }
                else{
                    break;
                }
            }
            wr[size+1]= '\0';
            memset(wr + size, '\0', sizeof(wr)- size);
            write(update, wr, size);
            printf("Update: \n%s", wr);
            bzero(buffer, sizeof(buffer));
        }
        else
        {
            printf("There is a Conflict File!!!\n");
            write(sockfd, "ready", 6);
            if((check = recv(sockfd, buffer, BUFSIZ,0)) == -1)
            {
                errdie("failure to receive data for Update\n");
            }
            size = atoi(buffer);
            char wr[size];
            bzero(wr, size);
            strcpy(pathUpdate, project);
            strcat(pathUpdate, "/.Conflict"); 
            update = open (pathUpdate, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0600);
            if (update == -1) 
            {
                printf ("%s can't open update file.\n", pathUpdate);
                exit(-1);
            }
             while(1)
            {
                write(sockfd, "Ready", 6);
                if((check = read(sockfd, wr, size)) == -1)
                {
                    errdie("failure to receive data for manifest\n");
                }
                else{
                    break;
                }
            }
            memset(wr + size, '\0', sizeof(wr)- size);
            write(update, wr, size);
            bzero(buffer, sizeof(buffer));
        }
     exit(0);
}
void configure(char* ipHost, char* portnum)
{
    int fileConfig, i;
     if(checkWrite == 0)
        {
            fileConfig = open(".configure", O_CREAT | O_WRONLY | O_APPEND | O_TRUNC, 0600);
            checkWrite ++;
        }
        else
        {
            fileConfig = open(".configure", O_CREAT | O_WRONLY | O_APPEND, 0600);
        }
        if(fileConfig == -1){
            printf("Failed to create and write to write_file");
            return;
        }
         i = 0;
        while(i < (strlen(ipHost)))
        {
            //printf("this is char: %c\n", ipHost[i]);
            write(fileConfig, &ipHost[i], 1);
            if(i == strlen(ipHost)-1){
                write(fileConfig, " ", 1);
            }
            i++;
        }
        i=0;
         while(i < (strlen(portnum)))
        {
            //printf("this is char: %c\n", portnum[i]);
            write(fileConfig, &portnum[i], 1);
            i++;
        }
    
    close(fileConfig);
    
}
 
void readConfigure(int a)
{
 
    int fd, num_Bytes, i, loadIndx, delim;
    loadIndx = 0;
    char token;
    char loadArr[1000];
    int checkArr = 0;
    fd = open(".configure", O_RDONLY,0);
    if(fd == -1){
        printf("Failed to open and read file");
        exit(-1);
    }
     while(fd != -1)
    {
        num_Bytes = read(fd, &token, 1);
         if((num_Bytes == 0) || (isspace(token) != 0))
        {
            if((num_Bytes == 0) && (isspace(token)!=0)){
                break;
            }
            if(checkArr == 0){
             ip = malloc(sizeof(char)*(loadIndx+1));
            }
            else if( checkArr == 1)
            {
                portserver = malloc(sizeof(char)*(loadIndx+1));
            }
            if(checkArr == 0)
            {
                for(i = 0; i <= loadIndx; i++)
                {
                    if(i == loadIndx)
                    {
                        ip[loadIndx] = '\0';
                    }
                    else
                    {
                        ip[i] = loadArr[i];
                    }
                }
            }
            else if(checkArr == 1)
            {
                for(i = 0; i <= loadIndx; i++)
                {
 
                    if(i == loadIndx)
                    {
                        portserver[loadIndx] = '\0';
                    }
                    else
                    {
                        portserver[i] = loadArr[i];
                    }
 
                }
 
            }
           
            loadIndx = 0; //reset next word
            checkArr = checkArr + 1;
            //stop at end of file
            if(num_Bytes == 0)
            {
                break;
            }
        }
        else
        {
            loadArr[loadIndx] = token;
            loadIndx ++;
        }
    }
 
    //printf("ipArr: %s\n", ip);
    //printf("portnum: %s\n", portserver);
    close(fd);
 
}
 
int argComp(char* string)
{
    if(strcmp(string, "configure") == 0)
    {
        return 1;
    }
    else if(strcmp(string, "create") == 0)
    {
        return 2;
    }
    else if(strcmp(string, "add") == 0)
    {
        return 3;
    }
    else if(strcmp(string, "remove") == 0)
    {
        return 4;
    }
    else if(strcmp(string, "update") == 0)
    {
        return 5;
    }
    else if(strcmp(string, "upgrade") == 0)
    {
        return 6;
    }
    else if(strcmp(string, "destroy") == 0)
    {
        return 7;
    }
    else if(strcmp(string, "checkout") == 0)
    {
        return 8;
    }
    else if(strcmp(string, "currentversion") == 0)
    {
        return 9;
    }
     else if(strcmp(string, "currentversion") == 0)
    {
        return 9;
    }
     else if(strcmp(string, "commit") == 0)
    {
        return 10;
    }
     else if(strcmp(string, "push") == 0)
    {
        return 11;
    }
     else if(strcmp(string, "history") == 0)
    {
        return 12;
    }
    else if(strcmp(string, "rollback") == 0)
    {
        return 13;
    }
 
    return 0;
}
 
void errdie(const char*fmt, ...)
{
    int errno_s;
    va_list ap;
 
    //save errno
    errno_s = errno;
 
    //print args to std out
    va_start(ap, fmt);
    vfprintf(stdout, fmt, ap);
    fprintf(stdout, "\n");
    fflush(stdout);
 
    //print out error message iff errno was saved
    if(errno_s!= 0)
    {
        fprintf(stdout, "(errno = %d) : %s \n", errno_s, strerror(errno_s));
        fprintf(stdout,"\n");
        fflush(stdout);
    }
    va_end(ap);
    
    //terminate with error
    exit(1);
 
}
 
unsigned char* hash(char* project, char* path)
{
    //unsigned char c[MD5_DIGEST_LENGTH];
    unsigned char* c = malloc(MD5_DIGEST_LENGTH);
    int i = 0;
    char file_size[256];
    struct stat file_stat;
    MD5_CTX mdContext;
    int bytes;
    char token[1000];
    int fd = open (path, O_RDONLY, 0);
    if (fd == -1) {
        printf ("%s can't be opened to hash. File does not exist in project.\n", path);
        remove_line(project, path);
        exit(-1);
    }
 
    //find size of file before hash 
     if (fstat(fd, &file_stat) < 0)
        {
                errdie("could not get file size\n");
                exit(EXIT_FAILURE);
        }
        
        char data[file_stat.st_size];
        MD5_Init (&mdContext);
        while ((bytes = read(fd, &data, file_stat.st_size )) != 0)
        {
            //printf("bytes : %d\n", bytes);
            //printf("string: %s\n", data);
           MD5_Update (&mdContext, data, bytes);
        }
        MD5_Final (c,&mdContext);
    
    close(fd);
    //printf("c: %02x\n", c);
    return c;
    
}
int* remove_line(char* project, char* file_name)
{
    char path[1000];
    char file_path[1000];
    int fd, temp, reader, writer;
    int *removed = malloc(sizeof(int));
    char buffer[1000];
    char token;
    char *ptr;
    int line = 0;
    int buf_indx = 0;
    int check = 0;
 
    strcpy(path, project);
    strcat(path, "/");
    strcat(path, ".Manifest");
 
    strcpy(file_path, project);
    strcat(file_path, "/");
    strcat(file_path, "tempManifest");
 
    fd = open(path, O_RDONLY, 0 );
    if(fd == -1)
    {
        printf("path: %s\n", path);
        errdie("Failed to open project manifest. Project may not exist\n");
    }
 
    temp = open(file_path, O_WRONLY | O_CREAT | O_APPEND, 0600);
     if(temp == -1)
    {
        errdie("Failed to open project manifest. Project may not exist\n");
    }
    while(fd != -1)
    {
        reader = read(fd, &token, 1);
        //printf("token %c\n", token);
        if(token== '\n' || reader == 0){
            if(reader == 0 &&(isspace(token)!=0)) break;
            else if((strstr(buffer,file_name) == NULL) || line == 0)//not present write
            {
                if(check != 0){
                    writer = write(temp, "\n", 1 );
                }
                writer = write(temp, buffer, buf_indx);
                //printf("token %s\n", buffer);
                check++;
                line ++;
            }
            else//present dont write
            {
                ptr = &buffer[0]; 
                int place = atoi(ptr);
                *removed = place;
            }
 
            if(reader == 0)
            {
                break;
            }
 
            bzero(buffer, buf_indx);
            buf_indx = 0;
        }
        else
        {
            buffer[buf_indx] = token;
            //printf("token %c\n", token);
            //printf("buffer %s\n", buffer);
            buf_indx = buf_indx + 1;
        }
    }
    close(fd);
    close(temp);
    remove(path);
    rename(file_path, path);
 
    if(removed == 0)
    {
        printf("did not remove\n");
        return removed;
    }
    else
    {
        //printf("value: %d\n", *removed);
        return removed;
    }
 
}
void add(char* project, char* file_name)
{
    int fd, i, writer, removed;
    int* version = NULL;
    char* sendFile;
    char* aPath = malloc(PATH_MAX);
    char path[1000];
    char checkHash[1000];
    char file_path[1000];
    char file_size[256];
    char actualpath [PATH_MAX + 1];
    struct stat file_stat;
    unsigned char* hashcode = NULL;
    char hashdump[32];
 
    sendFile = malloc(strlen(file_name+1));
    sendFile = basename(file_name);
    bzero(path, 1000);
    strcpy(path, project);
    strcat(path, "/");
    strcat(path, ".Manifest");
    //printf("pathname: %s\n", path);
 
    strcpy(file_path, project);
    strcat(file_path, "/");
    strcat(file_path, sendFile);
    //printf("pathname: %s\n", file_path);
    bzero(aPath, sizeof(aPath));
    aPath = recursive(project, file_name);
    //realpath(file_path, actualpath);
    if(aPath == NULL)
    {
       printf("file does not exist in directory cannot hash and add to manifest\n");
       exit(0);
    }
    int check  = checkMan(aPath, project);
    fd = open(path, O_WRONLY | O_APPEND, 0600 );
    if(fd == -1)
    {
        errdie("Failed to open project manifest. Project may not exist\n");
    }
    if (fstat(fd, &file_stat) < 0)
        {
                errdie("could not get file size\n");
                exit(EXIT_FAILURE);
        }
    if(check == 1)
    {
        hashcode = malloc(MD5_DIGEST_LENGTH);
        hashcode = hash(project, aPath);
        
        for(i = 0; i < MD5_DIGEST_LENGTH; i++)
        {
            sprintf(hashdump+(i*2), "%02x", hashcode[i]);
        } 
        //printf("hashdump: %s\n", hashdump);
        strcpy(checkHash, aPath);
        strcat(checkHash, " ");
        strcat(checkHash, hashdump);
        check  = checkMan(checkHash, project);
        if(check == 1)
        {
            printf("file name already exists with same content\n");
            close(fd);
        }
        else{
            close(fd);
            char ver[1000];
            version = remove_line(project, aPath);
            *version = *version + 1;  
            sprintf(ver , "%d",*version);
 
            removed = open(path, O_WRONLY | O_APPEND, 0600 );
            if(removed == -1)
            {
                errdie("Failed to open Removed\n");
            }
            writer = write(removed, "\n", 1);
            writer = write(removed, ver, strlen(ver));
            writer = write(removed, " ", 1);
            writer = write(removed, aPath, strlen(aPath));
            writer = write(removed, " ", 1);
            writer = write(removed, hashdump, 2 * MD5_DIGEST_LENGTH);
        }
        close(removed);
    }
    else{
        hashcode = malloc(MD5_DIGEST_LENGTH);
        writer = write(fd, "\n", 1);
        writer = write(fd, "1 ", 2);
        writer = write(fd, aPath, strlen(aPath));
        hashcode = hash(project, aPath);
        //printf("hashey: %02x\n", hashcode);
        for(i = 0; i < 2 * MD5_DIGEST_LENGTH; i++)
        {
            //printf("hashcode: %02x\n", hashcode[i]);
            sprintf(hashdump+(i*2), "%02x", hashcode[i]);
        } 
        //printf("hashdump: %s\n", hashdump);
        writer = write(fd, " ", 1);
        writer = write(fd, hashdump, 2 * MD5_DIGEST_LENGTH);
        close(fd);
    }
}
 
void create(int sockfd, char* project, char* arg1)
{
     char buffer[BUFSIZ];
     int file_size;
     char path [1000];
     if(send(sockfd, arg1, strlen(arg1), 0) == -1)
        {
            errdie("could not send command to server");
        }
        else
        {
            int fd;
            int len = 0;
            struct stat st = {0};
            if(stat(project, &st) == -1)
            {
                mkdir(project, 0700);
            }
            else
            {
                errdie("Project with that name already exists!!!\n");
            }
            strcpy(path, project);
            strcat(path, "/");
            strcat(path, ".Manifest");
            //printf("pathname: %s\n", path);
        
            while (1)
            {
                 if(recv(sockfd, buffer, BUFSIZ,0) == -1)
                 {
                     errdie("failure to receive data for manifest\n");
                 }
                 else
                 {
                     //printf("buffer: %d\n", buffer);
                     break;
                 }
            }
            fd = open(path, O_WRONLY | O_APPEND | O_CREAT, 0600);
            if(fd == -1)
            {
                printf("ERROR creating manifest for project\n");
                exit(-1);
            }
             file_size = atoi(buffer);
             //printf("this is file size: %d\n", file_size);
            int i=0;
            while(1){
                write(sockfd, "ready", 6);
                if((len = recv(sockfd, buffer, 1, 0)) > 0)
                {
                    write(fd, buffer, sizeof(char));
                    file_size -= len;
                    //printf("Receive %d bytes andwhat we wrote :- %c bytes\n", len, buffer[i]);
                    i ++;
                }
                if(file_size <=0)
                {
                    break;
                }
                if(len == -1){
                    errdie("error: did not receive contents of file");
                }
                //printf("this if filesize: %d\n", file_size);
            }
            //printf("this is length: %d\n", len);
            close(file_size);
        }
        
}
int checkMan(char* file_name, char* project)
{
    char path[1000];
    int fd, reader;
    char buffer[1000];
    char token;
    int buf_indx = 0;
 
    bzero(path, sizeof(path));
    strcpy(path, project);
    strcat(path, "/");
    strcat(path, ".Manifest");
 
    fd = open(path, O_RDONLY, 0 );
    if(fd == -1)
    {
        errdie("Failed to open project manifest. Project may not exist\n");
    }
 
    while(fd != -1)
    {
        reader = read(fd, &token, 1);
        //printf("token: %c\n", token);
        if(token== '\n' || reader == 0){
            //buf_indx= buf_indx + 1;
            //buffer [buf_indx] = '\0';
            memset(buffer+buf_indx, '\0', sizeof(buffer)-buf_indx);
    
            if(reader == 0 &&(isspace(token)!=0)) break;
            else if(strstr(buffer,file_name) == NULL)//not found
            {
               
            }
            else//found
            {
                close(fd);
                return 1;
            }
 
            if(reader == 0)
            {
                break;
            }
 
            bzero(buffer, buf_indx);
            buf_indx = 0;
        }
        else
        {
            buffer[buf_indx] = token;
            //printf("char: %c\n", buffer[buf_indx]);
            buf_indx = buf_indx + 1;
        }
    }
    close(fd);
 
    return 0;
 
}
char *recursive(char *basePath, char *fileName)
{
    char path[1000];
    char *check1 = malloc(PATH_MAX);
    char *retPath = malloc(PATH_MAX);
    struct dirent* input_file;
    int check = 0;
    DIR* FD = opendir(basePath);
    if (NULL == (FD = opendir (basePath))) //open input directory
    {
        check = checkFile(basePath);
        if(check == 1 && strstr(basePath, fileName)!=NULL)
        {
           int i;
           for(i = 0; i<= strlen(basePath);i++){
                retPath[i] = basePath[i];
           }    
            memset(retPath + i, '\0', sizeof(retPath)-1);
            return retPath;
        }
        return;
    }
    while ((input_file = readdir(FD)) != NULL)
    {
        if (strcmp(input_file->d_name, ".") != 0 && strcmp(input_file->d_name, "..") != 0)
        {
            //printf("file: %s\n", input_file->d_name);
            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, input_file->d_name);
            check1 = recursive(path, fileName);
            if(check1 !=NULL)
            {
                return check1;
            }
        }
    }
    closedir(FD);
}
int checkFile(char *basePath)
{
    int nBytes, fd;
    char token;
    int entry_file, write_file;
    fd = open(basePath, O_RDONLY,0);
    //check file descriptor
    if(fd == -1){
        printf("Failed to open and read file");
        return 0;
    }
    return 1;
}
struct node* insertValues(struct node* head, char* path){
    /*First open the .Manifest in the server repository.
    - compare the server's and client's .Manifest version numbers.
        - if they are equal, return and do nothing.
        - if they are not the same, something has to change.
            - break up the file, where each node in a list is a line from the file.
    */
    //head = NULL;
    char* vNum;
    char* file;
    char* hash;
    char vNumA[100];
    char fileA[PATH_MAX+1];
    char hashA[32];
    int fd, reader;
    char c;
    int counter = 0;
    fd = open(path, O_RDONLY, 0600);
    int i = 0;
    int j = 0;
    int last = 0;
    struct stat file_stat;
    
    if (fstat(fd, &file_stat) < 0)
    {
        errdie("could not get file size\n");
        exit(EXIT_FAILURE);
    }
    while( (reader = read(fd, &c, 1)) != -1){
        last = last + 1;
        if(c == ' '){
            if(counter == 0){
                memset(vNumA + i,'\0',sizeof(vNumA)- i);
                vNum = malloc(i);
                
                for(j = 0; j < i; j++){
                    vNum[j] = vNumA[j];
                }
                vNum[i] = '\0';
                memset(vNumA, '\0', i);
            }
            else if(counter == 1){
                memset(fileA + i,'\0',sizeof(fileA) - i);
                i++;
                file = malloc(i);
                for(j = 0; j < i; j++){
                    file[j] = fileA[j];
                }
                file[i] = '\0';
                memset(fileA, '\0', i);
            }
            else if(counter == 2){
                i++;
                hash = malloc(i);
                for(j = 0; j < i; j++){
                    hash[j] = hashA[j];
                }
                memset(hashA, '\0', i);
            }
            counter++;
            i = 0;
        }
        else if(c == '\n' || (last == file_stat.st_size + 1)){
            if(counter == 0){
                memset(vNumA + i,'\0', sizeof(vNumA) - i);
                vNum = malloc(i);
                
                for(j = 0; j < i; j++){
                    vNum[j] = vNumA[j];
                }
                vNum[i] = '\0';
                memset(vNumA, '\0', i);
            }
            else if(counter == 1){
                memset(fileA + i,'\0', sizeof(fileA) - i);
                i++;
                file = malloc(i);
                for(j = 0; j < i; j++){
                    file[j] = fileA[j];
                }
                file[i] = '\0';
                memset(fileA, '\0', i);
            }
            else if(counter == 2){
                i++;
                hash = malloc(i);
                for(j = 0; j < i; j++){
                    hash[j] = hashA[j];
                }
                memset(hashA, '\0', i);
            }
            counter = 0;
            i = 0;
            head = makeList(head, vNum, file, hash);
            if(last == file_stat.st_size + 1) break;
        }
        else if(counter == 0){
            vNumA[i] = c;
 
            i++;
        }
        else if(counter == 1){
            fileA[i] = c;
            i++;
        }
        else if(counter == 2){
            hashA[i] = c;
            i++;
        }
    }
  
    memset(vNumA, '\0', sizeof(vNumA));
    memset(fileA, '\0', sizeof(fileA));
    memset(hashA, '\0', sizeof(hashA));
    count = 0;
    return head;
}
struct node* makeList(struct node* head, char* vNum, char* file, char* hash)
{
    if (head == NULL) {
        count++;
        head = (struct node*)malloc(sizeof(struct node*));
        if(vNum != NULL){
 
            head->versionNumber = vNum;
        }
        /*if(file != NULL){
            head->name = file;
            printf("This is the name: %s\n", head->name);
        }
        if(hash != NULL){
            head->hashcode = hash;
            printf("This is the hashcode: %s\n", head->hashcode);
        }*/
        head->next = NULL;
        return head;
    }
    struct node* pt = head;
    if(count == 1){
        pt->next = NULL;
    }
    while (pt->next != NULL) {
        //printf("Heree\n");
        pt = pt->next;
    } 
    count++;
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    newNode->versionNumber = vNum;
    newNode->name = file;
    newNode->hashcode = hash;
    newNode->next = NULL;
    pt->next = newNode;
    return head;
}
void commitCom(struct node* head, struct node* head2, int connfd, char *project){
    int i = 0, vptr = 0, vptr2 = 0, j = 0;
    struct node* ptr = head; // this is the list of the manifest from the client.
    struct node* ptr2 = head2; // this is the list of the manifest from the server.
    unsigned char *hashcode = NULL;
    char modHash[32];
    char buffer[100];
    char buffer2[100], path[1000];
    struct stat fileStatM;
    struct stat fileStatC;
    int check = 0;
 
    bzero(path, 1000);
    strcpy(path, project);
    strcat(path, "/");
    strcat(path, ".Commit");
 
    int fd = open(path, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0600);
    if(ptr->next != NULL) ptr = ptr->next;
    if(ptr2->next != NULL) ptr2 = ptr2->next;
 
    if(strcmp(head->versionNumber, head2->versionNumber) != 0){
        printf("Update local project, before commit!!!\n");
        check = 1;
        write(connfd, "0", 2);
        remove(path);
        return;
    }
    else{
        while(ptr != NULL){ // Loop for add.
            if(ptr->name == NULL) break;
            while(ptr2 != NULL){
                // if the file name from the client list is equal to the file name in the server list, stop. 
                // delete does not have to be performed.
                if(ptr2->name == NULL)
                {
                    ptr2 = ptr2->next;
                }
                else if(strcmp(ptr2->name, ptr->name) == 0){
                    i = 1;
                    vptr = atoi(ptr->versionNumber);
                    vptr2 = atoi(ptr2->versionNumber);
                    if((strcmp(ptr->hashcode, ptr2->hashcode) == 0)){
                        //printf("This is the name for ptr2->name: %s\n", ptr2->name);
                        hashcode = malloc(MD5_DIGEST_LENGTH);
                        hashcode = hash(project, ptr2->name);
                        //printf("hashcode live: %s\n", hashcode);
                        for(j = 0; j < MD5_DIGEST_LENGTH; j++)
                        {
                            sprintf(modHash+(j*2), "%02x", hashcode[j]);
                        }
                        //printf("This is the ptr2->hashcode and its length: %s, %d\n", ptr2->hashcode, strlen(ptr2->hashcode));
                        //printf("This is the modHash and its length: %s, %d\n", modHash, strlen(modHash));
                        if(strcmp(ptr2->hashcode, modHash) != 0){ // have to modify if the hashes are diff.
                            //printf("The file was not found, have to write to .Update for Modify: %s\n", ptr2->name);;
                            write(fd, "M ", 2);
                            write(fd, ptr2->name, strlen(ptr2->name));
                            write(fd, " ", 1);
                            write(fd, ptr2->hashcode, strlen(ptr2->hashcode));
                            write(fd, "\n", 1);
                        }
                    }
                    else if((strcmp(ptr->hashcode, ptr2->hashcode) != 0) && (vptr <= vptr2))
                    {
                        printf("client must synch with the repository before committing\n");
                        remove(path);
                        write(connfd, "0", 2);
                        return;
                    }
                    
                    //write(connfd, "1", 1);
                    break;
                }
                else
                {
                    ptr2 = ptr2->next;
                } 
            }
            if(i != 1){ // file was not found.
                //memset(ptr2->name + strlen(ptr2->name), '\0', strlen(ptr2->name));
                //printf("The file was not found, have to write to .Update for Add: %s\n", ptr->name);
                write(fd, "A ", 2);
                write(fd, ptr->name, strlen(ptr->name));
                write(fd, " ", 1);
                write(fd, ptr->hashcode, strlen(ptr->hashcode));
                write(fd, "\n", 1);
            }
            else{
                //printf("The file was found, continue to the next server file.\n");
                i = 0;
            }
            ptr2 = head2->next;
            ptr = ptr->next;
        }
 
        ptr = head->next;
        ptr2 = head2->next;
 
        while(ptr2 != NULL){ // Loop for delete.
            while(ptr != NULL){
                // if the file name from the client list is equal to the file name in the server list, stop. 
                // Add does not have to be performed.
                if(ptr->name == NULL)
                {
                    ptr = ptr->next;
                }
                else if(strcmp(ptr->name, ptr2->name) == 0){
                    //ptr = head;
                    i = 1;
                    break;
                }
                else{
                    ptr = ptr->next;
                } 
            }
            if(i != 1){ // file was not found.
                //memset(ptr2->name + strlen(ptr2->name), '\0', strlen(ptr2->name));
                //printf("The file was not found, have to write to .Update for Delete, %s\n", ptr2->hashcode);
                //printf("Here is the length of the hashcode: %d\n", strlen(ptr2->hashcode));
                //memset(ptr2->name + strlen(ptr2->hashcode) - 1, '\0', 1);
                write(fd, "D ", 2);
                write(fd, ptr2->name, strlen(ptr2->name));
                write(fd, " ", 1);
                int n = write(fd, ptr2->hashcode, strlen(ptr2->hashcode));
                //printf("Here is the length of the hashcode again: %d\n", strlen(ptr2->hashcode));
 
                write(fd, "\n", 1);
            }
            else{
                //printf("The file was found, continue to the next client file.\n");
                i = 0;
            }
            ptr = head->next;
            ptr2 = ptr2->next;
        }
    }
    write(connfd, "1", 2);
    memset(modHash, '\0', strlen(modHash));
    close(fd);    
    fd = open(path, O_RDONLY, 0);
    if(fstat(fd, &fileStatM) < 0){
        errdie("could not get a file size\n");
    }
    char one[1];
    char m[fileStatM.st_size];
    read(fd, &m, fileStatM.st_size);
    sprintf(buffer, "%d", fileStatM.st_size);
    write(connfd, ip, strlen(ip));
    read(connfd, one, 2);
    write(connfd, buffer, fileStatM.st_size);
    send(connfd, &m, fileStatM.st_size-1, 0);
    return;
}
void* upMan(char* project, char* ver)
{
    char path[1000];
    char file_path[1000];
    int fd, temp, reader, writer;
    int *removed = malloc(sizeof(int));
    char buffer[1000];
    char token;
    char *ptr;
    int line = 0;
    int buf_indx = 0;
    int check = 0;
 
    strcpy(path, project);
    strcat(path, "/");
    strcat(path, ".Manifest");
 
    strcpy(file_path, project);
    strcat(file_path, "/");
    strcat(file_path, "tempManifest");
 
    fd = open(path, O_RDONLY, 0 );
    if(fd == -1)
    {
        printf("path: %s\n", path);
        errdie("Failed to open project manifest. Project may not exist\n");
    }
 
    temp = open(file_path, O_WRONLY | O_CREAT | O_APPEND, 0600);
     if(temp == -1)
    {
        errdie("Failed to open project manifest. Project may not exist\n");
    }
    while(fd != -1)
    {
        reader = read(fd, &token, 1);
        //printf("token %c\n", token);
        if(token== '\n' || reader == 0){
            if(reader == 0 &&(isspace(token)!=0)) break;
            else if(line == 0)//upgrade version
            {
                writer = write(temp, ver, strlen(ver));
                line ++;
            }
            else//present dont write
            {
                buffer[buf_indx] = '\0';
                write(temp, "\n", 1);
                write(temp, buffer, strlen(buffer));
            }
 
            if(reader == 0)
            {
                break;
            }
 
            bzero(buffer, buf_indx);
            buf_indx = 0;
        }
        else
        {
            buffer[buf_indx] = token;
            //printf("token %c\n", token);
            //printf("buffer %s\n", buffer);
            buf_indx = buf_indx + 1;
        }
    }
 
    close(fd);
    close(temp);
    remove(path);
    rename(file_path, path);
 
}


