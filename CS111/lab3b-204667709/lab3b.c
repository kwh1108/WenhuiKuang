#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


struct block_structure
{
    unsigned long block;
    unsigned long inode;
    unsigned long entry;
    unsigned long indirect;
    unsigned long directory;
    struct block_structure *next;
};
/*missing link count*/
struct link_count_dir
{
    unsigned long int inode_number;
    unsigned long int link_count;
};

struct block_structure** block_structure_insert(struct block_structure** arr, struct block_structure* node, int* size, int* alloc_size)
{
    if (*size >= *alloc_size)
    {
        *alloc_size *= 2;
        arr = (struct block_structure**)realloc(arr, (*alloc_size) * sizeof(struct block_structure*));
    }
    //printf("size:%d, alloc_size:%d\n", *size, *alloc_size);
    int i;
    for (i = 0; i < *size; i++)
    {
        if (arr[i]->block == node->block)
        {
            struct block_structure* node_pointer = arr[i];
            while (node_pointer->next != NULL)
            {
                node_pointer = node_pointer->next;
            }
            node_pointer->next = node;
            return arr;
        }
    }
    arr[*size] = node;
    *size += 1;
    return arr;
}

struct block_structure** block_structure_free(struct block_structure** arr, int* size, int* alloc_size){
    if (arr == NULL)
        return arr;
    int i;
    for (i = 0; i < *size; i++)
    {
        struct block_structure * ptr = arr[i];
        struct block_structure * tofree;
        
        while (ptr->next != NULL){
            tofree = ptr;
            ptr = ptr->next;
            free(tofree);
        }
        free(ptr);
    }
    free(arr);
    arr = NULL;
    *size = 0;
    *alloc_size = 0;
    return arr;
}

struct csv_struct{
    int rows;
    int cols;
    int rows_alloc;
    char*** table; //2D array of char*
};

int get_csv_num_cols(FILE* csv_fd){
    /*get number of columns, assume every line has same number of ',' and end with '\n'*/
    char c[1];
    int num_col = 0;
    
    fseek(csv_fd, 0, SEEK_SET);
    
    int i;
    while ( (i = fread(c, 1, 1, csv_fd)) == 1){
        if (c[0] == ',')
            num_col += 1;
        else if(c[0] == '\n')
            break;
    }
    
    if (num_col != 0 && i == 1)
        num_col = num_col + 1;
    return num_col;
}

int get_csv(FILE* csv_fd, struct csv_struct* csv){
    csv->cols = get_csv_num_cols(csv_fd);
    csv->rows = 0;
    if (csv->cols == 0)
        return 2;
    csv->rows_alloc = 64;
    csv->table = (char***)calloc(csv->rows_alloc, sizeof(char**));
    
    int offset = 0;
    int count_byte = 0;
    int count_word = 0;
    int line_capacity = 128;
    char* line = malloc(line_capacity);
    int char_size[csv->cols];
    
    fseek(csv_fd, 0, SEEK_SET);
    
    int open_quote = 0;
    while ( fread(line + offset, 1, 1, csv_fd) == 1){
        if (line[offset] == ',' && open_quote == 0){
            char_size[count_word] = count_byte;
            count_byte = 0;
            count_word += 1;
            offset += 1;
        }
        else if (line[offset] == '\n' && open_quote == 0){
            char_size[count_word] = count_byte;
            count_byte = 0;
            if (count_word != csv->cols - 1){
                free(line);
                return 1;
            }
            /* allocate char** */
            csv->table[csv->rows] = (char**)calloc(csv->cols, sizeof(char*));
            int j, k;
            int b = 0;
            for (j = 0; j < csv->cols; j++){
                csv->table[csv->rows][j] = (char*)malloc(char_size[j] + 1);
                for (k = 0; k < char_size[j]; k++){
                    csv->table[csv->rows][j][k] = line[b];
                    b += 1;
                }
                csv->table[csv->rows][j][k] = 0;
                b += 1;
            }
            ////
            csv->rows += 1;
            offset = 0;
            count_byte = 0;
            count_word = 0;
        }
        else{
            if (line[offset] == '"'){
                if (open_quote == 1)
                    open_quote = 0;
                else
                    open_quote = 1;
            }
            count_byte += 1;
            offset += 1;
        }
        
        if (count_word >= csv->cols){
            free(line);
            return 1;
        }
        
        if (offset >= line_capacity){
            line_capacity *= 2;
            line = (char*)realloc(line, line_capacity);
        }
        
        if (csv->rows >= csv->rows_alloc){
            csv->rows_alloc *= 2;
            csv->table = (char***)realloc(csv->table, csv->rows_alloc * sizeof(char**));
        }
    }
    free(line);
    return 0;
}

