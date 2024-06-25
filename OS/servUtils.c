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
#include "servUtils.h"
#include "dbinfo.h"
#include "comm.h"

int login(struct User user,int *fd)
{
    struct User buff;
    int isValid = 0;

    while (read(*fd,&buff,sizeof(struct User)))
    {
        if (strcmp(buff.username,user.username)==0 && strcmp(buff.password,user.password)==0 && buff.account_type == user.account_type)
        {
            if (buff.isLoggedIn == 1)
            {
                return USER_ALREADY_LOGGED_IN;
            }
            else if(buff.isLoggedIn == 0)
            {
                user.isLoggedIn == 1;
                lseek(*fd,-sizeof(struct User),SEEK_CUR);
                write(*fd,&user,sizeof(struct User));
                return 0;
            }
        }

        if (strcmp(buff.username,user.username)==0 && strcmp(buff.password,user.password)!=0)
        {
            return WRONG_PASSWORD;
        }
    }

    return USER_DOES_NOT_EXIST;

}

int addProduct(char name[],int cost,int quantity,int *fd)
{
    struct Product newProd,buffer;
    int inserted = 0;
    int duplicate = 0;
    int lastno;

    lseek(*fd,0,SEEK_SET);
    read(*fd,&lastno,sizeof(int));
                
    newProd.P_ID = lastno;
    strcpy(newProd.P_Name,name);
    newProd.Cost = cost;
    newProd.Stock = quantity;

    lseek(*fd, sizeof(int), SEEK_SET);

    while(read(*fd,&buffer,sizeof(struct Product)))
    {
        if (strcmp(buffer.P_Name,newProd.P_Name)==0)
        {
            duplicate = 1;
        }
    }

    if (duplicate == 0)
    {
        lseek(*fd, sizeof(int), SEEK_SET);

        while(read(*fd,&buffer,sizeof(struct Product)))
        {
            if (buffer.P_ID == -1)
            {
                lseek(*fd, -sizeof(struct Product), SEEK_CUR);
                write(*fd,&newProd,sizeof(struct Product));
                // printf("\n");
                // printf("Added Product %s with id %d\n",newProd.P_Name,newProd.P_ID);
                // printf("\n");
                (lastno)++;
                inserted = 1;
                lseek(*fd,0,SEEK_SET);
                write(*fd,&lastno,sizeof(lastno));
                break;
            }
        } 

        if (inserted == 0)
        {
            lseek(*fd,0,SEEK_END);
            write(*fd,&newProd,sizeof(struct Product));
            // printf("\n");
            // printf("Added Product %s with id %d\n",newProd.P_Name,newProd.P_ID);
            // printf("\n");
            (lastno)++;
            lseek(*fd,0,SEEK_SET);
            write(*fd,&lastno,sizeof(lastno));
        }
        return 0;
    }

    else
    {
        return ERROR_DUP;
    }    

    //printf("P_ID:%d\n",newProd.P_ID);   
}

int deleteProductByKey(int key, int *fd)
{
    int isDeleted = 0;
    struct Product buffer;

    lseek(*fd, sizeof(int), SEEK_SET);
    while(read(*fd,&buffer,sizeof(struct Product)))
    {
        if (buffer.P_ID == key)
        {
            lseek(*fd, -sizeof(struct Product), SEEK_CUR);
            buffer.P_ID = -1;
            char oldName[20];

            strcpy(oldName, buffer.P_Name);
            strcpy(buffer.P_Name,"DELETED");

            write(*fd,&buffer,sizeof(struct Product));
            // printf("\n");
            // printf("Deleted Product %s with id %d\n",oldName,key);
            // printf("\n");
            isDeleted = 1;
            break;
        }
    }
    if (isDeleted == 0)
    {
        // printf("Key does not exist....\n");
        return 1;
    }
    return 0;

}

