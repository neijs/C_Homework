#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>

#define LEN 100
#define COMLEN 30
#define SIZEFILENAME 30
#define STEP 10

void delete_spaces (char *); // delete spaces in a string
char * build_spaces (char *); // paste spaces as delimiters
char * build_spaces_redirection (char *); // paste spaces if there are redirections
int length_arr2 (char **); // computation of string array's length
int length_arr3 (char ***); // computation of array's of string arrays length
char * read_command (void); // reading commands from terminal
char ** dividing_commands (char *); // splitting commands
char *** preparing_to_exec (char **); // putting NULL in the end
void redirection (char **, int *); // three descriptors: input, output and errors
void delete_redirections (char **); // deleting redirections
void ign (pid_t); //
void arg (int); //
void stop (int); //
int shell (char ***); //
int cd (char **); // realization of command 'cd'
int shellExit (char **); // realization of command 'exit'
int mypwd (char **); // realization of command 'pwd'
void builtInCheck (char **); // checking if realized commands met
void manager (void); // main part of actions


char * read_commands (void) // don't know input string len, so using malloc; function is used for reading input string
{ 
	int str_length = LEN;
	char *str = (char *) malloc (str_length * sizeof(char));
   	int symbol = 0;
   	int pos = 0;

	if (str == NULL)
	{
        	fprintf(stderr, "-Memory allocation error-\n"); // malloc check
        	exit(EXIT_FAILURE);
    	}

	while (1)
	{
		symbol = fgetc(stdin); // if EOF or '\0' -> saving buffer 
		if (symbol == EOF)
		{
			free(str);
			fprintf(stdout, "-See EOF...Ending of program-\n");
			exit(EXIT_SUCCESS);
		}
    	    	if (symbol == '\n')
		{
			str[pos] = '\0';
        	    	return str;
	    	}
	    	else
			str[pos] = symbol; // or continue writing to buffer
	    	pos ++;

		if (pos >= str_length - 2)    // if buffer is full -> realloc
		{
			str_length += LEN;
            		str = realloc(str, str_length);
            		if (!str)
			{
                		fprintf(stderr, "-Memory reallocaion error-\n");
				free(str);
                		exit(EXIT_FAILURE);
            		}
        	}
	}
} //function is used for reading input string


void delete_spaces (char * str)  // function removes all spaces in string
{
	int i, k  = 0;
	
	if (str[0] == ' ') //if we met spaces in the beginning - skip it
        	while (str[k] == ' ')
	       		k++;
    	str = (str + k);
    
	k = 0;
    	for (i = 0; str[i]; i++)
    	{
        	str[i] = str[i + k];
        	if ((str[i] == ' ' && str[i - 1] != ' ') || (str[i] == '|'))
		       	continue;
        	if (str[i] == ' ' || str[i] == '\t')
        	{
        		k ++;
        	    	i --;
        	}
    	}
    	if (str[strlen(str) - 1] == ' ')
		str[strlen(str) - 1] = '\0';
}  // removing spaces


char * build_spaces (char * str) // function makes spaces before and after '|'
{
    	int k, i, j;
	int str_len = strlen(str);
    	char *s = malloc(str_len * sizeof(char));
	if (s == NULL)
	{
		fprintf(stderr,"-Memory allocation error-\n");
		exit(EXIT_FAILURE);
	}
    	strcpy(s, str); // copying string

    	for (i = 0; i < strlen(s); i++)
       	{
    		if (s[i] == '|' && (s[i - 1] != ' ' || s[i + 1] != ' '))   // making spaces before/after |, saving symbols, moving them forward
		{
			if (strlen(s) + 2 >= str_len)
			{
				str_len += COMLEN;
				s = realloc(s, str_len);
			}
			k = strlen(s) + 2;
			for (j = 0; j < strlen(s) - i; j++)
			{
				s[k] = s[k-2];
				k --;
			}
			s[i] = ' ';
			s[i + 1] = '|';
			s[i + 2] = ' ';
		}
    	}
	free(str); 
    	return s;
}  // adding spaces after/ before |

