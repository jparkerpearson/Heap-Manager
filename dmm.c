#include <stdio.h>  // needed for size_t
#include <unistd.h> // needed for sbrk
#include <assert.h> // needed for asserts
#include "dmm.h"

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc)  ((size) | (alloc))


/* You can improve the below metadata structure using the concepts from Bryant
 * and OHallaron book (chapter 9).
 */

// Footer doesnt need the previous or the next block, so to save space it only includes size
// This is used more as an indication of whether or not the block is occupied
typedef struct footer {
    size_t size;
}footer;

typedef struct metadata {
    /* size_t is the return type of the sizeof operator. Since the size of an
     * object depends on the architecture and its implementation, size_t is used
     * to represent the maximum size of any object in the particular
     * implementation. size contains the size of the data object or the number of
     * free bytes
     */
    size_t size;
    struct metadata* next;
    struct metadata* prev;
} metadata_t;
bool initialize=false;


/* freelist maintains all the blocks which are not in use; freelist is kept
 * sorted to improve coalescing efficiency
 */

static metadata_t* freelist = NULL;
static footer* prologue;
static footer* epilogue;

void* dmalloc(size_t numbytes) {
        /* initialize through sbrk call first time */
 //   printf("starting malloc \n");
    if(freelist == NULL) {
        if(!dmalloc_init()) {
            //     printf("already initialized ()()()()()()(()()()()(()()(()()(()()()(()()))()()() \n");
            return NULL;
        }
    }
    if (freelist->size==0) {
        return NULL;
    }
    
    int total=0;
    int biggest=0;
    metadata_t* freeListCopy2=freelist;
    int i=0;
    while (freeListCopy2!= NULL){
        i++;
        if (freeListCopy2->size >biggest ) {
            
            biggest=freeListCopy2->size;
            
        }
     //   printf("the %d address in free list is %d \n",i,freeListCopy2);
        total=total+freeListCopy2->size;
        
        freeListCopy2=freeListCopy2->next;
        
    }
  //  printf("biggest free block is: %i total free space is %i \n",biggest,total);
    
    
    
    
    // printf("epilogue is %d \n", epilogue);
    //printf("starting malloc free list is %d \n",freelist);
    //printf("starting malloc free list size at begining of malloc is %d \n",freelist->size);
    
    assert(numbytes > 0);
    int neededBytes=ALIGN(numbytes);
  //  printf("bytes needed is %d \n",neededBytes);
    // Make a copy of pointer to the free list so we can run down the list
    metadata_t* freeListCopy = freelist;
    while (freeListCopy != NULL) {
        if (freeListCopy->size <= neededBytes+9*sizeof(footer) && freeListCopy->size >= neededBytes +4*sizeof(footer)) {
            // if were not splitting
            size_t previousSize             =   freeListCopy->size;
            metadata_t* previousFree    =   freeListCopy->prev;
            metadata_t* nextFree        =   freeListCopy->next;
            //    printf("freelist ->next %d \n",freeListCopy->next);
            if (previousFree!=NULL ) {
                previousFree->next       =   nextFree;
                
            }
            else {
                freelist=nextFree;
            }
            if (nextFree!=NULL) {
                nextFree->prev       =   previousFree;
            }
            
            footer *newHeader = freeListCopy;
        //    printf("allocating %d \n",newHeader);
            newHeader->size= PACK(previousSize, 1); // mark block as allocated
            void* returnAddress    =  newHeader+ 1;
            
            footer *newFooter = newHeader+freeListCopy->size/WORD_SIZE-1;
            newFooter->size= PACK(previousSize, 1); // mark block as allocated
            
            
            
            //     printf("Allocated entire block \n");
            
            
            
            // printf("freelist size at end is: %d \n",freelist->size);
            ////printf("freelist ->next is: %d \n",freelist->next);
            //if (freelist->next=NULL){
            //  freelist=NULL;
            //}
            
            return returnAddress;
        }
        
        // check if block is more than big enough
        else if ((int)freeListCopy->size > (int)neededBytes+4*sizeof(footer)) { // if we are splitting
            
            // Allocated Block
            footer *temp1                   =   freeListCopy;
            temp1=temp1+sizeof(footer)/WORD_SIZE;             //   address of block that is big enough
            void* returnAdress=temp1;
            metadata_t* previousFree        =   freeListCopy->prev;                          //     save previous
            metadata_t* nextFree            =   freeListCopy->next;                            // save next
            size_t previousSize             =   freeListCopy->size;                             // save size
            
            // Footer at the front
            footer *newHeader = freeListCopy;
           // printf("free list copy %d \n",freeListCopy);
            footer *footerFreelistEnd       =   newHeader+previousSize/WORD_SIZE-1;
            
            newHeader->size = 4*sizeof(footer)+neededBytes; // Put in size of block
            newHeader->size= PACK(freeListCopy->size, 1); // mark block as allocated
            
            // Footer at the end
            footer *newFooter = newHeader+neededBytes/WORD_SIZE+3*sizeof(footer)/WORD_SIZE;
            newFooter->size = 4*sizeof(footer)+neededBytes; // Put in size of block
            size_t newSize=newFooter->size;
            newFooter->size= PACK(freeListCopy->size, 1); // mark block as allocated
            
            //Take care of new free block
            
            footer *tempHeader = newFooter+1;
            metadata_t*  newFreeHead        =   tempHeader;
            // change footer at the end of the free block to reflect the new change in size
            newFreeHead->size              =   previousSize-newSize;
            footerFreelistEnd->size        =    newFreeHead->size;
            newFreeHead->next              =   nextFree;
            newFreeHead->prev              =   previousFree;
            
            if (newFreeHead->prev!=NULL) {
                newFreeHead->prev->next        =   newFreeHead;
            }
            
            if (newFreeHead->next!=NULL) {
                newFreeHead->next->prev        =   newFreeHead;
            }
            
            if (newFreeHead->prev==NULL){
                // printf("newFreeHead location before change  is: %d \n",newFreeHead);
                freelist=newFreeHead;  // head of the free list is changed
            }
            
            //print_freelist();
            //      printf("freelist size is: %d \n",freelist->size);
             printf("freelist at end of malloc 1 %d \n",freelist);
            
            if (freelist!=NULL) {
                printf("freelist next at end of malloc is %d \n",freelist->next);
                if (freelist->next!=NULL) {
                    printf("freelist next at end of malloc is %d \n",freelist->next->next);
                    if (freelist->next->next!=NULL) {
                        printf("freelist next at end of malloc is %d \n",freelist->next->next->next);
                    }
                }
            }

            return returnAdress;
            
        }
        
        else if (1)  { // search through all the free blocks
            
            freeListCopy= freeListCopy->next;
        }
        
    }
    
    return NULL;
    
    
}

