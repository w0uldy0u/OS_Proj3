<div align="center"><h1>운영체제 [SWE3004_41]</h1>
<h3>PROJECT 3</h3>
<h4> Virtual Memory Management 기법 구현</h4></div>


<br>

<br>



<br>

<br>



## 실행 환경 및 실행 방법

인의예지 서버(Ubuntu 18.04.5 LTS)에서 개발 및 테스트했고 C언어로 구현하였습니다. 
Make 명령어로 컴파일한 뒤 argument 없이 $*./proj3*으로 실행시킬 수 있습니다. 
input.txt를 실행파일과 동일한 디렉토리에 위치시켜야 합니다.

input 형식은 다음과 같습니다
```
N M W K
r1 r2 r3 ··· rk
```

- N은 process가 갖는 page 개수
- M은 할당 page frame 개수
- W는 window size
- K는 page reference string 길이
- "r1 r2 r3 ··· rk"는 page reference string
	- Page 번호는 0번부터 시작



## 제출물 내 각 파일

- main.c
	- input.txt를 읽고 각각의 기법을 실행하는 소스코드입니다.
- pgrplc.c
	- 각각의 management 기법을 구현한 소스코드입니다.
- pgrplc.h
- Makefile
- input.txt



## 설계 · 구현 내용

Project 3는 Virtual Memory Management 기법을 구현하는 것이 목표입니다. 

구현하는 기법은 다음과 같습니다.

- MIN
- LRU
- LFU
- WS



### MIN

현재 시각을 기준으로 가장 먼 미래까지 reference되지 않을 page를 선택합니다.

본 프로젝트에서는 tie-breaking으로 가장 page 번호가 낮은 page를 선택합니다.



### LRU(Least Recently Used)

현재 시각을 기준으로 가장 오랫동안 reference되지 않은 page를 선택합니다.



### LFU(Least Frequently Used)

현재 시각까지 가장 reference된 횟수가 적은 page를 선택합니다.

Tie-breaking으로 LRU 기법을 사용합니다.



### WS(Working Set)

현재 시각까지 window size만큼의 시간 동안에서 사용된 page들을 메모리에 유지합니다.

Variable Allocation 기법의 일종이므로 메모리에 load된 page의 개수는 달라질 수 있습니다.



## Variable

```c
int page_num //프로세스가 갖는 page 개수를 저장하는 변수입니다
int frame_num //메모리에 있는 page frame의 개수를 저장하는 변수입니다
int window_size //Working Set 기법에서 필요한 window size를 저장하는 변수입니다
int refstr_size //Page reference string의 길이를 저장하는 변수입니다
```

```c
int *refstr //Page reference string을 저장하는 배열입니다
int *fault //언제 page fault가 발생했는지 저장하는 배열입니다(Page fault가 발생하면 1, 아니면 0)
  
int *ref_count //LFU 기법에서 각각의 page가 몇 번 reference됐는지 저장하는 배열입니다
int *rcnt_ref //LFU 기법에서 tie-breaking을 위해 각각의 page가 언제 가장 최근에 reference됐는지 저장하는 배열입니다

int *P //Working Set에 들어가는 page 번호를 저장하는 배열입니다
int *Q //Working Set에서 빠지는 page 번호를 저장하는 배열입니다
int *working_set //Working Set에 들어간 page 번호를 저장하는 배열입니다
```

```c
int **mem_state //Memory State를 시간과 frame number에 따라 2차원으로 저장하는 배열입니다
```



## Function

### main.c

```c
void execute(int frame_num, int refstr_size, int *refstr, int method);
```

- <u>method</u>에 따라 해당 값이 0이면 MIN, 1이면 LRU, 2이면 LFU를 실행하는 함수입니다.

- 현재 reference하려는 page가 메모리에 있는지 확인합니다.
  - 있다면 memory state를 변경하지 않습니다.
  - 없다면 비어있는 page frame이 있는지 확인합니다.
    - 있다면 비어있는 곳에 reference하려는 page를 넣습니다.
    - 없다면 <u>method</u>의 기법을 통해 특정 page를 replace합니다.

