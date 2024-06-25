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
#include <time.h>
#include "clientUtils.h"
#include "dbinfo.h"
#include "comm.h"




int clientDisplay(int totalRecords,int *sockfd)
{
    struct Product buffer;
    int iter = 0;
    
    printf("---------------------------------------------\n");
    printf("P_ID     P_Name           Price     Quantity\n");
    printf("---------------------------------------------\n");
    if (totalRecords == 0)
    {
        printf("<<THERE ARE NO ITEMS IN THE DATABASE>>\n");
        printf("---------------------------------------------\n");
        return 0;
    }
    while(recv(*sockfd,&buffer,sizeof(struct Product),0))
    {  
        printf("%-8d%-16s%-10d%-10d\n", buffer.P_ID, buffer.P_Name, buffer.Cost, buffer.Stock);
        iter++;
        if (iter == totalRecords)
        {
            break;
        }
    }
    printf("---------------------------------------------\n");
}

int generateLog(int totalRecords,int *sockfd, char name[])
{
    time_t seconds;
    seconds = time(NULL);
    char strtime[20];
    sprintf(strtime, "%ld", seconds);
    strcat(name,strtime);
    strcat(name,"_admin_log.txt");

    FILE *fp = fopen(name, "w+");

    struct Product buffer;
    int iter = 0;
    fprintf(fp,"<<LOG>>\n");
    fprintf(fp,"---------------------------------------------\n");
    fprintf(fp,"P_ID     P_Name           Price     Quantity\n");
    fprintf(fp,"---------------------------------------------\n");
    if (totalRecords == 0)
    {
        fprintf(fp,"<<THERE ARE NO ITEMS IN THE DATABASE>>\n");
        fprintf(fp,"---------------------------------------------\n");
        return 0;
    }

    while(recv(*sockfd,&buffer,sizeof(struct Product),0))
    {  
        fprintf(fp,"%-8d%-16s%-10d%-10d\n", buffer.P_ID, buffer.P_Name, buffer.Cost, buffer.Stock);
        iter++;
        if (iter == totalRecords)
        {
            break;
        }
    }
    fprintf(fp,"---------------------------------------------\n");

    return 0;
}

int clientDisplayUser(int totalRecords,int *sockfd)
{
    struct UserProduct buffer;
    int iter = 0;
    printf("-----------------------------------\n");
    printf("P_ID     P_Name           Price     \n");
    printf("-----------------------------------\n");
    while(recv(*sockfd,&buffer,sizeof(struct UserProduct),0))
    {  
        printf("%-8d%-16s%-10d\n", buffer.P_ID, buffer.P_Name, buffer.Cost);
        iter++;
        if (iter == totalRecords)
        {
            break;
        }
    }
    printf("-----------------------------------\n");
}

int displayCart(struct Product cart[])
{
    printf("PRINTING CART\n");
    int i = 0;
    int empty = 1;
    int totalQuantity = 0;
    int totalCost = 0;
    printf("---------------------------------------------\n");
    printf("P_ID     P_Name        Price     Quantity\n");
    printf("---------------------------------------------\n");
    for(i=0;i<CART_SIZE;i++)
    {
        if (cart[i].Stock > 0)
        {
            printf("%-8d%-16s%-10d%-10d\n", cart[i].P_ID, cart[i].P_Name, cart[i].Cost, cart[i].Stock);
            empty = 0;
            totalQuantity = totalQuantity + cart[i].Stock;
            totalCost = totalCost + cart[i].Stock*cart[i].Cost;
        }

    }
    if (empty == 1)
    {
        printf("<<CART IS EMPTY>>\n");
        printf("---------------------------------------------\n");
        return -1;

    }
    printf("---------------------------------------------\n");
    printf("Total Cost: %d, Total Quantity: %d\n\n",totalCost,totalQuantity);
    return totalCost;
}

int addCart(struct Product cart[],struct Product new)
{
    int i = 0;

    for(i=0;i<CART_SIZE;i++)
    {
        if (strcmp(cart[i].P_Name,new.P_Name)==0)
        {
            printf("Product exists in Cart... Use update to modify...\n");
            return PRODUCT_EXISTS_IN_CART;
        }
    }

    for(i=0;i<CART_SIZE;i++)
    {
        if (cart[i].Stock == 0)
        {
            cart[i].P_ID = new.P_ID;
            strcpy(cart[i].P_Name,new.P_Name);
            cart[i].Cost = new.Cost;
            cart[i].Stock = new.Stock;
            return 0;
        }
    }

    return CART_IS_FULL;
}

int deleteCart(struct Product cart[],char name[])
{
    int i = 0;
    
    for(i=0;i<CART_SIZE;i++)
    {
        if (strcmp(cart[i].P_Name,name)==0)
        {
            strcpy(cart[i].P_Name,"DELETED");
            cart[i].Stock = 0;
            return 0;
        }        
    }

    return PRODUCT_NOT_IN_CART;
}

int updateCart(struct Product cart[],struct Product updated)
{
    int i;

    for(i=0;i<CART_SIZE;i++)
    {
        if(strcmp(cart[i].P_Name,updated.P_Name)==0)
        {
            cart[i].Stock = updated.Stock;
            return 0;
        }
    }
    
    return PRODUCT_NOT_IN_CART;
}

int printReceipt(struct Product cart[],char name[])
{
    time_t seconds;
    seconds = time(NULL);
    char strtime[20];
    sprintf(strtime, "%ld", seconds);
    strcat(name,strtime);
    strcat(name,"_receipt.txt");

    FILE *fp = fopen(name, "w+");
    int totalQuantity = 0;
    int totalCost = 0;

    fprintf(fp,"<<RECEIPT>>\n");
    fprintf(fp,"----------------------------------------------------------------\n");
    fprintf(fp,"SrNo.    P_Name        Unit Price     Quantity      Total Price\n");
    fprintf(fp,"----------------------------------------------------------------\n");

    int i = 0;
    for(i=0;i<CART_SIZE;i++)
    {
        if (cart[i].Stock > 0)
        {
            fprintf(fp,"%-8d%-16s%-10d%-15d%-15d\n", i+1, cart[i].P_Name, cart[i].Cost, cart[i].Stock,cart[i].Stock*cart[i].Cost);
            totalQuantity = totalQuantity + cart[i].Stock;
            totalCost = totalCost + cart[i].Stock*cart[i].Cost;
        }
    }

    fprintf(fp,"---------------------------------------------\n");
    fprintf(fp,"Total Cost: %d, Total Quantity: %d\n\n",totalCost,totalQuantity);

    return totalCost;




}

int clearCart(struct Product cart[])
{
    for(int i = 0; i < CART_SIZE; i++)
    {
        cart[i].Cost = 0;
        cart[i].P_ID = 0;
        cart[i].Stock = 0;
        strcpy(cart[i].P_Name,"");
    }

}
