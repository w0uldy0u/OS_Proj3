#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(void)
{
    /* Get Input */
    int page_num, frame_num, window_size, refstr_size;

    FILE *fptr;
    fptr = fopen("input.txt","r");
    fscanf(fptr, "%d %d %d %d", &page_num, &frame_num, &window_size, &refstr_size);

    int *refstr = (int *)malloc(sizeof(int) * refstr_size);

    for(int i = 0; i < refstr_size; i++)
        fscanf(fptr, "%d", &refstr[i]);
    
    fclose(fptr);
}