- 실행결과를 출력합니다.



```c
void execute_ws(int page_num, int window_size, int refstr_size, int *refstr);
```

- Working Set 기법을 실행하는 함수입니다.
- 현재 reference하려는 page가 메모리에 없다면 추가합니다.
- Working Set 기법을 통해 메모리에서 제거해야할 page를 제거합니다.
- 실행결과를 출력합니다.



```c
void check_input(int page_num, int frame_num, int window_size, int refstr_size, int *refstr);
```

- input.txt를 통해 받은 input이 유효한지 검사하는 함수입니다.

- Input이 유효하지 않을 경우 에러메시지를 출력하고 프로그램을 종료합니다.



```c
int **create_arr(int frame_num, int refstr_size);
```

- Memory state를 나타내는 2차원 배열을 만들고 초기화하는 함수입니다.
- 모든 값을 -1로 초기화합니다.
- 2차원 배열의 포인터를 반환합니다.



```c
int isempty(int **mem_state, int frame_num, int time);
```

- 현재 비어있는 page frame이 있는지 확인하는 함수입니다.
- 비어있는 frame이 있으면 해당 frame number를, 없으면 -1을 반환합니다.



```c
int ishit(int **mem_state, int frame_num, int time, int ref);
```

- 현재 reference하려는 page가 메모리에 있는지 확인하는 함수입니다.
- 메모리에 있으면 1을, 없으면 0을 반환합니다.



### pgrplc.c

```c
int min(int refstr_size, int frame_num, int time, int *refstr, int **mem_state);
```

- MIN 기법을 구현한 함수입니다.
- Replace할 page number를 반환합니다.



```c
int lru(int refstr_size, int frame_num, int time, int *refstr, int **mem_state);
```

- LRU 기법을 구현한 함수입니다.
- Replace할 page number를 반환합니다.



```c
int lfu(int refstr_size, int frame_num, int time, int *refstr, int **mem_state);
```

- LFU 기법을 구현한 함수입니다.
- Replace할 page number를 반환합니다.



```c
int ws(int refstr_size, int window_size, int page_num, int time, int *refstr, int **mem_state);
```

- Working Set 기법을 구현한 함수입니다.
- Replace할 page가 있다면 해당 page의 page number를, 없다면 -1을 반환합니다.



```c
void print_result(int refstr_size, int frame_num, int *refstr, int *fault, int *P, int *Q, int **mem_state)
```

- 각 기법의 실행 결과를 출력하는 함수입니다.
- <u>P</u>가 NULL일 경우 P, Q는 출력하지 않습니다.
- P, Q는 Working Set 기법에서만 출력합니다.



## 실행결과

### Input

![스크린샷 2022-11-29 오전 3.52.24](/Users/space/Library/Application Support/typora-user-images/스크린샷 2022-11-29 오전 3.52.24.png)



### Output

![스크린샷 2022-11-29 오전 3.53.21](/Users/space/Library/Application Support/typora-user-images/스크린샷 2022-11-29 오전 3.53.21.png)

### Input

![스크린샷 2022-11-29 오전 4.03.33](/Users/space/Downloads/스크린샷 2022-11-29 오전 4.03.33.png)

### Output

![스크린샷 2022-11-29 오전 4.04.37](/Users/space/Library/Application Support/typora-user-images/스크린샷 2022-11-29 오전 4.04.37.png)

### Input

![스크린샷 2022-11-29 오전 4.20.21](/Users/space/Library/Application Support/typora-user-images/스크린샷 2022-11-29 오전 4.20.21.png)

### Output

![스크린샷 2022-11-29 오전 4.20.53](/Users/space/Library/Application Support/typora-user-images/스크린샷 2022-11-29 오전 4.20.53.png)