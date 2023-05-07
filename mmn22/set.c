/* This file contain the functions and the sets*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define LINE_LEN 100
#define NUMOFSETS 7
#define CHARSIZE 8
#define TMPSIZE 100
int commas=0;
int words=0;


/*Define sets and anitialize to 0*/
typedef char set[17]; 							
set SETA={0}, SETB={0}, SETC={0}, SETD={0}, SETE={0}, SETF={0};
char *setName[7]={"SETA","SETB","SETC","SETD","SETE","SETF","NONE"};
set *setPointer[7]={&SETA,&SETB,&SETC,&SETD,&SETE,&SETF,NULL};


/* get binary representation of x*/
char getbin(int x){
	return (1<<(x));
}

/*turn on bit in set*/
void biton (set* s,int x){
	(*s)[x/CHARSIZE] = (*s)[x/CHARSIZE]|getbin(x%CHARSIZE);
}


/* This function return a giiven part in a line devided bu spaces and commas*/
 char* get_part(char line[],int part, char *temp) {
	char blank_or_comma[]=" ,";
	char temp_line[TMPSIZE];
	strcpy(temp_line,line);
	char *portion = strtok(temp_line,blank_or_comma);
	while (portion != NULL && part>0) {
    	portion = strtok(NULL,blank_or_comma);
    	part--;
    }
    return strcpy(temp,portion);
}


/* Return a pointer to givven set, to print value in set: printf("%c",*(get_set("SETP"))[0]); */
set* get_set(char set_name[]){
	int i;
	for(i=0; i< NUMOFSETS-1; i++){
			if (strncmp(set_name,setName[i],4)==0)
					return setPointer[i];
			}
			
			printf("Undefined set name");
			return NULL;	
} 


/*functions to deal with text*/
/*check input*/
int check(void){
if (commas<2){
		printf("Missing parameter");
		return 0;
	}
	if (commas >2){
		printf("Extraneous text after end of command");
		return 0;
	}
	return 1;
}
/*return how many pats in line seperated by comma*/
int how_many_parts(char line[])
{
	int i,count=0;	
	for(i=0;line[i]!='\0';i++)
		if (line[i]==',')
			count++;
    return count;
}
/*return how many parts in line*/
int count_words(char line[])
{
	int i,count=0;	
	for(i=0;line[i]!='\0';i++)
		if (line[i]==','||line[i]==' ')
			count++;
    return count;
}

/*check input*/
int c_erors(char line[])
{
	
	int i;
	for(i=0;line[i]!='\0';i++)
		;
	int len=i;

	if (line[len-1]==','){
		printf("Extraneous text after end of command");
		return 0;
	}
		
	for(i=1;line[i]!='\0';i++){
		if (line[i]==','&& line[i-1]==','){
				printf("Multiple consecutive commas");
				return 0;
			}		
	}
    return 1;
}

/*read set*/
void read_set(char line[]){
	char temp[20];
	set t_set1;
	int j;
	char* part=(get_part(line,1,temp));
	set* S1= get_set(part);  
	if (S1!=NULL){
	for(j=0;j<8;j++)
		t_set1[j]=(*(*(S1)+j));
	int i=2;
	get_part(line,i,temp);
	int k=atoi(temp);
	if(commas<words-1){
		printf("Missing comma");
		return;
	}
	while (k != -1){
	commas--;
	/*check the range*/
	if (k<0 || k >128){		
			printf("Eror: Invalid set member – value out of range");
			for(j=0;j<8;j++)
				(*(*(S1)+j)) = t_set1[j];
			return;
	}
	/*check termination*/
	if(commas==0){
		printf("List of set members is not terminated correctly");
		for(j=0;j<8;j++)
			(*(*(S1)+j))=t_set1[j];
	  	return;
	 }
	/*check if int*/
	for (j=0;temp[j]!='\0';j++){
		if(temp[j] < 48 || temp[j] > 57){
			printf("Eror: Invalid set member – not an integer\n");
			for(j=0;j<8;j++)
				(*(*(S1)+j))=t_set1[j];
	  		return;
	  }	}
	 
		biton(S1,k);
		get_part(line,++i,temp);
		k=atoi(temp);	
	}
	
}}


/*print set*/
void print_set(char line[]){
	if (commas!=0){
		printf("Extraneous text after end of command");
		return;
	}
	char temp[TMPSIZE];
	char* part=(get_part(line,1,temp));
	set* S1= get_set(part);
	int i,j;
	for(i=0;i<16;i++){
		for(j=0;j<8;j++){
			if((((*(*(S1)+i))>>j)&(1))==1)
				printf("%i ",(8*i)+j);	
		}}
	}
/*union set*/
void union_set(char line[]){
	if (check()==0)
		return;
	int i;
	char temp[TMPSIZE];
	char* part1=(get_part(line,1,temp));
	set* S1= get_set(part1);
	char* part2=(get_part(line,2,temp));
	set* S2= get_set(part2);
	char* part3=(get_part(line,3,temp));
	set* S3= get_set(part3);
	for(i=0;i<18;i++)
		(*(*(S3)+i))=(*(*(S1)+i))|(*(*(S2)+i));
}
/*intersect set*/
void intersect_set(char line[]){
	if (check()==0)
		return;
	int i;
	char temp[TMPSIZE];
	char* part1=(get_part(line,1,temp));
	set* S1= get_set(part1);
	char* part2=(get_part(line,2,temp));
	set* S2= get_set(part2);
	char* part3=(get_part(line,3,temp));
	set* S3= get_set(part3);
	for(i=0;i<18;i++)
		(*(*(S3)+i))=(*(*(S1)+i))&(*(*(S2)+i));
}	
/*sub set*/
void sub_set(char line[]){
	if (check()==0)
		return;
	int i;
	char temp[TMPSIZE];
	char* part1=(get_part(line,1,temp));
	set* S1= get_set(part1);
	char* part2=(get_part(line,2,temp));
	set* S2= get_set(part2);
	char* part3=(get_part(line,3,temp));
	set* S3= get_set(part3);
	for(i=0;i<18;i++)
		(*(*(S3)+i))=(*(*(S1)+i))^(*(*(S2)+i));
}	
/*symdif set*/
void symdiff_set(char line[]){
	if (check()==0)
		return;
	int i;
	set t_set1,t_set2;
	char temp[TMPSIZE];
	char* part1=(get_part(line,1,temp));
	set* S1= get_set(part1);
	char* part2=(get_part(line,2,temp));
	set* S2= get_set(part2);
	char* part3=(get_part(line,3,temp));
	set* S3= get_set(part3);
	for(i=0;i<18;i++)								/*union to t_set1*/
		t_set1[i]=(*(*(S1)+i))|(*(*(S2)+i));
		
	for(i=0;i<18;i++)							/*intersect to t_set2*/
		t_set2[i]=(*(*(S1)+i))&(*(*(S2)+i));
		
	for(i=0;i<18;i++)							/*subste t_set1 and t_set2 to S3*/
		(*(*(S3)+i))=t_set1[i]^t_set2[i];
		
}

