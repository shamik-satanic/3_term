#ifndef DISCRETE_MATHEMATICS_1TASK_H
#define DISCRETE_MATHEMATICS_1TASK_H

#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <algorithm>

struct Node
{
    char data;
    Node *next;

    Node(char value) : data(value), next(nullptr) {}
};

class Set
{
private:
    Node *head;
    static std::vector<Set*> all_sets;

public:
    Set() : head(nullptr) {}

    ~Set()
    {
        Node *curr_iter = head;
        while (curr_iter)
        {
            Node *next = curr_iter->next;
            delete curr_iter;
            curr_iter = next;
        }

        auto it = std::find(all_sets.begin(), all_sets.end(), this);
        if (it != all_sets.end())
        {
            all_sets.erase(it);
        }
    }

    Set(char name)
    {
        if (name < 'A' || name > 'Z')
        {
            throw std::invalid_argument("имя множества должно быть в диапозоне [A, Z]");
        }

        Set *set = find_set(name);
        if (set != nullptr)
        {
            throw std::logic_error("множество уже существует");
        }

        head = new Node(name);
        all_sets.push_back(this);
    }

public:
    char get_name() const
    {
        if (head == nullptr)
        {
            throw std::logic_error("множество не инициализировано");
        }
        return head->data;
    }

public:
    static Set *find_set(char name)
    {
        for (Set *set : all_sets)
        {
            if (set->head->data == name)
            {
                return set;
            }
        }
        return nullptr;
    }

public:
    bool contains(char value) const
    {
        if (head == nullptr)
        {
            return false;
        }

        Node *curr_iter = head->next;
        while (curr_iter != nullptr)
        {
            if (curr_iter->data == value)
            {
                return true;
            }
            curr_iter = curr_iter->next;
        }
        return false;
    }

public:
    void add(char value)
    {
        if (head == nullptr)
        {
            throw std::logic_error("сначала создайте множество");
        }

        if (contains(value))
        {
            throw std::logic_error("элемент уже существует в множестве");
        }

        Node *new_node = new Node(value);
        Node *prev_iter = nullptr;
        Node *curr_iter  = head->next;
        while (curr_iter != nullptr && value > curr_iter->data)
        {
            prev_iter = curr_iter;
            curr_iter = curr_iter->next;
        }

        if (prev_iter == nullptr)
        {
            new_node->next = head->next;
            head->next = new_node;
            return;
        }
        prev_iter->next = new_node;
        new_node->next = curr_iter;
    }

public:
    void rem(char value)
    {
        if (head == nullptr)
        {
            throw std::invalid_argument("сначала создайте множество");
        }
        if (head->next == nullptr)
        {
            throw std::logic_error("множество пустое");
        }

        if (!contains(value))
        {
            throw std::logic_error("элемент уже существует в множестве");
        }

        Node *temp_ptr;
        Node *curr_iter = head->next;
        Node *prev_iter = head;
        while(curr_iter != nullptr && curr_iter->data != value)
        {
            prev_iter = curr_iter;
            curr_iter = curr_iter->next;
        }
        if (curr_iter != nullptr)
        {
            prev_iter->next = curr_iter->next;
            delete curr_iter;
        }
    }
public:
    static void see()
    {
        if (all_sets.size() == 0)
        {
            std::cout << "Не создано ни одного множества\n";
            return;
        }

        std::cout << "Список всех множеств:\n";
        for (Set *set : all_sets)
        {
            printf("  %c: { ", set->head->data);
            Node *iter = set->head->next;
            bool first = true;
            while (iter)
            {
                if (!first) printf(", ");
                printf("%c", iter->data);
                iter = iter->next;
                first = false;
            }
            printf(" }\n");
        }
    }

public:
    void see(char set_name) const
    {
        if (find_set(set_name) == nullptr)
        {
            throw std::invalid_argument("множество не существует");
        }

        printf("Элементы множества %c: { ", head->data);
        Node *iter = head->next;
        bool first = true;
        while (iter)
        {
            if (!first) printf(", ");
            printf("%c", iter->data);
            iter = iter->next;
            first = false;
        }
        printf(" }\n");
    }


public:
    void pow()
    {
        std::vector<char> elements;
        Node* current = head->next;
        while (current)
        {
            elements.push_back(current->data);
            current = current->next;
        }

        int n = elements.size();
        int total = 1 << n;

        std::cout << "булеан: " << head->data << ": {\n";

        for (int mask = 0; mask < total; mask++)
        {
            std::cout << "  {";
            bool first = true;

            for (int i = 0; i < n; i++)
            {
                if (mask & (1 << i))
                {
                    if (!first) std::cout << ", ";
                    std::cout << elements[i];
                    first = false;
                }
            }
            std::cout << "}";
            if (mask < total - 1) std::cout << ",";
            std::cout << "\n";
        }
        std::cout << "}" << std::endl;
    }

public:
    Set* union_merge(const Set& other) const
    {
        if (head == nullptr || other.head == nullptr)
        {
            throw std::logic_error("множетсва должны быть инициализированы");
        }

        char new_name = 'A';
        for (int i = 0; i < 26; i++)
        {
            new_name = 'A' + i;
            if (Set::find_set(new_name) == nullptr) break;
        }

        Set* result = new Set(new_name);

        Node* currentA = this->head->next;
        Node* currentB = other.head->next;

        while (currentA && currentB)
        {
            if (currentA->data < currentB->data)
            {
                result->add(currentA->data);
                currentA = currentA->next;
            }
            else if (currentA->data > currentB->data)
            {
                result->add(currentB->data);
                currentB = currentB->next;
            }
            else
            {
                result->add(currentA->data);
                currentA = currentA->next;
                currentB = currentB->next;
            }
        }

        while (currentA) {
            result->add(currentA->data);
            currentA = currentA->next;
        }
        while (currentB) {
            result->add(currentB->data);
            currentB = currentB->next;
        }

        return result;
    }

public:
    Set* intersection_merge(const Set& other) const
    {
        if (head == nullptr || other.head == nullptr)
        {
            throw std::logic_error("множетсва должны быть инициализированы");
        }

        char new_name = 'A';
        for (int i = 0; i < 26; i++)
        {
            new_name = 'A' + i;
            if (Set::find_set(new_name) == nullptr) break;
        }



        Set* result = new Set(new_name);

        Node* currentA = this->head->next;
        Node* currentB = other.head->next;

        while (currentA != nullptr && currentB != nullptr)
        {
            if (currentA->data < currentB->data)
            {
                currentA = currentA->next;
            }
            else if (currentA->data > currentB->data)
            {
                currentB = currentB->next;
            }
            else
            {
                result->add(currentA->data);
                currentA = currentA->next;
                currentB = currentB->next;
            }
        }
        return result;
    }

public:
    Set* difference_merge(const Set& other) const
    {
        if (head == nullptr || other.head == nullptr)
        {
            throw std::logic_error("множетсва должны быть инициализированы");
        }

        char new_name = 'A';
        for (int i = 0; i < 26; i++)
        {
            new_name = 'A' + i;
            if (Set::find_set(new_name) == nullptr) break;
        }

        Set* result = new Set(new_name);

        Node* currentA = this->head->next;
        Node* currentB = other.head->next;

        while (currentA != nullptr && currentB != nullptr)
        {
            if (currentA->data < currentB->data)
            {

                result->add(currentA->data);
                currentA = currentA->next;
            }
            else if (currentA->data > currentB->data)
            {

                currentB = currentB->next;
            }
            else
            {
                currentA = currentA->next;
                currentB = currentB->next;
            }
        }
        while (currentA != nullptr)
        {
            result->add(currentA->data);
            currentA = currentA->next;
        }

        return result;
    }

public:
    bool is_subset_of(const Set& other) const
    {
        if (head == nullptr || other.head == nullptr)
        {
            throw std::logic_error("множетсва должны быть инициализированы");
        }

        Node* currentA = this->head->next;
        Node* currentB = other.head->next;

        while (currentA != nullptr && currentB != nullptr)
        {
            if (currentA->data < currentB->data)
            {
                return false;
            }
            else if (currentA->data > currentB->data)
            {
                currentB = currentB->next;
            }
            else
            {
                currentA = currentA->next;
                currentB = currentB->next;
            }
        }

        return (currentA == nullptr);
    }

private:
    bool is_equal_to(const Set& other) const
    {
        if (head == nullptr || other.head == nullptr)
        {
            throw std::logic_error("множетсва должны быть инициализированы");
        }

        Node* currentA = this->head->next;
        Node* currentB = other.head->next;

        while (currentA != nullptr && currentB != nullptr)
        {
            if (currentA->data != currentB->data)
            {
                return false;
            }
            currentA = currentA->next;
            currentB = currentB->next;
        }
        return (currentA == nullptr && currentB == nullptr);
    }

public:
    bool operator<(const Set& other) const
    {
        return is_subset_of(other) && !is_equal_to(other);
    }

    bool operator<=(const Set& other) const
    {
        return is_subset_of(other);
    }


    bool operator==(const Set& other) const
    {
        return is_equal_to(other);
    }

    bool operator!=(const Set& other) const
    {
        return !is_equal_to(other);
    }
};

inline std::vector<Set*> Set::all_sets = {};

#endif //DISCRETE_MATHEMATICS_1TASK_H