/*
 * Summary:
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "Team",
    /* First member's full name */
    "Jonah Kubath",
    /* First member's email address */
    "jnn3350@wmich.edu",
    /* Second member's full name (leave blank if none) */
    "None",
    /* Second member's email address (leave blank if none) */
    "N/A"
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

//Data from the book
/* Basic constants and macros*/
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<13)

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/*Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its head and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, computer address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

static void *extend_heap(size_t words);
//static void *coalesce(void *bp);
static void *coalesceChunks(void *bp, int extend);
static void *find_fit(size_t size);
//static void *find_Best(size_t size);
//static void *find_Next(size_t size);
static void place(void *bp, size_t asize);
static int mm_check();

/* variables */
//First byte of the heap
//static char *mem_heap;
//Last byte of the heap + 1
//static char *mem_brk;
//Max legal heap addr + 1
//static char *mem_max_addr;
//Heap list
static char *heap_listp;
//Free List Start
static char *free_listp = NULL;
//Free list End
static char *free_listend = NULL;
//Used for find_Next
static char *startPointer = NULL;
//Holds the number of heap extensions
int numberOfExtension = 0;
//Holds the number of frees called before coalescing
int numberOfFree = 0;
//Number of times to call coalesce before the entire list is coalesced
int coalesceCount = 100;
int test = 1;


/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    //printf("Init started\n");
    free_listend = NULL;
    free_listp = NULL;
    /* Create the initial empty heap */
    if((heap_listp = mem_sbrk(4 * WSIZE)) == (void *) -1)
      return -1;
    //Alignment padding
    PUT(heap_listp, 0);
    //Prologue header
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));
    //Prologue footer
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));
    //Epiloge header
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));
    heap_listp += (2 * WSIZE);
    //printf("heap_listp %x\n",(int) heap_listp);
    // printf("heap_listp next %x\n",(int) NEXT_BLKP(heap_listp));
    //printf("heap_listp prev %x\n",(int) PREV_BLKP(heap_listp));

    //printf("Last header %x\n", (int) mem_brk);


    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if(extend_heap(CHUNKSIZE / WSIZE) == NULL)
      return -1;

    //Set the last block of heap
    //mem_brk = mem_heap_hi();
    //printf("mem_brk %x\n",(int) &mem_brk);

    //printf("Init finished\n");
    //mm_check();
    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    //printf("mmaloc - %zu\n", size);
    //mm_check();
    //printf("malloc called %zu\n", size);
    // int newsize = ALIGN(size + SIZE_T_SIZE);
    // void *p = mem_sbrk(newsize);
    // if (p == (void *)-1)
	  //    return NULL;
    // else {
    //     *(size_t *)p = size;
    //     return (void *)((char *)p + SIZE_T_SIZE);
    // }
    size_t asize;
    size_t extendsize;
    char *bp;

    //Don't allocate for size 0
    if(size == 0)
      return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if(size <= DSIZE)
      asize = 2 * DSIZE;
    else
      asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    /* Search the free list for a fit */
    if((bp = find_fit(asize)) != NULL) {
      place(bp, asize);
      //mm_check();
      return bp;
    }

    //Deferred coalescence
    //mm_check();
    coalesceChunks(NULL, 0);
    if((bp = find_fit(asize)) != NULL) {
      place(bp, asize);
      
      return bp;
    }

    //printf("done searching for place\n");
    /* No fit found. Extend the heap. */
    extendsize = MAX(asize, CHUNKSIZE);
    if(extend_heap(extendsize / WSIZE) == NULL)
      return NULL;

    place((bp = find_fit(asize)), asize);
    //mm_check();
    //printf("End of malloc\n");
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
  
  if(ptr != NULL) {
    //printf("free %x\n", (int) ptr);
  }
  else
    return;
  //mm_check();

  size_t size = GET_SIZE(HDRP(ptr));
  //Set the header and pointer to free
  PUT(HDRP(ptr), PACK(size, 0));
  PUT(FTRP(ptr), PACK(size, 0));

  //Put the pointer to the next block at the end of the list
  // printf("end %x\n", (int) free_listend);
  // printf("beginning %x\n", (int) free_listp);
  if(free_listend == 0){
    free_listend = ptr;

    if(free_listp == 0){
      free_listp = ptr;
    }

    PUT(free_listend, 0);
  }
  else {
    if(GET_SIZE(HDRP(free_listend)) < 100)
      return;
    //printf("Put ptr at the end of the free list - %x\n", (int) ptr);
    PUT(free_listend,(int) ptr);
    free_listend = ptr;
    PUT(free_listend, 0);
  }

  // if(numberOfFree > 100){
  //   coalesceChunks(NULL, 0);
  //   numberOfFree = 0;
  // }
  // else
  //   numberOfFree++;
  
  //mm_check();
  
  //printf("End of free\n");

}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
  //printf("Realloc\n");
  //mm_check();
  if(ptr != NULL) {
    //printf("realloc\n");
  }
  else if(size != 0) {
    return mm_malloc(size);
  }
  else {
    mm_free(ptr);
  }

    void *oldptr = ptr;
    void *newptr;
    size_t copySize;

    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    //printf("End of realloc\n");
    return newptr;
}