char * build_spaces_redirection (char *str) // making spaces if redirection symbol is met. Example: ls>a.txt -> ls > a.txt
{
        int k, i, j;
	int str_len = strlen(str);
        char *s = malloc(str_len * sizeof (char));

        strcpy(s,str);
        
	for (i = 0; i < strlen(s); i++) // making spaces before/after redirection symbols
	{
		if ((s[i] == '<' || s[i] == '>') && (s[i - 1] != ' ' || s[i + 1] != ' '))
		{
			if (strlen(s) + 2 >= str_len)
			{
				str_len += COMLEN;
				s = realloc(s, str_len);
			}
			k = strlen(s) + 2;
			for (j = 0; j < strlen(s) - i; j++)
			{
				s[k] = s[k - 2];
				k --;
			}
			if (s[i] == '<')
				s[i + 1] = '<';
			else
				s[i + 1] = '>';
			s[i] = ' ';
			s[i + 2] = ' ';
		}
		if (s[i] == '>' && (s[i - 1] == '>' || s[i - 1] == '2') && (s[i - 2] != ' ' || s[i + 1] != ' '))
		{
			if (strlen(s) + 2 >= str_len)
			{
				str_len += COMLEN;
				s = realloc(s, str_len);
			}
			k = strlen(s) + 2;
			for (j = 0; j < strlen(s) - i; j++)
			{
				s[k] = s[k-2];
				k--;
			}
			s[i + 2] = ' ';
			s[i + 1] = s[i];
			if (s[i - 1] == '>')
				s[i] = 'i';
			else 
				s[i] = '2';
			s[i - 1] = ' ';
		}
        }
	free(str);
        return s;
}  // adding spaces after/before redirections


int length_arr2 (char **arr) // return length of the string array;
{
	int i = 0;
	while (arr[i] != NULL)
	{
		i ++;
    	}
    	return i;
}  // return length of the string array;


int length_arr3 (char ***arr) // return length of the array of string arrays 
{
    	int i = 0;
   	while (arr[i] != NULL)
        	i ++;
 	return i;
} // return length array of string arrays


//**************************All above is right in a way of memory********************


char ** dividing_commands (char *string) // function is used for string parcing e.g : ls | wc -> (ls,|,wc), making array of strings
{
	int coef = STEP;
	char ** divided_array = malloc (coef * sizeof (char *));
	int pos = 0;
	char * pcommand;

    	if (divided_array == NULL)
	{
        	fprintf(stderr, "-Memory allocation error-\n");
		free(string); 
		exit(EXIT_FAILURE);
	}
	pcommand = strtok(string, " ");

	while (pcommand != NULL)
	{
		divided_array[pos] = pcommand;
       		pos ++;
        	if (pos >= coef)
		{
			coef += STEP;
            		divided_array = realloc (divided_array, coef * sizeof (char *));
            		if (divided_array == NULL)
			{
				fprintf(stderr, "Reallocation memory error-");
				free(string); //it is me
				free(divided_array); //it is me
				exit(EXIT_FAILURE);
			}
        	}
	        pcommand = strtok(NULL, " ");
    	}
	divided_array[pos] = NULL;
   	return divided_array;
} // parsing strings -> array of strings            ********it is also ok


int check_arr (char ** arr)
{
	char *sp[5];
	int i, j, k;

    	sp[0] = "<"; //input
    	sp[1] = ">"; //output
    	sp[2] = ">>"; //output with appending
    	sp[3] = "2>"; //errors
	sp[4] = "|";

	for (i = 1; i < length_arr2(arr) - 1; i ++)
	{
		for (j = 0; j < 5; j ++)
		{
			if (strcmp(arr[i], sp[j]) == 0)
			{
				for (k = 0; k < 5; k ++)
					if (strcmp(arr[i - 1], sp[k]) == 0 || (strcmp(arr[i + 1], sp[k]) == 0))
						return 0;
			}
		}
	}
	for (i = 0; i < 5; i ++)
		if (strcmp(arr[length_arr2(arr) - 1], sp[i]) == 0)
		       return 0;	
	return 1;
}


char *** preparing_to_exec (char ** inputarr)
{
	int i = 0, k = 0, l = 0, j;
	int coef = COMLEN;
	char ** bufArr = malloc (sizeof (char *) * coef);
	char *** splited = malloc (sizeof (char **) * coef);
	char ** del = bufArr;

	//for (i = 0; i < coef; i++)
	//	splited[i] = malloc(sizeof (char *) * coef);
	
    	for (i = 0; i < length_arr2(inputarr); i++)
       	{
        	if (inputarr[i][0] != '|')
		{
       			bufArr[l] = inputarr[i];
        		l ++;
       		}
        	else
		{
			splited[k] = malloc(sizeof (char *) * coef);
			if (splited[k] == NULL)
			{
				fprintf(stderr, "Reallocation memory error-");
				free(inputarr); //////////////////////////////////////////???????????????????????????
				free(splited); //it is me
				exit(EXIT_FAILURE);
			}
			for (j = 0; j < length_arr2(bufArr); j++) 
				splited[k][j] = bufArr[j];

     		    	splited[k][length_arr2(bufArr)] = NULL; //  closing with NULL for exec
            		bufArr = (bufArr + length_arr2(bufArr));
            		k ++;
            		l = 0;
		}
	}

	splited[k] = malloc(sizeof (char*) * coef);		
	if (splited[k] == NULL)
	{
		fprintf(stderr, "Reallocation memory error-");
		free(inputarr); //////////////////////////////////////////???????????????????????????
		free(splited); //it is me
		exit(EXIT_FAILURE);
	}

	for (j = 0; j < length_arr2(bufArr); j++)
		splited[k][j] = bufArr[j];
	splited[k][length_arr2(bufArr)] = NULL; //  closing with NULL for exec

	splited[k + 1] = malloc(sizeof(char*) * coef);
	if (splited[k + 1] == NULL)
	{
		fprintf(stderr, "Reallocation memory error-");
		free(inputarr); //////////////////////////////////////////???????????????????????????
		free(splited); //it is me
		exit(EXIT_FAILURE);
	}
 	splited[k + 1] = NULL;

	free(del);
	return splited;
} // making array of arrays of strings


