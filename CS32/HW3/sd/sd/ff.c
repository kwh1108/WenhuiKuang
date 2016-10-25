//
//  ff.c
//  sd
//
//  Created by wenhui kuang on 2/4/16.
//  Copyright (c) 2016 wenhui kuang. All rights reserved.
//
int main()
{
    char *a = "asdasd";
    char *b = "asdasd";
    char *c = "asdasd";
    char *d = "asdasd";
    char *e = "asdasd";
    
    char **array[5];
    array[0] = &a;
    printf("%s",*array[0]);
}
