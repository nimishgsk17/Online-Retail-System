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

int main()
{
    int fd,lastno=1;
    fd = open("db.dat", O_RDWR | O_CREAT , 0755);
    write(fd,&lastno,sizeof(int));
}