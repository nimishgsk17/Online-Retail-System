#ifndef DBINFO_H
#define DBINFO_H

#define MAX_PRODUCT_NAME_LENGTH 20

struct Product
{
    int P_ID;
    char P_Name[MAX_PRODUCT_NAME_LENGTH];
    int Cost;
    int Stock;
};

struct UserProduct
{
    int P_ID;
    char P_Name[MAX_PRODUCT_NAME_LENGTH];
    int Cost;
};

#endif