int deleteProductByName(char name[],int *fd)
{
    struct Product buffer;
    int isdeleted = 0;

    lseek(*fd, sizeof(int), SEEK_SET);
    while(read(*fd,&buffer,sizeof(struct Product)))
    {
        
        if (strcmp(buffer.P_Name,name)==0)
        {
            lseek(*fd, -sizeof(struct Product), SEEK_CUR);
            int oldid;
            oldid = buffer.P_ID;
            buffer.P_ID = -1;
            strcpy(buffer.P_Name,"DELETED");

            write(*fd,&buffer,sizeof(struct Product));
            // printf("\n");
            // printf("Deleted Product %s with id %d\n",name,oldid);
            isdeleted++;
            // printf("\n");
        }
    }
    if (isdeleted == 0)
    {
        return 1;
    }

    else
    {
        return 0;
    }

}

int updatePrice(int key, int price, int *fd)
{
    int newPrice;
    struct Product buffer;
    int isUpdated = 0;
                    
    lseek(*fd, sizeof(int), SEEK_SET);
    while(read(*fd,&buffer,sizeof(struct Product)))
    {
        if (buffer.P_ID == key)
        {
            lseek(*fd, -sizeof(struct Product), SEEK_CUR);
            buffer.Cost = price;
                        
            write(*fd,&buffer,sizeof(struct Product));
            // printf("\n");
            // printf("Updated price of Product %s to %d\n",buffer.P_Name,buffer.Cost);
            // printf("\n");
            isUpdated = 1;
            break;
        }
    }
    if (isUpdated == 0)
    {
        // printf("Key does not exist....\n");
        return 1;
    }
    return 0;

}

int updateQuantity(int key ,int quantity, int *fd)
{
    int newPrice;
    struct Product buffer;
    int isUpdated = 0;
                    
    lseek(*fd, sizeof(int), SEEK_SET);
    while(read(*fd,&buffer,sizeof(struct Product)))
    {
        if (buffer.P_ID == key)
        {
            lseek(*fd, -sizeof(struct Product), SEEK_CUR);
            buffer.Stock = quantity;
                        
            write(*fd,&buffer,sizeof(struct Product));
            // printf("\n");
            // printf("Updated stock of Product %s to %d\n",buffer.P_Name,buffer.Cost);
            // printf("\n");
            isUpdated = 1;
            break;
        }
    }
    if (isUpdated == 0)
    {
        // printf("Key does not exist....\n");
        return 1;
    }
    return 0;
}

int adminDisplay(int showDeleted, int *fd, int *nsd)
{
    struct Product buffer;
    

    lseek(*fd, sizeof(int), SEEK_SET);
    
    if(showDeleted == 0) 
    {
        while(read(*fd,&buffer,sizeof(struct Product)))
        {  
            send(*nsd,&buffer,sizeof(struct Product),0);
        }
    }

    else if (showDeleted == 1)
    {
        while(read(*fd,&buffer,sizeof(struct Product)))
        {  
            if (buffer.P_ID != -1)
            {
                send(*nsd,&buffer,sizeof(struct Product),0);
            }
        }
    }    
}

int adminDisplaySent(int showDeleted, int *fd)
{
    struct Product buffer;
    int numberSent = 0;

    lseek(*fd, sizeof(int), SEEK_SET);
    
    if(showDeleted == 0) 
    {
        while(read(*fd,&buffer,sizeof(struct Product)))
        {  
            numberSent++;
        }
    }

    else if (showDeleted == 1)
    {
        while(read(*fd,&buffer,sizeof(struct Product)))
        {  
            if (buffer.P_ID != -1)
            {
                numberSent++;
            }
        }
    }

    return numberSent;    
}

int userDisplay(int *fd, int *nsd)
{
    struct Product buffer;
    struct UserProduct user_buffer;
    
    lseek(*fd, sizeof(int), SEEK_SET);

    
    while(read(*fd,&buffer,sizeof(struct Product)))
    {  
        if (buffer.P_ID != -1)
        {
            user_buffer.Cost = buffer.Cost;
            user_buffer.P_ID = buffer.P_ID;
            strcpy(user_buffer.P_Name,buffer.P_Name);
            send(*nsd,&user_buffer,sizeof(struct UserProduct),0);
        }
    }
    
}

int userDisplaySent(int *fd)
{
    struct Product buffer;
    int numberSent = 0;

    lseek(*fd, sizeof(int), SEEK_SET);
    
    
    while(read(*fd,&buffer,sizeof(struct Product)))
    {  
        if (buffer.P_ID != -1)
        {
            numberSent++;
        }
    }
    

    return numberSent;
}

