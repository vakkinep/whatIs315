/* Lab 8 - Cache Simulator
 * Jenna Stephens, Austin Whaley, Venkat Akkinepally
 * CPE 315-03
 */
#define AMAX         10		         // Maximum (square) array size of 2 or 10
#define CACHESIM     1		         // Set to 1 if simulating Cache
#define CACHE_SIZE  256             // 16 or 256 words of cache
#define INDEX_BITS   4              // the amount of bits for the index in cache
#define TAG_BITS     26             // the rest of the 32 bits for tag either 26 or 22 bits
#define OFFSET_BITS  2              // the bits of offset in cache
#define ASSOCIATIVE  4              // associativity of 1, 2, or 4 and that affects tag and index size

#include <stdio.h>
#include <math.h>

int  misses = 0;
int  hits = 0;
double hit_rate = 0;

typedef struct word {
	unsigned long index;
	unsigned long tag;
	int valid;
} word;

word cache[CACHE_SIZE];
/*	memory management, code density, Cache emulation - statistics generation */
/*	Generated for CSC 315 Lab 5 */

void init_cache() {
	int i;
	for (i = 0; i < CACHE_SIZE; i++) {
		cache[i].valid = 0;
	}
}

void miss(word curr_word) {
   //increment misses and write the value into the associative cubby in Random order
   misses++;
   if (ASSOCIATIVE <= 1) {                //direct mapped
      cache[curr_word.index].tag = curr_word.tag;
      cache[curr_word.index].index = curr_word.index;    
      cache[curr_word.index].valid = curr_word.valid;
    } else {                           //Associative
      cache[curr_word.index + (rand() % ASSOCIATIVE)].tag = curr_word.tag;
      cache[curr_word.index + (rand() % ASSOCIATIVE)].index = curr_word.index;
      cache[curr_word.index + (rand() % ASSOCIATIVE)].valid = curr_word.valid;
    }
}

void check_in_cache(word curr_word) {
   // for loop checking indexes and comparing it to the data already in cache
   // should it check index or loop through it all? (Check using for loop for associative)
	int i;
	for (i = 0; i < ASSOCIATIVE ; i++) {
      if (cache[i].valid) {					      //check if valid
         if (curr_word.tag == cache[i].tag) {	//if valid, check tag
            hits++;								      //if in cache, hit.
            return;
        }
    }
  }
   miss(curr_word);								      //if not in cache, miss protocall
}

void cache_word(int *mp, word* new_word) {
   // translate the pointer into a cache word struct
   // need variable sizes of all the bit width for each field based on tag/index
	new_word->tag = (((unsigned long) mp) >> (INDEX_BITS + OFFSET_BITS));
   new_word->index  = (((unsigned long) mp) << TAG_BITS);
   new_word->index  %= (unsigned long)floor(CACHE_SIZE/ASSOCIATIVE); // throw away tag bits
   new_word->index >>= (TAG_BITS + OFFSET_BITS); // now remove the offset and put it at the bottom of the word
   new_word->valid = 1;
}

/* This function gets called with each "read" reference to memory */

//TODO
void mem_read(int *mp)
{
   word new_word;
   cache_word(mp, &new_word);
   check_in_cache(new_word);
   // USE A STRUCT TO SIMULATE CACHE HERE AND IN MEM_WRITE
   // HAVE AN ARRAY HOLDING ALL RECENTLY USED ADDRESSES AND INCREMENT HIT AND MISSES
}


/* This function gets called with each "write" reference to memory */

void mem_write(int *mp)
{
   misses++;                                             //write-through
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
      hit_rate = (((double)hits) / ((double) (hits + misses))) * 100.0;
      printf("STATS: HITS \t%d\t MISSES \t%d\t TOTAL \t%lf%\n", hits, misses, hit_rate);
     	return 0;
     }