static void *extend_heap(size_t words){
  //printf("Extend heap\n");
  //mm_check();

  char *bp;
  size_t size;

  /* Allocate an even number of words to maintain alignment*/
  //If there is a remainder when % 2, pad with + 1 word size
  size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;

  //printf("extend heap by %zu\n", size);

  if((long)(bp = mem_sbrk(size)) == -1){
    return NULL;
  }

  //printf("heap extended %x\n",(int) bp);

  /* Initialize free block header/footer and the epiloge header */
  //Free block header
  PUT(HDRP(bp), PACK(size, 0));
  //Free block footer
  PUT(FTRP(bp), PACK(size, 0));
  //New epilogue header - set the end of the heap pointer
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0,1));



  //printf("Size of heap extension %d\n", GET_SIZE(HDRP(bp)));

  //List is empty
  if(free_listp == NULL){
    //Set the next pointer to null
    //printf("Set the free_listp\n");
    PUT(bp, 0);
    free_listp = bp;
    free_listend = bp;
    // printf("BP - %x\n",(int) bp);
    // printf("free - %x\n",(int) free_listp);
    // printf("EXTEND HEAP1\n");
    //mm_check();
  }
  //Add the new pointer to the end of the list
  else {
    //printf("Added new block to the end\n");
    //Is the previous block able to be coalesced?
    //if(!GET_ALLOC(HDRP(PREV_BLKP(bp)))){
    if(0){
      //printf("Coalesced with the previous block\n");
      char *prev = PREV_BLKP(bp);
      //printf("Previous - %d\t%d\n", GET_SIZE(HDRP(prev)), GET_ALLOC(HDRP(prev)));
      int newSize = size + GET_SIZE(HDRP(prev));
      PUT(HDRP(prev), PACK(newSize, 0));
      PUT(FTRP(prev), PACK(newSize, 0));
      PUT(HDRP(NEXT_BLKP(prev)), PACK(0,1));
    
      // PUT(prev, 0);
      // free_listend = prev;
    }
    else {
    //Set the next point and reset the beginning of the free list
      PUT(free_listend, (int) bp);
      free_listend = bp;
      PUT(bp, 0);
    }
    
    //printf("Finished extension\n");
    //mm_check();
    //PUT(free_listend,(int) bp);
    //free_listend = bp;
    //PUT(bp, 0);
    //printf("EXTEND HEAP2\n");
    //mm_check();
  }

  //printf("done\n");
  //printf("End of extend heap\n");
  return (void *) 1;

}

