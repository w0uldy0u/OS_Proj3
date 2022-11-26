#pragma once

int min(int refstr_size, int frame_num, int time, int *refstr, int **mem_state);
int lru(int refstr_size, int frame_num, int time, int *refstr, int **mem_state);
int lfu(int refstr_size, int frame_num, int time, int *refstr, int **mem_state);
int ws(int refstr_size, int window_size, int page_num, int time, int *refstr, int **mem_state);
void print_result(int refstr_size, int frame_num, int *refstr, int *fault, int *P, int *Q, int **mem_state);