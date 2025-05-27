/*
gcc main.c -o main -pthread
./main 0 10
*/
// 有 7 個實驗，一開始有 8 個麵包板，
// 如果沒有足夠的麵包板，學生便持續等待，一旦有足夠數量的麵包板就進行借用。
// 麵包板借用順序的方式不限，例如學生借用麵包板 0, 4, 5 。
// 學生每次借用的時間是 p 秒(1 <= p <= 5， p 為隨機)，
// 每個實驗做完的時候，必須先還回所有的板子，然後再重新進行下一個實驗的借用，
// 當某個學生做完了所有的實驗，他就不再借用任何的麵包板。
// 當所有學生都不再借用，就結束模擬。
// 每個學生都必須各別用一個thread來模擬。
// 學生的編號從 1 開始依序編號。麵包板的編號是 0～7 。
// 在程式進行中，請將學生的狀態詳細輸出，以便檢查。
// 同時要印出時間，以 “時：分：秒” 方式呈現。
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NUM_EXPERIMENTS 7
#define NUM_BREADBOARDS 8

pthread_mutex_t lock;
pthread_cond_t cond;
int breadboards[NUM_BREADBOARDS];
int students_completed = 0;

typedef struct {
  int student_id;
  int num_experiments;
} StudentData;

void printCurrentTime() {
  time_t now;
  struct tm* tm_now;
  char time_str[9];

  time(&now);
  tm_now = localtime(&now);

  sprintf(time_str, "%02d:%02d:%02d", (tm_now->tm_hour) + 8, tm_now->tm_min,
          tm_now->tm_sec);
  printf("%s ", time_str);
}

void* studentThread(void* arg) {
  StudentData* student_data = (StudentData*)arg;
  int student_id = student_data->student_id;
  int num_experiments = student_data->num_experiments;

  int wait_time =
      rand() % 10 + 1;  // Randomly determine the wait time for each student
  sleep(wait_time);     // Wait for the determined time before coming

  printCurrentTime();
  printf("student %02d comes\n", student_id);

  bool wait_output[NUM_EXPERIMENTS];
  memset(wait_output, true, sizeof(wait_output));

  for (int i = 0; i < num_experiments; i++) {
    int num_breadboards_needed =
        rand() % 3 + 1;  // Randomly determine the number of breadboards needed

    pthread_mutex_lock(&lock);

    while (1) {
      // Check if there are enough available breadboards
      int available_breadboards = 0;
      for (int j = 0; j < NUM_BREADBOARDS; j++) {
        if (breadboards[j] == 0) {
          available_breadboards++;
        }
      }

      if (available_breadboards >= num_breadboards_needed) {
        // Found enough available breadboards, borrow them
        int borrowed_boards = 0;
        printCurrentTime();
        printf("student %02d needs %d boards for lab %d, gets board ",
               student_id, num_breadboards_needed, i + 1);
        for (int k = 0; k < NUM_BREADBOARDS; k++) {
          if (breadboards[k] == 0 && borrowed_boards < num_breadboards_needed) {
            breadboards[k] = student_id;
            printf("%d", k);
            borrowed_boards++;
            if (borrowed_boards < num_breadboards_needed) {
              printf(", ");
            }
          }
        }
        printf("\n");
        break;
      } else {
        // Wait for available breadboards
        if (wait_output[i] == true) {
          printCurrentTime();
          printf("student %02d needs %d boards for lab %d, waits\n", student_id,
                 num_breadboards_needed, i + 1);
          wait_output[i] = false;
        }
        pthread_cond_wait(&cond, &lock);
      }
    }

    pthread_mutex_unlock(&lock);

    int lab_time = rand() % 5 +
                   1;  // Randomly determine the time taken to complete the lab
    sleep(lab_time);   // Simulate time taken to complete the lab

    pthread_mutex_lock(&lock);

    // Return the borrowed breadboards
    printCurrentTime();
    int borrowed_boards = 0;
    printf("student %02d: finishes lab %d, returns board ", student_id, i + 1);
    for (int j = 0; j < NUM_BREADBOARDS; j++) {
      if (breadboards[j] == student_id) {
        breadboards[j] = 0;
        printf("%d", j);
        borrowed_boards++;
        if (borrowed_boards < num_breadboards_needed) {
          printf(", ");
        }
      }
    }
    printf("\n");
    pthread_cond_broadcast(&cond);  // Signal that breadboards are returned
    pthread_mutex_unlock(&lock);
  }

  pthread_mutex_lock(&lock);
  students_completed++;
  pthread_cond_broadcast(
      &cond);  // Signal that a student has completed all labs
  pthread_mutex_unlock(&lock);

  return NULL;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf("Usage: ./main <random_seed> <num_students>\n");
    return 1;
  }

  int seed = atoi(argv[1]);
  int num_students = atoi(argv[2]);

  pthread_t threads[num_students];
  StudentData student_data[num_students];

  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&cond, NULL);

  srand(seed);

  for (int i = 0; i < NUM_BREADBOARDS; i++) {
    breadboards[i] = 0;
  }

  for (int i = 0; i < num_students; i++) {
    student_data[i].student_id = i + 1;
    student_data[i].num_experiments = NUM_EXPERIMENTS;

    pthread_create(&threads[i], NULL, studentThread, (void*)&student_data[i]);
  }

  for (int i = 0; i < num_students; i++) {
    pthread_join(threads[i], NULL);
  }

  printCurrentTime();
  printf("All students finish their labs\n");

  pthread_mutex_destroy(&lock);
  pthread_cond_destroy(&cond);

  return 0;
}
