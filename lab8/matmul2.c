#define AMAX         10		    // Maximum (square) array size of 2 or 10
#define CACHESIM     1		    // Set to 1 if simulating Cache
#define CACHE_WORDS  16             // 16 or 256 words of cache
#define INDEX_BITS   4              // the amount of bits for the index in cache
#define TAG_BITS     26             // the rest of the 32 bits for tag either 26 or 22 bits
#define OFFSET_BITS  2              // the bits of offset in cache
#define ASSOCIATIVE  1              // associativity of 1, 2, or 4 and that affects tag and index size

#include <stdio.h>

//TESTING SCRIPT FOR AUSTIN
word cache[CACHE_WORDS] = {NULL};
int  misses = 0;
int  hits = 0;

typedef struct word {
   unsigned long index;
   unsigned long tag;
   unsigned long offset;
   signed   int data;
} word;

/*	memory management, code density, Cache emulation - statistics generation */
/*	Generated for CSC 315 Lab 5 */

void check_in_cache(word curr_word) {
   // for loop checking indexes and comparing it to the data already in cache
   // should it check index or loop through it all?
   int i;
   for (i = 0; i < CACHE_WORDS ; i++) {
      printf("CACHE[%d]\t 0x%08X\n", i, cache[i]);
      if (cache[i] != NULL) {
         if (curr_word.tag == cache[i].tag) {
            hits++;
            printf("\n\nHIT! \n\n");
            return;
         }
      }
      printf("Random number %d\n", (rand() % CACHE_WORDS));       // so we can use this in miss
   }
   miss(curr_word);
}

void cache_word(int *mp, word* new_word) {
   // translate the pointer into a cache word struct
   // need variable sizes of all the bit width for each field based on tag/index
   new_word->tag = (((unsigned int) mp) >> (INDEX_BITS + OFFSET_BITS));
   new_word->index  = (((unsigned int) mp) << TAG_BITS); // throw away tag bits
   new_word->index >>= (TAG_BITS + OFFSET_BITS); // now remove the offset and put it at the bottom of the word
   new_word->data = (*mp);
}

void miss(word curr_word) {
   //increment misses and write the value into the array in Random order
   misses++;
   cache[curr_word.index] = curr_word;
}

/* This function gets called with each "read" reference to memory */

//TODO
void mem_read(int *mp)
{
   unsigned long addr;
   // USE A STRUCT TO SIMULATE CACHE HERE AND IN MEM_WRITE
   // HAVE AN ARRAY HOLDING ALL RECENTLY USED ADDRESSES AND INCREMENT HIT AND MISSES
   printf("Memory read from location %p\n", mp);

   // Is the size of the pointer 8 bytes or 4? Bc the %p only prints 3 bytes and this is assuming 32 bit addr
   //TEST TO SEE IF THE ARITHMATIC OR LOGICAL SHIFT WORKS SO WE CAN USE IT IN INDEX 
   printf("PRE-SHIFT \t0x%08X\n", mp);
   //sizeof INT is 4 bytes so 8 hex and shift by 8 bits in a byte times 2 bytes so 16 bits to cut it in half
   addr = (((unsigned int) mp) << 16); 
   printf("POST-SHIFT \t0x%08X\n", addr);
   addr >>= 16; 
   printf("RE-SHIFT \t0x%08X\n", addr);
}


/* This function gets called with each "write" reference to memory */

void mem_write(int *mp)
{
   printf("Memory write to location %p\n", mp);
}

/* Statically define the arrays a, b, and mult, where mult will become the cross product of a and b, i.e., a x b. */

static int a[AMAX][AMAX], b[AMAX][AMAX], mult[AMAX][AMAX];

void matmul(int r1,int c1,int c2 )
{
   int i,j,k;
   int *mp1, *mp2, *mp3;


   /* Initializing elements of matrix mult to 0.*/
   for(i=0; i<r1; ++i)
      for(j=0; j<c2; ++j)
      {
         mult[i][j]=0;
      }

   /* Multiplying matrix a and b and storing in array mult. */

   for(i=0; i<r1; ++i) {
      for(j=0; j<c2; ++j) {
         for(k=0; k<c1; ++k)
         {

#if CACHESIM		/* "Hooks" to measure memory references - enabled if CACHESIM  */

            mp1 = &mult[i][j];
            mp2 = &a[i][k];
            mp3 = &b[k][j];   
            mem_read(mp1);
            mem_read(mp2);
            mem_read(mp3);
            mem_write(mp1); 
#endif

            mult[i][j]+=a[i][k]*b[k][j];
         }
      }
   }
}

int main()
{
   int r1, c1, r2, c2, i, j, k;

   int *mp1, *mp2, *mp3;
   srand(0);                  // so we can use consistent "random" values for writing to cache

   check_in_cache();

   printf("Size of pointer is: %d bytes\n\n", sizeof(mp1));

   printf("Enter rows and column for first matrix: ");
   scanf("%d%d", &r1, &c1);
   printf("Enter rows and column for second matrix: ");
   scanf("%d%d",&r2, &c2);

   /* If column of first matrix in not equal to row of second matrix, asking user to enter the size of matrix again. */
   while (c1 != r2)
   {
      printf("Error! column of first matrix not equal to row of second.\n");
      printf("Enter rows and column for first matrix: ");
      scanf("%d%d", &r1, &c1);
      printf("Enter rows and column for second matrix: ");
      scanf("%d%d",&r2, &c2);
   }

   /* Storing elements of first matrix. */

   printf("\nEnter elements of matrix 1:\n");
   for(i=0; i<r1; ++i)
      for(j=0; j<c1; ++j)
      {
         //        printf("Enter elements a%d%d: ",i+1,j+1);
         //        scanf("%d",&a[i][j]);
         a[i][j] = i+j; // build sample data

      }

   /* Storing elements of second matrix. */
   printf("\nEnter elements of matrix 2:\n");
   for(i=0; i<r2; ++i)
      for(j=0; j<c2; ++j)
      {
         //        printf("Enter elements b%d%d: ",i+1,j+1);
         //        scanf("%d",&b[i][j]);
         b[i][j] = 10 + i + j;
      }

   matmul(r1, c1, c2);  	/* Invoke matrix multiply function */	

   /* Displaying the multiplication of two matrix. */
   printf("\nOutput Matrix:\n");
   for(i=0; i<r1; ++i) {
      for(j=0; j<c2; ++j)
      {
         printf("%d  ",mult[i][j]);
         if(j==c2-1)
            printf("\n\n");
      }
   }
   return 0;
}
