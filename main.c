#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "pgrplc.h"

void execute(int frame_num, int refstr_size, int *refstr, int method);
void execute_ws(int page_num, int window_size, int refstr_size, int *refstr);
void check_input(int page_num, int frame_num, int window_size, int refstr_size, int *refstr);
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

    check_input(page_num, frame_num, window_size, refstr_size, refstr);

    /* Execute each method */
    for(int i = 0; i < 3; i++)
        execute(frame_num, refstr_size, refstr, i);
    
    execute_ws(page_num, window_size, refstr_size, refstr);

    free(refstr);
    return 0;
}

void execute(int frame_num, int refstr_size, int *refstr, int method)
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
    print_result(refstr_size, frame_num, refstr, fault, NULL, NULL, mem_state);
    
    /* Free memory */
    for(int i = 0; i < frame_num; i++)
        free(mem_state[i]);
    free(mem_state);
    free(fault);

    return;
}

void execute_ws(int page_num, int window_size, int refstr_size, int *refstr)
{
    int **mem_state = create_arr(page_num, refstr_size);
    int *fault = (int *)calloc(refstr_size, sizeof(int));
    int *P = (int *)malloc(refstr_size * sizeof(int));
    int *Q = (int *)malloc(refstr_size * sizeof(int));

    /* Initialize */
    for(int i = 0; i < refstr_size; i++)
    {
        P[i] = -1;
        Q[i] = -1;
    }

    for(int time = 0; time < window_size; time++)
    {
        if(time != 0)
        {
            for(int i = 0; i < page_num; i++)
               mem_state[i][time] = mem_state[i][time - 1];
        }
        mem_state[refstr[time]][time] = refstr[time];
        P[time] = refstr[time];
        fault[time] = 1;
    }

    for(int time = window_size; time < refstr_size; time++)
    {
        for(int i = 0; i < page_num; i++)
           mem_state[i][time] = mem_state[i][time - 1];

        Q[time] = ws(refstr_size, window_size, page_num, time, refstr, mem_state);

        if(!ishit(mem_state, page_num, time, refstr[time]))
        {
            P[time] = refstr[time];
            mem_state[refstr[time]][time] = refstr[time];
            fault[time] = 1;
        }

        if(Q[time] == refstr[time])
            Q[time] = -1;

        if(Q[time] != -1)
            mem_state[Q[time]][time] = -1;
    }

    printf("\nWorking Set\n");

    int pgfault_num = 0;
    for(int i = 0; i < refstr_size; i++)
    {
        if(fault[i] == 1)
            pgfault_num++;
    }

    printf("(Number of Page Faults: %d)\n", pgfault_num);
    print_result(refstr_size, page_num, refstr, fault, P, Q, mem_state);
    
    /* Free memory */
    for(int i = 0; i < page_num; i++)
        free(mem_state[i]);
    free(mem_state);
    free(fault);
    free(P);
    free(Q);

    return;
}

void check_input(int page_num, int frame_num, int window_size, int refstr_size, int *refstr)
{
    if(page_num > 100 || frame_num > 20 || window_size > 100 || 
    refstr_size > 1000)
    {
        fprintf(stderr, "Input is Not Valid\n");
        exit(1);
    }
    
    for(int i = 0; i < refstr_size; i++)
    {
        if(refstr[i] >= page_num || refstr[i] < 0)
        {
            fprintf(stderr, "Input is Not Valid\n");
            exit(1);
        }
    }

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