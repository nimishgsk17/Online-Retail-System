#ifndef COMM_H
#define COMM_H

#define ADMIN 1
#define USER 0


struct Client_req
{
    int mode;  // Admin: 1, User : 2
    int funct; // Add : 1, Delete : 2, Update Price : 3, Update Quantity :4 
    int no_of_params;  // Count
    int no_of_args;
    int params[10];
    char args[20][20];

};

struct User
{
    char username[50];
    char password[20];
    int account_type;
    int isLoggedIn;
};

#endif
