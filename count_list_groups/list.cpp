#include "list.hpp"

// Write your implementation here.
// Feel free to add any code you need in this file.
int count_list_groups(int n, std::vector<ListNode*>& lists) {
  int count = 0;
  for (int i = 0; i < lists.size(); i++) {
    if (lists[i]->next == NULL) {
      count++;
    }
  }
  return count;
}