void redirection (char **args, int * redirr) // if we have redirection, have to prepare descriptors
{
	int read = 0; // <
	int write = 0; // > >>
	int writeer = 0; // 2>
   	int switcher = 0;
   	char *fileName = malloc(SIZEFILENAME * sizeof(char));
    	char *redirectors[4];
	int i, j;

    	redirectors[0] = "<"; //input
    	redirectors[1] = ">"; //output
    	redirectors[2] = ">>"; //output with appending
    	redirectors[3] = "2>"; //errors

    	for (i = 0; i < length_arr2(args); i++)
       	{
       		for (j = 0; j < 4 ; j++)
	       	{
            		if (strcmp(args[i], redirectors[j]) == 0 && i + 1 != length_arr2(args))
			{
                		switcher = j + 1;
                		strcpy(fileName, args[i + 1]);
            		}
        	}
        	if ((strcmp(args[i], redirectors[1]) == 0 || (strcmp(args[i], redirectors[0]) == 0) || (strcmp(args[i], redirectors[2]) == 0) || (strcmp(args[i], redirectors[3]) == 0)) && args[i + 1] == NULL)
            		switcher = -2;

        	if (switcher == -2)
        	    	kill(getppid(), SIGUSR1);  // sending signal to parent, invalid redirection args
   		switch (switcher)
	       	{
            		case 1:
			       	read = open(fileName, O_RDONLY);
			       	break;
            		case 2:
			       	write = creat(fileName, 0666);
            			write = open(fileName, O_WRONLY);
			       	break;
     		       	case 3:
				write = open(fileName, O_WRONLY | O_APPEND);
				if (write == -1)
				       	write = creat(fileName, 0666);
				break;
            		case 4:
			       	writeer = open(fileName, O_WRONLY);
				break;
		        case -1:
			       	printf("check args\n");
			       	break;
            		default:
				continue;
        	}

    	}
    	redirr[0] = read;
    	redirr[1] = write;
    	redirr[2] = writeer;

//	free (fileName); // it is me
    	return;
}  // working with redirections


void delete_redirections (char **arr) // clear arg arr from redirectors
{
	char *redirectors[4];
    	int count = 0; 
	int k = 0;
     	int i = 0, j;

    	redirectors[0] = "<"; //input
    	redirectors[1] = ">"; //output
    	redirectors[2] = ">>"; //output with appending
    	redirectors[3] = "2>"; //errors

    	while (arr[i] != NULL)
	{
        	for (j = 0; j < 4; j++)
            		if (strcmp(arr[i], redirectors[j]) == 0) 
			{
				count ++;
                		k = i;
				while (arr[k] != NULL)
				{
					arr[k] = arr[k + 1];
					k ++;
				}
				break;
            		}
		i ++;
    	}
	arr[i - count] = NULL;
}  // cleaning redirect from cur command list e.g. (ls,>,a.txt) -> (ls,a.txt);


void ign (pid_t s)
{
	signal(SIGINT, SIG_IGN);
}


void arg (int s)
{
	signal(SIGUSR1, arg);
	printf("arg!!!\n");
}


void stop (int s)
{
    pid_t pid;
    pid = getpid();
    printf("stopped pid = %d", pid);
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
} // used to igone CTRL+Z, working on it


