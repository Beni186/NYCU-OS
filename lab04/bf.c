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
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

typedef struct block {
	size_t size;
	int free;
	struct block *prev;
	struct block *next;
}Block;

void *add = 0;
Block *head = 0;
static const size_t align_to = 32;

void *malloc(size_t size)
{
    Block *cur = 0, *temp = 0;
    if(size)
    {
        size = (size + (align_to - 1)) & ~ (align_to - 1);
        if(!add)
        {
            add = mmap(0, 20000, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
            head = add;
            head->size = 20000 - 32;
            head->free = 1;
            head->prev = NULL;
            head->next = NULL;
        }

        cur = head;
        size_t maxsize = 65536;
        while(cur != NULL)
        {
            if(cur->free && cur->size < maxsize && cur->size >= size)
            {
                maxsize = cur->size;
                temp = cur;
            } 
            cur = cur->next;
        }
        cur = temp;

        if(cur->size == size)
        {
            cur->free = 0;
            return cur + 1;
        }
        else
        {
            temp = cur + 1 + size/32;
            temp->free = 1;
            temp->prev = cur;
            temp->next = cur->next;
            temp->size = cur->size - 32 - size;
            if(cur->next != NULL)
                cur->next->prev = temp;
            cur->next = temp;
            cur->free = 0;
            cur->size = size;
            return cur + 1;
        }

        return 0;
    }
    else if(!size)
    {
        cur = head;
        size_t maxsize = 0;
        while(cur != NULL)
        {
            if(cur->size > maxsize && cur->free)
                maxsize = cur->size;
            cur = cur->next;
        }
        char m[30];
        memset(m, 0, 30);
        snprintf(m, 30, "Max Free Chunk Size = %ld\n", maxsize);
        write(1, m, 30);
        return 0;
    }
}

void free(void *ptr)
{
    Block *cur;
    cur = ptr;
    cur -= 1;
    cur->free = 1;
    if(cur->next != NULL && cur->next->free)
    {
        cur->size += 32 + cur->next->size;
        if(cur->next->next != NULL)
            cur->next->next->prev = cur;
        cur->next = cur->next->next;
    }
    if(cur->prev != NULL && cur->prev->free)
    {
        cur->prev->size += 32 + cur->size;
        if(cur->next != NULL)
            cur->next->prev = cur->prev;
        cur->prev->next = cur->next;
    }
    return;
}