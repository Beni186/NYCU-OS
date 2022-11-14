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
#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/syscall.h>


using namespace std;

vector <int> num;
int fr[8];
int s[8];
int size;
bool done[16];
sem_t finish, Mutex;
queue <int> job;

void bubblesort(int l, int r)
{
    for(int i=l; i<r-1; i++)
        for(int j=i+1; j<r; j++)
            if(num[i] > num[j])
                swap(num[i], num[j]);
}

void merge(int front, int mid, int end)
{
    vector<int> LeftSub(num.begin()+front, num.begin()+mid+1),
                RightSub(num.begin()+mid+1, num.begin()+end+1);

    LeftSub.insert(LeftSub.end(), INT_MAX);
    RightSub.insert(RightSub.end(), INT_MAX);

    int idxLeft = 0, idxRight = 0;

    for (int i = front; i <= end; i++) {

        if (LeftSub[idxLeft] <= RightSub[idxRight] ) {
            num[i] = LeftSub[idxLeft];
            idxLeft++;
        }
        else{
            num[i] = RightSub[idxRight];
            idxRight++;
        }
    }
}

void *handler(void *)
{
    while(1)
    {
        //cout<<syscall(SYS_gettid)<<" ";
        sem_wait(&Mutex);
        if(job.empty())
        {
            sem_post(&Mutex);
            sem_post(&finish);
            return NULL;
        }
        
        int work = job.front();
        job.pop();
        sem_post(&Mutex);

        //bubble sort
        if(work > 7)
        {
            int l = 0;
            for(int i=0; i<work-8; i++)
                l += s[i];
            int r = l + s[work-8];
            bubblesort(l, r);
        }
        //merge
        else
        {
            int front, mid, end;
            if(work>3)
            {
                front = fr[work];
                mid = front + s[work*2 - 8] - 1;
                end = mid + s[work*2 - 7];
            }
            else
            {
                front = fr[work];
                mid = fr[work*2 + 1] - 1;
                if(work == 2)
                    end = fr[6] - 1;
                else
                    end = size - 1;
            }
            merge(front, mid, end);
        }

        sem_wait(&Mutex);
        done[work] = true;
        if(work%2 == 0)
        {
            if(done[work+1])
                job.push(work/2);
        }
        else
        {
            if(done[work-1])
                job.push((work-1)/2);
        }
        sem_post(&Mutex);

    }
}

int main()
{
    ifstream in;
    ofstream out;
    in.open("input.txt");
    in >> size;
    vector<int> temp(size, 0);
    num.assign(size, 0);
    for(int i=0; i<size; i++)
        in>>temp[i];
    in.close();

    int count = size % 8;
    for(int i=0; i<8; i++)
    {
        if(count > 0)
        {
            s[i] = (size / 8) + 1;
            count--;
        }
        else 
            s[i] = (size / 8);
    }

    fr[1] = 0;
    fr[2] = 0;
    fr[4] = 0;
    for(int i=5; i<=7; i++)
        fr[i] = fr[i-1] + s[(i*2)-10] + s[(i*2)-9];
    fr[3] = fr[6];
    
    for(int n=1; n<=8; n++)
    {
        num = temp;
        memset(done, false, 16);
        sem_init(&finish, 0, 0);
        sem_init(&Mutex, 0, 1);
        pthread_t threads[n];

        struct timeval start, end;
        gettimeofday(&start, 0);

        for(int i=8; i<=15; i++)
            job.push(i);

        for(int i=0; i<n; i++)
            pthread_create(&threads[i], NULL, handler, NULL);
        
        sem_wait(&finish);

        gettimeofday(&end, 0);

        for(int i=0; i<n; i++)
            pthread_join(threads[i], NULL);

        float ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
        printf("worker thread #%d, elapsed %f ms\n", n, ms);

        string str;
        str.append("output_").append(to_string(n)).append(".txt");
        out.open(str);
        for(int i=0; i<size; i++)
            out<<num[i]<<" ";
        out.close();
        sem_destroy(&finish);
        sem_destroy(&Mutex);
    }
    
}