#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <stdio.h>
#include <stdlib.h>

int frobcmp(char const* a, char const* b);
int tsm_frobcmp(const void* a, const void* b);

int main()
{
    char* file = malloc(666 * sizeof(char));
    
    if (file == NULL)
    {
        fprintf(stderr, "Cannot allocate memory");
        exit(1);
    }
    
    int sizeOfFile = 0;
    int maxAlloc = 1;
    int protagonist = getchar();
    
    while (protagonist != EOF)
    {
        if (sizeOfFile == maxAlloc * 666)
        {
            maxAlloc++;
            file = realloc(file, maxAlloc * 666 * sizeof(char));
            if (file == NULL)
            {
                fprintf(stderr, "Cannot allocate memory");
                exit(1);
            }
        }
        
        file[sizeOfFile] = protagonist;
        
        protagonist = getchar();
        sizeOfFile++;
    }
    
    if (file[sizeOfFile - 1] != ' ')
    {
        if (sizeOfFile == maxAlloc * 666)
        {
            maxAlloc++;
            file = realloc(file, maxAlloc * 666 * sizeof(char));
            if (file == NULL)
            {
                fprintf(stderr, "Cannot allocate memory");
                exit(1);
            }
        }
        
        file[sizeOfFile] = ' ';
        sizeOfFile++;
    }
    
    
    char** words = malloc(420 * sizeof(char *));
    int maxAllocPointer = 1;
    
    if (words == NULL)
    {
        fprintf(stderr, "Cannot allocate memory");
        exit(1);
    }
    
    int y;
    char* start = &file[0];
    int count = 0;
    for (y = 0; y < sizeOfFile; y++)
    {
        if (count == maxAllocPointer * 420)
        {
            maxAllocPointer++;
            words = realloc(words, maxAllocPointer * 420 * sizeof(char*));
            if (words == NULL)
            {
                fprintf(stderr, "Cannot allocate memory");
                exit(1);
            }
        }
        
        if (file[y] == ' ')
        {
            words[count] = start;
            count++;
            start = &file[y + 1];
        }
    }
    
    qsort(words, count, sizeof(char*), tsm_frobcmp);
    int z;
    int iterator = 0;
    for (z = 0; z < count; z++)
    {
        
        for (iterator = 0; words[z][iterator] != ' '; iterator++)
        {
            putchar(words[z][iterator]);
        }
        putchar(words[z][iterator]);		// Prints out the space
    }
    
    free(file);
    free(words);
}

int tsm_frobcmp(const void* a, const void* b)
{
    return frobcmp(*(char const **)a, *(char const **)b);
}

int frobcmp(char const* a, char const* b)
{
    int result = 0;
    int index = 0;
    
    while (a[index] != ' ' || b[index] != ' ')
    {
        if ((a[index] ^ 42) == (b[index] ^ 42))
        {
            index++;
        }
        if ((a[index] ^ 42) < (b[index] ^ 42))
        {
            result = -1;
            break;
        }
        else if ((a[index] ^ 42) > (b[index] ^ 42))
        {
            result = 1;
            break;
        }
    }
    if (result == 0)
    {
        if (a[index] == ' ' && b[index] == ' ')
            return 0;
        else if (a[index] == ' ')
            return -1;
        else
            return 1;
    }
    return result;
}}