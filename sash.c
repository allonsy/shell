/* Alec Snyder Shell for cs154 with plenty of help from the wonderful Sam Baugh 
 * The name 'sash' is derived from the concatenation of the first letters of 
 * our names and sh for shell. The Sam and Alec SHell*/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>

int executeCommand(char *command);
int parseString(char **returnList,char *input,char *mychar);
int reprocessForRedirects(char **arguments,int numargs,char **newIn,char **newOut);
void batch(char *infile);
void freeargs(char **arguments, int numArgs);

char *errormsg="An error has occurred\n";
void parseCommand(char *cmdbuf);
void myPrint(char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}

int main(int argc, char *argv[]) 
{
	if(argc>2)
	{
		myPrint(errormsg);
		exit(1);
	}
	else if(argc==2)
	{
		batch(argv[1]);
	}

    char *cmdbuf=(char *)NULL;
    while (1) 
    {
		cmdbuf=readline("sash> ");
		if(cmdbuf==NULL)
		{
			myPrint("\n");
			free(cmdbuf);
			exit(0);
		}
		if(*cmdbuf)
		{
			add_history(cmdbuf);
		}
		parseCommand(cmdbuf);
		free(cmdbuf);
    }
}

int executeCommand(char *command)
{
   char **arguments=(char **)(malloc(50*sizeof(char *))); //arguments
   memset(arguments, 0, 50*sizeof(char *)); //clear anything there
   int numargs=parseString(arguments,command," \t");
   if(numargs==0)
	{
		freeargs(arguments, numargs);
		return 0;
	}
   char *tmp="tmp";
   char **newIn=&tmp;
   char **newOut=&tmp;
   int newIO;
   int fileO;//,fileI;

   //exit(0);

   if((newIO=reprocessForRedirects(arguments,numargs,newIn,newOut))<0) {
           freeargs(arguments,numargs);
           myPrint(errormsg);
           return -1;
     }
   //free(tmp);//MEMORY LEAK CHANGE
   
	if(arguments[0]==NULL)
	{
		freeargs(arguments, numargs);
		myPrint(errormsg);
		return 1;
	} 
	if(arguments[0]==NULL)
	{
		freeargs(arguments, numargs);
		return 0;
	}

   if(strcmp(arguments[0],"exit")==0) //exit command
   {
	   freeargs(arguments, numargs);
        if(newIO==1||numargs!=1) {myPrint(errormsg);return -1;}
	   exit(0);
   }
   else if(strcmp(arguments[0],"pwd")==0)//&&numargs!=1)
   {
      if(newIO==1||numargs!=1) {freeargs(arguments,numargs); myPrint(errormsg);return -1;}
      char cwd[1024]; 
     getcwd(cwd,sizeof(cwd));
     myPrint(cwd);
     myPrint("\n");
      //free(cwd);
	   freeargs(arguments, numargs);
	   return 1;
   }
   else if(strcmp(arguments[0], "cd")==0) //cd command
   {
        if(newIO==1) {freeargs(arguments,numargs); myPrint(errormsg);return -1;}
	   if(numargs>2)
	   {
		   myPrint(errormsg);
		   freeargs(arguments, numargs);
		   return 1;
	   }
	   else if(numargs==1)
	   {
		   int ret;
	   	   ret=chdir(getenv("HOME"));
		   if(ret!=0)
			   myPrint(errormsg);
		   freeargs(arguments, numargs);
		   return ret;
	   }
	   else
	   {
		   int ret=chdir(arguments[1]);
			if(ret!=0)
			{
				myPrint(errormsg);
			}
			freeargs(arguments, numargs);
			return ret;
	   }
   }

   

   pid_t cpid; //a regular command, fork, and execvp
   cpid=fork();
   if(cpid==0)
   {
           if(newIO==1) {
          //newOut//
               if((fileO=open(*newOut,O_WRONLY))>=0) {
                    freeargs(arguments,numargs);
                    myPrint(errormsg);
				   exit(1);	
                    //chmod(*newOut,0x00180);
                }
               fileO=creat(*newOut,0x00180);

               if(dup2(fileO,1)<1) 
               {
                   myPrint(errormsg); 
                   freeargs(arguments, numargs);
                   exit(1);
               } 

           }
 

           /*if(newIO%2==1)  { 
             fileI=4;
             fileI++;

            * int fileI = open(*newIn,O_RDONLY);
             if(fileI<0) {myPrint(errormsg); return -1;}
 
             char *newinbuff="";
             int iu=0;
             char *thing=fgets(newinbuff,513,fileI);
             printf("Here is %s\n",thing);
             myPrint(newinbuff);
             while (!thing) 
              {    myPrint("new input2\n");
                   arguments[numargs+iu]=strdup(newinbuff);
               }
              fclose(fileI);
	      free(newinbuff);
               if(dup2(STDOUT_FILENO,1)<1) {myPrint(errormsg); return -1;} */
           //}


	   execvp(arguments[0], arguments);
	   myPrint(errormsg);//if execve returns, then there was an error
	   exit(0);
   }
   
   int res=0;
   waitpid(cpid, &res, 0); //wait until child is done
   freeargs(arguments, numargs); //free memory
   return res;

}

