#ifndef DISCRETE_MATHEMATICS_2TASK_H
#define DISCRETE_MATHEMATICS_2TASK_H

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

struct Datastruct
{
    std::vector<char> set;
    std::vector<std::pair<char, char>> pairs;
    bool is_equivalence_relation = false;
    bool is_order_relation = false;
};

Datastruct extract_data_from_file(std::ifstream &f)
{
    Datastruct data;
    std::string line;
    while(std::getline(f, line))
    {
        if (!line.empty()) break;
    }

    for (char c : line)
    {
        if (c != ' ')
        {
            data.set.push_back(c);
        }
    }

    while (getline(f, line))
    {
        if (line.empty()) continue;
        bool read_first = true;

        char first = ' ', second = ' ';
        for (char c : line)
        {
            if (c != ' ')
            {
                if (read_first)
                {
                    first = c;
                    read_first = false;
                } else{
                    second = c;
                    break;
                }
            }
        }
        if (first != ' ' && second != ' ')
        {
            data.pairs.push_back({first, second});
        }

    }
    std::cout << "Elements of set: ";
    for (char e : data.set) std::cout << e << " ";
    std::cout << "\nrelation pairs: ";
    for (auto& p : data.pairs) std::cout << "(" << p.first << "," << p.second << ") ";
    std::cout << std::endl;
    return data;
}

void check_relation(Datastruct *data)
{
    bool is_reflexive = true, is_antireflexive = false, is_symmetric = true, is_antisymmetric = true,
            is_asymmetric = false, is_transitive = true, is_antitransitive = true, is_complete = true;

    for (char const set_elem : data->set)
    {
        bool found = false;
        for (auto const pair : data->pairs)
        {
            if (pair.first == set_elem && pair.second == set_elem)
            {
                found = true;
                is_antireflexive = false;
                break;
            }
        }
        if (!found)
        {
            is_reflexive = false;
        }

    }

    for (auto const pair : data->pairs)
    {
        bool found = false;
        for (auto const other_pair : data->pairs)
        {
            if ((pair.first == other_pair.second) && (pair.second == other_pair.first))
            {
                if (pair.first != pair.second) {is_antisymmetric = false;}
                found = true;
                break;
            }
        }
        if (!found)
        {
            is_symmetric = false;
        }
    }

    if (is_antireflexive && is_antisymmetric) {is_asymmetric = true;}

    for (auto const &p1 : data->pairs)
    {
        for(auto const &p2 : data->pairs)
            if (p1.second == p2.first)
            {
                bool found = false;
                for (auto const &p3 : data->pairs)
                {
                    if (p3.first == p1.first && p3.second == p2.second)
                    {
                        found = true;
                        is_antitransitive = false;
                        break;
                    }
                }
                if (!found)
                {
                    is_transitive = false;
                }
            }
    }

    for (size_t i = 0; i < data->set.size(); ++i)
    {
        for (size_t j = i + 1; j < data->set.size(); ++j)
        {
            char a = data->set[i];
            char b = data->set[j];
            bool found_ab = false, found_ba = false;

            for (auto const &pair : data->pairs)
            {
                if (pair.first == a && pair.second == b) found_ab = true;
                if (pair.first == b && pair.second == a) found_ba = true;
            }

            if (!found_ab && !found_ba)
            {
                is_complete = false;
                break;
            }
        }
        if (!is_complete) break;
    }

    if (is_reflexive && is_symmetric && is_transitive) {data->is_equivalence_relation = true;}

    if (is_antisymmetric && is_transitive) {data->is_order_relation = true;}

    std::cout << "Свойства отношения:" << std::endl;
    std::cout << "1. Рефлексивность:          " << (is_reflexive ? "+" : "-") << std::endl;
    std::cout << "2. Антирефлексивность:      " << (is_antireflexive ? "+" : "-") << std::endl;
    std::cout << "3. Симметричность:          " << (is_symmetric ? "+" : "-") << std::endl;
    std::cout << "4. Антисимметричность:      " << (is_antisymmetric ? "+" : "-") << std::endl;
    std::cout << "5. Асимметричность:         " << (is_asymmetric ? "+" : "-") << std::endl;
    std::cout << "6. Транзитивность:          " << (is_transitive ? "+" : "-") << std::endl;
    std::cout << "7. Антитранзитивность:      " << (is_antitransitive ? "+" : "-") << std::endl;
    std::cout << "8. Полнота:                 " << (is_complete ? "+" : "-") << std::endl << std::endl;


    std::cout << "Является ли отношением эквивалентности   " << (data->is_equivalence_relation ? "ДА": "НЕТ") << std::endl;
    std::cout << "Является ли отношением порядка           " << (data->is_order_relation ? "ДА": "НЕТ") << std::endl;
}

void print_equivalence_info(Datastruct const *data)
{
    std::vector<bool> visited(data->set.size(), false);
    int class_count = 0;

    std::cout << "\nКлассы эквивалентности:" << std::endl;

    for (size_t i = 0; i < data->set.size(); ++i)
    {
        if (!visited[i])
        {
            class_count++;
            std::vector<char> current_class;
            current_class.push_back(data->set[i]);
            visited[i] = true;

            for (size_t j = i + 1; j < data->set.size(); ++j)
            {
                if (!visited[j])
                {
                    bool equivalent = false;
                    for (auto const &pair : data->pairs)
                    {
                        if (pair.first == data->set[i] && pair.second == data->set[j])
                        {
                            equivalent = true;
                            break;
                        }
                    }
                    if (equivalent)
                    {
                        current_class.push_back(data->set[j]);
                        visited[j] = true;
                    }
                }
            }

            for (char elem : current_class)
            {
                std::cout << elem << ": ";
                for (size_t k = 0; k < current_class.size(); ++k)
                {
                    std::cout << current_class[k];
                    if (k < current_class.size() - 1)
                    {
                        std::cout << ", ";
                    }
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }

    std::cout << "Индекс разбиения: " << class_count << std::endl;
}

void print_min_elements(Datastruct const *data)
{
    std::vector<char> result;
    for (char const elem : data->set)
    {
        bool is_find = false;
        for (auto const &pair :data->pairs)
        {
            if (elem == pair.second && pair.first != pair.second)
            {
                is_find = true;
            }
        }
        if (!is_find)
        {
            result.push_back(elem);
        }
    }

    for (size_t i = 0; i < result.size(); ++i)
    {
        std::cout << result[i];
        if (!(i == result.size() - 1))
        {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

void print_max_elements(Datastruct const *data)
{
    std::vector<char> result;
    for (char const elem : data->set)
    {
        bool is_find = false;
        for (auto const &pair :data->pairs)
        {
            if (elem == pair.first && pair.first != pair.second)
            {
                is_find = true;
            }
        }
        if (!is_find)
        {
            result.push_back(elem);
        }
    }

    for (size_t i = 0; i < result.size(); ++i)
    {
        std::cout << result[i];
        if (!(i == result.size() - 1))
        {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}

#endif //DISCRETE_MATHEMATICS_2TASK_H
