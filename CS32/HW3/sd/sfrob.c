//
//  sfrob.c
//  sd
//
//  Created by wenhui kuang on 2/4/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//


#include<stdio.h>
#include<stdlib.h>

int frobcmp (const void *a, const void *b)
{
    const char *c = *(const char**)a;    
    const char *d = *(const char**)b;
    
    while ((*c^42) == (*d^42) && *c != ' ' && *d != ' ')
    {
        c++;
        d++;
    }
    if (*c == ' ' && *d == ' ')
        return 0;
    if (*c == ' ' || (*c^42) < (*d^42))
        return -1;
    if (*d == ' ' || (*c^42) > (*d^42))
        return 1;
    return 0;
}
int main(){
    int size = 512;
    int numChars = 0;
    char *buffer = (char *)malloc(sizeof(char) * size);
    
    while (1)
    {
        if (numChars == size)
        {
            buffer = realloc(buffer, (size *= 2) * sizeof(char));
            if (buffer == NULL)
            {
                fprintf(stderr, "Cannot allocate memory");
                exit(1);
            }
        }
        char c = getchar();
        if (c == EOF)
            break;
        buffer[numChars] = c;
        numChars++;
    }
    
    if (buffer[numChars - 1] != ' ')
    {
        if(numChars == size)
        {
            buffer = realloc(buffer, (size *= 2) * sizeof(char));
            if (buffer == NULL)
            {
                fprintf(stderr, "Cannot allocate memory");
                exit(1);
            }
        }
        buffer[numChars] = ' ';
        numChars++;
    }
    
    char **words = (char **)malloc(sizeof(char*) *size);
    
    if (words == NULL)
    {
        fprintf(stderr, "Cannot allocate memory");
        exit(1);
    }
    char *begin = buffer;
    int i;
    int numWords = 0;
    for (i = 0; i < numChars; i++)
    {
        if (numWords == size)
        {
            buffer = realloc(buffer, (size *= 2) * sizeof(char));
            if (buffer == NULL)
            {
                fprintf(stderr, "Cannot allocate memory");
                exit(1);
            }
        }
        if (buffer[i] == ' ' )
        {
            words[numWords] = begin;
            numWords++;
            begin = buffer + (i + 1);
        }
    }
    
    qsort(words, numWords, sizeof(char*), frobcmp);
    
    int j,p;
    for (j = 0; j < numWords; j++)
    {
        for (p = 0; words[j][p] != ' '; p++)
        {
            putchar(words[j][p]);
        }
        putchar(words[j][p]); // prints out the spaces.
    }
    
    free(words);
    free(buffer);
    return 0;
}