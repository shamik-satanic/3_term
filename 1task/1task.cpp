#include "1task.h"
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

void print_menu()
{
    std::cout << "1.  new A               - добавить новое множество A\n";
    std::cout << "2.  del A               - удалить множество A\n";
    std::cout << "3.  add A x             - добавить элемент x к множеству A\n";
    std::cout << "4.  rem A x             - убрать элемент x из множества A\n";
    std::cout << "5.  pow A               - вычислить булеан множества A\n";
    std::cout << "6.  see [A]             - вывести множество A или все множества\n";
    std::cout << "7.  A + B               - объединение множеств A и B\n";
    std::cout << "8.  A & B               - пересечение множеств A и B\n";
    std::cout << "9.  A - B               - разность множеств A и B\n";
    std::cout << "10. A < B               - проверить, является ли A подмножеством B\n";
    std::cout << "11. A = B               - проверить, равны ли множества A и B\n";
    std::cout << "12. exit                - выход\n";
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    std::string command;

    while (true)
    {
        std::cout << "\n> ";
        std::getline(std::cin, command);

        if (command.empty()) continue;

        size_t start = command.find_first_not_of(" \t");
        size_t end = command.find_last_not_of(" \t");
        if (start == std::string::npos) continue;
        command = command.substr(start, end - start + 1);

        std::stringstream ss(command);
        std::string action;
        ss >> action;

        std::string action_lower = action;
        for (auto& c : action_lower) c = tolower(c);

        try
        {
            if (action_lower == "new")
            {
                char name;
                ss >> name;
                name = toupper(name);

                new Set(name);
                std::cout << "Множество " << name << " создано\n";
            }
            else if (action_lower == "del")
            {
                char set_name;
                ss >> set_name;
                set_name = toupper(set_name);

                Set* set = Set::find_set(set_name);
                if (set == nullptr)
                {
                    std::cout << "Ошибка: множество " << set_name << " не существует\n";
                    continue;
                }

                delete set;
                std::cout << "Множество " << set_name << " удалено\n";
            }
            else if (action_lower == "add")
            {
                char set_name, element;
                ss >> set_name >> element;
                set_name = toupper(set_name);

                Set* set = Set::find_set(set_name);
                if (set == nullptr)
                {
                    std::cout << "Ошибка: множество " << set_name << " не существует\n";
                    continue;
                }

                try
                {
                    set->add(element);
                    std::cout << "Элемент '" << element << "' добавлен в множество " << set_name << "\n";
                }
                catch (const std::exception& e)
                {
                    std::cout << "Ошибка добавления: элемент '" << element << "' уже существует в множестве " << set_name << "\n";
                }
            }
            else if (action_lower == "rem")
            {
                char set_name, element;
                ss >> set_name >> element;
                set_name = toupper(set_name);

                Set* set = Set::find_set(set_name);
                if (set == nullptr)
                {
                    std::cout << "Ошибка: множество " << set_name << " не существует\n";
                    continue;
                }

                try
                {
                    set->rem(element);
                    std::cout << "Элемент '" << element << "' удалён из множества " << set_name << "\n";
                }
                catch (const std::exception& e)
                {
                    std::cout << "Ошибка удаления: элемент '" << element << "' не существует в множестве " << set_name << "\n";
                }
            }
            else if (action_lower == "see")
            {
                char set_name;
                if (ss >> set_name)
                {
                    set_name = toupper(set_name);
                    Set* set = Set::find_set(set_name);
                    if (set == nullptr)
                    {
                        std::cout << "Ошибка: множество " << set_name << " не существует\n";
                        continue;
                    }
                    set->see(set_name);
                }
                else
                {
                    Set::see();
                }
            }
            else if (action_lower == "pow")
            {
                char set_name;
                ss >> set_name;
                set_name = toupper(set_name);

                Set* set = Set::find_set(set_name);
                if (set == nullptr)
                {
                    std::cout << "Ошибка: множество " << set_name << " не существует\n";
                    continue;
                }

                set->pow();
            }
            else if (action_lower == "help")
            {
                print_menu();
            }
            else if (action_lower == "exit")
            {
                std::cout << "Выход из программы\n";
                break;
            }

            else if (action.length() == 1 && action[0] >= 'A' && action[0] <= 'Z')
            {
                char set1_name = toupper(action[0]);
                std::string operation;
                char set2_name;

                ss >> operation >> set2_name;
                set2_name = toupper(set2_name);

                Set* set1 = Set::find_set(set1_name);
                Set* set2 = Set::find_set(set2_name);

                if (set1 == nullptr || set2 == nullptr)
                {
                    std::cout << "Ошибка: одно или оба множества не существуют\n";
                    continue;
                }

                if (operation == "+")
                {
                    Set* result = set1->union_merge(*set2);
                    std::cout << "Создано новое множество " << result->get_name() << " = "
                              << set1_name << " ∪ " << set2_name << "\n";
                    result->see(result->get_name());
                }
                else if (operation == "&")
                {
                    Set* result = set1->intersection_merge(*set2);
                    std::cout << "Создано новое множество " << result->get_name() << " = "
                              << set1_name << " ∩ " << set2_name << "\n";
                    result->see(result->get_name());
                }
                else if (operation == "-")
                {
                    Set* result = set1->difference_merge(*set2);
                    std::cout << "Создано новое множество " << result->get_name() << " = "
                              << set1_name << " \\ " << set2_name << "\n";
                    result->see(result->get_name());
                }
                else if (operation == "<")
                {
                    if (*set1 < *set2)
                    {
                        std::cout << set1_name << " ⊂ " << set2_name << " (истина)\n";
                    }
                    else if (*set1 <= *set2)
                    {
                        std::cout << set1_name << " ⊆ " << set2_name << " (истина, множества равны)\n";
                    }
                    else
                    {
                        std::cout << set1_name << " ⊄ " << set2_name << " (ложь)\n";
                    }
                }
                else if (operation == "=")
                {
                    if (*set1 == *set2)
                    {
                        std::cout << set1_name << " = " << set2_name << " (истина)\n";
                    }
                    else
                    {
                        std::cout << set1_name << " ≠ " << set2_name << " (ложь)\n";
                    }
                }
                else
                {
                    std::cout << "Неизвестная операция. Используйте +, &, -, <, =\n";
                }
            }
            else
            {
                std::cout << "Неизвестная команда. Введите help для справки\n";
            }
        }
        catch (const std::exception& e)
        {
            std::cout << "Ошибка: " << e.what() << "\n";
        }
    }

    return 0;
}