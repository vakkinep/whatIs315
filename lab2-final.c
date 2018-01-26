//Venkat Akkinepally
//Jenna Stephens
//Austin Whaley

#include <stdio.h>
#include <stdlib.h>


typedef struct _intfloat {
    unsigned int sign;
    int exponent;
    int fraction;
} INTFLOAT, *INTFLOAT_PTR;

/*
//calling conventions
This function is being called by main to test the example cases provded on the lab worksheet

//assumptions
We are assuming that the format of the inputs are being passed in as unisgned ints

//return parameters
This will return the unsigned in, which is a product of the multiplication
*/
/*************** Part 1: Multiply  **********************/
unsigned int umultiply(unsigned int a, unsigned int b) {
    long multiply = 0;
    int i = 0;

    unsigned int product = 0;
    unsigned int oproduct = 0;
    unsigned int carry = 0;

    product = (unsigned int) a;
    multiply = ((unsigned int) b) << 16;

    for (i= 0 ; i < 16 ; i++) {
        if (product & 1)
        {
            oproduct = product;
            carry = 0;
            product += multiply;
            if (product < oproduct) {
                carry = 0x80000000;
            }
        }
        product = (product >> 1) | carry;
    }
    return product;
}

/*
//calling conventions
This function is only being tested to see if the extraction is working properly, so we do not need to return anything.

//assumptions
Not many assumptions are being made here, but we are considering this to be a single point precision problem.

//return parameters
This will return nothing because we are calling it from main to test if the extraction is being done successfuly
*/
/*************** Part 2: Extract   **********************/
void extract_float(INTFLOAT_PTR xp, float f) {
    unsigned floatbits;

    floatbits = * (unsigned int *) &f; // extract binary float #

    // get the sign as a boolean, true if NEGATIVE   [A]

    xp->sign = (floatbits & 0x80000000);	// high-order bit = 1

    // get the exponent from floatbits [B]

    xp->exponent = (floatbits >> 23) & 0x000000FF; // 8-bit exponent [C]
    xp->exponent -= 127;			// ... less the bias of 127

    // get the fraction as 2's complement, binary point is to the right of the hidden 1.

    xp->fraction = (floatbits << 7) & 0x3FFFFF80;	// 23 bits of fraction [D]
    xp->fraction |= 0x40000000;			// force hidden '1' (0 1 [.] b-1 b-2 b-3 etc.) [E]
    if (xp->sign) xp->fraction = -xp->fraction;	// negate 2's comp if negative [F]
    printf("FOR FLOAT %f--------\n\tSIGN %i\n\tEXP %i\n\tFRACTION %08X\n",
        xp->sign, xp->exponent, xp->fraction);
}

// This is essentially the opposite of the function above. We are taking a struct with the individual parts of IEEE number. We are taking those parts and packing them back into the float format. This is also not expected to return anything, so we are just printing the value to double check.
/*************** Part 3: Pack  **********************/
void pack_float(float* f, INTFLOAT_PTR x) {
    unsigned complement = 0;

    //test for zero
    if (x->fraction == 0) {
        complement = 0;
        return;
    }

    //convert 2's complement to signed magnitude
    if (x->sign)  {
        x->fraction = -x->fraction;
        complement |= 0x80000000;
    }

    //add fraction to float
    complement |= (0x003FFFFF & (x->fraction));// throwing away hidden 1

    //add bias value of 127
    x->exponent += 127;
    x->exponent = x->exponent << 23;
    complement |= x->exponent; //add exponent to float

    *f = (float) complement;
}

//This part is practicing the part of normalizing a fraction. We do this because we want the best precision of any number with leading 0's. This function also is called by main and used by other functions to help with the process of converting from a IEEE format to a 2's complement number
/**************** Part 4 *******************/
void normalize(INTFLOAT_PTR x) {
    unsigned int signflag;

    //check if zero (already "normalized")
    if (x->fraction == 0) return;

    signflag = (x->fraction >> 1); //sign bit moved over 1

    //shift left until high-order bit is 1
    while ( ((x->fraction ^ signflag) & 0x40000000) ==0) {
        x->fraction <<= 1; //shift fraction left
        x->exponent--; //decrement exponent
    }
}

