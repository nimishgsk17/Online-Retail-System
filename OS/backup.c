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
#include "dbinfo.h"


int main()
{
    struct sockaddr_un serv,cli;
    int fd,lastno;
    
    struct Product buffer;

    fd = open("db.dat", O_RDWR | O_CREAT , 0755);
    lseek(fd,0,SEEK_SET);
    int ret = read(fd,&lastno,sizeof(int));
    if (ret == 0 || ret == -1)
    {
        lseek(fd,0,SEEK_SET);
        lastno = 1;
        write(fd,&lastno,sizeof(int));
    }
    

    int sockfd = socket(AF_UNIX,SOCK_STREAM,0);

    serv.sun_family = AF_UNIX;
    strcpy(serv.sun_path,".");
    bind(sockfd,(struct sockaddr *)&serv,sizeof(serv));

    listen(sockfd,10);

    while(1)
    {
        int option,ad_option;
        printf("1. Admin\n");
        printf("2. User\n");
        printf("3. End\n");
        printf("Choose option: ");
        scanf("%d",&option);

        if(option == 1)
        {
            printf("Welcome ADMIN\n");
            printf("1. ADD PRODUCT\n");
            printf("2. DELETE PRODUCT\n");
            printf("3. UPDATE PRICE\n");
            printf("4. UPDATE QUANTITY\n");
            printf("5. DISPLAY(Debug mode....)\n");
            printf("Choose option: ");
            scanf("%d",&ad_option);

            if (ad_option == 1)
            {
                struct Product newProd;
                int inserted = 0;
                
                newProd.P_ID = lastno;
                printf("Enter Name,Cost,Stock: ");
                scanf("%s%d%d",newProd.P_Name,&newProd.Cost,&newProd.Stock);

                lseek(fd, sizeof(int), SEEK_SET);
                while(read(fd,&buffer,sizeof(struct Product)))
                {
                    if (buffer.P_ID == -1)
                    {
                        lseek(fd, -sizeof(struct Product), SEEK_CUR);
                        write(fd,&newProd,sizeof(struct Product));
                        printf("\n");
                        printf("Added Product %s with id %d\n",newProd.P_Name,newProd.P_ID);
                        printf("\n");
                        lastno++;
                        inserted = 1;
                        break;
                    }
                }

                if (inserted == 0)
                {
                    lseek(fd,0,SEEK_END);
                    write(fd,&newProd,sizeof(struct Product));
                    printf("\n");
                    printf("Added Product %s with id %d\n",newProd.P_Name,newProd.P_ID);
                    printf("\n");
                    lastno++;
                }


            }
            else if (ad_option == 2)
            {
                int kn_option,key;
                char name[20];
                printf("Delete on key(0) or name(1): ");
                scanf("%d",&kn_option);

                if (kn_option == 0)
                {
                    int isDeleted = 0;
                    printf("Choose key to delete: ");
                    scanf("%d",&key);

                    lseek(fd, sizeof(int), SEEK_SET);
                    while(read(fd,&buffer,sizeof(struct Product)))
                    {
                        if (buffer.P_ID == key)
                        {
                            lseek(fd, -sizeof(struct Product), SEEK_CUR);
                            buffer.P_ID = -1;
                            char oldName[20];

                            strcpy(oldName, buffer.P_Name);
                            strcpy(buffer.P_Name,"DELETED");

                            write(fd,&buffer,sizeof(struct Product));
                            printf("\n");
                            printf("Deleted Product %s with id %d\n",oldName,key);
                            printf("\n");
                            isDeleted = 1;
                            break;
                        }
                    }
                    if (isDeleted == 0)
                    {
                        printf("Key does not exist....\n");
                    }

                }
                else if(kn_option == 1)
                {
                    int isDeleted = 0;
                    printf("Choose name to delete (Deletes all duplicate names): ");
                    scanf("%s",name);

                    lseek(fd, sizeof(int), SEEK_SET);
                    while(read(fd,&buffer,sizeof(struct Product)))
                    {
                        if (strcmp(buffer.P_Name,name)==0)
                        {
                            lseek(fd, -sizeof(struct Product), SEEK_CUR);
                            int oldid;
                            oldid = buffer.P_ID;
                            buffer.P_ID = -1;
                            strcpy(buffer.P_Name,"DELETED");

                            write(fd,&buffer,sizeof(struct Product));
                            printf("\n");
                            printf("Deleted Product %s with id %d\n",name,oldid);
                            isDeleted++;
                            printf("\n");
                        }
                    }
                    if (isDeleted == 0)
                    {
                        printf("Name does not exist....\n");
                    }
                    else
                    {
                        printf("Deleted %d values.\n",isDeleted);
                    }
                }
            }

            else if (ad_option == 3)
            {
                int key,newPrice;
                printf("Choose key to update price: ");
                scanf("%d",&key);
                int isUpdated = 0;
                    
                lseek(fd, sizeof(int), SEEK_SET);
                while(read(fd,&buffer,sizeof(struct Product)))
                {
                    if (buffer.P_ID == key)
                    {
                        lseek(fd, -sizeof(struct Product), SEEK_CUR);
                        printf("Enter new price: ");
                        scanf("%d",&buffer.Cost);
                        
                        write(fd,&buffer,sizeof(struct Product));
                        printf("\n");
                        printf("Updated price of Product %s to %d\n",buffer.P_Name,buffer.Cost);
                        printf("\n");
                        isUpdated = 1;
                        break;
                    }
                }
                if (isUpdated == 0)
                {
                    printf("Key does not exist....\n");
                }

            }

            else if (ad_option == 4)
            {
                int key,newPrice;
                printf("Choose key to update quantity: ");
                scanf("%d",&key);
                int isUpdated = 0;
                    
                lseek(fd, sizeof(int), SEEK_SET);
                while(read(fd,&buffer,sizeof(struct Product)))
                {
                    if (buffer.P_ID == key)
                    {
                        lseek(fd, -sizeof(struct Product), SEEK_CUR);
                        printf("Enter new quantity: ");
                        scanf("%d",&buffer.Stock);
                        
                        write(fd,&buffer,sizeof(struct Product));
                        printf("\n");
                        printf("Updated stock of Product %s to %d\n",buffer.P_Name,buffer.Cost);
                        printf("\n");
                        isUpdated = 1;
                        break;
                    }
                }
                if (isUpdated == 0)
                {
                    printf("Key does not exist....\n");
                }

            }

            else if (ad_option == 5)
            {
                int del;
                printf("No deleted values? (0-No or 1-Yes): ");
                scanf("%d",&del);
                lseek(fd, sizeof(int), SEEK_SET);
                printf("\n");
                printf("P_ID     P_Name           Price     Quantity\n");
                printf("---------------------------------------------\n");
                if(del == 0) 
                {
                    while(read(fd,&buffer,sizeof(struct Product)))
                    {  
                        printf("%-8d%-16s%-10d%-10d\n", buffer.P_ID, buffer.P_Name, buffer.Cost, buffer.Stock);
                    }
                }

                else if (del == 1)
                {
                    while(read(fd,&buffer,sizeof(struct Product)))
                    {  
                        if (buffer.P_ID != -1)
                        {
                            printf("%-8d%-16s%-10d%-10d\n", buffer.P_ID, buffer.P_Name, buffer.Cost, buffer.Stock);
                        }
                    }
                }    
                printf("\n");
            }

        }

        else if(option == 2)
        {
            socklen_t clilen = sizeof(cli);
            int cli_option;
            int newsock = accept(sockfd, (struct sockaddr *)&cli, &clilen);
            read(newsock,&cli_option,sizeof(int));
            //printf("%d",cli_option);
                

            if (cli_option==1)
            {
                lseek(fd,sizeof(int),SEEK_SET);
                while (read(fd,&buffer,sizeof(struct Product)))
                {
                    if (buffer.P_ID > 0)
                    {
                        lseek(fd, -sizeof(struct Product), SEEK_CUR);
                        write(newsock,&buffer,sizeof(struct Product));
                    }
                }
            }
            else if (cli_option==2)
            {

            }
            else if (cli_option==3)
            {
                
            }
            else if (cli_option==4)
            {
                
            }
            else if (cli_option==5)
            {
                
            }
    
                
        }

        else
        {
            int destroyer;
            printf("Do you want to delete the db.dat file(for debugging purposes only): ");
            scanf("%d",&destroyer);
            lseek(fd,0,SEEK_SET);
            write(fd,&lastno,sizeof(int));
            if (destroyer == 1)
            {
                close(fd);
                unlink("db.dat");
            }    
            close(fd);
            break;
        }
    }


    


    return 0;
}