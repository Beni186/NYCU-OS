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
#include <bits/stdc++.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/syscall.h>

using namespace std;

int main(int argc, char** argv)
{
    ifstream in;
    timeval beg, end;
    double sec;
    cout<<"LFU policy:\n";
    cout<<"Frame\tHit\t\tMiss\t\tPage fault ratio\n";
    gettimeofday(&beg, 0);
    for(int frame=64; frame<=512; frame*=2)
    {
        unordered_map<int, pair<int, list<int>::iterator>> freq;
        unordered_map<int, list<int>> freqlist;
        in.open(argv[1]);
        int p;
        int miss = 0, hit = 0, lowestfreq = 1;
        int cnt =0;
        while(in>>p)
        {
            auto it = freq.find(p);
            if(it == freq.end())
            {
                miss++;
                //frame is full
                if(freq.size() == frame)
                {
                    freq.erase(freqlist[lowestfreq].back());
                    freqlist[lowestfreq].pop_back();
                    freqlist[1].push_front(p);
                    auto position = freqlist[1].begin();
                    freq[p] = make_pair(1, position);
                    lowestfreq = 1;
                }
                else
                {
                    freqlist[1].push_front(p);
                    auto position = freqlist[1].begin();
                    freq[p] = make_pair(1, position);
                    lowestfreq = 1;
                }
            }
            else
            {
                hit++;
                freqlist[it->second.first].erase(it->second.second);
                if(freqlist[it->second.first].empty() && lowestfreq == it->second.first)
				    lowestfreq++;
                it->second.first++;
                freqlist[it->second.first].push_front(p);
                it->second.second = freqlist[it->second.first].begin();
            }
        }
        printf("%d\t%d\t\t%d\t\t%.10f\n", frame, hit, miss, miss*1./(hit+miss));
        in.close();
    }
    gettimeofday(&end, 0);
	sec = (end.tv_sec - beg.tv_sec) + (end.tv_usec - beg.tv_usec) / 1e6;
	printf("Total elapsed tme %.4f sec\n\n", sec);
    //LRU
    cout<<"LRU policy:\n";
    cout<<"Frame\tHit\t\tMiss\t\tPage fault ratio\n";
    gettimeofday(&beg, 0);
    for(int frame=64; frame<=512; frame*=2)
    {
        unordered_map<int, list <int>::iterator> used;
        list <int> l;
        in.open(argv[1]);
        int p;
        int hit = 0, miss = 0;
        while(in>>p)
        {
            auto it = used.find(p);
            if(it != used.end())
            {
                hit++;
                l.erase(it->second);
                l.push_front(p);
                used[p] = l.begin();
            }
            else
            {
                miss++;
                if(used.size()!=frame)
                {
                    l.push_front(p);
                    used[p] = l.begin();
                }
                else
                {
                    used.erase(l.back());
                    l.pop_back();
                    l.push_front(p);
                    used[p] = l.begin();
                }
            }
        }
        printf("%d\t%d\t\t%d\t\t%.10f\n", frame, hit, miss, miss*1./(hit+miss));
        in.close();
    }
    gettimeofday(&end, 0);
	sec = (end.tv_sec - beg.tv_sec) + (end.tv_usec - beg.tv_usec) / 1e6;
	printf("Total elapsed tme %.4f sec\n", sec);
}