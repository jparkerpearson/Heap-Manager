//
//  test_stress1b.c
//  
//
//  Created by Parker Pearson on 1/30/17.
//
//

#include "test_stress1b.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "dmm.h"

int main(int argc, char *argv[])
{
    int size;
    void *ptr[41];
    int i;
    
    /*
     * try mallocing four pieces, each 1/4 of total size
     */
    printf("starting \n");
    for (int i=0; i<25; i++){
        printf("malloc prt[%d] \n",i);
        ptr[i] = dmalloc(80000);
        if(ptr[i] == NULL) {
            printf("malloc of ptr[%d] failed for size \n",i);
            exit(1);
        }
    }
    printf("done malloc round one \n");
    for (int j=24; j>=0;j--) {
        printf("freeing %d \n",j);
        dfree(ptr[j]);
    }
    
    
    ptr[11] = dmalloc(1572864);
    if(ptr[11] == NULL) {
        printf("malloc of 1000 failed \n");
    }
    
    
    
       printf("Stress testcases1 passed!\n");
    
    exit(0);
}