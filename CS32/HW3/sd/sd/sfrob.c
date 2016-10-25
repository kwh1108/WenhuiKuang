//
//  sfrob.c
//  sd
//
//  Created by wenhui kuang on 2/4/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//


#include<stdio.h>
#include<stdlib.h>

int main(){
    char *s = "\030";
    char *w= s;
    while (*w != '\0')
    {
        char e = *w^42;
        printf("%c",e);
        w++;
    }
}