int shell (char ***argsArr) // shell func
{
	pid_t pid, ppid;
	int status;
	int fd[2];
	int i = 0, j, k;
	int *redirr = malloc (3 * sizeof(int));

	while (argsArr[i] != NULL)
	{
        	if (pipe(fd) < 0)
		{
			perror("-Pipe error-\n");
			free(redirr);
			for (j = 0; j < length_arr3(argsArr); j++)
				for (k = 0; k < length_arr2(argsArr[i]); k ++)
				       	free(argsArr[j][k]);
			free(argsArr);
			return 1;
		} // making pipe

        	int oldfd0; // saving descriptor from previous command, so each time new pipe is made, can't read from previous without saving
        	switch ((pid = fork()))
	       	{
            		case -1:
				printf("-Fork error-\n");
				free(redirr);
				for (j = 0; j < length_arr3(argsArr); j++)
					for (k = 0; k < length_arr2(argsArr[i]); k ++)
					       	free(argsArr[j][k]);
				free(argsArr);				
				return 1;
            		case 0:
                		signal(SIGINT, stop); // redirecting signal CTRL+C, works
                		signal(SIGTSTP, stop); // redirecting signal CTRL+Z, doesn't work
                		redirection(argsArr[i], redirr); // getting redirection if required
                		delete_redirections(argsArr[i]); // cleaning from redirections
                		
				if (i + 1 != length_arr3(argsArr)) // while not last command
                    			dup2(fd[1], 1); // output to pipe
                		if (i != 0)
                    			dup2(oldfd0, 0);   // if not first read from pipe
                		if (redirr[2] != 0)
				{
                    			dup2(redirr[2], 2); // redir from stderr to file
                    			close(redirr[2]);
                		}

               			if (redirr[1] != 0)
				{
                    			dup2(redirr[1], 1);
                    			close(redirr[1]);
                		}
                		if (redirr[0] != 0)	
				{
                    			dup2(redirr[0], 0);
                    			close(redirr[0]);
                		}
                		close(fd[1]); // closing descriptors
                		close(fd[0]);

                		execvp(argsArr[i][0], argsArr[i]); // executing commands
                		if (execvp(argsArr[i][0], argsArr[i]) == -1)
                			printf("-Execvp error-\n");

				//for (j = 0; j < length_arr3(argsArr); j ++) //   ************AND WHY IS IT A PROBLEM?
				//	for (k = 0; k < length_arr2(argsArr[i]); k ++)
				//	       	free(argsArr[j][k]);
                		exit(0);
		}
        	oldfd0 = fd[0];
        	close(fd[1]);

        	i++;
	}
    	while (wait(NULL) != -1); // waiting sons
    	
	free(redirr);
//	for (j = 0; j < length_arr3(argsArr); j ++) //   ************AND WHY IS IT A PROBLEM?
//		for (k = 0; k < length_arr2(argsArr[i]); k ++)
//		       	free(argsArr[j][k]);
                		
	return 1;
} // shell function


// builtin funcs:
int cd (char **args)
{
	if (args[1] == NULL)
	{
        	fprintf(stderr, "shell: waiting for \"cd\"\n");
	}
       	else
       	{
        	if (chdir(args[1]) != 0)
		{
            		perror("args");
        	}
    	}
    	return 1;
}


int shellExit (char **args)
{
	printf("goodbye!\n");
    	exit(0);
}


int mypwd(char **args)
{
	char cwd[LEN/2];
    	getcwd(cwd, sizeof(cwd));
    	printf("\nDirectory: %s", cwd);
    	return 0;
}


void builtInCheck (char **args)  // working with built in shell funcs
{
	int i = 0;

	if (strcmp(args[0], "cd") == 0)
       	{
        	cd(args);
        	mypwd(args);
    	}

    	if (strcmp(args[0], "exit") == 0)
		shellExit(args);

    	for (i = 0; i < length_arr2(args); i++)
        	if (strcmp(args[i], "pwd") == 0 && args[i + 1] == NULL)
            		mypwd(args);
}


void manager (void)
{
	int status;
	int k, i;
    	int myComs = 3;
    	char * inputstr;
    	char ** divided;
    	char *** executerArr;
    	char * ListOfBuiltInFuncs[myComs];
    
	ListOfBuiltInFuncs[0] = "exit";
    	ListOfBuiltInFuncs[1] = "cd";
    	ListOfBuiltInFuncs[2] = "pwd";
    	do
       	{
        	printf(">");

        	inputstr = read_commands();
		printf("%s\n", inputstr);

        	delete_spaces(inputstr);
		printf("%s\n", inputstr);

        	inputstr = build_spaces(inputstr);
		printf("%s\n", inputstr);

        	inputstr = build_spaces_redirection(inputstr);

    		divided = dividing_commands(inputstr);
		fprintf(stdout, "nodes = %d\n", length_arr2(divided));
		k = check_arr(divided);
		if (!k)
		{
			for (i = 0; i < length_arr2(divided); i++) // WHY IS IT A MISTAKE?
				free(divided[i]);
			free(divided);
			fprintf(stdout, "-Invalid input commands-\n");
			exit(EXIT_FAILURE);
		}

    		builtInCheck(divided);
    		printf("\n");
    		executerArr = preparing_to_exec(divided);
   		status = shell(executerArr);
	} while (status);
	free(inputstr);
	free(divided);
	free(executerArr);
}


int main()
{
	pid_t fPid;
    	fPid = getppid();
    	ign(fPid);
    	signal(SIGINT, ign);
    	signal(SIGTSTP, ign);
    	signal(SIGUSR1, arg);
	manager();

    	return 0;
}