void dfree(void* ptr) {
    //Get current block header and footer
    if (ptr== NULL) {
        return;
    }
    footer *currentHeadPointer = ptr-sizeof(footer);
    if (currentHeadPointer->size % 8 ==0) {
        return;
    }
    metadata_t* freelistCopy4 =freelist;

    
    
    
    
    footer *previousFooter=currentHeadPointer-1;
    currentHeadPointer->size= currentHeadPointer->size -1; // mark block as free
   
    footer *currentFooter=currentHeadPointer+(currentHeadPointer->size)/WORD_SIZE-1;
    currentFooter->size=currentHeadPointer->size; //mark footer as free
    footer *nextHeader = currentFooter+1;
    //printf("footer is %d \n",currentFooter);
    // printf("footer size is %d \n",currentFooter->size);
    metadata_t* currentHead=(metadata_t*)currentHeadPointer;
    
    currentHead->size=currentHeadPointer->size;
    
    // Search for previous
    bool foundPrevious=false;
    bool foundNext=false;
    int loopTracker=0;
    int nextLoopTracker=0;
    while (!foundPrevious) { // While still searching for previous free pointer
        bool done=false;
        if (previousFooter==prologue  ) { // if one before currentHeadPointer
            
            metadata_t* earliestHead= currentHead; //  head pointer is the earliest head in the heap
            size_t previousSize=currentHead->size;
            metadata_t* freeListCopy3 = freelist; // save old head
            
            freelist=earliestHead;
            freelist->next=freeListCopy3;
           
            currentHead->next=freeListCopy3;
            freelist->prev=NULL;
            if (freeListCopy3!=NULL) {
                freeListCopy3->prev=freelist;
            }
            
            freelist->size=previousSize;
            done=true;
            foundPrevious=true;
        }
        
        else if (previousFooter->size % WORD_SIZE==0 &&loopTracker==0) {
            metadata_t* thisHeadPointer=(previousFooter-(previousFooter->size)/WORD_SIZE+1);
            size_t savedSize= currentHead->size;
            size_t secondSavedSize=thisHeadPointer->size;
            thisHeadPointer->size= savedSize+secondSavedSize;
            currentFooter->size=thisHeadPointer->size;
            currentHead=thisHeadPointer;
            foundPrevious=true;
            done=true;
        }
        else if (previousFooter->size % WORD_SIZE==0) {
            
            metadata_t* thisHeadPointer=(metadata_t*)(previousFooter-(previousFooter->size)/WORD_SIZE+1);
            thisHeadPointer->next=currentHead;
            currentHead->prev=thisHeadPointer;
            foundPrevious=true;
            done=true;
        }
        
        
        if (! done ) {
            
            loopTracker++;
            previousFooter=previousFooter-previousFooter->size/WORD_SIZE;
        }
    }
    
    while (!foundNext) { // While still searching for previous free pointer
        
        bool done=false;
        if (nextHeader==epilogue  ) { // if one before currentHeadPointer
            
            currentHead->next=NULL; //  head pointer is the earliest head in the heap
            done=true;
            foundNext=true;
        }
        
        else if (nextHeader->size % WORD_SIZE==0 &&nextLoopTracker==0) {
            currentHead->size=currentHead->size+nextHeader->size;

            metadata_t* temp2=nextHeader;
           
            
            metadata_t* temp3=temp2->next;
            
            currentHead->next=temp3;
            
            footer *nextFooter = nextHeader+(nextHeader->size)/WORD_SIZE-1;
           
            nextFooter->size=currentHead->size;
            foundNext=true;
            done=true;
        }
        else if (nextHeader->size % WORD_SIZE==0) {
//printf(" found another free \n");
            metadata_t* nextHead=(metadata_t*)nextHeader;
            currentHead->next=nextHead;
            nextHead->prev=currentHead;
            foundNext=true;
            done=true;
        }
        
        if (! done ) {
            nextHeader=nextHeader+nextHeader->size/WORD_SIZE;

            nextLoopTracker++;
        }
        
    }
  
}

