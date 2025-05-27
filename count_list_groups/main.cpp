#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "list.hpp"

int main()
{
    std::string input_filename;
    std::cin >> input_filename;

    std::ifstream input;
    input.open(input_filename);

    if (!input.is_open())
    {
        std::cerr << "failed to open test data\n";
        return 1;
    }

    int n, m;
    input >> n >> m;

    // generate n nodes
    std::vector<ListNode *> lists(n);
    for (size_t i = 0; i < n; i++)
    {
        lists[i] = new ListNode(i);
    }

    // read m edges
    for (size_t i = 0; i < m; i++)
    {
        int f, t;
        input >> f >> t;
        lists[f]->next = lists[t];
    }

    int c = count_list_groups(n, lists);
    std::cout << c << '\n';

    input.close();
    return 0;
}
