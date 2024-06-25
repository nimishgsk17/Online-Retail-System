#ifndef SERVUTILS_H
#define SERVUTILS_H
#include "dbinfo.h"
#include "comm.h"

#define ACK 1
#define DUP_NACK 0

#define ERROR_DUP 1

#define ADMIN_MODE 1
#define USER_MODE 2
#define QUIT 3

#define ADD 1
#define DELETE_KEY 2
#define DELETE_NAME 3
#define UPDATE_PRICE 4
#define UPDATE_QUANTITY 5
#define ADMIN_DISPLAY 6
#define ADMIN_QUIT 7


#define MAX_FUNCTIONS 7

#define DISPLAY_PRODUCTS 1
#define DISPLAY_CART 2
#define ADD_CART 3
#define DELETE_FROM_CART 4
#define CHANGE_QUANTITY 5
#define BUY 6
#define USER_QUIT 7

#define MAX_USER_FUNCTIONS 7

#define PRODUCT_NOT_FOUND 1
#define PRODUCT_QUANTITY_INSUFFICIENT 2

#define USER_ALREADY_LOGGED_IN 1
#define WRONG_PASSWORD 2
#define USER_DOES_NOT_EXIST 3
 


int login(struct User user,int *fd);
int addProduct(char name[],int cost,int quantity,int *fd);
int deleteProductByKey(int key, int *fd);
int deleteProductByName(char name[],int *fd);
int updatePrice(int key, int price, int *fd);
int updateQuantity(int key ,int quantity, int *fd);
int adminDisplaySent(int showDeleted,int *fd);
int adminDisplay(int showDeleted,int *fd, int *nsd);
int userDisplaySent(int *fd);
int userDisplay(int *fd, int *nsd);
int endServ(int *fd, int *fd2, int *sockfd,int destroyer);
int printOnConsole(int *fd);
int checkAddCart(struct Product *item,int *fd);
int checkUpdateCart(struct Product *item,int *fd);
int checkAvailability(struct Product cart[],int amt[],int cartsize,int *fd);
int payForCart(struct Product cart[],int *fd,int cartsize);

int lockFileForReading(int *fd);
int lockFileForWriting(int *fd);
int unlockFile(int *fd);

#endif