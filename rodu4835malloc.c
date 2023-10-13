/* 
 * mm-implicit.c -  Simple allocator based on implicit free lists, 
 *                  first fit placement, and boundary tag coalescing. 
 *
 * Each block has header and footer of the form:
 * 
 *      31                     3  2  1  0 
 *      -----------------------------------
 *     | s  s  s  s  ... s  s  s  0  0  a/f
 *      ----------------------------------- 
 * 
 * where s are the meaningful size bits and a/f is set 
 * iff the block is allocated. The list has the following form:
 *
 * begin                                                          end
 * heap                                                           heap  
 *  -----------------------------------------------------------------   
 * |  pad   | hdr(8:a) | ftr(8:a) | zero or more usr blks | hdr(8:a) |
 *  -----------------------------------------------------------------
 *          |       prologue      |                       | epilogue |
 *          |         block       |                       | block    |
 *
 * The allocated prologue and epilogue blocks are overhead that
 * eliminate edge conditions during coalescing.
 */

/* -----------------------------------------------------------------------------------------
 * MEMORY ALLOCATION LAB - RONALD DURHAM 
 *
 * Memory allocator based on the implementation from our textbook CS:APP
 * Specifically, using implicit free lists, boundary tag coalescing, and first-fit placement
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
  /* Team name */
  "roduMalloc",
  /* First member's full name */
  "Ronald Durham",
  /* First member's email address */
  "rodu4835@colorado.edu",
  /* Second member's full name (leave blank if none) */
  "",
  /* Second member's email address (leave blank if none) */
  ""
};

/////////////////////////////////////////////////////////////////////////////
// Constants and macros
//
// These correspond to the material in Figure 9.43 of the text
// The macros have been turned into C++ inline functions to
// make debugging code easier.
//
/////////////////////////////////////////////////////////////////////////////
#define WSIZE       4                                   /* word size (bytes) */  
#define DSIZE       8                                   /* doubleword size (bytes) */
#define CHUNKSIZE  (1<<12)                              /* initial heap size (bytes) */
#define OVERHEAD    8                                   /* overhead of header and footer (bytes) */

#define ALIGN(size) (((size) + (8-1)) & ~0x7)           /* rounds up to the nearest multiple of the bytesize(8) aligned to */
#define BUFFER (1<<7)                                   /* Reallocation buffer size */

static inline int MAX(int x, int y) {
  return x > y ? x : y;
}

static inline int MIN(int x, int y) {
  return x > y ? y : x;
}

//
// Pack a size and allocated bit into a word
// We mask of the "alloc" field to insure only
// the lower bit is used
//
static inline uint32_t PACK(uint32_t size, int alloc) {
  return ((size) | (alloc & 0x1));
}

//
// Read and write a word at address p
//
static inline uint32_t GET(void *p) { 
    return  *(uint32_t *)p; 
}
static inline int GET_TAG(void *p) {
  return GET(p) & 0x2;
}
/* Preserve tag bit */
static inline void PUT( void *p, uint32_t val) {
  *((uint32_t *)p) = (val | GET_TAG(p));
}
/* Clear tag bit */
static inline void PUT_NOTAG( void *p, uint32_t val) {
  *((uint32_t *)p) = val;
}
/* Adjust the tag bit*/
static inline uint32_t SET_TAG(void *p) { 
    return  *(uint32_t *)p = (GET(p) | 0x2); 
}
static inline uint32_t UNSET_TAG(void *p) { 
    return  *(uint32_t *)p = (GET(p) & ~0x2); 
}


//
// Read the size and allocated fields from address p
//
static inline uint32_t GET_SIZE(void *p) { 
  return GET(p) & ~0x7;
}
static inline int GET_ALLOC(void *p) {
  return GET(p) & 0x1;
}

//
// Given block ptr bp, compute address of its header and footer
//
static inline void *HDRP(void *bp) {
  return ((char *)bp) - WSIZE;
}
static inline void *FTRP(void *bp) {
  return ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE);
}

//
// Given block ptr bp, compute address of next and previous blocks
//
static inline void *NEXT_BLKP(void *bp) {
  return  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)));
}

static inline void* PREV_BLKP(void *bp) {
  return  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)));
}

/////////////////////////////////////////////////////////////////////////////
//
// Global Variables
//

static char *heap_listp;  /* pointer to first block */
static char *temp;        /* pointer to temp block  */

//
// function prototypes for internal helper routines
//
static void *extend_heap(uint32_t words);
static void place(void *bp, uint32_t asize);
static void *find_fit(uint32_t asize);
static void *coalesce(void *bp);
static void printblock(void *bp); 
static void checkblock(void *bp);
static void checkheap(int check);


