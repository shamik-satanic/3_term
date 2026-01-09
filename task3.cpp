#include "3task.h"

// Конструктор парсера
FormulaParser::FormulaParser(const std::string& formula) : input(formula), pos(0) {}

void FormulaParser::skipSpaces() {
    while (pos < input.length() && std::isspace(input[pos])) {
        pos++;
    }
}

char FormulaParser::peek() {
    skipSpaces();
    return pos < input.length() ? input[pos] : '\0';
}

char FormulaParser::get() {
    skipSpaces();
    return pos < input.length() ? input[pos++] : '\0';
}

bool FormulaParser::match(char c) {
    if (peek() == c) {
        get();
        return true;
    }
    return false;
}

std::string FormulaParser::parseVariable() {
    skipSpaces();
    std::string var;

    if (pos >= input.length() || !std::isalpha(input[pos])) {
        throw std::runtime_error("Ожидалась переменная");
    }

    var += input[pos++];

    if (pos < input.length() && input[pos] == '_') {
        var += input[pos++];
        if (pos >= input.length() || !std::isdigit(input[pos])) {
            throw std::runtime_error("После '_' ожидалось число");
        }
        while (pos < input.length() && std::isdigit(input[pos])) {
            var += input[pos++];
        }
    }

    return var;
}

std::shared_ptr<ExprNode> FormulaParser::parsePrimary() {
    char c = peek();

    if (c == '-') {
        get();
        auto node = std::make_shared<ExprNode>(NodeType::NOT);
        node->left = parsePrimary();
        return node;
    }

    if (c == '(' || c == '[' || c == '{') {
        char open = get();
        char close = (open == '(') ? ')' : (open == '[') ? ']' : '}';

        auto expr = parseExpression();

        if (get() != close) {
            throw std::runtime_error("Ожидалась закрывающая скобка");
        }

        return expr;
    }

    if (std::isalpha(c)) {
        return std::make_shared<ExprNode>(parseVariable());
    }

    throw std::runtime_error("Неожиданный символ в выражении");
}

std::shared_ptr<ExprNode> FormulaParser::parseExpression() {
    auto left = parsePrimary();

    char op = peek();
    NodeType type;

    switch (op) {
        case '+': type = NodeType::OR; break;
        case '&': type = NodeType::AND; break;
        case '@': type = NodeType::XOR; break;
        case '~': type = NodeType::EQUIV; break;
        case '>': type = NodeType::IMPL; break;
        case '|': type = NodeType::NAND; break;
        case '!': type = NodeType::NOR; break;
        default: return left;
    }

    get();
    auto node = std::make_shared<ExprNode>(type);
    node->left = left;
    node->right = parsePrimary();

    return node;
}

std::shared_ptr<ExprNode> FormulaParser::parse() {
    auto result = parseExpression();
    if (pos < input.length() && !std::isspace(input[pos])) {
        skipSpaces();
        if (pos < input.length()) {
            throw std::runtime_error("Неожиданные символы после выражения");
        }
    }
    return result;
}

// BooleanFunction implementation
BooleanFunction::BooleanFunction(std::shared_ptr<ExprNode> expr) : root(expr) {
    std::set<std::string> vars;
    collectVariables(root, vars);
    variables.assign(vars.begin(), vars.end());
    std::sort(variables.begin(), variables.end());
    buildTruthTable();
}

void BooleanFunction::collectVariables(const std::shared_ptr<ExprNode>& node, std::set<std::string>& vars) {
    if (!node) return;

    if (node->type == NodeType::VARIABLE) {
        vars.insert(node->var_name);
    }

    collectVariables(node->left, vars);
    collectVariables(node->right, vars);
}

bool BooleanFunction::evaluate(const std::shared_ptr<ExprNode>& node, const std::map<std::string, bool>& values) {
    if (node->type == NodeType::VARIABLE) {
        return values.at(node->var_name);
    }

    bool l = node->left ? evaluate(node->left, values) : false;
    bool r = node->right ? evaluate(node->right, values) : false;

    switch (node->type) {
        case NodeType::NOT: return !l;
        case NodeType::OR: return l || r;
        case NodeType::AND: return l && r;
        case NodeType::XOR: return l != r;
        case NodeType::EQUIV: return l == r;
        case NodeType::IMPL: return !l || r;
        case NodeType::NAND: return !(l && r);
        case NodeType::NOR: return !(l || r);
        default: return false;
    }
}

void BooleanFunction::buildTruthTable() {
    int n = variables.size();
    int rows = 1 << n;

    truth_table.clear();
    function_values.clear();

    for (int i = 0; i < rows; i++) {
        std::vector<bool> row;
        std::map<std::string, bool> values;

        for (int j = 0; j < n; j++) {
            bool val = (i >> (n - 1 - j)) & 1;
            row.push_back(val);
            values[variables[j]] = val;
        }

        bool result = evaluate(root, values);
        row.push_back(result);
        function_values.push_back(result);
        truth_table.push_back(row);
    }
}

void BooleanFunction::printTruthTable() {
    std::cout << "Таблица истинности:\n";

    for (const auto& var : variables) {
        std::cout << var << "\t";
    }
    std::cout << "f\n";

    for (const auto& row : truth_table) {
        for (bool val : row) {
            std::cout << val << "\t";
        }
        std::cout << "\n";
    }
}