int parseString(char **returnList,char *input,char *tokens)
{ 


char tmpchar;
int i;
int j=0;
int length = strlen(input);

for(i=0;i<length;i++)
{  tmpchar=*(input+i);
   if(tmpchar==tokens[0]||tmpchar==tokens[1]) {*(input+i)='\0';}
}
char *store=NULL;
for(i=0;i<length;i++)
{
    if(*(input+i)=='\0') {continue;}
    store=input+i;
    
        while((tmpchar=*(input+i))!='\0') {i++;}
        returnList[j]=strdup(store);
        j++;
    
}

int tmpret = j;

for(;j<50;j++)
{
   returnList[j]=NULL;
}

return tmpret;
 
}

void freeargs(char **arguments, int numArgs)
{
	int i;
	for(i=0; i<numArgs; i++)
	{
		free(arguments[i]);
	}
	free(arguments);
}

void parseCommand(char *cmdbuf)
{
    int lenbuf=strlen(cmdbuf);
    int start=0; //parse semicolons
    int end=0;
    char *minibuf;
    while(end<lenbuf) //break up into commands
    {
        if(cmdbuf[end]==';')
        {
            minibuf=(char *)(malloc(end-start+1));
            assert(minibuf!=NULL);
            strncpy(minibuf, cmdbuf+start, (end-start));
            minibuf[end-start]='\0';
            executeCommand(minibuf);
            free(minibuf);
			start=end+1;
		}
        end++;
    }
    if(start!=end&&(cmdbuf[start]!='\0'))
    {
        minibuf=(char *)(malloc(end-start+1));
        assert(minibuf!=NULL);
        strncpy(minibuf, cmdbuf+start, (end-start));
        minibuf[end-start]='\0';
		executeCommand(minibuf);
        free(minibuf);
    }
}

void batch(char *infile)
{
	char *cmdbuf=(char *)(malloc(514));
        char *overbuf=(char *)(malloc(514));
	FILE *in=fopen(infile, "r");
    if(in==NULL){ myPrint(errormsg); exit(1);}

    while (1) 
    {   cmdbuf[512]='\007';
        if (!fgets(cmdbuf, 513, in)) //read until EOF
		{
			free(cmdbuf);
                        free(overbuf);
                        //fclose(*in);
			exit(0);
        }
        
	if(cmdbuf[512]!='\007'){/*&&(cmdbuf[512]!='\n'&&cmdbuf[512]!='\0')) {*/
             
            fgets(overbuf,513,in);
            myPrint(cmdbuf); myPrint(overbuf);
            myPrint(errormsg);
            continue;}
        
       int i;
       int lencmdbuf = strlen(cmdbuf);

       int stupidbool=1;

        for(i=0;i<lencmdbuf;i++)
        {
            if((cmdbuf[i]!=' ')&&(cmdbuf[i]!='\t')&&(cmdbuf[i]!='\n'))
             {stupidbool=0;break;} 
        }

        if(!stupidbool)
        { 
       

        myPrint(cmdbuf);
        
        if(cmdbuf[strlen(cmdbuf)-1]!='\n')
        {
            myPrint(errormsg);
        }
        else
        {
            parseCommand(cmdbuf);
        }
       }
    }
	fclose(in);
}


int reprocessForRedirects(char **arguments,int numargs,char **newIn,char **newOut)
{

//------------------SAM'S REPROCESSING FOR REDIRECTS------------

//Indeces
int index=0;
int i;
int found=0;
int toggleOut=-1;
char mychar;
int length;
int store=0;

mychar='>';


while(index<numargs)
{
length=strlen(arguments[index]);

for(i=0;i<length;i++)
{

   if(*(arguments[index]+i)==mychar) {if(found){return -1;} found=1; store=i; *(arguments[index]+i)='\0';}
}

if(found)
{
    *newOut = arguments[index]+store+1;
    if(store+1==length){/*myPrint("toggle");*/toggleOut=index;}
}
    index++;
}

if(toggleOut!=-1)
{   //myPrint("one");
    if(toggleOut>=numargs-1) {return -1;}
    *newOut=arguments[toggleOut+1];
}


for(i=0;i<numargs;i++)
{   //myPrint(arguments[i]); myPrint("\n");
    if(arguments[i][0]=='\0'||!strcmp(arguments[i],*newOut))
    { arguments[i]=NULL;}
}

return found;

//--------------END REPROCESSING--------------
}





