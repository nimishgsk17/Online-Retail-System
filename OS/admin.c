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
#include "dbinfo.h"
#include "comm.h"
#include "servUtils.h"
#include "clientUtils.h"


int main()
{
    struct sockaddr_in serv,cli;
    memset(&cli, 0, sizeof(cli));
    int fd,lastno,fd_auth;
    
    struct Product buffer;

    fd = open("db.dat", O_RDWR | O_CREAT , 0755);
    fd_auth = open("login_info.dat", O_RDWR | O_CREAT , 0755);
    lseek(fd,0,SEEK_SET);
    // int ret = read(fd,&lastno,sizeof(int));
    // if (ret == 0 || ret == -1)
    // {
    //     lseek(fd,0,SEEK_SET);
    //     lastno = 1;
    //     write(fd,&lastno,sizeof(int));
    // }
    
    read(fd,&lastno,sizeof(int));

    int sockfd=socket(AF_INET,SOCK_STREAM,0);

	serv.sin_family=AF_INET;
	serv.sin_addr.s_addr=inet_addr("127.0.0.1");
	serv.sin_port=htons(5000);

    bind(sockfd,(struct sockaddr *)&serv,sizeof(serv));

    listen(sockfd,10);

    while (1) 
    {
        struct Client_req message;
        int size = sizeof(cli);
        int nsd = accept(sockfd,(struct sockaddr *)&cli,&size);
        int ret = fork();

        if (ret==0) 
        {
            close(sockfd);
            
            while(1)
            {
                recv(nsd,&message,sizeof(struct Client_req),0);
                //printf("%d %d %s %d %d\n",message.mode,message.funct,message.args[0],message.params[0],message.params[1]);
              
                if (message.mode == ADMIN_MODE)
                {
                    if (message.funct == ADD)
                    {
                        lockFileForWriting(&fd);
                        int retr = addProduct(message.args[0],message.params[0],message.params[1],&fd);
                        if (retr == 0)
                        {
                            int ack = ACK;
                            send(nsd,&ack,sizeof(ack),0);
                            unlockFile(&fd);
                        }
                        else if (retr == ERROR_DUP)
                        {
                            int ack = DUP_NACK;
                            send(nsd,&ack,sizeof(ack),0);
                            unlockFile(&fd);
                        }
                        printOnConsole(&fd);
                    }

                    else if (message.funct == DELETE_KEY)
                    {
                        lockFileForWriting(&fd);
                        deleteProductByKey(message.params[0],&fd);
                        int ack = ACK;
                        send(nsd,&ack,sizeof(ack),0);
                        unlockFile(&fd);

                        printOnConsole(&fd);
                    }

                    else if (message.funct == DELETE_NAME)
                    {
                        lockFileForWriting(&fd);
                        deleteProductByName(message.args[0],&fd);
                       
                        int ack = ACK;
                        send(nsd,&ack,sizeof(ack),0);
                        unlockFile(&fd);

                        printOnConsole(&fd);
                    
                    }

                    else if (message.funct == UPDATE_PRICE)
                    {
                        lockFileForWriting(&fd);
                        updatePrice(message.params[0],message.params[1],&fd);
                        int ack = ACK;
                        send(nsd,&ack,sizeof(ack),0);
                        unlockFile(&fd);

                        printOnConsole(&fd);
                    }

                    else if (message.funct == UPDATE_QUANTITY)
                    {
                        lockFileForWriting(&fd);
                        updateQuantity(message.params[0],message.params[1],&fd);
                        int ack = ACK;
                        send(nsd,&ack,sizeof(ack),0);
                        unlockFile(&fd);

                        printOnConsole(&fd);
                    }

                    else if (message.funct == ADMIN_DISPLAY)
                    {
                        int noSent;
                        noSent = adminDisplaySent(message.params[0],&fd);
                        send(nsd,&noSent,sizeof(noSent),0);
                        
                        adminDisplay(message.params[0],&fd,&nsd);

                    }

                    else if (message.funct == ADMIN_QUIT)
                    {
                        int noSent,command;
                        noSent = adminDisplaySent(1,&fd);
                        send(nsd,&noSent,sizeof(noSent),0);

                        adminDisplay(message.params[0],&fd,&nsd);

                        recv(nsd,&command,sizeof(int),0);
                        if (command == 1)
                        {
                            endServ(&fd,&fd_auth,&sockfd,0);
                            exit(0);
                        }

                    }

                    
                }

                else if(message.mode == USER_MODE)
                {
                    if (message.funct == DISPLAY_PRODUCTS)
                    {
                        int noSent;
                        noSent = userDisplaySent(&fd);
                        send(nsd,&noSent,sizeof(noSent),0);
                        
                        userDisplay(&fd,&nsd);

                    }

                    else if (message.funct == DISPLAY_CART)
                    {
                        ;
                    }

                    else if (message.funct == ADD_CART)
                    {
                        struct Product toAdd = {0};
                        strcpy(toAdd.P_Name,message.args[0]);
                        toAdd.Stock = message.params[0];
                        int ret = checkAddCart(&toAdd,&fd);

                        send(nsd,&ret,sizeof(ret),0);

                        if (ret == 0)
                        {
                            send(nsd,&toAdd,sizeof(struct Product),0);
                        }
                        
                    }

                    else if (message.funct == DELETE_FROM_CART)
                    {
                        ;

                    }

                    else if (message.funct == CHANGE_QUANTITY)
                    {
                        struct Product toUpdate = {0};
                        strcpy(toUpdate.P_Name,message.args[0]);
                        toUpdate.Stock = message.params[0];
                        int ret = checkUpdateCart(&toUpdate,&fd);

                        send(nsd,&ret,sizeof(ret),0);

                        if (ret == 0)
                        {
                            send(nsd,&toUpdate,sizeof(struct Product),0);
                        }

                    }

                    else if (message.funct == BUY)
                    {
                        lockFileForWriting(&fd);

                        int arr[CART_SIZE] = {0};
                        struct Product tempCart[CART_SIZE] = {0};
                        struct Product buf;
                        int command;
                        int ack;
                        // printf("Before Recv...\n");
                        
                        recv(nsd,tempCart,CART_SIZE*sizeof(struct Product),0);
                        
                        // printf("Before checkavailability...\n");
                        int ret = checkAvailability(tempCart,arr,CART_SIZE,&fd);
                        // printf("%d\n",ret);
                        // for(int i = 0;i<5;i++)
                        // {
                        //     printf("%s %d %d\n",tempCart[i].P_Name,tempCart[i].Stock,arr[i]);
                        // }

                        send(nsd,&ret,sizeof(ret),0);
                        send(nsd,arr,sizeof(int)*CART_SIZE,0);

                        recv(nsd,&command,sizeof(int),0);

                        if (command == 1)
                        {
                            payForCart(tempCart,&fd,CART_SIZE);
                            ack = ACK;
                            send(nsd,&ack,sizeof(int),0);
                            unlockFile(&fd);
                        }

                        else if (command == 2)
                        {
                            unlockFile(&fd);
                        }
                        
                        else if (command == 3)
                        {
                            unlockFile(&fd);
                        }
                        


                    }

                    else if (message.funct == USER_QUIT)
                    {
                        close(sockfd);
                        close(fd);
                        close(fd_auth);
                        exit(0);
                          
                    }

                }
            }    
           
            
        }
        else
        {
          close(nsd);
        }
    }


    close(fd);


    return 0;
}