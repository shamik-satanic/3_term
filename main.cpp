#include "1task.h"
#include "2task.h"
#include "3task.h"
#include <windows.h>

// task1


//int main()
//{
//SetConsoleOutputCP(CP_UTF8);
//SetConsoleCP(CP_UTF8);
//    Set A('A');
//    Set B('B');
//    Set C('C');
//
//    A.add('a'); A.add('b'); A.add('c');
//    B.add('b'); B.add('c'); B.add('d');
//    C.add('a'); C.add('c'); C.add('e');
//
//    printf("1. METHOD object.see(name):\n");
//    printf("A: "); A.see(A.get_name());
//    printf("B: "); B.see(B.get_name());
//    printf("C: "); C.see(C.get_name());
//    printf("\n");
//
//    printf("2. UNION:\n");
//    Set* U1 = A.union_merge(B);
//    printf("A ∪ B: "); U1->see(U1->get_name());
//    delete U1;
//
//    Set* U2 = A.union_merge(C);
//    printf("A ∪ C: "); U2->see(U2->get_name());
//    delete U2;
//    printf("\n");
//
//    printf("3. INTERSECT:\n");
//    Set* I1 = A.intersection_merge(B);
//    printf("A ∩ B: "); I1->see(I1->get_name());
//    delete I1;
//
//    Set* I2 = A.intersection_merge(C);
//    printf("A ∩ C: "); I2->see(I2->get_name());
//    delete I2;
//    printf("\n");
//
//    printf("4. DIFFERENCE:\n");
//    Set* D1 = A.difference_merge(B);
//    printf("A - B: "); D1->see(D1->get_name());
//    delete D1;
//
//    Set* D2 = B.difference_merge(A);
//    printf("B - A: "); D2->see(D2->get_name());
//    delete D2;
//    printf("\n");
//
//    printf("5. SUBSETS:\n");
//    Set D('D');
//    D.add('a'); D.add('c');
//    printf("D: "); D.see(D.get_name());
//
//    printf("D ⊆ A: %s\n", D.is_subset_of(A) ? "true" : "false");
//    printf("D ⊆ B: %s\n", D.is_subset_of(B) ? "true" : "false");
//    printf("D ⊂ A: %s\n", (D < A) ? "true" : "false");
//    printf("\n");
//
//    printf("6. EQUALITY:\n");
//    Set E('E');
//    E.add('a'); E.add('b'); E.add('c');
//    printf("E: "); E.see(E.get_name());
//
//    printf("A == E: %s\n", (A == E) ? "true" : "false");
//    printf("A == B: %s\n", (A == B) ? "true" : "false");
//    printf("A != C: %s\n", (A != C) ? "true" : "false");
//    printf("\n");
//
//    printf("7. POW(BULEAN):\n");
//    Set F('F');
//    F.add('x'); F.add('y'); F.add('#'), F.add('4'); F.add('Z'); F.add('0');
//    printf("F: "); F.see(F.get_name());
//    printf("Bulean F:\n");
//    F.pow();
//    printf("\n");
//
//    printf("8. COMBINATED OPERATIONS:\n");
//    Set* complex = A.union_merge(B);
//    Set* complex_final = complex->intersection_merge(C);
//    printf("(A ∪ B) ∩ C: "); complex_final->see(complex_final->get_name());
//    delete complex;
//    delete complex_final;
//    printf("\n");
//
//    printf("9. EMPTY SET:\n");
//    Set Empty('G');
//    printf("empty G: "); Empty.see(Empty.get_name());
//    printf("∅ ⊆ A: %s\n", Empty.is_subset_of(A) ? "true" : "false");
//    printf("∅ == ∅: %s\n", (Empty == Empty) ? "true" : "false");
//    printf("\n");
//
//    printf("10. METHOD see() for all sets:\n");
//    Set::see();
//    return 0;
//}





// task 2

//int main()
//{
//SetConsoleOutputCP(CP_UTF8);
//SetConsoleCP(CP_UTF8);
//    std::string path;
//    std::cin >> path;
//
//    std::ifstream input_file(path);
//    if (!input_file.is_open())
//    {
//        throw std::runtime_error("file isn't open");
//    }
//
//    Datastruct data = extract_data_from_file(input_file);
//    check_relation(&data);
//    if (data.is_equivalence_relation)
//    {
//        print_equivalence_info(&data);
//    }
//
//    if (data.is_order_relation)
//    {
//        print_max_elements(&data);
//        print_min_elements(&data);
//    }
//
//    return 0;
//}


// task  3

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    try {
        std::string filename = "formula.txt";
        std::string formula = readFormulaFromFile(filename);

        std::cout << "Формула: " << formula << "\n\n";

        FormulaParser parser(formula);
        auto expr = parser.parse();

        std::cout << "Формула успешно распознана!\n\n";

        BooleanFunction func(expr);
        printResults(func);

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}



