/*
gcc -o player1 player1.c -lrt
gcc -o player2 player2.c -lrt
./player1 2
./player2 5
*/
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// .
// 先執行player1再執行player2
// 代表player1的data[0](=p)傳給player2的q做比較
// 然而題目要求是player2的q傳給player1比對
// 因此先進入player2的while輸出guess(=q)傳給player1
// player1再輸出評判結果(p vs data[0])並將大小的三種情況(data[1])傳給player2
// player2執行while判斷data[1]的三種情況
// 假設一開始player1的p=84，player2的q=42
// player2輸出player1: Miss. Too low.，並將min=43，max=100，q=72傳給player1
// player1的data[0]=72，p(=84) > data[0](=72)，因此data[1]=1
// 之後player2輸出player1: Miss. Too low.，min=73，max=100，q=87
// 之後min=73，max=86，q=80
// 之後min=81，max=86，q=84，結束程序
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h> /* 亂數相關函數 */
#include <sys/mman.h>
#include <time.h> /* 時間相關函數 */
#include <unistd.h>
#ifndef PROTOCOL_H
#define PROTOCOL_H
#endif /* PROTOCOL_H */
#define NAME "/shmem-example"
#define NUM 3
#define SIZE (NUM * sizeof(int))
int main(int argc, char* argv[]) {
  int seedm = atoi(argv[1]);
  if (seedm > 0 && seedm < 100) {
    srand(seedm);
    /* 指定亂數範圍 */
    int min = 1;
    int max = 100;
    /* 產生 [min , max] 的整數亂數 */
    int p = rand() % (max - min + 1) + min;
    printf("Answer: %d\n", p);

    int fd = shm_open(NAME, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
      perror("shm_open()");
      return EXIT_FAILURE;
    }
    ftruncate(fd, SIZE);
    int* data = (int*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    data[1] = 0;

    while (1) {
      if (p > data[0]) {
        data[1] = 1;
      }
      if (p < data[0]) {
        data[1] = 2;
      }
      if (p == data[0]) {
        data[1] = 3;
        break;
      }
    }
    munmap(data, SIZE);
    close(fd);
    return EXIT_SUCCESS;
  } else {
    printf("seed must be between 1 and 100\n");
  }
}