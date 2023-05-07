/*Author: Yam Chernichovsky, ID: 327484085.
About: This program rottate the binary representation of given number */

#include <stdio.h>


/* print in binary */ 
void bin(unsigned n)
{
    if (n>1)
		bin(n/2);
	printf("%d",n%2);
}
 


/* check int length on the specific maccine*/
int int_len(void)
{
	return(sizeof(size_t)*8);
}

/*rotatte the bits of givven int a, b times right */
unsigned int my_rottate (unsigned int a, int b)
{
	unsigned int y = a;    
	if (b<0)
		b=(int_len()+b);
	return ((a>>b) | (y<< (int_len()-b)));
}


/*the main function*/
int main()
{
    unsigned int input,x;
	int param;
/*get user input*/
    printf("Plese enter an integer: ");			
    scanf("%u", &input);
	printf("Plese enter a rotation parameter: ");
    scanf("%d", &param);
/*print before rottation*/
	printf("\n----------------Before rottation----------------\n");
	printf("Decemal value is: %u\nBinary value is:",input);
    bin(input);
 	printf("\n");
	printf("Octal value is: %o\n",input);
	printf("Hexadecimal valuse is: %X\n",input);
/*print affter rottation*/
    x=my_rottate(input,param);  
	printf("\n----------------Affter rottation----------------\n");
    printf("Decemal value is: %u\nBinary value is:",x);
    bin(x); 
    printf("\n");
	printf("Octal value is: %o\n",x);
	printf("Hexadecimal valuse is: %X\n",x);
	
    return 0;
}