//
// mm_init - Initialize the memory manager 
//
int mm_init(void) {
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1) {
        return -1;
    }
    PUT_NOTAG(heap_listp, 0);                           /* Alignment padding */
    PUT_NOTAG(heap_listp + (1*WSIZE), PACK(DSIZE, 1));  /* Prologue header   */
    PUT_NOTAG(heap_listp + (2*WSIZE), PACK(DSIZE, 1));  /* Prologue footer   */
    PUT_NOTAG(heap_listp + (3*WSIZE), PACK(0, 1));      /* Epilogue header   */
    heap_listp += (2*WSIZE);
    
    temp = heap_listp;
    
    /* Extend the empty heap with a free block of CHUMSIZE bytes */
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL) {
        return -1;
    }
        return 0;
}

//
// extend_heap - Extend heap with free block and return its block pointer
//
static void *extend_heap(uint32_t words) {
    char *bp;
    size_t size;
    
    /* Allocate an even number of words to maintain alignment */
    size = ALIGN(words);
    
    if ((long)(bp = mem_sbrk(size)) == -1) {
        return NULL;
    }
    
    /* Initialize free block header/footer and the epilogue header */
    PUT_NOTAG(HDRP(bp), PACK(size, 0));            /* Free block header */
    PUT_NOTAG(FTRP(bp), PACK(size, 0));            /* Free block footer */
    PUT_NOTAG(HDRP(NEXT_BLKP(bp)), PACK(0, 1));    /* New epilogue header */
    
    /* Coalesce if the previous block was free */
    return coalesce(bp);
}


//
// Practice problem 9.8
//
// find_fit - Find a fit for a block with asize bytes 
//
static void *find_fit(uint32_t asize) {
    /* creating a next fit serach */
    char *prevtemp = temp;
    
    /* traverse list from temp to end */
    for ( ; GET_SIZE(HDRP(temp)) > 0; temp = NEXT_BLKP(temp)) {
        // check for when temp is not Header, asize < tempsize, and no tag bit for temp
        if ((!GET_ALLOC(HDRP(temp)) && (asize <= GET_SIZE(HDRP(temp)))) && !(GET_TAG(HDRP(temp)))) {
            return temp;
        }
    }
    
     /* traverse list from start to prevtemp */
    for (temp = heap_listp; temp < prevtemp; temp = NEXT_BLKP(temp)) {
        // check for when temp is not Header, asize < tempsize, and no tag bit for temp 
        if ((!GET_ALLOC(HDRP(temp)) && (asize <= GET_SIZE(HDRP(temp)))) && !(GET_TAG(HDRP(temp)))) {
            return temp;
        }
    }

    return NULL; /* No fit */
}

// 
// mm_free - Free a block 
//
void mm_free(void *bp) {
    size_t size = GET_SIZE(HDRP(bp));
    UNSET_TAG(HDRP(NEXT_BLKP(bp)));       /* unset tag bit for next block */
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

//
// coalesce - boundary tag coalescing. Return pointer to coalesced block
//
static void *coalesce(void *bp) {
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));
    
    if (GET_TAG(HDRP(PREV_BLKP(bp)))) {             /* check previous block tag bit do not coalesce if marked reallocated */
        prev_alloc = 1;
    }
    if (prev_alloc && next_alloc) {                 /* Case 1 */
        return bp;
    } else if (prev_alloc && !next_alloc) {         /* Case 2 */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    } else if (!prev_alloc && next_alloc) {         /* Case 3 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    } else {                                        /* Case 4 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    
    /* check temp pointer should not be aimed at the now coalesced free block */
    if ((temp > (char *)bp) && (temp < (char *)NEXT_BLKP(bp))) {
        temp = bp;
    }
    return bp;
}

//
// mm_malloc - Allocate a block with at least size bytes of payload 
//
void *mm_malloc(uint32_t size) {
    size_t asize;                   /* Adjusted block size */
    size_t heapextend;              /* Amount to extend heap if no fit */
    char *bp;
    
    /* Ignore spurious requests */
    if (size == 0){
        return NULL;
    }
    
    /* Adjust block size to for alignment reqs. and overhead */
    if (size <= DSIZE){
        asize = 2 * DSIZE;
    } else {
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
    }
    /* Search for a fit in the free list */
    if ((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }
    /* allocate more memory and place block for when no fit was found */
    heapextend = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(heapextend)) == NULL){
        return NULL;
    }
    place(bp, asize);
    return bp;
} 

