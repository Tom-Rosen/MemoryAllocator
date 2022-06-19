#include "mem.h"
extern BLOCK_HEADER* first_header;

// return a pointer to the payload
// if a large enough free block isn't available, return NULL
void* Mem_Alloc(int size){
    // find a free block that's big enough
    // per block data
    int id = 0;
    int alloc;
    BLOCK_HEADER *alloc_location;
    void *return_pointer;
    int block_size;
    int payload;
    BLOCK_HEADER *current = first_header;
    int allocSize;
    int pad;
    if(size > current->payload)
    {
        return NULL;
    }
   while(1){
        // per block data
        id++;
        alloc = current->size_alloc & 1;
        block_size = current->size_alloc & 0xFFFFFFFE;
        payload = current->payload;
    
        if(alloc != 1 && payload >= size)
        {
            alloc_location = current;
            break;    
        }
    
        
        // last block
        if (current->size_alloc == 1) block_size = 8;

         if (current->size_alloc == 1) return NULL;
         current = (BLOCK_HEADER *)((unsigned long)current + block_size);
     }
    // return NULL if we didn't find a block

    // allocate the block
    alloc_location->size_alloc = alloc_location->size_alloc | 1;
    return_pointer = (void*)((unsigned long)alloc_location + 8);
    int headerSize = 8;
    pad = 0;
    allocSize = size + headerSize + pad;
    while((allocSize % 16) != 0)
    {
        pad++;
        allocSize = size + headerSize + pad;
    }

    if(size >= 1576)
    {
        alloc_location->size_alloc = allocSize + 1;
        alloc_location->payload = size;
        return return_pointer;
    }

    if((alloc_location->payload - size) == 0)
    {
        alloc_location->size_alloc = allocSize + 1;
        alloc_location->payload = size;
        return return_pointer; 
    }

    BLOCK_HEADER *newHead = (BLOCK_HEADER *)((unsigned long)alloc_location + allocSize);
    newHead->size_alloc = alloc_location->size_alloc - allocSize - 1;
    //newHead->payload = newHead->size_alloc - allocSize;    
    newHead->payload = alloc_location->payload - allocSize;
    
    alloc_location->size_alloc = allocSize + 1;
    alloc_location->payload = size;
    
    // split if necessary (if padding size is greater than or equal to 16 split the block)

    return return_pointer;
}


// return 0 on success
// return -1 if the input ptr was invalid
int Mem_Free(void *ptr){
    // traverse the list and check all pointers to find the correct block 
    // if you reach the end of the list without finding it return -1
    int alloc;
    int block_size;
    int payload;
    int padding;

    BLOCK_HEADER *current = first_header;
    while(1){
        // per block data
        alloc = current->size_alloc & 1;
        block_size = current->size_alloc & 0xFFFFFFFE;
        payload = current->payload;
        padding = block_size - 8 - payload;

        void *loc = (void *)((unsigned long)current+8);
        if(alloc == 1 && loc == ptr)
        {
            break;
        }
     // last block
        if (current->size_alloc == 1) padding = 0;
        if (current->size_alloc == 1) block_size = 8;
        if (current->size_alloc == 1) return -1;
        current = (BLOCK_HEADER *)((unsigned long)current + block_size);
     }
    // free the block 
    current->size_alloc = current->size_alloc & 0xFFFFFFFE;
    current->payload = current->payload + padding;
    // coalesce adjacent free blocks
        
    //BLOCK_HEADER *next = first_header;
    current = first_header;
    BLOCK_HEADER *post; 
    BLOCK_HEADER *fml;    

    for(int i = 0; i < 2; i++)
    {
    while(1)
    {
        alloc = current->size_alloc & 1;
        block_size = current->size_alloc & 0xFFFFFFFE;
        post =  (BLOCK_HEADER *)((unsigned long)current + block_size);
        int alloc2 = post->size_alloc & 1;
        if(alloc == 0 && alloc2 == 0)
        {
            break;
        }
        
        if (current->size_alloc == 1) padding = 0;
        if (current->size_alloc == 1) block_size = 8;
        if (current->size_alloc == 1) return 0;
        current = (BLOCK_HEADER *)((unsigned long)current + block_size);
    } 
    block_size = post->size_alloc & 0xFFFFFFFE;    
    fml = (BLOCK_HEADER *)((unsigned long)post + block_size);
    unsigned long bigSize = ((unsigned long)fml - (unsigned long)current);
    //next->size_alloc = bigSize;
    //next->payload = next->size_alloc - 8;
    current->size_alloc = bigSize;
    current->payload = current->size_alloc - 8;
    post->size_alloc = 0;
    post->payload = 0;
    }
    return 0;
}