int endServ(int *fd,int *fd2,int *sockfd,int destroyer)
{        
    if (destroyer == 1)
    {
        close(*fd);
        close(*fd2);
        close(*sockfd);
        unlink("db.dat");
    }    
    close(*fd);
    close(*fd2);
    close(*sockfd);
}

int printOnConsole(int *fd)
{
    struct Product buffer;
    lseek(*fd, sizeof(int), SEEK_SET);
    printf("\n");
    printf("P_ID     P_Name           Price     Quantity\n");
    printf("---------------------------------------------\n");
    
    while(read(*fd,&buffer,sizeof(struct Product)))
    {  
        printf("%-8d%-16s%-10d%-10d\n", buffer.P_ID, buffer.P_Name, buffer.Cost, buffer.Stock);
    }
}

int checkAddCart(struct Product *item,int *fd)
{
    struct Product buffer;
    int exist = 0;

    lseek(*fd, sizeof(int), SEEK_SET);

    while(read(*fd,&buffer,sizeof(struct Product)))
    {
        
        if (strcmp(buffer.P_Name,(*item).P_Name)==0)
        {
            lseek(*fd, -sizeof(struct Product), SEEK_CUR);

            (*item).P_ID = buffer.P_ID;
            (*item).Cost = buffer.Cost;

            if ((*item).Stock <= buffer.Stock)
            {
                return 0;
            }
            else
            {
                return PRODUCT_QUANTITY_INSUFFICIENT;
            } 
        }
    }
    if (exist == 0)
    {
        return PRODUCT_NOT_FOUND;
    }

}

int checkUpdateCart(struct Product *item,int *fd)
{
    struct Product buffer;
    int exist = 0;

    lseek(*fd, sizeof(int), SEEK_SET);

    while(read(*fd,&buffer,sizeof(struct Product)))
    {
        
        if (strcmp(buffer.P_Name,(*item).P_Name)==0)
        {
            lseek(*fd, -sizeof(struct Product), SEEK_CUR);

            (*item).P_ID = buffer.P_ID;
            (*item).Cost = buffer.Cost;

            if ((*item).Stock <= buffer.Stock)
            {
                return 0;
            }
            else
            {
                return PRODUCT_QUANTITY_INSUFFICIENT;
            } 
        }
    }
    if (exist == 0)
    {
        return PRODUCT_NOT_FOUND;
    }
}

int checkAvailability(struct Product cart[],int amt[],int cartsize,int *fd)
{
    struct Product buffer;
    int i = 0;
    int isFail = 0;
    int isCartEmpty = 1;
    

    for(i=0;i<cartsize;i++)
    {
        if (cart[i].Stock > 0)
        {
            int found = 0;
            lseek(*fd, sizeof(int), SEEK_SET);

            while(read(*fd,&buffer,sizeof(struct Product)))
            {
            
                if (strcmp(cart[i].P_Name,buffer.P_Name)==0)
                {
                    found = 1;
                    if (cart[i].Stock > buffer.Stock)
                    {
                        amt[i] = buffer.Stock;
                        isFail = 1;
                    }        
                }
            }

            

            if (found == 0)
            {
                amt[i] = -1;
                isFail = 1;
            }
            isCartEmpty = 0;
        }    
    }

    if (isCartEmpty == 1)
    {
        isFail = 2;
    }

    return isFail;


}

int payForCart(struct Product cart[],int *fd,int cartsize)
{
    struct Product buffer;
    int i = 0;
    
    for(i=0;i<cartsize;i++)
    {
        if (cart[i].Stock > 0)
        {
            lseek(*fd, sizeof(int), SEEK_SET);
            
            while(read(*fd,&buffer,sizeof(struct Product)))
            {
            
                if (strcmp(cart[i].P_Name,buffer.P_Name)==0)
                {
                    lseek(*fd, -sizeof(struct Product), SEEK_CUR);
                    buffer.Stock = buffer.Stock - cart[i].Stock;
                    write(*fd,&buffer,sizeof(struct Product));
                    lseek(*fd, sizeof(struct Product), SEEK_CUR);         
                }
            }

            
        }    
    }
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