std::vector<std::string> BooleanFunction::findFictiveVariables() {
    std::vector<std::string> fictive;
    int n = variables.size();

    for (int var_idx = 0; var_idx < n; var_idx++) {
        bool is_fictive = true;
        int rows = truth_table.size();

        for (int i = 0; i < rows; i += 2) {
            if ((i >> (n - 1 - var_idx)) & 1) continue;

            int j = i | (1 << (n - 1 - var_idx));
            if (j < rows && function_values[i] != function_values[j]) {
                is_fictive = false;
                break;
            }
        }

        if (is_fictive) {
            fictive.push_back(variables[var_idx]);
        }
    }

    return fictive;
}

void BooleanFunction::removeFictiveVariable(const std::string& var) {
    auto it = std::find(variables.begin(), variables.end(), var);
    if (it == variables.end()) return;

    int var_idx = std::distance(variables.begin(), it);
    int n = variables.size();

    std::vector<std::vector<bool>> new_table;
    std::vector<bool> new_values;

    for (size_t i = 0; i < truth_table.size(); i++) {
        if (!((i >> (n - 1 - var_idx)) & 1)) {
            std::vector<bool> row;
            for (int j = 0; j < n; j++) {
                if (j != var_idx) {
                    row.push_back(truth_table[i][j]);
                }
            }
            row.push_back(truth_table[i][n]);
            new_table.push_back(row);
            new_values.push_back(function_values[i]);
        }
    }

    variables.erase(it);
    truth_table = new_table;
    function_values = new_values;
}

BooleanFunction BooleanFunction::getDual() {
    auto dual_func = *this;
    for (size_t i = 0; i < dual_func.function_values.size(); i++) {
        dual_func.function_values[i] = !dual_func.function_values[i];
    }

    for (size_t i = 0; i < dual_func.truth_table.size(); i++) {
        dual_func.truth_table[i].back() = !dual_func.truth_table[i].back();
    }

    return dual_func;
}

std::string BooleanFunction::getPDNF() {
    std::vector<std::string> minterms;
    int n = variables.size();

    for (size_t i = 0; i < function_values.size(); i++) {
        if (function_values[i]) {
            std::string term;
            for (int j = 0; j < n; j++) {
                if (!term.empty()) term += " & ";
                if (!truth_table[i][j]) {
                    term += "-";
                }
                term += variables[j];
            }
            minterms.push_back("(" + term + ")");
        }
    }

    if (minterms.empty()) return "0";

    std::string result;
    for (size_t i = 0; i < minterms.size(); i++) {
        if (i > 0) result += " + ";
        result += minterms[i];
    }

    return result;
}

std::string BooleanFunction::getPCNF() {
    std::vector<std::string> maxterms;
    int n = variables.size();

    for (size_t i = 0; i < function_values.size(); i++) {
        if (!function_values[i]) {
            std::string term;
            for (int j = 0; j < n; j++) {
                if (!term.empty()) term += " + ";
                if (truth_table[i][j]) {
                    term += "-";
                }
                term += variables[j];
            }
            maxterms.push_back("(" + term + ")");
        }
    }

    if (maxterms.empty()) return "1";

    std::string result;
    for (size_t i = 0; i < maxterms.size(); i++) {
        if (i > 0) result += " & ";
        result += maxterms[i];
    }

    return result;
}

std::string BooleanFunction::getANF() {
    int n = variables.size();
    int size = 1 << n;
    std::vector<bool> coeffs = function_values;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < size; j++) {
            if ((j >> i) & 1) {
                coeffs[j] = coeffs[j] != coeffs[j ^ (1 << i)];
            }
        }
    }

    std::vector<std::string> terms;
    for (int i = 0; i < size; i++) {
        if (coeffs[i]) {
            if (i == 0) {
                terms.push_back("1");
            } else {
                std::string term;
                for (int j = 0; j < n; j++) {
                    if ((i >> j) & 1) {
                        if (!term.empty()) term += " & ";
                        term += variables[n - 1 - j];
                    }
                }
                terms.push_back(term);
            }
        }
    }

    if (terms.empty()) return "0";

    std::string result;
    for (size_t i = 0; i < terms.size(); i++) {
        if (i > 0) result += " @ ";
        result += terms[i];
    }

    return result;
}

std::string readFormulaFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Не удалось открыть файл: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void printResults(BooleanFunction& func) {
    func.printTruthTable();

    std::cout << "\nПоиск фиктивных переменных...\n";
    auto fictive = func.findFictiveVariables();

    if (!fictive.empty()) {
        std::cout << "Фиктивные переменные: ";
        for (size_t i = 0; i < fictive.size(); i++) {
            if (i > 0) std::cout << ", ";
            std::cout << fictive[i];
        }
        std::cout << "\n";

        for (const auto& var : fictive) {
            func.removeFictiveVariable(var);
        }

        std::cout << "\nТаблица истинности после удаления фиктивных переменных:\n";
        func.printTruthTable();
    } else {
        std::cout << "Фиктивных переменных нет.\n";
    }

    std::cout << "\nСДНФ: " << func.getPDNF() << "\n";
    std::cout << "СКНФ: " << func.getPCNF() << "\n";
    std::cout << "АНФ: " << func.getANF() << "\n";

    auto dual = func.getDual();
    std::cout << "\nДвойственная функция:\n";
    dual.printTruthTable();
    std::cout << "СДНФ двойственной: " << dual.getPDNF() << "\n";
    std::cout << "СКНФ двойственной: " << dual.getPCNF() << "\n";
}
