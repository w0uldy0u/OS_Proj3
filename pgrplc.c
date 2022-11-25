#include "pgrplc.h"
#include <stdio.h>
#include <stdlib.h>

int min(int refstr_size, int frame_num, int time, int *refstr, int **mem_state)
{
    int max = -1, maxidx;
    for(int i = 0; i < frame_num; i++)
    {
        for(int j = time + 1; j < refstr_size; j++)
        {
            if(refstr[j] == mem_state[i][time])
            {
                if((j - time) > max)
                {
                    max = j - time;
                    maxidx = i;
                }
                break;
            }

            else if(j == (refstr_size - 1))
                return i;
        }
    }
    return maxidx;
}

int lru(int refstr_size, int frame_num, int time, int *refstr, int **mem_state)
{
    int max = -1, maxidx;
    for(int i = 0; i < frame_num; i++)
    {
        for(int j = time - 1; j >= 0; j--)
        {
            if(refstr[j] == mem_state[i][time])
            {
                if((time - j) > max)
                {
                    max = time - j;
                    maxidx = i;
                }
                break;
            }
        }
    }
    return maxidx;
}

int lfu(int refstr_size, int frame_num, int time, int *refstr, int **mem_state)
{
    int *ref_count = (int *)calloc(sizeof(int), frame_num);
    int *rcnt_ref = (int *)calloc(sizeof(int), frame_num);

    for(int i = 0; i < frame_num; i++)
    {
        for(int j = time - 1; j >= 0; j--)
        {
            if(refstr[j] == mem_state[i][time])
            {
                if(rcnt_ref[i] == 0)
                    rcnt_ref[i] = time - j;

                ref_count[i]++;
            }
        }
    }

    int min = 1000, minidx;
    for(int i = 0; i < frame_num; i++)
    {
        if(ref_count[i] < min)
        {
            min = ref_count[i];
            minidx = i;
        }
    }

    int tie_count = 0;
    int *tie = (int *)calloc(sizeof(int), frame_num);
    for(int i = 0; i < frame_num; i++)
    {
        if(ref_count[i] == min)
        {
            tie[i] = 1;
            tie_count++;
        }
    }

    if(tie_count != 1)
    {
        int max = -1, maxidx;
        for(int i = 0; i < frame_num; i++)
        {
            if(tie[i] == 1 && rcnt_ref[i] > max)
            {
                max = rcnt_ref[i];
                maxidx = i;
            }
        }
        free(ref_count);
        free(rcnt_ref);
        free(tie);

        return maxidx;
    }

    free(ref_count);
    free(rcnt_ref);
    free(tie);

    return minidx;
}

int ws(int refstr_size, int window_size, int time, int *refstr, int **mem_state)
{
    
}

void print_result(int refstr_size, int frame_num, int *refstr, int *fault, int **mem_state)
{
    for(int i = 0; i < refstr_size; i++)
        printf("=====");
    printf("\n");

    printf("%-12s", "Time");
    for(int i = 0; i < refstr_size; i++)
        printf(" %2d ", i + 1);
    printf("\n");

    for(int i = 0; i < refstr_size; i++)
        printf("-----");
    printf("\n");

    printf("%-12s", "Ref. String");
    for(int i = 0; i < refstr_size; i++)
        printf(" %2d ", refstr[i]);
    printf("\n");

    for(int i = 0; i < refstr_size; i++)
        printf("-----");
    printf("\n");

    printf("%-12s", "Mem. State");
    for(int i = 0; i < refstr_size; i++)
    {
        if(mem_state[0][i] == -1)
            printf(" %2s ", "-");
        
        else
            printf(" %2d ", mem_state[0][i]);
    }
    printf("\n");

    for(int i = 1; i < frame_num; i++)
    {
        printf("%-12s","");
        for(int j = 0; j < refstr_size; j++)
        {
            if(mem_state[i][j] == -1)
                printf(" %2s ", "-");
        
            else
                printf(" %2d ", mem_state[i][j]);
        }
        printf("\n");
    }

    for(int i = 0; i < refstr_size; i++)
        printf("-----");
    printf("\n");

    printf("%-12s", "Fault");
    for(int i = 0; i < refstr_size; i++)
    {
        if(fault[i] == 1)
            printf(" %2s ", "F");
        else
            printf(" %2s ", "-");
    }
    printf("\n");
    
    for(int i = 0; i < refstr_size; i++)
        printf("=====");
    printf("\n");
}