// static void *coalesce(void *bp){
//   //printf("coalesce %x\n", (int) bp);
//   // printf("Previous %x\n",(int) PREV_BLKP(bp));
//   // printf("Next %x\n", (int) NEXT_BLKP(bp));
//   size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
//   //printf("1\n");
//   size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
//   //printf("2\n");
//   size_t size = GET_SIZE(HDRP(bp));
//
//   // printf("1Previous %x\n", (int) PREV_BLKP(bp));
//   // printf("2Next %x\n", (int) NEXT_BLKP(bp));
//
//   if(prev_alloc && next_alloc){
//     //printf("Previous allocated and next allocated\n");
//     return bp;
//   }
//   else if(prev_alloc && !next_alloc){
//     //printf("Previous allocated, next not--\n");
//     size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
//     PUT(HDRP(bp), PACK(size, 0));
//     //PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
//     PUT(FTRP(bp), PACK(size, 0));
//     //printf("done\n");
//
//   }
//   else if(!prev_alloc && next_alloc){
//     //printf("Previous not allocated, next is\n");
//     size += GET_SIZE(HDRP(PREV_BLKP(bp)));
//     PUT(FTRP(bp), PACK(size, 0));
//     PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
//     bp = PREV_BLKP(bp);
//   }
//   else {
//     //printf("Previous not allocated, next not\n");
//     size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
//     PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
//     PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
//     bp = PREV_BLKP(bp);
//   }
//
//   //Set the position to start with find_Next()
//   startPointer = bp;
//   return bp;
// }
//
static void *coalesceChunks(void *bp, int extend) {
  //printf("coalesceChunks started\n");
  char *start = heap_listp;
  char *end = NULL;
  size_t blockSize = 0;

  if(free_listp == 0){
    return NULL;
  }

  // if(extend) {
  //   startPointer = bp;
  //   return bp;
  // }

  // if(numberOfFree < coalesceCount){
  //   numberOfFree++;
  //   numberOfExtension++;
  //   startPointer = bp;
  //   return bp;
  // }

  //numberOfFree = 0;

  //char *returnPointer = NULL;
  char *nextPointer = NULL;
  char *previousPointer = NULL;
  int first = 1;

  //Go until the end of the list is found Size == 0
  while(GET_SIZE(HDRP(start))){
    //printf("Current - %x\n", (int) start);
    //Search until a block can be coalesced together
    if(!GET_ALLOC(HDRP(start))){
      //printf("First free %x - %d\n", (int) start, GET_SIZE(HDRP(start)));

      //Set the start of the free list to the first free block found
      if(first){
        free_listp = start;
        previousPointer = start;
        first = 0;
      }
      else {
        // printf("previous - %x\n",(int) previousPointer);
        // printf("Now points to - %x\n",(int) start);
        PUT(previousPointer,(int) start);
        previousPointer = start;
      }

      blockSize = GET_SIZE(HDRP(start));
      //Iterate until an allocated block is found
      nextPointer = start;
      end = NEXT_BLKP(start);
      while(!GET_ALLOC(HDRP(end)) && GET_SIZE(HDRP(end))) {
        //printf("Searching %d - %x\n", GET_SIZE(HDRP(end)), (int) end);
        //The return pointer is now saved to the coalesced block
        // if(end == free_listp){
        //   free_listp = start;
        // }
        // else if(end == free_listend)
        //   free_listend = start;

        blockSize += GET_SIZE(HDRP(end));
        
        nextPointer = end;
        end = NEXT_BLKP(end);
      }

      // printf("Next %x - %d\t%d\n", (int) nextPointer, GET_SIZE(HDRP(nextPointer)), GET_ALLOC(HDRP(nextPointer)));
      // printf("End %x - %d\t%d\n", (int) end, GET_SIZE(HDRP(end)), GET_ALLOC(HDRP(end)));
      //Set the footer and header after the free blocks have been searched
      PUT(HDRP(start), PACK(blockSize, 0));
      PUT(FTRP(start), PACK(blockSize, 0));
      //Put the next pointer from end into the start block
      if(nextPointer != start)
        PUT(start,(int) GET(nextPointer));

      //printf("Searching done\n");
      //Set the footer and header after the free blocks have been searched
      // PUT(HDRP(start), PACK(blockSize, 0));
      // if(end != NULL) {
      //   end = PREV_BLKP(end);
      //   PUT(FTRP(start), PACK(blockSize, 0));
      //   //Put the next pointer from end into the start block
      //   PUT(start, GET(end));
      // }
      // else
      //   PUT(FTRP(start), PACK(blockSize, 0));

      //Reset the blocksize
      blockSize = 0;

      start = NEXT_BLKP(start);
      end = NULL;
      //printf("Finished coalescing this block\n");
    }
    else
      //Iterate to the next block
      start = NEXT_BLKP(start);
    
  }

  free_listend = previousPointer;
  PUT(free_listend, 0);
  //printf("Finished Coalescing the list\n");
  // if(returnPointer != NULL) {
  //   startPointer = returnPointer;
  //   return returnPointer;
  // }
  // else {
  //   startPointer = bp;
  //   return bp;
  // }

  //mm_check();
  //sleep(1000);
  
  return 0;

}

