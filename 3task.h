#ifndef DISCRETE_MATHEMATICS_3TASK_H
#define DISCRETE_MATHEMATICS_3TASK_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cmath>

enum class NodeType {
    VARIABLE,
    NOT,
    OR,
    AND,
    XOR,
    EQUIV,
    IMPL,
    NAND,
    NOR
};

struct ExprNode {
    NodeType type;
    std::string var_name;
    std::shared_ptr<ExprNode> left;
    std::shared_ptr<ExprNode> right;

    ExprNode(NodeType t) : type(t), left(nullptr), right(nullptr) {}
    ExprNode(const std::string& name) : type(NodeType::VARIABLE), var_name(name), left(nullptr), right(nullptr) {}
};

class FormulaParser {
private:
    std::string input;
    size_t pos;

    void skipSpaces();
    char peek();
    char get();
    bool match(char c);
    std::string parseVariable();
    std::shared_ptr<ExprNode> parseExpression();
    std::shared_ptr<ExprNode> parsePrimary();

public:
    FormulaParser(const std::string& formula);
    std::shared_ptr<ExprNode> parse();
};

class BooleanFunction {
private:
    std::shared_ptr<ExprNode> root;
    std::vector<std::string> variables;
    std::vector<std::vector<bool>> truth_table;
    std::vector<bool> function_values;

    void collectVariables(const std::shared_ptr<ExprNode>& node, std::set<std::string>& vars);
    bool evaluate(const std::shared_ptr<ExprNode>& node, const std::map<std::string, bool>& values);
    void buildTruthTable();

public:
    BooleanFunction(std::shared_ptr<ExprNode> expr);

    void printTruthTable();
    std::vector<std::string> findFictiveVariables();
    void removeFictiveVariable(const std::string& var);

    BooleanFunction getDual();
    std::string getPDNF();
    std::string getPCNF();
    std::string getANF();

    const std::vector<std::string>& getVariables() const { return variables; }
    const std::vector<bool>& getFunctionValues() const { return function_values; }
};

std::string readFormulaFromFile(const std::string& filename);
void printResults(BooleanFunction& func);

std::string nodeToString(const std::shared_ptr<ExprNode>& node);

#endif //DISCRETE_MATHEMATICS_3TASK_H