void free_csv(struct csv_struct* csv){
    int i, j;
    if(csv->table==NULL || csv->rows <= 0 || csv->cols <= 0)
        return;
    for (i = 0; i < csv->rows; i++){
        for (j = 0; j < csv->cols; j++)
            free(csv->table[i][j]);
        free(csv->table[i]);
    }
    free(csv->table);
}

void test_csv(char* filename, struct csv_struct* csv){
    char* newline = "\n";
    char* comma = ",";
    FILE* tt = fopen(filename, "w");
    int r = csv->rows;
    int c = csv->cols;
    char*** arr = csv->table;
    int i, j;
    for (i = 0; i < r; i++){
        for (j = 0; j < c; j++){
            fwrite(arr[i][j], strlen(arr[i][j]), 1, tt);
            if (j==c-1)
                fwrite(newline, 1, 1, tt);
            else
                fwrite(comma, 1, 1, tt);
        }
    }
    fclose(tt);
}

void errorANDexit(char* error_msg){
    perror(error_msg);
    exit(EXIT_FAILURE);
}

int main(){
    /*input output files descriptors*/
    int fd;
    FILE* super_csv;
    FILE* group_csv;
    FILE* bitmap_csv;
    FILE* inode_csv;
    FILE* directory_csv;
    FILE* indirect_csv;
    
    /*open input & output file descriptors*/
    fd = creat("lab3b_check.txt", 0644);
    
    super_csv = fopen("super.csv", "r");
    if (super_csv == NULL)
        errorANDexit("fopen");
    group_csv = fopen("group.csv", "r");
    if (group_csv == NULL)
        errorANDexit("fopen");
    bitmap_csv = fopen("bitmap.csv", "r");
    if (bitmap_csv == NULL)
        errorANDexit("fopen");
    inode_csv = fopen("inode.csv", "r");
    if (inode_csv == NULL)
        errorANDexit("fopen");
    directory_csv = fopen("directory.csv", "r");
    if (directory_csv == NULL)
        errorANDexit("fopen");
    indirect_csv = fopen("indirect.csv", "r");
    if (indirect_csv == NULL)
        errorANDexit("fopen");
    
    struct csv_struct super;
    struct csv_struct group;
    struct csv_struct bitmap;
    struct csv_struct inode;
    struct csv_struct directory;
    struct csv_struct indirect;
    get_csv(super_csv, &super);
    get_csv(group_csv, &group);
    get_csv(bitmap_csv, &bitmap);
    get_csv(inode_csv, &inode);
    get_csv(directory_csv, &directory);
    get_csv(indirect_csv, &indirect);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*inode freelist & block freelist*/
    int size_inode_freelist = 0;
    int size_block_freelist = 0;
    char** inode_freelist = (char**)calloc(bitmap.rows , sizeof(char*));
    char** block_freelist = (char**)calloc(bitmap.rows , sizeof(char*));
    
    int i, j, k, l, m;
    for (i = 0; i < bitmap.rows; i++){
        for (j = 0; j < group.rows; j++){
            if (strcmp(bitmap.table[i][0], group.table[j][4])==0){ //inode
                inode_freelist[size_inode_freelist] = malloc(strlen(bitmap.table[i][1]) + 1);
                strcpy(inode_freelist[size_inode_freelist], bitmap.table[i][1]);
                size_inode_freelist += 1;
                break;
            }
            else if (strcmp(bitmap.table[i][0], group.table[j][5]) == 0){//block
                block_freelist[size_block_freelist] = malloc(strlen(bitmap.table[i][1]) + 1);
                strcpy(block_freelist[size_block_freelist], bitmap.table[i][1]);
                size_block_freelist += 1;
                break;
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*1. unallocated block*/
    int block_structure_size = 0;
    int block_structure_allocated_size = 4;
    struct block_structure** heads = calloc(block_structure_allocated_size, sizeof(struct block_structure*));
    /*2. duplicately allocated block*/
    int dup_alloc_size = 0;
    int dup_alloc_malloced = 64;
    struct block_structure** dup_alloc = calloc(dup_alloc_malloced, sizeof(struct block_structure*));
    /*7. invalid block pointer*/
    int invalid_block_size = 0;
    int invalid_block_malloced = 64;
    struct block_structure** invalid_block = calloc(invalid_block_malloced, sizeof(struct block_structure*));
    
    
    /*largest block number*/
    unsigned long largest_block_number = strtoul(super.table[0][2], NULL, 10);
    
    /*iterate through each inode*/
    struct block_structure* bbb;
    for (i = 0; i < inode.rows; i++){
        int num_blocks = strtoul(inode.table[i][10], NULL, 10);
        int blocks_processed = 0;
        /*direct block*/
        for (j = 0; j < 12; j++){
            if (blocks_processed >= num_blocks)
                break;
            for (k = 0; k < size_block_freelist; k++){
                if (strtoul(inode.table[i][j + 11], NULL, 16) == strtoul(block_freelist[k], NULL, 10)){
                    bbb = malloc(sizeof(struct block_structure));
                    bbb->block = strtoul(inode.table[i][j + 11], NULL, 16);
                    bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                    bbb->entry = (unsigned long)j;
                    bbb->indirect = 0;
                    bbb->next = NULL;
                    heads = block_structure_insert(heads, bbb, &block_structure_size, &block_structure_allocated_size);
                    break;
                }
            }
            bbb = malloc(sizeof(struct block_structure));
            bbb->block = strtoul(inode.table[i][j + 11], NULL, 16);
            bbb->inode = strtoul(inode.table[i][0], NULL, 10);
            bbb->entry = (unsigned long)j;
            bbb->indirect = 0;
            bbb->next = NULL;
            if (strtoul(inode.table[i][j + 11], NULL, 16) == 0 || strtoul(inode.table[i][j + 11], NULL, 16) > largest_block_number){
                //if (strtoul(inode.table[i][j + 11], NULL, 16) > largest_block_number)
                invalid_block = block_structure_insert(invalid_block, bbb, &invalid_block_size, &invalid_block_malloced);
            }
            else{
                dup_alloc = block_structure_insert(dup_alloc, bbb, &dup_alloc_size, &dup_alloc_malloced);
            }
            blocks_processed += 1;
        }
        
        /*indirect block 12*/
        if (blocks_processed >= num_blocks)
            continue;
        bbb = malloc(sizeof(struct block_structure));
        bbb->block = strtoul(inode.table[i][12 + 11], NULL, 16);
        bbb->inode = strtoul(inode.table[i][0], NULL, 10);
        bbb->entry = 12;
        bbb->indirect = 0;
        bbb->next = NULL;
        if (strtoul(inode.table[i][12 + 11], NULL, 16) == 0 || strtoul(inode.table[i][12 + 11], NULL, 16) > largest_block_number){
            //if (strtoul(inode.table[i][12 + 11], NULL, 16) > largest_block_number)
            invalid_block = block_structure_insert(invalid_block, bbb, &invalid_block_size, &invalid_block_malloced);
        }
        else
        {
            dup_alloc = block_structure_insert(dup_alloc, bbb, &dup_alloc_size, &dup_alloc_malloced);
            for (k = 0; k < size_block_freelist; k++){
                if (strtoul(inode.table[i][12 + 11], NULL, 16) == strtoul(block_freelist[k], NULL, 10)){
                    bbb = malloc(sizeof(struct block_structure));
                    bbb->block = strtoul(inode.table[i][12 + 11], NULL, 16);
                    bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                    bbb->entry = 12;
                    bbb->indirect = 0;
                    bbb->next = NULL;
                    heads = block_structure_insert(heads, bbb, &block_structure_size, &block_structure_allocated_size);
                    break;
                }
            }
            for (j = 0; j < indirect.rows; j++){
                if (blocks_processed >= num_blocks)
                    break;
                if (strcmp(inode.table[i][12 + 11], indirect.table[j][0]) == 0){
                    bbb = malloc(sizeof(struct block_structure));
                    bbb->block = strtoul(indirect.table[j][2], NULL, 16);
                    bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                    bbb->entry = strtoul(indirect.table[j][1], NULL, 16);
                    bbb->indirect = strtoul(indirect.table[j][0], NULL, 16);
                    bbb->next = NULL;
                    if (strtoul(indirect.table[j][2], NULL, 16) == 0 || strtoul(indirect.table[j][2], NULL, 16) > largest_block_number){
                        //if (strtoul(indirect.table[j][2], NULL, 16) > largest_block_number)
                        invalid_block = block_structure_insert(invalid_block, bbb, &invalid_block_size, &invalid_block_malloced);
                    }
                    else{
                        dup_alloc = block_structure_insert(dup_alloc, bbb, &dup_alloc_size, &dup_alloc_malloced);
                    }
                    for (k = 0; k < size_block_freelist; k++){
                        if (strtoul(indirect.table[j][2], NULL, 16) == strtoul(block_freelist[k], NULL, 10)){
                            bbb = malloc(sizeof(struct block_structure));
                            bbb->block = strtoul(indirect.table[j][2], NULL, 16);
                            bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                            bbb->entry = strtoul(indirect.table[j][1], NULL, 16);
                            bbb->indirect = strtoul(indirect.table[j][0], NULL, 16);
                            bbb->next = NULL;
                            heads = block_structure_insert(heads, bbb, &block_structure_size, &block_structure_allocated_size);
                            break;
                        }
                    }
                    blocks_processed += 1;
                }
            }
        }
        blocks_processed += 1;
        
        /*indirect block 13*/
        if (blocks_processed >= num_blocks)
            continue;
        bbb = malloc(sizeof(struct block_structure));
        bbb->block = strtoul(inode.table[i][13 + 11], NULL, 16);
        bbb->inode = strtoul(inode.table[i][0], NULL, 10);
        bbb->entry = 13;
        bbb->indirect = 0;
        bbb->next = NULL;
        if (strtoul(inode.table[i][13 + 11], NULL, 16) == 0 || strtoul(inode.table[i][13 + 11], NULL, 16) > largest_block_number){
            //if (strtoul(inode.table[i][13 + 11], NULL, 16) > largest_block_number)
            invalid_block = block_structure_insert(invalid_block, bbb, &invalid_block_size, &invalid_block_malloced);
        }
        else
        {
            dup_alloc = block_structure_insert(dup_alloc, bbb, &dup_alloc_size, &dup_alloc_malloced);
            for (k = 0; k < size_block_freelist; k++){
                if (strtoul(inode.table[i][13 + 11], NULL, 16) == strtoul(block_freelist[k], NULL, 10)){
                    bbb = malloc(sizeof(struct block_structure));
                    bbb->block = strtoul(inode.table[i][13 + 11], NULL, 16);
                    bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                    bbb->entry = 13;
                    bbb->indirect = 0;
                    bbb->next = NULL;
                    heads = block_structure_insert(heads, bbb, &block_structure_size, &block_structure_allocated_size);
                    break;
                }
            }
            for (j = 0; j < indirect.rows; j++){
                if (blocks_processed >= num_blocks)
                    break;
                if (strcmp(inode.table[i][13 + 11], indirect.table[j][0]) == 0){
                    bbb = malloc(sizeof(struct block_structure));
                    bbb->block = strtoul(indirect.table[j][2], NULL, 16);
                    bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                    bbb->entry = strtoul(indirect.table[j][1], NULL, 16);
                    bbb->indirect = strtoul(indirect.table[j][0], NULL, 16);
                    bbb->next = NULL;
                    if (strtoul(indirect.table[j][2], NULL, 16) == 0 || strtoul(indirect.table[j][2], NULL, 16) > largest_block_number){
                        //if (strtoul(indirect.table[j][2], NULL, 16) > largest_block_number)
                        invalid_block = block_structure_insert(invalid_block, bbb, &invalid_block_size, &invalid_block_malloced);
                    }
                    else{
                        dup_alloc = block_structure_insert(dup_alloc, bbb, &dup_alloc_size, &dup_alloc_malloced);
                    }
                    for (k = 0; k < size_block_freelist; k++){
                        if (strtoul(indirect.table[j][2], NULL, 16) == strtoul(block_freelist[k], NULL, 10)){
                            bbb = malloc(sizeof(struct block_structure));
                            bbb->block = strtoul(indirect.table[j][2], NULL, 16);
                            bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                            bbb->entry = strtoul(indirect.table[j][1], NULL, 16);
                            bbb->indirect = strtoul(indirect.table[j][0], NULL, 16);
                            bbb->next = NULL;
                            heads = block_structure_insert(heads, bbb, &block_structure_size, &block_structure_allocated_size);
                            break;
                        }
                    }
                    for (l = 0; l < indirect.rows; l++){
                        if (blocks_processed >= num_blocks)
                            break;
                        if (strcmp(indirect.table[l][0], indirect.table[j][2]) == 0){
                            bbb = malloc(sizeof(struct block_structure));
                            bbb->block = strtoul(indirect.table[l][2], NULL, 16);
                            bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                            bbb->entry = strtoul(indirect.table[l][1], NULL, 16);
                            bbb->indirect = strtoul(indirect.table[l][0], NULL, 16);
                            bbb->next = NULL;
                            if (strtoul(indirect.table[l][2], NULL, 16) == 0 || strtoul(indirect.table[l][2], NULL, 16) > largest_block_number){
                                //if (strtoul(indirect.table[l][2], NULL, 16) > largest_block_number)
                                invalid_block = block_structure_insert(invalid_block, bbb, &invalid_block_size, &invalid_block_malloced);
                            }
                            else{
                                dup_alloc = block_structure_insert(dup_alloc, bbb, &dup_alloc_size, &dup_alloc_malloced);
                            }
                            for (k = 0; k < size_block_freelist; k++){
                                if (strtoul(indirect.table[l][2], NULL, 16) == strtoul(block_freelist[k], NULL, 10)){
                                    bbb = malloc(sizeof(struct block_structure));
                                    bbb->block = strtoul(indirect.table[l][2], NULL, 16);
                                    bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                                    bbb->entry = strtoul(indirect.table[l][1], NULL, 16);
                                    bbb->indirect = strtoul(indirect.table[l][0], NULL, 16);
                                    bbb->next = NULL;
                                    heads = block_structure_insert(heads, bbb, &block_structure_size, &block_structure_allocated_size);
                                    break;
                                }
                            }
                            blocks_processed += 1;
                        }
                    }
                    blocks_processed += 1;
                }
            }
        }
        blocks_processed += 1;
        
        /*indirect block 14*/
        if (blocks_processed >= num_blocks)
            continue;
        bbb = malloc(sizeof(struct block_structure));
        bbb->block = strtoul(inode.table[i][14 + 11], NULL, 16);
        bbb->inode = strtoul(inode.table[i][0], NULL, 10);
        bbb->entry = 14;
        bbb->indirect = 0;
        bbb->next = NULL;
        if (strtoul(inode.table[i][14 + 11], NULL, 16) == 0 || strtoul(inode.table[i][14 + 11], NULL, 16) > largest_block_number){
            //if (strtoul(inode.table[i][14 + 11], NULL, 16) > largest_block_number)
            invalid_block = block_structure_insert(invalid_block, bbb, &invalid_block_size, &invalid_block_malloced);
        }
        else
        {
            dup_alloc = block_structure_insert(dup_alloc, bbb, &dup_alloc_size, &dup_alloc_malloced);
            for (k = 0; k < size_block_freelist; k++){
                if (strtoul(inode.table[i][14 + 11], NULL, 16) == strtoul(block_freelist[k], NULL, 10)){
                    bbb = malloc(sizeof(struct block_structure));
                    bbb->block = strtoul(inode.table[i][14 + 11], NULL, 16);
                    bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                    bbb->entry = 13;
                    bbb->indirect = 0;
                    bbb->next = NULL;
                    heads = block_structure_insert(heads, bbb, &block_structure_size, &block_structure_allocated_size);
                    break;
                }
            }
            for (j = 0; j < indirect.rows; j++){
                if (blocks_processed >= num_blocks)
                    break;
                if (strcmp(inode.table[i][14 + 11], indirect.table[j][0]) == 0){
                    bbb = malloc(sizeof(struct block_structure));
                    bbb->block = strtoul(indirect.table[j][2], NULL, 16);
                    bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                    bbb->entry = strtoul(indirect.table[j][1], NULL, 16);
                    bbb->indirect = strtoul(indirect.table[j][0], NULL, 16);
                    bbb->next = NULL;
                    if (strtoul(indirect.table[j][2], NULL, 16) == 0 || strtoul(indirect.table[j][2], NULL, 16) > largest_block_number){
                        //if (strtoul(indirect.table[j][2], NULL, 16) > largest_block_number)
                        invalid_block = block_structure_insert(invalid_block, bbb, &invalid_block_size, &invalid_block_malloced);
                    }
                    else{
                        dup_alloc = block_structure_insert(dup_alloc, bbb, &dup_alloc_size, &dup_alloc_malloced);
                    }
                    blocks_processed += 1;
                    for (k = 0; k < size_block_freelist; k++){
                        if (strtoul(indirect.table[j][2], NULL, 16) == strtoul(block_freelist[k], NULL, 10)){
                            bbb = malloc(sizeof(struct block_structure));
                            bbb->block = strtoul(indirect.table[j][2], NULL, 16);
                            bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                            bbb->entry = strtoul(indirect.table[j][1], NULL, 16);
                            bbb->indirect = strtoul(indirect.table[j][0], NULL, 16);
                            bbb->next = NULL;
                            heads = block_structure_insert(heads, bbb, &block_structure_size, &block_structure_allocated_size);
                            break;
                        }
                    }
                    for (l = 0; l < indirect.rows; l++){
                        if (blocks_processed >= num_blocks)
                            break;
                        if (strcmp(indirect.table[l][0], indirect.table[j][2]) == 0){
                            bbb = malloc(sizeof(struct block_structure));
                            bbb->block = strtoul(indirect.table[l][2], NULL, 16);
                            bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                            bbb->entry = strtoul(indirect.table[l][1], NULL, 16);
                            bbb->indirect = strtoul(indirect.table[l][0], NULL, 16);
                            bbb->next = NULL;
                            if (strtoul(indirect.table[l][2], NULL, 16) == 0 || strtoul(indirect.table[l][2], NULL, 16) > largest_block_number){
                                // if (strtoul(indirect.table[l][2], NULL, 16) > largest_block_number)
                                invalid_block = block_structure_insert(invalid_block, bbb, &invalid_block_size, &invalid_block_malloced);
                            }
                            else{
                                dup_alloc = block_structure_insert(dup_alloc, bbb, &dup_alloc_size, &dup_alloc_malloced);
                            }
                            for (k = 0; k < size_block_freelist; k++){
                                if (strtoul(indirect.table[l][2], NULL, 16) == strtoul(block_freelist[k], NULL, 10)){
                                    bbb = malloc(sizeof(struct block_structure));
                                    bbb->block = strtoul(indirect.table[l][2], NULL, 16);
                                    bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                                    bbb->entry = strtoul(indirect.table[l][1], NULL, 16);
                                    bbb->indirect = strtoul(indirect.table[l][0], NULL, 16);
                                    bbb->next = NULL;
                                    heads = block_structure_insert(heads, bbb, &block_structure_size, &block_structure_allocated_size);
                                    break;
                                }
                            }
                            for (m = 0; m < indirect.rows; l++){
                                if (blocks_processed >= num_blocks)
                                    break;
                                if (strcmp(indirect.table[m][0], indirect.table[l][2]) == 0){
                                    bbb = malloc(sizeof(struct block_structure));
                                    bbb->block = strtoul(indirect.table[m][2], NULL, 16);
                                    bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                                    bbb->entry = strtoul(indirect.table[m][1], NULL, 16);
                                    bbb->indirect = strtoul(indirect.table[m][0], NULL, 16);
                                    bbb->next = NULL;
                                    if (strtoul(indirect.table[m][2], NULL, 16) == 0 || strtoul(indirect.table[m][2], NULL, 16) > largest_block_number){
                                        //if (strtoul(indirect.table[m][2], NULL, 16) > largest_block_number)
                                        invalid_block = block_structure_insert(invalid_block, bbb, &invalid_block_size, &invalid_block_malloced);
                                    }
                                    else{
                                        dup_alloc = block_structure_insert(dup_alloc, bbb, &dup_alloc_size, &dup_alloc_malloced);
                                    }
                                    for (k = 0; k < size_block_freelist; k++){
                                        if (strtoul(indirect.table[m][2], NULL, 16) == strtoul(block_freelist[k], NULL, 10)){
                                            bbb = malloc(sizeof(struct block_structure));
                                            bbb->block = strtoul(indirect.table[m][2], NULL, 16);
                                            bbb->inode = strtoul(inode.table[i][0], NULL, 10);
                                            bbb->entry = strtoul(indirect.table[m][1], NULL, 16);
                                            bbb->indirect = strtoul(indirect.table[m][0], NULL, 16);
                                            bbb->next = NULL;
                                            heads = block_structure_insert(heads, bbb, &block_structure_size, &block_structure_allocated_size);
                                            break;
                                        }
                                    }
                                    blocks_processed += 1;
                                }
                            }
                            blocks_processed += 1;
                        }
                    }
                    blocks_processed += 1;
                }
            }
        }
        blocks_processed += 1;
    }
    
    /*1. output unallocated block*/
    for (i = 0; i < block_structure_size; i++){
        struct block_structure* ptr = heads[i];
        dprintf(fd, "UNALLOCATED BLOCK < %lu > REFERENCED BY", ptr->block);
        do
        {
            if (ptr->indirect == 0){
                dprintf(fd, " INODE < %lu > ENTRY < %lu >", ptr->inode, ptr->entry);
            }
            else{
                dprintf(fd, " INODE < %lu > INDIRECT BLOCK < %lu > ENTRY < %lu >", ptr->inode, ptr->indirect, ptr->entry);
            }
            ptr = ptr->next;
        } while (ptr!=NULL);
        dprintf(fd, "\n");
    }
    
    /*2. output duplicately allocated block*/
    for (i = 0; i < dup_alloc_size; i++){
        struct block_structure* ptr = dup_alloc[i];
        if (ptr->next != NULL){
            dprintf(fd, "MULTIPLY REFERENCED BLOCK < %lu > BY", ptr->block);
            do
            {
                if (ptr->indirect == 0){
                    dprintf(fd, " INODE < %lu > ENTRY < %lu >", ptr->inode, ptr->entry);
                }
                else{
                    dprintf(fd, " INODE < %lu > INDIRECT BLOCK < %lu > ENTRY < %lu >", ptr->inode, ptr->indirect, ptr->entry);
                }
                ptr = ptr->next;
            } while (ptr != NULL);
            dprintf(fd, "\n");
        }
    }
    
    /*7. output invalid block pointer*/
    
    for (i = 0; i < invalid_block_size; i++){
        struct block_structure* ptr = invalid_block[i];
        dprintf(fd, "INVALID BLOCK < %lu > IN", ptr->block);
        do
        {
            if (ptr->indirect == 0){
                dprintf(fd, " INODE < %lu > ENTRY < %lu >", ptr->inode, ptr->entry);
            }
            else{
                dprintf(fd, " INODE < %lu > INDIRECT BLOCK < %lu > ENTRY < %lu >", ptr->inode, ptr->indirect, ptr->entry);
            }
            ptr = ptr->next;
        } while (ptr != NULL);
        dprintf(fd, "\n");
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*3. unallocated inode*/
    int f,found2 = 0,found = 0,b;
    int array_size = 64,num_unallocated_inodes = 0;
    char** unallocated_inodes = (char**)malloc(array_size*sizeof(char*));
    
    
    
    found = 0;
    for (i = 0; i < directory.rows; i++) {
        for (j = 0; j < inode.rows; j++) {
            if (strcmp(directory.table[i][4], inode.table[j][0]) == 0)
            {
                found = 1;
                break;
            }
        }
        if (!found) {
            for (f = 0; f < num_unallocated_inodes; f++) {
                if (strcmp(directory.table[i][4], unallocated_inodes[f]) == 0)
                {
                    found2 = 1;
                    break;
                }
            }
            if (!found2) {
                unallocated_inodes[num_unallocated_inodes] = (char*)malloc(strlen(directory.table[i][4])*sizeof(char));
                strcpy(unallocated_inodes[num_unallocated_inodes], directory.table[i][4]);
                num_unallocated_inodes++;
                dprintf(fd,"UNALLOCATED INODE < %s > REFERENCED BY DIRECTORY < %s > ENTRY < %s >", directory.table[i][4],directory.table[i][0],directory.table[i][1]);
                for (b = i + 1; b < directory.rows; b++) {
                    if (strcmp(directory.table[i][4], directory.table[b][4]) == 0)
                    {
                        dprintf(fd," DIRECTORY < %s > ENTRY < %s >", directory.table[b][0],directory.table[b][1]);
                    }
                }
                found2 = 0;
            }
            dprintf(fd,"\n");
        }
        found = 0;
    }
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*4. missing inode*/
    for (i = 11; i < strtoul(super.table[0][1], NULL, 10) + 1; i++) {
        int in_freelist = 1, in_allocatedlist = 1;
        for (j = 0;  j < size_inode_freelist; j++) {
            if ((unsigned long int)i == strtoul(inode_freelist[j],NULL,10)) {
                in_freelist = 0;
                break;
            }
        }
        for (b = 0; b < directory.rows; b++) {
            if ((unsigned long int)i == strtoul(directory.table[b][4],NULL,10)) {
                in_allocatedlist = 0;
                break;
            }
        }
        if (in_allocatedlist && in_freelist) {
            dprintf(fd,"MISSING INODE < %d > SHOULD BE IN FREE LIST",i);
            long int aa = strtoul(super.table[0][5], NULL, 10);
            int which_freelist = 0;
            while (i - aa > 0) {
                which_freelist++;
                i -= aa;
            }
            dprintf(fd," < %s >\n",group.table[which_freelist][4]);
        }
    }
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*5. incorrect link count*/
    int count = 0;
    struct link_count_dir* link_count = calloc(inode.rows, sizeof(struct link_count_dir));
    for (i = 0; i < inode.rows; i++) {
        link_count[i].inode_number = strtoul(inode.table[i][0], NULL, 10);
        link_count[i].link_count = strtoul(inode.table[i][5], NULL, 10);
    }
    for (i = 0; i < inode.rows; i++){
        for (j = 0; j < directory.rows; j++) {
            if (link_count[i].inode_number == strtoul(directory.table[j][4], NULL, 10))
                count++;
        }
        if (count != link_count[i].link_count) {
            dprintf(fd,"LINKCOUNT < %ld > IS < %ld > SHOULD BE < %d >\n",link_count[i].inode_number, link_count[i].link_count,count);
        }
        count = 0;
    }
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*6. incorrect directory entry*/
    found = 0;
    for (i = 0; i < directory.rows; i++) {
        if (strcmp(directory.table[i][5], "\".\"") == 0) {
            if (strcmp(directory.table[i][0], directory.table[i][4]) != 0) {
                dprintf(fd,"INCORRECT ENTRY IN < %s > NAME < . > LINK TO < %s > SHOULD BE < %s >\n", directory.table[i][0], directory.table[i][4], directory.table[i][0]);
            }
        }
        else if (strcmp(directory.table[i][5], "\"..\"") == 0){
            if (strcmp(directory.table[i][0], "2") == 0 &&  strcmp(directory.table[i][4], "2") != 0 ) {
                dprintf(fd,"INCORRECT ENTRY IN < %s > NAME < .. > LINK TO < %s > SHOULD BE < %s >\n", directory.table[i][0], directory.table[i][4], directory.table[i][0]);
                continue;
            }
            if (strcmp(directory.table[i][0], "2") == 0 &&  strcmp(directory.table[i][4], "2") == 0 )
            {
                continue;
            }
            if (strcmp(directory.table[i][0], "2") != 0)
            {
                for (j = 0; j < directory.rows; j++) {
                    if (strcmp(directory.table[i][4], directory.table[j][0]) == 0 && (strcmp(directory.table[j][5], "\".\"") != 0 && strcmp(directory.table[j][5], "\"..\"") != 0)) {
                        if (strcmp(directory.table[i][0], directory.table[j][4]) == 0) {
                            //printf("haha\n");
                            found = 1;
                            break;
                        }
                    }
                }
                if (!found) {
                    //printf("haha1\n");
                    for (b = 0; b < directory.rows; b++) {
                        if (strcmp(directory.table[i][0], directory.table[b][4]) == 0 && (strcmp(directory.table[b][5], "\".\"") != 0 && strcmp(directory.table[b][5], "\"..\"") != 0)) {
                            dprintf(fd,"INCORRECT ENTRY IN < %s > NAME < .. > LINK TO < %s > SHOULD BE < %s >\n", directory.table[i][0], directory.table[i][4], directory.table[b][0]);
                        }
                    }
                }
                
            }
            found = 0;
        }
    }
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*free memories*/
    free_csv(&super);
    free_csv(&group);
    free_csv(&bitmap);
    free_csv(&inode);
    free_csv(&directory);
    free_csv(&indirect);
    heads = block_structure_free(heads, &block_structure_size, &block_structure_allocated_size);
    dup_alloc = block_structure_free(dup_alloc, &dup_alloc_size, &dup_alloc_malloced);
    invalid_block = block_structure_free(invalid_block, &invalid_block_size, &invalid_block_malloced);
    /*close files*/
    close(fd);
    fclose(super_csv);
    fclose(group_csv);
    fclose(bitmap_csv);
    fclose(inode_csv);
    fclose(directory_csv);
    fclose(indirect_csv);
    
    exit(EXIT_SUCCESS);
}