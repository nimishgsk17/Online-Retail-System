#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#include "comm.h"

int lockFileForReading(int *fd);
int lockFileForWriting(int *fd);
int unlockFile(int *fd);

int main()
{
    int fd = open("login_info.dat",O_RDWR|O_CREAT , 0755);
    printf("WELCOME TO REGISTRATION\n");
    
    while (1)
    {
        struct User user = {0},buff;
        int nameExists = 0;
        printf("admin (%d) or user (%d) account,(press 2 to quit): ",ADMIN,USER);
        scanf("%d",&user.account_type);

        lockFileForWriting(&fd);
        printf("Gaining access...\n");

        if (user.account_type == 2)
        {
            break;
        }

        else if (user.account_type != ADMIN && user.account_type != USER)
        {
            printf("No such account type.\n");
            continue;
        }

        else if (user.account_type == ADMIN)
        {
            printf("Creating admin account...\n");
        }

        else if (user.account_type == USER)
        {
            printf("Creating user account...\n");
        }

        printf("Enter username and password to register: ");
        scanf("%s%s",user.username,user.password);
        user.isLoggedIn = 0;

        lseek(fd,0,SEEK_SET);
        while (read(fd,&buff,sizeof(struct User)))
        {
            if (strcmp(buff.username,user.username)==0)
            {
                printf("This usename already exists...\n");
                nameExists = 1;
                break;
            }
        }

        if (nameExists == 1)
        {
            continue;
        }

        lseek(fd,0,SEEK_END);
        int ret_write = write(fd,&user,sizeof(struct User));
        
        if (ret_write == -1)
        {
            perror("write");
        }

        else if (ret_write == sizeof(struct User))
        {
            printf("Account succesfully created...\n");
        }

        unlockFile(&fd);
    }    

    return 0;
}

int lockFileForReading(int *fd) 
{
    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    return fcntl(*fd, F_SETLKW, &lock);
}


int lockFileForWriting(int *fd) 
{
    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    return fcntl(*fd, F_SETLKW, &lock);
}


int unlockFile(int *fd) 
{
    struct flock lock;
    lock.l_type = F_UNLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    return fcntl(*fd, F_SETLKW, &lock);
}