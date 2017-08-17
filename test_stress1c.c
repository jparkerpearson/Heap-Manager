#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "dmm.h"

int main(int argc, char *argv[])
{
    int size;
    void *ptr[10];
    int i;
    
    /*
     * try mallocing four pieces, each 1/4 of total size
     */
    printf("+++++++++++++++++++++++++++++++++++++++++ \n");
    ptr[0] = dmalloc(200);
    printf("+++++++++++++++++++++++++++++++++++++++++ \n");

    ptr[1] = dmalloc(200);
    printf("+++++++++++++++++++++++++++++++++++++++++ \n");

    ptr[2] = dmalloc(200);
    printf("+++++++++++++++++++++++++++++++++++++++++ \n");

    ptr[3] = dmalloc(200);
    printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy \n");

    dfree(ptr[0]);
    printf("+++++++++++++++++++++++++++++++++++++++++ \n");

    ptr[4] = dmalloc(200);
    printf("+++++++++++++++++++++++++++++++++++++++++ \n");

    ptr[5] = dmalloc(56);
    printf("POINTER 5 %d \n",ptr[5]);
    printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy \n");

    dfree(ptr[3]);
    printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy \n");

    dfree(ptr[2]);
    printf("+++++++++++++++++++++++++++++++++++++++++ \n");

    ptr[6] = dmalloc(200);
    printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy \n");

    dfree(ptr[1]);
    printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy \n");

    dfree(ptr[6]);
    printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy \n");

    dfree(ptr[4]);
    printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy \n");

    dfree(ptr[5]);
    printf("Stress testcases1 passed!\n");
    
    exit(0);
}