static void *find_fit(size_t size) {
  //printf("find fit\n");
  //Start at the beginning and look for a position to add size
  char *current = free_listp;
  char *previous = free_listp;

  while(current != 0){
    //printf("Current - %d\n", GET_SIZE(HDRP(current)));
    if(GET_SIZE(HDRP(current)) >= size){
      //printf("Found a location\n");
      //Returning the first point in the free list
      if(current == free_listp) {
        //Is more than one node in the free list
        if(GET(current)){
          free_listp =(char *) GET(current);
        }
        else {
          free_listp = NULL;
          free_listend = NULL;
        }
      }
      else if(current == free_listend){
        free_listend = previous;
        PUT(free_listend, 0);
      }
      //We are returning a pointer in the middle of the list
      //Set the previous node to point to the current.next
      else
        PUT(previous,(int) GET(current));

      return current;
    }

    previous = current;
    current = (char *) GET(current);
  }

  //printf("End of find fit\n");
  //End of list is found and no block is able to hold size
  return NULL;

}

// static void *find_Best(size_t size) {
//   // printf("find fit\n");
//   //Start at the beginning and look for a position to add size
//   char *current = heap_listp;
//   int diff = -1;
//   char *best = NULL;
//
//   while(GET_SIZE(HDRP(current))){
//     if(!GET_ALLOC(HDRP(current)) && GET_SIZE(HDRP(current)) >= size){
//       // printf("diff - %d\n", diff);
//       if(diff == -1) {
//         diff = GET_SIZE(HDRP(current)) - size;
//         best = current;
//       }
//       else if((GET_SIZE(HDRP(current)) - size) < diff){
//         diff = GET_SIZE(HDRP(current)) - size;
//         best = current;
//       }
//
//       //Stop if you find a perfect block
//       if(diff == 0){
//         return best;
//       }
//     }
//
//     current = NEXT_BLKP(current);
//   }
//
//
//   if(diff != -1) {
//     // printf("returned diff - %d\n", diff);
//     return best;
//   }
//   else
//     return NULL;
//
// }
//
// static void *find_Next(size_t size) {
//   //Malloc before the heap is extended
//   if(startPointer == NULL)
//     return NULL;
//   else if(!GET_ALLOC(HDRP(startPointer)) && GET_SIZE(HDRP(startPointer)) >= size) {
//     return startPointer;
//   }
//
//   char *current = NEXT_BLKP(startPointer);
//
//   while(current != startPointer) {
//     //printf("Searching in find_Next\n");
//     if(!GET_ALLOC(HDRP(current)) && GET_SIZE(HDRP(current)) >= size){
//       return current;
//     }
//     else if(GET_SIZE(HDRP(current)) == 0){
//       current = heap_listp;
//     }
//     else
//       current = NEXT_BLKP(current);
//   }
//
//   //List has been traversed and no position found...return NULL
//   return NULL;
//
//
//
//
// }

