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
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<sys/time.h>

int main()
{
    int size;
    printf("Input dimension: ");
    scanf("%d", &size);

    unsigned int matA[size*size];
    for(int i=0; i<size; i++)
    {
        for(int j=0; j<size; j++)
            matA[i*size+j] = i*size+j;      
    }

    int shmid = shmget(0, size*size*sizeof(unsigned int), IPC_CREAT|0600);
    int *matC = shmat(shmid, NULL, 0);

    struct timeval start, end;
    for(int p=1; p<=16; p++)
    {
        unsigned int sum = 0;
        pid_t pid;

        gettimeofday(&start,0);

        for(int now=1; now<=p; now++)
        {
            int row = size/p, temp = 0, stop;
            if(now == p)
                stop = size;
            else
                stop = row * now;
            pid = fork();
            if(pid == 0){
                for(int i=row*(now-1); i<stop; i++)
                    {
                        for(int j=0; j<size; j++)
                        {   
                            temp = 0;

                            for(int k=0; k<size; k++)
                                temp += matA[i * size + k] * matA[k * size + j];

                            matC[i*size+j] = temp;
                        }             
                    }
                exit(0);
            }         
        }

        for(int i=0; i<p; i++)
            wait(NULL);

        for(int i=0; i<size; i++)
        {
            for(int j=0; j<size; j++)
                sum += matC[i*size+j];
        }

        gettimeofday(&end, 0);
        int sec=end.tv_sec - start.tv_sec;
        int usec = end.tv_usec - start.tv_usec;
        printf("Multiplying matrices using %d processes\nelapsed %f sec, Checksum: %u\n", p, (sec*1000+(usec/1000.0))/1000, sum);
    }

    shmdt(matC);
    shmctl(shmid, IPC_RMID, NULL);
}
