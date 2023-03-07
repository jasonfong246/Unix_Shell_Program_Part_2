/*Name: Jason Fong Shen Yik*/

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#define MAX_HISTORY 11
#define BUFFER_SIZE 50
#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */

static char buffer[BUFFER_SIZE];
int hist_count = 1;
char *hist_list[MAX_HISTORY][MAX_LINE] = {NULL};
char *temp[MAX_HISTORY][MAX_LINE] = {NULL};
int z=1,count,test;

void setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        j,
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */
        ct = 0;
    
    /* read what the user enters on the command line */
  
    
    while ((length = read(STDIN_FILENO, inputBuffer, MAX_LINE)) < 0) {
        if (errno == EINTR) {
            /* the read system call was interrupted by a signal, retry */
            continue;
        } else {
            perror("error reading the command");
            exit(-1);
        }
    }
    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
    if (length < 0){
        perror("error reading the command");
	exit(-1);           /* terminate with error code of -1 */
    }
    
    /* examine every character in the inputBuffer */
    for (i = 0; i < length; i++) { 
        switch (inputBuffer[i]){
        case ' ':
        case '\t' :               /* argument separators */
            if(start != -1){
                args[ct] = &inputBuffer[start];    /* set up pointer */
                ct++;
            }
            inputBuffer[i] = '\0'; /* add a null char; make a C string */
            start = -1;
            break;
        case '\n':                 /* should be the final char examined */
            if (start != -1){
                args[ct] = &inputBuffer[start];     
                ct++;
            }
            inputBuffer[i] = '\0';
            args[ct] = NULL; /* no more arguments to this command */
            break;
        case '&':
            *background = 1;
            inputBuffer[i] = '\0';
            break;
        default :             /* some other character */
            if (start == -1)
                start = i;
    }
    }
    args[ct] = NULL; /* just in case the input line was > 80 */
} 
void ins_list(char *args[]){
    int j,k;
    if(hist_count%11==0){
        hist_count=1;
        count++;
    }
    if(count>0){
        for(j=1;j<10;j++){
            for(k=0;hist_list[j+1][k]!=NULL;k++){
                temp[j][k]=hist_list[j+1][k];
            }
        }
        for(k=0;args[k] != NULL; k++) {
		    temp[10][k] = strdup(args[k]);
	    }
        memcpy(hist_list, temp, sizeof(temp));
        count++;
    }else{
	    for(k=0;args[k] != NULL; k++) {
		    hist_list[hist_count][k] = strdup(args[k]);
	    }
    }
    hist_count++;
}

void exec_command(char *args[], int background)
{
    pid_t pid;
    int returnCode;
	pid = fork();
	    if (pid <0){
            printf("ERROR");
            exit(-1);
        }else if(pid == 0){
            struct sigaction ignore;
            ignore.sa_handler = SIG_IGN;
            sigaction(SIGINT, &ignore, NULL); 
            execvp(args[0],args);
            exit(1);
        }else{
            if(background==0){
                pid = wait(&returnCode);
            }
    }
}

int find_char(char x)
{
	int temp = hist_count - 1;
	for(;temp >= 0; temp--) {
		if(hist_list[temp][0][0] == x) return temp;
	}
	return -1;
}

/* the signal handler function */
void handle_SIGINT() {
    int i = 1,j;
    printf("\nHistory:\n");
    if(count==0){
        z=i;
        for(i; i < MAX_HISTORY; i++,z++){
            printf("%d: ",z);
            for(j=0;hist_list[i][j]!=NULL;j++){
                printf("%s",  hist_list[i][j]);
            }
            printf("\n");
        }
    }else{
        z=0;
        z=count+z;
        for(i; i < MAX_HISTORY; i++,z++){
            printf("%d: ",z);
            for(j=0;hist_list[i][j]!=NULL;j++){
                printf("%s",  hist_list[i][j]);
            }
            printf("\n");
        }
    }
    fflush(stdout);
    strcpy(buffer,"Caught <ctrl><c>\n");
    write(STDOUT_FILENO,buffer,strlen(buffer));
    printf("COMMAND->");
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    /* set up the signal handler */
    struct sigaction handler;
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background, x, found;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2+1];/* command line (of 80) has max of 40 arguments */
    handler.sa_handler = handle_SIGINT; 
    sigaction(SIGINT, &handler, NULL);

    /* wait for <Control>-<C> */
    while (1){            /* Program terminates normally inside setup */
		background = 0;
        found = -1;
		printf("COMMAND->");
        fflush(0);
        setup(inputBuffer, args, &background);       /* get next command */
        if(strcmp(args[0],"r")==0){
            found = find_char(inputBuffer[2]);
            if(found == -1){
                perror("Unable to find command");
            }else{
                exec_command(hist_list[found],background);
            }
        }else{
            ins_list(args);
            exec_command(args,background);
        }
    }
    return 0;
}