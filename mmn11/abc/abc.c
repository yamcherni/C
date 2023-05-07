/* Author: Yam Chernichovsky, ID: 327484085.
About this program: This program get string input from user, detect following series of following letters and if there more then nore then 2 following letters it will return only the first and the last letter and "-" betwin them.*/ 

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* this funcrion prints the array without following letters*/
void func (char a[],int len)
{
	int i,start,count;
	count=start=0;

/* the loop will start from the second character becouse first must be printed*/
	putchar(a[0]);
	for (i=1;i<=len;i++)
	{
		if (a[i]-1==a[i-1] && isalpha(a[i])>0)  /* count following letters*/
			count++;
		else if (count>1)								/*if a[i] isnt followung print one of the folloing*/
		{
			
			putchar(45);								/* option 1: there is a gap bitween the letters */
			putchar(a[i-1]);
			count=0;
			i--;
		}
		else if (count==1)							/* option 2: there is only 2 following letters*/
		{			
			putchar(a[i-1]);
			putchar(a[i]);
			start=i;
			count=0;
		}
		else     										/* no gap at all a[i] is the following letter*/
		{
			putchar(a[i]);
			start=i;
			count=0;
		}
	}
		 
}
	


int main()
			{
			char arr[80];
			
			printf("Please enter a string\n");
			scanf("%s", arr);
			printf("The sequence is:%s\n",arr);
			printf("The new sequence is:");
			func(arr,strlen(arr));
			printf("\n");
			 return 0;
			}
