#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "pgrplc.h"

void execute(int page_num, int frame_num, int refstr_size, int *refstr, int method);
int **create_arr(int frame_num, int refstr_size);
int isempty(int **mem_state, int frame_num, int time);
int ishit(int **mem_state, int frame_num, int time, int ref);

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

    /* Execute each method */
    for(int i = 0; i < 3; i++)
        execute(page_num, frame_num, refstr_size, refstr, i);
    
    //execute_ws();

    free(refstr);
    return 0;
}

void execute(int page_num, int frame_num, int refstr_size, int *refstr, int method)
{
    int **mem_state = create_arr(frame_num, refstr_size);
    int *fault = (int *)calloc(refstr_size, sizeof(int));

    mem_state[0][0] = refstr[0];
    fault[0] = 1;

    for(int time = 1; time < refstr_size; time++)
    {
        int empty;

        for(int i = 0; i < frame_num; i++)
            mem_state[i][time] = mem_state[i][time - 1];
        
        if(ishit(mem_state, frame_num, time, refstr[time])) //hit
            continue;
        
        empty = isempty(mem_state, frame_num, time);
        if(empty == -1) //need to replace
        {
            int replace;
            /* Replacement method */
            if(method == 0)
                replace = min(refstr_size, frame_num, time, refstr, mem_state);

            else if(method == 1)
                replace = lru(refstr_size, frame_num, time, refstr, mem_state);
            
            else
                replace = lfu(refstr_size, frame_num, time, refstr, mem_state);

            mem_state[replace][time] = refstr[time];
            fault[time] = 1;
        }

        else    //frame not full
        {
            mem_state[empty][time] = refstr[time];
            fault[time] = 1;
        }
    }

    /* Print Result */
    if(method == 0)
        printf("MIN\n");
    else if(method == 1)
        printf("\nLRU\n");
    else
        printf("\nLFU\n");

    int pgfault_num = 0;
    for(int i = 0; i < refstr_size; i++)
    {
        if(fault[i] == 1)
            pgfault_num++;
    }

    printf("(Number of Page Faults: %d)\n", pgfault_num);
    print_result(refstr_size, frame_num, refstr, fault, mem_state);
    
    /* Free memory */
    for(int i = 0; i < frame_num; i++)
        free(mem_state[i]);
    free(mem_state);
    free(fault);

    return;
}

int **create_arr(int frame_num, int refstr_size)
{
    int **mem_state = (int **)malloc(sizeof(int *) * frame_num);
    for(int i = 0; i < frame_num; i++)
        mem_state[i] = (int *)malloc(sizeof(int) * refstr_size);
    
    /* Initialize */
    for(int i = 0; i < frame_num; i++)
        for(int j = 0; j < refstr_size; j++)
            mem_state[i][j] = -1;

    return mem_state;
}

int isempty(int **mem_state, int frame_num, int time)
{
    for(int i = 0; i < frame_num; i++)
    {
        if(mem_state[i][time] == -1)
            return i;
    }

    return -1;
}

int ishit(int **mem_state, int frame_num, int time, int ref)
{
    for(int i = 0; i < frame_num; i++)
    {
        if(mem_state[i][time] == ref)
            return 1;
    }

    return 0;
}