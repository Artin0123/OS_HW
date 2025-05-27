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
  int seedn = atoi(argv[1]);
  if (seedn > 0 && seedn < 100) {
    srand(seedn);
    /* 指定亂數範圍 */
    int min = 1;
    int max = 100;
    /* 產生 [min , max] 的整數亂數 */
    int q = rand() % (max - min + 1) + min;
    int fd = shm_open(NAME, O_RDWR, 0600);
    if (fd < 0) {
      perror("shm_open()");
      return EXIT_FAILURE;
    }
    int* data = (int*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    data[0] = q;
    data[1] = 0;

    int count = 0;
    while (1) {
      if (data[1] == 1) {
        printf("player2: Guess: %d\n", q);
        printf("player1: Miss. Too low.\n");
        min = q + 1;
        if ((q + 1 + max) % 2 == 1) {
          q = (q + 2 + max) / 2;
        } else {
          q = (q + 1 + max) / 2;
        }
        data[0] = q;
        count++;
        data[1] = 0;
      }
      if (data[1] == 2) {
        printf("player2: Guess: %d\n", q);
        printf("player1: Miss. Too high.\n");
        max = q - 1;
        if ((q - 1 + min) % 2 == 1) {
          q = (q + min) / 2;
        } else {
          q = (q - 1 + min) / 2;
        }
        data[0] = q;
        count++;
        data[1] = 0;
      }
      if (data[1] == 3) {
        printf("player2: Guess: %d\n", q);
        count++;
        printf("Bingo! It took you %d turns!\n", count);
        printf("player2: Bye!\n");
        printf("player1: See you!\n");
        break;
      }
    }
    munmap(data, SIZE);
    close(fd);
    shm_unlink(NAME);
    return EXIT_SUCCESS;
  } else {
    printf("seed must be between 1 and 100\n");
  }
}