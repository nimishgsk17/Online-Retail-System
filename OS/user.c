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
#include <arpa/inet.h>
#include <errno.h>
#include "dbinfo.h"
#include "comm.h"
#include "servUtils.h"
#include "clientUtils.h"

int main()
{
    struct Product cart[CART_SIZE] = {0},buffer;
    struct sockaddr_in serv;
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    char name[50],passwd[20];

    serv.sin_family=AF_INET;
	serv.sin_addr.s_addr=inet_addr("127.0.0.1");
	serv.sin_port=htons(5000);



    connect(sockfd,(struct sockaddr *)&serv,sizeof(serv));
    

    while(1)
    {
        int mode,function;
        struct User user = {0};
        int log_ret;
        
        printf("Join as:\n");
        printf("1) Admin\n");
        printf("2) User\n");
        printf("Enter option: ");

        scanf("%d",&mode);

        printf("Please Enter your username: ");
        scanf("%s",name);
        
        

        if (mode == ADMIN_MODE)
        {
            printf("WELCOME ADMIN\n");
            while(1)
            {
                struct Client_req message={};

                printf("Options:\n");
                printf("1) ADD PRODUCT\n");
                printf("2) DELETE PRODUCT BY KEY\n");
                printf("3) DELETE PRODUCT BY NAME\n");
                printf("4) UPDATE PRICE OF PRODUCT\n");
                printf("5) UPDATE QUANTITY OF PRODUCT\n");
                printf("6) DISPLAY DB\n");
                printf("7) QUIT\n");
                

                message.mode = ADMIN_MODE;
                printf("Choose option: ");
                scanf("%d",&function);

                printf("\n");

                if (function < 1 || function > MAX_FUNCTIONS)
                {
                    printf("Invalid option... choose between (1 to %d)...\n",MAX_FUNCTIONS);
                }                

                if (function == ADD)
                {
                    int ack;
                    message.funct = ADD;
                    message.no_of_params = 2;
                    message.no_of_args = 1;
                    printf("Enter name,cost,quantity: ");
                    scanf("%s%d%d",message.args[0],&message.params[0],&message.params[1]);
                    printf("Sending request to server...\n");
                    // write(nsd,&message,sizeof(struct Client_req));
                    
                    int ret = send(sockfd,&message,sizeof(struct Client_req),0);
                    // printf("I AM HERE %d\n",ret);
                    if (ret == -1)
                    {
                        perror("send");
                    }

                    
                    recv(sockfd,&ack,sizeof(ack),0);
                    if(ack == 1)
                    {
                        printf("Added new product with name: %s ...\n",message.args[0]);
                    }
                    else if(ack == DUP_NACK)
                    {
                        printf("Name you entered already exists...\n");
                    }
                }

                else if (function == DELETE_KEY)
                {
                    int ack;
                    message.funct = DELETE_KEY;
                    message.no_of_params = 1;
                    message.no_of_args = 0;
                    printf("Enter key: ");
                    scanf("%d",&message.params[0]);
                    printf("Sending request to server...\n");
                    int ret = send(sockfd,&message,sizeof(struct Client_req),0);
                    if (ret == -1)
                    {
                        perror("send");
                    }

                    recv(sockfd,&ack,sizeof(ack),0);
                    if(ack == 1)
                    {
                        printf("deleted product with key %d...\n",message.params[0]);
                    }
                }

                else if (function == DELETE_NAME)
                {
                    int ack;
                    message.funct = DELETE_NAME;
                    message.no_of_params = 0;
                    message.no_of_args = 1;
                    printf("Enter name: ");
                    scanf("%s",message.args[0]);
                    printf("Sending request to server...\n");
                    int ret = send(sockfd,&message,sizeof(struct Client_req),0);
                    if (ret == -1)
                    {
                        perror("send");
                    }

                    recv(sockfd,&ack,sizeof(ack),0);
                    if(ack == 1)
                    {
                        printf("deleted product with name %s...\n",message.args[0]);
                    }
                    
                }

                else if (function == UPDATE_PRICE)
                {
                    int ack;
                    message.funct = UPDATE_PRICE;
                    message.no_of_params = 2;
                    message.no_of_args = 0;
                    printf("Enter key and new price: ");
                    scanf("%d%d",&message.params[0],&message.params[1]);
                    printf("Sending request to server...\n");
                    int ret = send(sockfd,&message,sizeof(struct Client_req),0);
                    if (ret == -1)
                    {
                        perror("send");
                    }

                    recv(sockfd,&ack,sizeof(ack),0);
                    if(ack == 1)
                    {
                        printf("Updated price of product with key %d...\n",message.params[0]);
                    }    
                }

                else if (function == UPDATE_QUANTITY)
                {
                    int ack;
                    message.funct = UPDATE_QUANTITY;
                    message.no_of_params = 2;
                    message.no_of_args = 0;
                    printf("Enter key and new quantity: ");
                    scanf("%d%d",&message.params[0],&message.params[1]);
                    printf("Sending request to server...\n");
                    int ret = send(sockfd,&message,sizeof(struct Client_req),0);
                    if (ret == -1)
                    {
                        perror("send");
                    }

                    recv(sockfd,&ack,sizeof(ack),0);
                    if(ack == 1)
                    {
                        printf("Updated quantity of product with key %d...\n",message.params[0]);
                    }    
                }

                else if (function == ADMIN_DISPLAY)
                {
                    int ack;
                    int noSent;
                    message.funct = ADMIN_DISPLAY;
                    message.no_of_params = 1;
                    message.no_of_args = 0;
                    printf("Do you wish to not see deleted quantities?(0-see,1-not see): ");
                    scanf("%d",&message.params[0]);
                    printf("Sending request to server...\n");
                    int ret = send(sockfd,&message,sizeof(struct Client_req),0);
                    if (ret == -1)
                    {
                        perror("send");
                    }

                    recv(sockfd,&noSent,sizeof(noSent),0);

                    clientDisplay(noSent,&sockfd);

                       
                }

                else if (function == ADMIN_QUIT)
                {
                    int noSent,command = 1;
                    message.funct = ADMIN_QUIT;
                    message.no_of_params = 0;
                    message.no_of_args = 0;
                    int ret = send(sockfd,&message,sizeof(struct Client_req),0);
                    if (ret == -1)
                    {
                        perror("send");
                    }

                    recv(sockfd,&noSent,sizeof(noSent),0);
                    int retval = generateLog(noSent,&sockfd,name);
                    if (retval == 0)
                    {
                        send(sockfd,&command,sizeof(int),0);
                        exit(0);
                    }

                }


            }    
        }

        else if(mode == USER_MODE)
        {
            printf("WELCOME USER\n");
            int length = 0;
            while (1)
            {
                struct Client_req message={};

                printf("Options:\n");
                printf("1) DISPLAY PRODUCT LIST\n");
                printf("2) DISPLAY CART\n");
                printf("3) ADD TO CART\n");
                printf("4) DELETE FROM CART\n");
                printf("5) UPDATE QUANTITY OF PRODUCT IN CART\n");
                printf("6) BUY\n");
                printf("7) QUIT\n");

                message.mode = USER_MODE;
                printf("Choose option: ");
                scanf("%d",&function);

                if (function < 1 || function > MAX_USER_FUNCTIONS)
                {
                    printf("Invalid option... choose between (1 to %d)...\n", MAX_USER_FUNCTIONS);
                } 

                if (function == DISPLAY_PRODUCTS)
                {
                    int ack;
                    int noSent;
                    message.funct = DISPLAY_PRODUCTS;
                    message.no_of_params = 0;
                    message.no_of_args = 0;
                    
                    int ret = send(sockfd,&message,sizeof(struct Client_req),0);
                    if (ret == -1)
                    {
                        perror("send");
                    }

                    recv(sockfd,&noSent,sizeof(noSent),0);

                    clientDisplayUser(noSent,&sockfd);

                }

                else if (function == DISPLAY_CART)
                {
                    displayCart(cart);

                }

                else if (function == ADD_CART)
                {
                    struct Product temp;
                    message.funct = ADD_CART;
                    message.no_of_params = 1;
                    message.no_of_args = 1;
                    printf("Enter Product name,quantity: ");
                    scanf("%s%d",message.args[0],&message.params[0]);
                    printf("Sending request to server...\n");
                    int ret = send(sockfd,&message,sizeof(struct Client_req),0);
                    if (ret == -1)
                    {
                        perror("send");
                    }

                    int retval;
                    recv(sockfd,&retval,sizeof(retval),0);
                    if (retval == 0)
                    {
                        recv(sockfd,&temp,sizeof(struct Product),0);
                        addCart(cart,temp);
                        printf("Added new product\n");
                    }

                    else if (retval == PRODUCT_QUANTITY_INSUFFICIENT)
                    {
                        printf("Error Product quantity available is insufficent...\n");
                        printf("Aborting...\n");
                    }

                    else if (retval == PRODUCT_NOT_FOUND)
                    {
                        printf("Please enter valid product name...\n");
                    }

                    
                }

                else if (function == DELETE_FROM_CART)
                {
                    char tmp[MAX_PRODUCT_NAME_LENGTH];
                    printf("Enter name of product to remove from cart: ");
                    scanf("%s",tmp);

                    printf("Deleting product %s...\n",tmp);
                    int ret = deleteCart(cart,tmp);
                    if (ret == 0)
                    {
                        printf("Succesfully deleted product...\n");
                    }

                    else if(ret == PRODUCT_NOT_IN_CART)
                    {
                        printf("Product not found in cart...\n");
                    }


                }

                else if (function == CHANGE_QUANTITY)
                {
                    struct Product temp;
                    message.funct = CHANGE_QUANTITY;
                    message.no_of_params = 1;
                    message.no_of_args = 1;
                    printf("Enter name,new quantity: ");
                    scanf("%s%d",message.args[0],&message.params[0]);
                    printf("Sending request to server...\n");
                    int retr = send(sockfd,&message,sizeof(struct Client_req),0);
                    if (retr == -1)
                    {
                        perror("send");
                    }

                    int retval;
                    recv(sockfd,&retval,sizeof(retval),0);

                    if (retval == 0)
                    {
                        recv(sockfd,&temp,sizeof(struct Product),0);
                        updateCart(cart,temp);
                        printf("Update successful.\n");
                    }

                    else if (retval == PRODUCT_QUANTITY_INSUFFICIENT)
                    {
                        printf("Error Product quantity available is insufficent...\n");
                        printf("Update unsuccesful.\n");
                    }

                    else if (retval == PRODUCT_NOT_FOUND)
                    {
                        printf("Please enter valid product name...\n");
                        printf("Update unsuccesful.\n");
                    }

                }

                else if (function == BUY)
                {
                    int permit = 1;
                    int tmparr[CART_SIZE] = {0};
                    int payment,ack;
                    printf("\nDisplaying your cart...\n\n");
                    int cost = displayCart(cart);
                    message.funct = BUY;
                    message.no_of_params = 0;
                    message.no_of_args = 0;
                    printf("Sending request to server...\n");
                    int ret = send(sockfd,&message,sizeof(struct Client_req),0);
                    if (ret == -1)
                    {
                        perror("send");
                    }
                    
                    printf("Sending Cart\n");
                    send(sockfd,cart,CART_SIZE*sizeof(struct Product),0);
                    //printf("send cart returns: %ld %d\n",sizeof(struct Product),retrr);

                    int retval;
                    recv(sockfd,&retval,sizeof(retval),0);
                    recv(sockfd,tmparr,sizeof(int)*CART_SIZE,0);

                    if (retval == 0)
                    {
                        printf("Your total cost is %d\n",cost);
                        printf("Enter amount: ");
                        scanf("%d",&payment);
                        if (payment == cost)
                        {
                            printReceipt(cart,name);
                            send(sockfd,&permit,sizeof(permit),0);
                            recv(sockfd,&ack,sizeof(int),0);
                            if (ack == 1)
                            {
                                clearCart(cart);
                            }
                            
                            printf("Payment complete.\n");
                        }
                        
                    }

                    else if (retval == 1)
                    {
                        for(int j=0;j<CART_SIZE;j++)
                        {
                            if(tmparr[j]!=0)
                            {
                                if (tmparr[j] == -1)
                                {
                                    printf("Product %s was deleted or is out of stock....\n",cart[j].P_Name);
                                }

                                else if (tmparr[j] > 0)
                                {
                                    printf("only %d units of %s are currently available...\n",tmparr[j],cart[j].P_Name);
                                }
                            }
                        }

                        permit = REORGANIZATION; 
                        send(sockfd,&permit,sizeof(permit),0);
                        printf("Please reorganise you cart again...\n");

                    }

                    else if (retval == 2)
                    {
                        permit = CART_EMPTY; 
                        send(sockfd,&permit,sizeof(permit),0);
                        printf("\nYour cart is empty\n");
                    }



                }

                else if (function == USER_QUIT)
                {
                    message.funct = USER_QUIT;
                    message.no_of_params = 0;
                    message.no_of_args = 0;
                    printf("Quitting...\n");
                    int ret = send(sockfd,&message,sizeof(struct Client_req),0);
                    if (ret == -1)
                    {
                        perror("send");
                    }

                    printf("Bye\n");
                    close(sockfd);
                    exit(0);
                }

            }


        }

        printf("\n");

    }



    return 0;
}