//This is a very simple float addition method. As you can see very short and is useful because other functions rely on this to compose this simple arithmatic.
//-------------------------5 FLOAT ADD-----------------------------------------
void scale_it(INTFLOAT_PTR xp, int n)	  {
    int i;

    xp->fraction >>= n;	// shift right by number of positions to scale
    xp->exponent += n;	// Increase exponent by amount scaled
}
//This is the main function in this block and it adds two floats. As you can see, it used the above function to carry out some operations. This is a very fundamental calculation that we learned in class. This gets called by main with two parameters and relies on other functions like normalize and extract_float.
float fadd(float a, float b) {
    INTFLOAT ax, bx;
    INTFLOAT result;
    int diffexp;
    float retval;

    extract_float(&ax, a);	// get a operand into 2s complement
    extract_float(&bx, b);	// and get b operand too

    diffexp = ax.exponent - bx.exponent;	// Difference in exponents (a-b)
    if (diffexp>0) scale_it(&bx, diffexp);	// a is bigger, scale down b
    if (diffexp<0) scale_it(&ax, -diffexp);	// b is bigger, scale down a

    result.fraction = (ax.fraction >> 1) + (bx.fraction >> 1); // add the fractions
    // scale each down 1 bit to avoid overflow
    result.exponent = ax.exponent + 1;	// account for scaled down value

    // Need to normalize and pack result. (NOT IMPLEMENTED HERE)

    normalize(&result);	// go normalize it

    printf("Post-normalize, single_float_add called with a=%.8f, b=%.8f\n", a, b);
    printf("result.fraction = 0x%08X, result.exponent = 0x%08X (%d)\n\n", result.fraction, result.exponent, result.exponent);
    return retval;
}

//Just like the above method, this is essentially doing the opposite. This also used helper functions, such as packing a float, extracting a float, and then adding a float. The above function is used because we wanted to cut down on the repetetiveness. Subtracting a numebr is essentially adding it's opposite.
//-------------------------6 FLOAT SUB-----------------------------------------
float fsub(float a, float b) {
    INTFLOAT_PTR flt_a;

    extract_float(flt_a,a);

    if (flt_a->sign)
        flt_a->sign = 0;
    else
        flt_a->sign = 1;

    pack_float(&a, flt_a);

    return fadd(a,b);
}
//-----------------------------------------------------------------------------
//This function's purpose is to multiply. Which means adding reeatedly until the desired number of times. This function also gets called by main and used numerous helper functions that were defined above. This function has the most helper functions, which made it simple to write, but at the same time required us to implement every single method preceding it.
//-------------------------7 FLOAT MULTIPLY------------------------------------
float fmul(float a, float b) {
    INTFLOAT_PTR result;
    INTFLOAT_PTR flt_a;
    INTFLOAT_PTR flt_b;
    float res = 0;

    extract_float(flt_a, a); //get into 2's compliment values
    extract_float(flt_b, b);

    result->sign     = ((flt_a->sign)     * (flt_b->sign));     //find sign
    result->fraction = ((flt_a->fraction) * (flt_b->fraction)); //multiply fractions
    result->exponent = ((flt_a->exponent) + (flt_b->exponent)); //sum of exp's

    normalize(flt_a);
    normalize(flt_b);

    pack_float(&res, result);
    return res;
}
//-----------------------------------------------------------------------------

int main() {
   printf("1a. 0x%04X\n", umultiply(0x0001,0x0001));
   printf("1b. 0x%04X\n", umultiply(0x0001,0xFFFF));
   printf("1c. 0x%04X\n", umultiply(0x8000,0x0001));
   printf("1d. 0x%04X\n", umultiply(0x4000,0x4000));
   printf("1e. 0x%04X\n", umultiply(0x8000,0x8000));
   //Q2 examples
   INTFLOAT_PTR a;
   extract_float(a, 0x40C80000);
   extract_float(a, 0xC3000000);
   extract_float(a, 0x3E000000);
   extract_float(a, 0x3EAAAAAB);
   //Q3 exmples
   float flt;
   pack_float(&flt, a);
   printf("3a. 0x%04X\n", flt);
   //Q4 examples
   a->exponent = 0x00000001;
   a->fraction = 0x40000000;
   normalize(a);
   a->exponent = 0x00000000;
   a->fraction = 0x00200000;
   normalize(a);
   a->exponent = 0x0000000C;
   a->fraction = 0xFFC00000;
   normalize(a);
   a->exponent = 0xFFFFFFF8;
   a->fraction = 0x02000000;
   normalize(a);
   //Q5 examples
   fadd(0xBF800000,0x3F800000);
   fadd(0x3F800000,0x3E800000);
   fadd(0x40800000,0xBE000000);
   //Q6 examples
   fsub(0x40400000,0x3F800000);
   fsub(0x40400000,0xBF800000);
   fsub(0x40000000,0x40000000);
   //Q7 examples
   fmul(0x40200000,0x40200000);
   fmul(0xC1700000,0x45800000);
   return 0;
}