bool dmalloc_init() {
    if(initialize) {  // Make sure the heap is only initialized once
        return false;
    }
    initialize=true;
    size_t max_bytes = ALIGN(MAX_HEAP_SIZE);
    footer *heap = (footer*) sbrk(max_bytes);
    prologue=heap;
    epilogue=prologue+max_bytes/WORD_SIZE-1;
    heap++;
    freelist= heap ;
    //printf("problem here \n");
    /* Q: Why casting is used? i.e., why (void*)-1? */
    if (freelist == (void *)-1) {
        perror("sbrk failed");
        return false;
    }
    // printf("footer %d \n",epilogue-1);
    footer *newFooter = epilogue-1;
    newFooter->size = max_bytes-2*WORD_SIZE;
    // printf("problem here \n");
    freelist->prev = NULL;
    freelist->next = NULL;
    
    freelist->size =   max_bytes-2*WORD_SIZE; //one word for the prologue, one for epilogue
    return true;
}


/* for debugging; can be turned off through -NDEBUG flag*/
void print_freelist() {
    metadata_t *freelist_head = freelist;
    while(freelist_head != NULL) {
        DEBUG("\tFreelist Size:%zd, Head:%p, Prev:%p, Next:%p\t",
              freelist_head->size,
              freelist_head,
              freelist_head->prev,
              freelist_head->next);
        freelist_head = freelist_head->next;
    }
    DEBUG("\n");
}



