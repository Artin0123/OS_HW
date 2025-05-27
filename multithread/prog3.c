/*
gcc prog3.c -pthread -o prog3
./prog3 24 12
*/
#include <errno.h>     /* Errors */
#include <pthread.h>   /* POSIX Threads */
#include <stdio.h>     /* Input/Output */
#include <stdlib.h>    /* General Utilities */
#include <string.h>    /* String handling */
#include <sys/types.h> /* Primitive System Data Types */
#include <time.h>      // clock 函數所需之標頭檔
#include <unistd.h>    /* Symbolic Constants */

/* prototype for thread routine */
void print_message_function(void* ptr);

/* struct to hold data to be passed to a thread
   this shows how multiple data items can be passed to a thread */
typedef struct str_thdata {
  int thread_no;
  int input;
} thdata;

int num[50] = {0};

int main(int argc, char* argv[]) {
  // 儲存時間用的變數
  clock_t start, end;
  double cpu_time_used;

  // 計算開始時間
  start = clock();

  // 主要計算
  int ai = 0;
  pthread_t thread[5]; /* thread variables */
  thdata data[5];      /* structs to be passed to threads */
  for (int i = 1; i < argc; i++) {
    ai = atoi(argv[i]);
    data[i - 1].thread_no = i;
    data[i - 1].input = ai;
    printf("[Main thread] worker thread %d calculates P(%d)\n", i, ai);
  }

  /* create threads 1 and 2 */
  for (int i = 1; i < argc; i++) {
    pthread_create(&thread[i - 1], NULL, (void*)&print_message_function,
                   (void*)&data[i - 1]);
  }

  /* Main block now waits for both threads to terminate, before it exits
     If main block exits, both threads exit, even if the threads have not
     finished their work */
  for (int i = 1; i < argc; i++) {
    pthread_join(thread[i - 1], NULL);
  }

  printf("[Main thread] the results are\n");
  for (int i = 1; i < argc; i++) {
    printf("P(%d)=%d\n", data[i - 1].input, num[data[i - 1].input]);
  }

  // 計算結束時間
  end = clock();

  // 計算實際花費時間
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC * 1000;
  // 輸出實際花費時間
  printf("[Main thread] %.3lfms Bye.\n", cpu_time_used);

  /* exit */
  exit(0);
}

/**
 * print_message_function is used as the start routine for the threads used
 * it accepts a void pointer
 **/
void print_message_function(void* ptr) {
  thdata* data;
  data = (thdata*)ptr; /* type cast to a pointer to thdata */

  if (data->input <= 50 && data->input >= 0) {
    // P(n)=P(n-2)+P(n-3)
    // P(3)=P(1)+P(0)=1+1=2
    // P(4)=P(2)+P(1)=1+1=2
    // P(5)=P(3)+P(2)=2+1=3
    // P(6)=P(4)+P(3)=2+2=4
    // P(7)=P(5)+P(4)=3+2=5
    int n[50] = {0};
    n[0] = 1, n[1] = 1, n[2] = 1;
    for (int i = 3; i <= data->input; i++) {
      n[i] = n[i - 2] + n[i - 3];
    }
    int count = 0;
    for (int i = n[data->input]; i > 0; i = i / 10) {
      count++;
    }
    num[data->input] = n[data->input];
    printf("[Worker %d] P(%d)=%d, %d digits\n", data->thread_no, data->input,
           n[data->input], count);
  } else {
    printf("[Worker %d] P(%d) is out of range.\n", data->thread_no,
           data->input);
  }

  pthread_exit(0); /* exit */
}