static void place(void *bp, size_t asize) {
  //printf("place - %zu\n", asize);
  //mm_check();
  //Get the size of the original block
  size_t originalSize = GET_SIZE(HDRP(bp));

  //Will the original block have enough space to split
  // + sizeof(char *)
  if((originalSize - asize) >= (2 * DSIZE)) {
    PUT(HDRP(bp), PACK(asize, 1));
    PUT(FTRP(bp), PACK(asize, 1));
    //Shift to 2nd half of the split block
    if(bp == free_listp) {
      free_listp = NEXT_BLKP(bp);
      PUT(free_listp, GET(bp));
    }
    // else if(free_listp != NULL && (int) free_listend < (int) free_listp) {
    //   free_listend = free_listp;
    //   PUT(free_listend, 0);
    // }
    bp = NEXT_BLKP(bp);

    //printf("BP in place - %x\n", (int) bp);

    PUT(HDRP(bp), PACK(originalSize - asize, 0));
    PUT(FTRP(bp), PACK(originalSize - asize, 0));

    //add the free split block to the free List
    // if(GET_SIZE(HDRP(bp)) < 100){

    // }
    // else 
    if(bp == free_listp){
      //Do nothing
    }
    if(free_listend != NULL) {
      PUT(free_listend,(int) bp);
      free_listend = bp;
      PUT(bp, 0);
    }
    else {
      free_listp = bp;
      free_listend = bp;
      PUT(free_listp, 0);
    }



  }
  //Otherwise just place the asize into bp and leave the extra as fragmentation
  else {
    PUT(HDRP(bp), PACK(originalSize, 1));
    PUT(FTRP(bp), PACK(originalSize, 1));

  }

  
  //mm_check();
  //printf("End of place\n");
}

static int mm_check() {
  char *current = heap_listp;
  int count = 0;
  int returnValue = 0;


  //Print the entire heap
  printf("%s\t%s\t%s\n", "Index", "Size", "Alloc");
  while(GET_SIZE(HDRP(current))) {
    printf("%d\t%d\t%d\t%x\n", count, GET_SIZE(HDRP(current)), GET_ALLOC(HDRP(current)), (int) current);
    current = NEXT_BLKP(current);
    count++;
  }

  current = free_listp;
  int count1 = 0;
  printf("Free list\n");
  printf("Beginning - %x\n",(int) free_listp);
  printf("End - %x\n",(int) free_listend);
  // if(current != NULL)
  //   printf("%d\t%x\t%d\t%x\n", count1,(int) current, GET_SIZE(HDRP(current)), GET(current));
  while(current != NULL && current != 0){
    //sleep(10);
    printf("%d\t%x\t%d\t%x\n", count1,(int) current, GET_SIZE(HDRP(current)), GET(current));
    count1++;
    current =(char *) GET(current);
  }


  //Blocks that missed coalescing
  // int previousFree = 0;
  // int currentFree = 0;
  int noneFound = 1;
  // while(GET_SIZE(HDRP(current))) {
  //   currentFree = GET_ALLOC(HDRP(current));
  //   if(previousFree && currentFree) {
  //     if(noneFound){
  //       printf("%s\n", "Missed Coalescing");
  //       printf("%s\t%s\t%s\n", "Index", "Size", "Pointer");
  //     }
  //     noneFound = 0;
  //     printf("%d\t%d\t0x%x\n", count, GET_SIZE(HDRP(current)), (int) current);
  //   }
  //   current = NEXT_BLKP(current);
  //   previousFree = currentFree;
  //   count++;
  // }

  if(!noneFound){
    returnValue = 1;
    //printf("No blocks were missed in coalescing\n");
  }
  return 0;
  //return returnValue;
}
