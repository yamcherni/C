/*header file to shere betweed myset and sets*/

int commas;
int words;

typedef char set[17]; 							
set SETA,SETB,SETC,SETD,SETE,SETF;
char *setName[7];
set *setPointer[7];

char* get_part(char line[],int part, char *temp);
char getbin(int x);
int check(void);
int how_many_parts(char line[]);
int count_words(char line[]);
int c_erors(char line[]);
void biton (set s,int x);
void read_set(char line[]);
void print_set(char line[]);
void union_set(char line[]);
void intersect_set(char line[]);
void sub_set(char line[]);
void symdiff_set(char line[]);