//
//
// Practice problem 9.9
//
// place - Place block of asize bytes at start of free block bp 
//         and split if remainder would be at least minimum block size
//
static void place(void *bp, uint32_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    if ((csize - asize) >= (2*DSIZE)) {  /* split */
        PUT(HDRP(bp), PACK(asize, 1));   /* set block header */
        PUT(FTRP(bp), PACK(asize, 1));   /* set block footer */
        bp = NEXT_BLKP(bp);              /* set bp to next block for new block */
        PUT_NOTAG(HDRP(bp), PACK(csize-asize, 0));   /* set next block header */
        PUT_NOTAG(FTRP(bp), PACK(csize-asize, 0));   /* set next block footer */
    } else {                           /* dont split */
        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
    }
}

//
// mm_realloc -- implemented for you
//
void *mm_realloc(void *bp, uint32_t size)
{
    void *new_bp = bp;      /* Pointer to be returned */
    size_t new_size;        /* Initialize size for new block */
    int remainder;          /* to set new block sizes */
    int heapextend;         /* heap extension amount */
    int blkBUFFER;          /* block buffer amount */
    
    /* Ignore spurious requests */
    if (size == 0) {
        return NULL;
    }
    
    /* Adjust block size to for alignment reqs. and overhead */
    if (size <= DSIZE) {
        new_size = 2 * DSIZE;
    } else {
        new_size = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
    }
    new_size += BUFFER;
  
    /* determine amount for the block buffer */
    blkBUFFER = GET_SIZE(HDRP(bp)) - new_size;
  
    /* Designate more space if overhead < MIN and check type of next block, free or epilogue*/
    if (blkBUFFER < 0) {
        if (!GET_ALLOC(HDRP(NEXT_BLKP(bp))) || !GET_SIZE(HDRP(NEXT_BLKP(bp)))) {
            remainder = GET_SIZE(HDRP(bp)) + GET_SIZE(HDRP(NEXT_BLKP(bp))) - new_size;
            if (remainder < 0) {
                heapextend = MAX(-remainder, CHUNKSIZE);
                if (extend_heap(heapextend) == NULL) {
                    return NULL;
                }
                remainder += heapextend;
            }
            /* no split and put block header/footer */
            PUT_NOTAG(HDRP(bp), PACK(new_size + remainder, 1));
            PUT_NOTAG(FTRP(bp), PACK(new_size + remainder, 1));
        } else {
            new_bp = mm_malloc(new_size - DSIZE);
            memmove(new_bp, bp, MIN(size, new_size));   /* moves bp ptr to new_bp ptr with either size or new_size*/
            mm_free(bp);
        }
        blkBUFFER = GET_SIZE(HDRP(new_bp)) - new_size;
    }  

    /* Set tag block if blockbuffer < 2 * buffer */
    if (blkBUFFER < 2 * BUFFER) {
        SET_TAG(HDRP(NEXT_BLKP(new_bp)));
    }

    /* fresh new block pointer */
    return new_bp;
}

static void printblock(void *bp) 
{
    uint32_t hsize, halloc, fsize, falloc;
    
    checkheap(0);
    hsize = GET_SIZE(HDRP(bp));
    halloc = GET_ALLOC(HDRP(bp));  
    fsize = GET_SIZE(FTRP(bp));
    falloc = GET_ALLOC(FTRP(bp));  
    
    if (hsize == 0) {
        printf("%p: EOL\n", bp);
        return;
    }
    printf("%p: header: [%d:%c] footer: [%d:%c]\n", bp, (int) hsize, (halloc ? 'a' : 'f'), (int) fsize, (falloc ? 'a' : 'f')); 
}

static void checkblock(void *bp) 
{
    if ((uintptr_t)bp % 8) {
        printf("Error: %p is not doubleword aligned\n", bp);
    }
    if (GET(HDRP(bp)) != GET(FTRP(bp))) {
        printf("Error: header does not match footer\n");
    }
}

void checkheap(int check) 
{
    char *bp = heap_listp;

    if (check) {
        printf("Heap (%p):\n", heap_listp);
        printblock(bp);
    }
    if ((GET_SIZE(HDRP(bp)) != 0) || !(GET_ALLOC(HDRP(bp)))) {
        printf("Bad epilogue header\n");
    }
    if ((GET_SIZE(HDRP(heap_listp)) != DSIZE) || !GET_ALLOC(HDRP(heap_listp))) {
        printf("Bad prologue header\n");
    }
    checkblock(heap_listp);
    for (bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        if (check) {
            printblock(bp);
        }
        checkblock(bp);
    }
}