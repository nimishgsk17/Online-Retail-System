#ifndef CLIENTUTILS_H
#define CLIENTUTILS_H

#include "dbinfo.h"
#include "comm.h"

#define CART_SIZE 100
#define CART_IS_FULL 1
#define PRODUCT_EXISTS_IN_CART 2
#define PRODUCT_NOT_IN_CART 3
#define CART_EMPTY 3
#define REORGANIZATION 2

int clientDisplay(int totalRecords,int *sockfd);
int clientDisplayUser(int totalRecords,int *sockfd);
int displayCart(struct Product cart[]);
int addCart(struct Product cart[],struct Product new);
int deleteCart(struct Product cart[],char name[]);
int updateCart(struct Product cart[],struct Product updated);
int printReceipt(struct Product cart[],char name[]);
int clearCart(struct Product cart[]);
int generateLog(int totalRecords,int *sockfd, char name[]);



#endif