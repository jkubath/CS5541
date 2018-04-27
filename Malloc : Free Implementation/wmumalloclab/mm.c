/*
 * Summary:  mm.c is a memory allocation .c file.  The functions are as normal except with mm_ before the call.
 * Example: mm_malloc().  This memory allocation application using an implicit free list with a find_next search algorithm.
 * If testing is needed, mm_check() can be un commented to provide helpful feedback.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

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

/* Basic constants and macros*/
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1<<12)

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
//static void *find_fit(size_t size);
//static void *find_Best(size_t size);
static void *find_Next(size_t size);
static void place(void *bp, size_t asize);
//static int mm_check();

/* variables */
//Heap list
static char *heap_listp;
//Used for find_Next
static char *startPointer = NULL;
//Holds the number of heap extensions
int numberOfExtension = 0;
//Holds the number of frees called before coalescing
int numberOfFree = 0;
//Number of times to call coalesce before the entire list is coalesced
int coalesceCount = 100;


/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
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
    
    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if(extend_heap(CHUNKSIZE / WSIZE) == NULL)
      return -1;

    return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
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
    if((bp = find_Next(asize)) != NULL) {
      place(bp, asize);
      return bp;
    }

    /* No fit found. Extend the heap. */
    extendsize = MAX(asize, CHUNKSIZE);
    if((bp = extend_heap(extendsize / WSIZE)) == NULL)
      return NULL;
    place(bp, asize);

    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
  if(ptr == NULL) {
    return;
  }

  size_t size = GET_SIZE(HDRP(ptr));
  //Set the header and pointer to free
  PUT(HDRP(ptr), PACK(size, 0));
  PUT(FTRP(ptr), PACK(size, 0));

  coalesceChunks(ptr, 0);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
  if(ptr != NULL) {
    //Continue
  }
  else if(size != 0) {
    //Call malloc
    return mm_malloc(size);
  }
  else {
    mm_free(ptr);
  }

  void *oldptr = ptr;
  void *newptr;
  size_t copySize;

  //Allocate new space
  newptr = mm_malloc(size);
  if (newptr == NULL)
    return NULL;

  //Copy the old data to the new
  copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
  if (size < copySize)
    copySize = size;
  memcpy(newptr, oldptr, copySize);

  //Free the old pointer
  mm_free(oldptr);
  return newptr;
}

static void *extend_heap(size_t words){

  char *bp;
  size_t size;

  /* Allocate an even number of words to maintain alignment
  * If there is a remainder when % 2, pad with + 1 word size
  */
  size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;

  if((long)(bp = mem_sbrk(size)) == -1){
    return NULL;
  }

  /* Initialize free block header/footer and the epiloge header */
  //Free block header
  PUT(HDRP(bp), PACK(size, 0));
  //Free block footer
  PUT(FTRP(bp), PACK(size, 0));
  //New epilogue header - set the end of the heap pointer
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0,1));

  return coalesceChunks(bp, 1);

  // if(test){

  //   char *temp = coalesceChunks(bp, 1);
  //   //printf("Called by extend %x\n",(int) temp);
  //   return temp;
  // }
  // else
  // /* Coalesce if the previous block was free */
  //   return coalesce(bp);

}

// static void *coalesce(void *bp){

//   size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
//   size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
//   size_t size = GET_SIZE(HDRP(bp));

//   if(prev_alloc && next_alloc){
//     return bp;
//   }
//   else if(prev_alloc && !next_alloc){
//     size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
//     PUT(HDRP(bp), PACK(size, 0));
//     PUT(FTRP(bp), PACK(size, 0));

//   }
//   else if(!prev_alloc && next_alloc){
//     size += GET_SIZE(HDRP(PREV_BLKP(bp)));
//     PUT(FTRP(bp), PACK(size, 0));
//     PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
//     bp = PREV_BLKP(bp);
//   }
//   else {
//     size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
//     PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
//     PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
//     bp = PREV_BLKP(bp);
//   }

//   //Set the position to start with find_Next()
//   startPointer = bp;
//   return bp;
// }

static void *coalesceChunks(void *bp, int extend) {

  char *start = heap_listp;
  char *end = NULL;
  size_t blockSize = 0;

  if(extend) {
    startPointer = bp;
    return bp;
  }

  if(numberOfFree < coalesceCount){
    numberOfFree++;
    numberOfExtension++;
    startPointer = bp;
    return bp;
  }

  numberOfFree = 0;

  char *returnPointer = NULL;

  //Go until the end of the list is found Size == 0
  while(GET_SIZE(HDRP(start))){
    //Search until a block can be coalesced together
    if(!GET_ALLOC(HDRP(start))){
      if(start == bp){
        returnPointer = start;
      }

      blockSize = GET_SIZE(HDRP(start));
      //Iterate until an allocated block is found
      end = NEXT_BLKP(start);
      while(!GET_ALLOC(HDRP(end)) && GET_SIZE(HDRP(end))) {
        //The return pointer is now saved to the coalesced block
        if(end == bp){
          returnPointer = start;
        }

        blockSize += GET_SIZE(HDRP(end));
        end = NEXT_BLKP(end);
      }

      //Set the footer and header after the free blocks have been searched
      PUT(HDRP(start), PACK(blockSize, 0));
      if(end != NULL) {
        end = PREV_BLKP(end);
        PUT(FTRP(start), PACK(blockSize, 0));
      }
      else
        PUT(FTRP(start), PACK(blockSize, 0));

      //Reset the blocksize
      blockSize = 0;

      start = NEXT_BLKP(start);
      end = NULL;
    }
    else
    //Iterate to the next block
      start = NEXT_BLKP(start);
  }


  if(returnPointer != NULL) {
    startPointer = returnPointer;
    return returnPointer;
  }
  else {
    startPointer = bp;
    return bp;
  }

}

