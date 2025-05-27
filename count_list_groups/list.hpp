// Do not modify this file.
// All modification will be dropped.

#ifndef LIST_HPP
#define LIST_HPP

#include <string>
#include <vector>

// Definition for singly-linked list.
struct ListNode
{
    int id;
    ListNode *next;
    ListNode(int i) : id(i), next(NULL) {}
};

int count_list_groups(int n, std::vector<ListNode *> &lists);

#endif