/*
Student No.:
109550167
Student Name:
陳唯文
Email:
ben167.cs09@nycu.edu.tw
SE tag:
xnxcxtxuxoxsx
Statement: I am fully aware that this program is not
supposed to be posted to a public server, such as a
public GitHub repository or a public web page.
*/
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

void split_line(char*input, char **words, int *zombie)
{
    //char **words=malloc(80*sizeof(char*)), *delim=" \n";
    char *delim=" \n";
    int i=0;
    char *word=strtok(input, delim);
    while(word!=NULL)
    {
        words[i]=word;
        word=strtok(NULL, delim);
        i++;
    }
    if(strcmp(words[i-1], "&")==0)
    {
        *zombie=1;
        words[i-1]=NULL;
    }
    else  
        words[i]=NULL;
    return ;
}

int checkpipe(char *input)
{
    char *cp=malloc(80);
    strcpy(cp, input);
    //check pipe
    char *pipe=strtok(cp, "|");
    if(strcmp(input, pipe)==0)
    {
        pipe=strtok(cp, ">");
        if(strcmp(input, pipe)==0)
        {
            pipe=strtok(cp, "<");
            if(strcmp(input, pipe)==0)
            {
                return 0;
            }
            // <
            else
                return 3;
        }
        // >
        else 
            return 2;
    }
    // |
    else 
        return 1;
    //return strcmp(input, pipe)==0 ? 0 : 1;
}

void parsepipe(char *input, char *front , char *back)
{
    char *cp=malloc(80);
    strcpy(cp, input);
    char *pipe=strtok(cp, "|");
    strcpy(front, pipe);
    pipe = strtok(NULL, "|");
    strcpy(back, pipe);
}

void parse1(char *input, char *front , char *back)
{
    char *cp=malloc(80);
    strcpy(cp, input);
    char *pipe=strtok(cp, ">");
    strcpy(front, pipe);
    pipe = strtok(NULL, ">");
    strcpy(back, pipe);
}

void parse2(char *input, char *front , char *back)
{
    char *cp=malloc(80);
    strcpy(cp, input);
    char *pipe=strtok(cp, "<");
    strcpy(front, pipe);
    pipe = strtok(NULL, "<");
    strcpy(back, pipe);
}

void runpipe(char **front, char **back)
{
    pid_t pid1, pid2;
    int fd[2];

    if(pipe(fd) == -1)
        printf("pipe error");

    pid1=fork();
    if(pid1<0)
    {
        fprintf(stderr, "fork failed");
        exit(-1);
    }
    //child1 write(fd[0]-> read/ fd[1] -> write)
    else if(pid1==0)
    {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        //close(fd[1]);
        execvp(front[0], front);
    }

    //child2 read
    pid2=fork();
    if(pid2<0)
    {
        fprintf(stderr, "fork failed");
        exit(-1);
    }
    else if(pid2==0)
    {
        dup2(fd[0], STDIN_FILENO);
        //close(fd[0]);
        close(fd[1]);
        execvp(back[0], back);
    }

    close(fd[0]);
    close(fd[1]);
    wait(NULL);
    wait(NULL);
    return ;
}

void redirect1(char **args, char *file)
{
    pid_t pid;
    pid = fork();
    if(pid < 0)
        fprintf(stderr, "fork failed");
    else if(pid == 0)
    {
        int ffile = open(file, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR);
        
        dup2(ffile, STDOUT_FILENO);
        execvp(args[0], args);
    }
    wait(NULL);
    return ;
}

void redirect2(char **args, char *file)
{
    pid_t pid;
    pid = fork();
    if(pid < 0)
        fprintf(stderr, "fork failed");
    else if(pid == 0)
    {
        int ffile = open(file, O_RDONLY, S_IRUSR|S_IWUSR);
        
        dup2(ffile, STDIN_FILENO);
        execvp(args[0], args);
    }
    wait(NULL);
    return ;
}

void launch(char **words, int zombie)
{
    pid_t pid;
    pid=fork();
    if(zombie)
    {
        if(pid<0)
        {
            fprintf(stderr, "fork failed");
            exit(-1);
        }
        else if(pid==0)
        {
            pid=fork();
            if(pid<0)
            {
                fprintf(stderr, "fork failed");
                exit(-1);
            }
            else if(pid>0)
            {
                exit(0);
            }
            else
            {
                sleep(2);
                execvp(words[0], words);
                exit(0);
            }
        }
        else
        {
            wait(NULL);
            return;
        }
    }
    else
    {
        if(pid<0)
        {
            fprintf(stderr, "fork failed");
            exit(-1);
        }
        else if(pid==0)
        {
            execvp(words[0], words);
            exit(0);
        }
        else
        {
            wait(NULL);
            return;
        }
    }
        
}

int main()
{

    // char *input=(char*)malloc(80*sizeof(char)), **r;
    char *input=malloc(80);
    char **words= malloc(30*sizeof(char *));
    for (int i=0; i<30; i++) {
        words[i]= malloc(40);
    }
    printf("> ");
    while(fgets(input, 80, stdin))
    {
        if(strcmp(input, "exit\n")==0)
            break;
        
        int check = checkpipe(input);
        if(check==1)
        {
            char *front = malloc(80), *back = malloc(80);
            char **f= malloc(30*sizeof(char *)), **b=malloc(30*sizeof(char *));
            for (int i=0; i<30; i++) {
                f[i] = malloc(40);
                b[i] = malloc(40);
            }

            parsepipe(input, front, back);
            int n1, n2;
            split_line(front, f, &n1);
            split_line(back, b, &n2);

            runpipe(f, b);
        }
        // >
        else if(check==2)
        {
            int n;
            char *file = malloc(50), *temp = malloc(50);
            char **args= malloc(30*sizeof(char *));
            for (int i=0; i<30; i++)
                args[i] = malloc(40);
            
            parse1(input, temp, file);
            split_line(temp, args, &n);
            file = strtok(file, " \n");
            redirect1(args, file);
        }
        // <
        else if(check==3)
        {
            int n;
            char *file = malloc(50), *temp = malloc(50);
            parse2(input, temp, file);
            char **args= malloc(30*sizeof(char *));
            for (int i=0; i<30; i++)
                args[i] = malloc(40);
            
            parse2(input, temp, file);
            split_line(temp, args, &n);
            file = strtok(file, " \n");
            redirect2(args, file);
        }
        else if(check==0)
        {
            int zombie=0;
            split_line(input, words, &zombie);
            
            launch(words, zombie);
        }   
        printf("> ");
    }
    return 0;
}
