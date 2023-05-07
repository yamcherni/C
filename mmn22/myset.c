/*	Author Yam chernichovski
*	About: This program is mini groups calculator, it takes string input *	from user and alow the user to do basic groups calculations. 
*	-this file is contain a main function */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "set.h"

#define LINE_LEN 100
#define NUMOFSETS 7
#define CHARSIZE 8
#define TMPSIZE 100
extern int commas;
extern int words;

void find_command(char line[]);

/*function manadge*/
struct { 
	char *name;
	void (*func)(char*);
	} cmd[]={
		{"read_set",read_set},
		{"print_set", print_set},
		{"union_set", union_set},
		{"intersect_set", intersect_set},
		{"sub_set", sub_set},
		{"symdiff_set", symdiff_set},
		{"not_valid", NULL}
		};
			

int main(){
	char line[LINE_LEN];
	char stop[]="stop";
	
			
    printf("Please enter a command:"); /* User friendly message */
    while (fgets(line, LINE_LEN, stdin) != NULL) {
    	 if(strncmp(line,stop,4)==0)
    		exit(0);
    		
	 /* print the line the user entered: */
    printf("The line you entered is: %s",line);
    commas = how_many_parts(line);
    words = count_words(line);
    if (c_erors(line)==1)
		find_command(line);	
	printf("\nPlease enter a command:");
	
	}	
    printf("\nReached EOF before stop command\n");
	return 0;	
}

		
/* Find and run a command*/
	void find_command(char line[])
	{
		int	i,j=0;
		char temp[TMPSIZE];
		char* part=(get_part(line,0,temp));
		for (i=0;cmd[i].func!=NULL;i++){
			if (strcmp(part,cmd[i].name)==0){
				(*(cmd[i].func))(line);
				j=i;
			}
		}
		if (strcmp(part,cmd[j].name)!=0)
			printf("Undefined command name");
	}