// static void *find_fit(size_t size) {
// //  printf("find fit\n");
//   //Start at the beginning and look for a position to add size
//   char *current = heap_listp;

//   while(GET_SIZE(HDRP(current))){
//     if(!GET_ALLOC(HDRP(current)) && GET_SIZE(HDRP(current)) >= size){
//       return current;
//     }

//     current = NEXT_BLKP(current);
//   }

//   //End of list is found and no block is able to hold size
//   return NULL;

// }

// static void *find_Best(size_t size) {
//   // printf("find fit\n");
//   //Start at the beginning and look for a position to add size
//   char *current = heap_listp;
//   int diff = -1;
//   char *best = NULL;

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

//       //Stop if you find a perfect block
//       if(diff == 0){
//         return best;
//       }
//     }

//     current = NEXT_BLKP(current);
//   }


//   if(diff != -1) {
//     // printf("returned diff - %d\n", diff);
//     return best;
//   }
//   else
//     return NULL;

// }

static void *find_Next(size_t size) {
  //Malloc before the heap is extended
  if(startPointer == NULL)
    return NULL;
  else if(!GET_ALLOC(HDRP(startPointer)) && GET_SIZE(HDRP(startPointer)) >= size) {
    return startPointer;
  }

  char *current = NEXT_BLKP(startPointer);

  while(current != startPointer) {
    if(!GET_ALLOC(HDRP(current)) && GET_SIZE(HDRP(current)) >= size){
      return current;
    }
    else if(GET_SIZE(HDRP(current)) == 0){
      current = heap_listp;
    }
    else
      current = NEXT_BLKP(current);
  }

  //List has been traversed and no position found...return NULL
  return NULL;

}

static void place(void *bp, size_t asize) {
  //Get the size of the original block
  size_t originalSize = GET_SIZE(HDRP(bp));

  //Will the original block have enough space to split
  if((originalSize - asize) >= (2 * DSIZE)) {
    PUT(HDRP(bp), PACK(asize, 1));
    PUT(FTRP(bp), PACK(asize, 1));
    //Shift to 2nd half of the split block
    bp = NEXT_BLKP(bp);

    PUT(HDRP(bp), PACK(originalSize - asize, 0));
    PUT(FTRP(bp), PACK(originalSize - asize, 0));
    startPointer = bp;
  }
  //Otherwise just place the asize into bp and leave the extra as fragmentation
  else {
    PUT(HDRP(bp), PACK(originalSize, 1));
    PUT(FTRP(bp), PACK(originalSize, 1));

  }
}

// static int mm_check() {
//   char *current = heap_listp;
//   int count = 0;
//   int returnValue = 0;


//   //Print the entire heap
//   printf("%s\t%s\t%s\n", "Index", "Size", "Alloc");
//   while(GET_SIZE(HDRP(current))) {
//     printf("%d\t%d\t%d\n", count, GET_SIZE(HDRP(current)), GET_ALLOC(HDRP(current)));
//     current = NEXT_BLKP(current);
//     count++;
//   }

//   //Blocks that missed coalescing
//   int previousFree = 0;
//   int currentFree = 0;
//   int noneFound = 1;
//   while(GET_SIZE(HDRP(current))) {
//     currentFree = GET_ALLOC(HDRP(current));
//     if(previousFree && currentFree) {
//       printf("%s\n", "Missed Coalescing");
//       printf("%s\t%s\t%s\n", "Index", "Size", "Pointer");
//       noneFound = 0;
//       printf("%d\t%d\t0x%x\n", count, GET_SIZE(HDRP(current)), (int) current);
//     }
//     current = NEXT_BLKP(current);
//     previousFree = currentFree;
//     count++;
//   }

//   if(!noneFound){
//     returnValue = 1;
//     //printf("No blocks were missed in coalescing\n");
//   }

//   //All blocks in the free list are actually free
//   char *freeList = free_listp;
//   while(GET(freeList) || !GET_SIZE(HDRP(freeList))){
//     if(GET_ALLOC(HDRP(freeList))) {
//       //Block was found that was not free
//       returnValue = 1;
//     }

//     freeList = NEXT_BLKP(freeList);

//   }

//   //Do any heap blocks overlap
//   char *pointer = heap_listp;
//   char *ftrp;
//   char *hdrp;
//   while(GET(pointer)){
//     //Is the next block the end of the free list
//     if(GET_SIZE(HDRP(NEXT_BLKP(pointer)))){
//       break;
//     }

//     ftrp = FTRP(pointer);
//     hdrp = HDRP(NEXT_BLKP(pointer));

//     //The hdrp of the next block interlaps with the ftrp of the current block
//     if((int) hdrp < (int) ftrp){
//       returnValue = 1;
//     }

//     pointer = NEXT_BLKP(pointer);

//   }


//   return returnValue;
// }
