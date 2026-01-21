#include <cctype>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <iostream>
#include <fstream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <algorithm>
#include <cmath>
#include <bitset>
#include <functional>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

enum class OpType
{
    NONE,
    NOT,
    OR,
    AND,
    XOR,
    EQ,
    IMPL,
    NAND,
    NOR,
    L_PAREN, R_PAREN, L_BRACE, R_BRACE, L_BRACKET, R_BRACKET
};

struct Token
{
    OpType type;
    string value;

    Token(OpType t = OpType::NONE, string v = "")
            : type(t), value(v) {}
};

class BooleanFormula
{
private:
    string rawFormula;
    vector<Token> tokens;
    vector<string> variables;
    map<string, int> varIndex;
    vector<vector<bool>> truthTable;
    vector<bool> functionValues;
    vector<bool> isEssential;

    bool isVariableChar(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
               c == '_' || (c >= '0' && c <= '9');
    }

    bool isOperator(char c)
    {
        return c == '+' || c == '&' || c == '@' || c == '~' ||
               c == '>' || c == '|' || c == '!' || c == '-';
    }

    bool isBinaryOperator(OpType op)
    {
        return op == OpType::OR || op == OpType::AND ||
               op == OpType::XOR || op == OpType::EQ ||
               op == OpType::IMPL || op == OpType::NAND ||
               op == OpType::NOR;
    }

    bool isLeftBracket(OpType op)
    {
        return op == OpType::L_PAREN ||
               op == OpType::L_BRACE ||
               op == OpType::L_BRACKET;
    }

    bool isRightBracket(OpType op)
    {
        return op == OpType::R_PAREN ||
               op == OpType::R_BRACE ||
               op == OpType::R_BRACKET;
    }

    bool isBracket(OpType op)
    {
        return (isLeftBracket(op) || isRightBracket(op));
    }

    OpType charToOpType(char c)
    {
        switch(c)
        {
            case '+': return OpType::OR;
            case '&': return OpType::AND;
            case '@': return OpType::XOR;
            case '~': return OpType::EQ;
            case '>': return OpType::IMPL;
            case '|': return OpType::NAND;
            case '!': return OpType::NOR;
            case '-': return OpType::NOT;
            case '(': return OpType::L_PAREN;
            case ')': return OpType::R_PAREN;
            case '{': return OpType::L_BRACE;
            case '}': return OpType::R_BRACE;
            case '[': return OpType::L_BRACKET;
            case ']': return OpType::R_BRACKET;
            default: return OpType::NONE;
        }
    }

    int getPrecedence(OpType op)
    {
        switch(op)
        {
            case OpType::NOT: return 6;
            case OpType::AND:
            case OpType::NAND:
            case OpType::NOR: return 5;
            case OpType::OR: return 4;
            case OpType::XOR: return 3;
            case OpType::EQ: return 2;
            case OpType::IMPL: return 1;
            default: return 0;
        }
    }

    bool is_valid_variable(const string& var_candidate)
    {
        if (var_candidate.empty() || var_candidate.length() < 3)
        {
            return false;
        }

        if (!isalpha(var_candidate[0]))
        {
            return false;
        }

        if (var_candidate[1] != '_')
        {
            return false;
        }

        for (size_t i = 2; i < var_candidate.length(); i++)
        {
            if (!isdigit(var_candidate[i]))
            {
                return false;
            }
        }
        return true;
    }

    bool toRPN()
    {
        vector<Token> output;
        stack<Token> opStack;

        for (const Token& token : tokens)
        {
            if (token.type == OpType::NONE)
            {
                output.push_back(token);
            }
            else if (token.type == OpType::NOT)
            {
                opStack.push(token);
            }
            else if (isLeftBracket(token.type))
            {
                opStack.push(token);
            }
            else if (isRightBracket(token.type))
            {
                while (!opStack.empty() && !isLeftBracket(opStack.top().type))
                {
                    output.push_back(opStack.top());
                    opStack.pop();
                }
                if (!opStack.empty())
                {
                    opStack.pop();
                }
            }
            else if (isBinaryOperator(token.type))
            {
                int currentPrec = getPrecedence(token.type);
                while (!opStack.empty() &&
                       !isLeftBracket(opStack.top().type) &&
                       isBinaryOperator(opStack.top().type) &&
                       getPrecedence(opStack.top().type) >= currentPrec)
                {
                    output.push_back(opStack.top());
                    opStack.pop();
                }
                opStack.push(token);
            }
        }

        while (!opStack.empty())
        {
            output.push_back(opStack.top());
            opStack.pop();
        }

        tokens = output;
        return true;
    }

    bool evaluate(const map<string, bool>& values)
    {
        stack<bool> evalStack;

        for (const Token& token : tokens)
        {
            if (token.type == OpType::NONE)
            {
                auto it = values.find(token.value);
                if (it == values.end())
                {
                    throw runtime_error("Variable not found: " + token.value);
                }
                evalStack.push(it->second);
            }
            else if (token.type == OpType::NOT)
            {
                if (evalStack.empty())
                {
                    throw runtime_error("Not enough operands for NOT");
                }
                bool val = evalStack.top();
                evalStack.pop();
                evalStack.push(!val);
            }
            else if (isBinaryOperator(token.type))
            {
                if (evalStack.size() < 2)
                {
                    throw runtime_error("Not enough operands for binary operator");
                }
                bool b = evalStack.top();
                evalStack.pop();
                bool a = evalStack.top();
                evalStack.pop();

                bool result;
                switch(token.type)
                {
                    case OpType::OR: result = a || b; break;
                    case OpType::AND: result = a && b; break;
                    case OpType::XOR: result = a != b; break;
                    case OpType::EQ: result = a == b; break;
                    case OpType::IMPL: result = !a || b; break;
                    case OpType::NAND: result = !(a && b); break;
                    case OpType::NOR: result = !(a || b); break;
                    default: return false;
                }
                evalStack.push(result);
            }
        }

        if (evalStack.size() != 1)
        {
            throw runtime_error("Invalid expression");
        }

        return evalStack.top();
    }

    bool checkParentheses()
    {
        stack<char> brackets;

        for (char c : rawFormula)
        {
            if (c == '(' || c == '{' || c == '[')
            {
                brackets.push(c);
            }
            else if (c == ')' || c == '}' || c == ']')
            {
                if (brackets.empty())
                {
                    return false;
                }

                char top = brackets.top();
                if ((c == ')' && top != '(') ||
                    (c == '}' && top != '{') ||
                    (c == ']' && top != '['))
                {
                    return false;
                }
                brackets.pop();
            }
        }

        return brackets.empty();
    }

    void collectVariables()
    {
        set<string> uniqueVars;
        for (const Token& token : tokens)
        {
            if (token.type == OpType::NONE)
            {
                uniqueVars.insert(token.value);
            }
        }

        variables.assign(uniqueVars.begin(), uniqueVars.end());
        sort(variables.begin(), variables.end());

        varIndex.clear();
        for (size_t i = 0; i < variables.size(); i++)
        {
            varIndex[variables[i]] = i;
        }
    }

public:
    void read_formula_from_file(const char *filename)
    {
        ifstream file(filename);
        if (!(file.is_open()))
        {
            throw runtime_error("Cannot open file: " + string(filename));
        }

        string line;
        rawFormula.clear();
        while (getline(file, line))
        {
            rawFormula += line + " ";
        }

        size_t start = rawFormula.find_first_not_of(" \t\n\r");
        size_t end = rawFormula.find_last_not_of(" \t\n\r");

        if (start == string::npos)
        {
            throw runtime_error("File is empty");
        }

        rawFormula = rawFormula.substr(start, end - start + 1);
    }

    void read_formula_from_console()
    {
        cout << "Enter boolean formula (variables format: x_1, y_2, etc.):" << endl;
        cout << "Operators: + (OR), & (AND), @ (XOR), ~ (EQ), > (IMPL), | (NAND), ! (NOR), - (NOT)" << endl;
        cout << "Formula: ";

        getline(cin, rawFormula);

        size_t start = rawFormula.find_first_not_of(" \t\n\r");
        size_t end = rawFormula.find_last_not_of(" \t\n\r");

        if (start == string::npos)
        {
            throw runtime_error("Empty formula");
        }

        rawFormula = rawFormula.substr(start, end - start + 1);
    }

    bool parse()
    {
        if (!checkParentheses())
        {
            cerr << "Error: unbalanced parentheses" << endl;
            return false;
        }

        if (!tokenize())
        {
            return false;
        }

        collectVariables();

        if (!toRPN())
        {
            cerr << "Error: failed to convert to RPN" << endl;
            return false;
        }

        return true;
    }

    bool tokenize()
    {
        tokens.clear();
        size_t rawformula_length = rawFormula.length();
        size_t i = 0;
        string var_candidate;
        var_candidate.clear();
        bool inVariable_flag = false;

        while(i < rawformula_length)
        {
            char c = rawFormula[i];

            if (inVariable_flag)
            {
                if (isspace(c))
                {
                    if (is_valid_variable(var_candidate))
                    {
                        tokens.emplace_back(OpType::NONE, var_candidate);
                        var_candidate.clear();
                        inVariable_flag = false;
                        i++;
                        continue;
                    }
                    else
                    {
                        cerr << "Error: invalid variable name: " << var_candidate << endl;
                        return false;
                    }
                }
                else if(isVariableChar(c))
                {
                    var_candidate += c;
                    i++;
                    continue;
                }
                else if (isOperator(c) || isBracket(charToOpType(c)))
                {
                    if (is_valid_variable(var_candidate))
                    {
                        tokens.emplace_back(OpType::NONE, var_candidate);
                        var_candidate.clear();
                        inVariable_flag = false;
                        continue;
                    }
                    else
                    {
                        cerr << "Error: invalid variable name: " << var_candidate << endl;
                        return false;
                    }
                }
                else
                {
                    cerr << "Error: invalid symbol in the boolean formula: '" << c << "'" << endl;
                    return false;
                }
            }
            else
            {
                if (isspace(c))
                {
                    i++;
                    continue;
                }
                else if (isBracket(charToOpType(c)))
                {
                    string symbol(1, c);
                    tokens.emplace_back(charToOpType(c), symbol);
                    i++;
                    continue;
                }
                else if (isOperator(c))
                {
                    string symbol(1, c);
                    tokens.emplace_back(charToOpType(c), symbol);
                    i++;
                    continue;
                }
                else if(isVariableChar(c))
                {
                    inVariable_flag = true;
                    var_candidate = c;
                    i++;
                    continue;
                }
                else
                {
                    cerr << "Error: invalid symbol in the boolean formula: '" << c << "'" << endl;
                    return false;
                }
            }
        }

        if (inVariable_flag)
        {
            if (is_valid_variable(var_candidate))
            {
                tokens.emplace_back(OpType::NONE, var_candidate);
            }
            else
            {
                cerr << "Error: invalid variable name at end: " << var_candidate << endl;
                return false;
            }
        }

        if (tokens.empty())
        {
            cerr << "Error: no tokens found" << endl;
            return false;
        }

        return true;
    }

    void buildTruthTable()
    {
        int n = variables.size();
        int rows = 1 << n;

        truthTable.clear();
        functionValues.clear();

        for (int i = 0; i < rows; i++)
        {
            vector<bool> row(n);
            map<string, bool> values;

            for (int j = 0; j < n; j++)
            {
                row[j] = (i >> (n - 1 - j)) & 1;
                values[variables[j]] = row[j];
            }

            try
            {
                bool funcValue = evaluate(values);
                truthTable.push_back(row);
                functionValues.push_back(funcValue);
            }
            catch (const exception& e)
            {
                cerr << "Error evaluating function: " << e.what() << endl;
                throw;
            }
        }
    }

    void analyzeVariables()
    {
        int n = variables.size();
        int rows = truthTable.size();

        isEssential.resize(n, true);

        for (int varIdx = 0; varIdx < n; varIdx++)
        {
            bool isFictitious = true;

            for (int i = 0; i < rows; i++)
            {
                int oppositeRow = i ^ (1 << (n - 1 - varIdx));

                if (functionValues[i] != functionValues[oppositeRow])
                {
                    isFictitious = false;
                    break;
                }
            }

            isEssential[varIdx] = !isFictitious;
        }
    }

    void removeFictitiousVariables()
    {
        vector<string> newVariables;

        for (size_t i = 0; i < variables.size(); i++)
        {
            if (isEssential[i])
            {
                newVariables.push_back(variables[i]);
            }
        }

        if (newVariables.size() < variables.size())
        {
            cout << "Removed fictitious variables: ";
            for (size_t i = 0; i < variables.size(); i++)
            {
                if (!isEssential[i])
                {
                    cout << variables[i] << " ";
                }
            }
            cout << endl;

            variables = newVariables;
            rebuildTruthTableAfterRemoval();
        }
        else
        {
            cout << "No fictitious variables found" << endl;
        }
    }

    void rebuildTruthTableAfterRemoval()
    {
        buildTruthTable();
        analyzeVariables();
    }

    vector<bool> getDualFunction() const
    {
        vector<bool> dual;
        int rows = functionValues.size();

        for (int i = 0; i < rows; i++)
        {
            int dualIndex = rows - 1 - i;
            dual.push_back(!functionValues[dualIndex]);
        }

        return dual;
    }

    string getSDNF() const
    {
        string sdnf;
        int n = variables.size();
        int rows = functionValues.size();
        bool firstTerm = true;

        for (int i = 0; i < rows; i++)
        {
            if (functionValues[i])
            {
                if (!firstTerm)
                {
                    sdnf += " ∨ ";
                }
                sdnf += "(";

                bool firstVar = true;
                for (int j = 0; j < n; j++)
                {
                    if (!firstVar)
                    {
                        sdnf += " ∧ ";
                    }

                    if (!truthTable[i][j])
                    {
                        sdnf += "¬";
                    }
                    sdnf += variables[j];

                    firstVar = false;
                }
                sdnf += ")";
                firstTerm = false;
            }
        }

        if (sdnf.empty())
        {
            sdnf = "0";
        }

        return sdnf;
    }

    string getSKNF() const
    {
        string sknf;
        int n = variables.size();
        int rows = functionValues.size();
        bool firstTerm = true;

        for (int i = 0; i < rows; i++)
        {
            if (!functionValues[i])
            {
                if (!firstTerm)
                {
                    sknf += " ∧ ";
                }
                sknf += "(";

                bool firstVar = true;
                for (int j = 0; j < n; j++)
                {
                    if (!firstVar)
                    {
                        sknf += " ∨ ";
                    }

                    if (truthTable[i][j])
                    {
                        sknf += "¬";
                    }
                    sknf += variables[j];

                    firstVar = false;
                }
                sknf += ")";
                firstTerm = false;
            }
        }

        if (sknf.empty())
        {
            sknf = "1";
        }

        return sknf;
    }

    string getANF() const
    {
        int n = variables.size();
        int rows = functionValues.size();

        if (rows == 0)
        {
            return "0";
        }

        vector<bool> coeff = functionValues;

        for (int step = 1; step < rows; step <<= 1)
        {
            for (int i = 0; i < rows; i += 2 * step)
            {
                for (int j = i; j < i + step; j++)
                {
                    bool temp = coeff[j];
                    coeff[j] = temp;
                    coeff[j + step] = temp ^ coeff[j + step];
                }
            }
        }

        string anf;
        bool firstTerm = true;

        if (coeff[0])
        {
            anf += "1";
            firstTerm = false;
        }

        for (int i = 1; i < rows; i++)
        {
            if (coeff[i])
            {
                if (!firstTerm)
                {
                    anf += " ⊕ ";
                }

                bool firstVar = true;
                for (int j = 0; j < n; j++)
                {
                    if (i & (1 << (n - 1 - j)))
                    {
                        if (!firstVar)
                        {
                            anf += "·";
                        }
                        anf += variables[j];
                        firstVar = false;
                    }
                }

                if (firstVar)
                {
                    anf += "1";
                }

                firstTerm = false;
            }
        }

        if (anf.empty())
        {
            anf = "0";
        }

        return anf;
    }

    string getDualSKNF() const
    {
        vector<bool> dual = getDualFunction();

        string result;
        int n = variables.size();
        int rows = functionValues.size();
        bool firstTerm = true;

        for (int i = 0; i < rows; i++)
        {
            if (!dual[i])
            {
                if (!firstTerm)
                {
                    result += " ∧ ";
                }
                result += "(";

                bool firstVar = true;
                for (int j = 0; j < n; j++)
                {
                    if (!firstVar)
                    {
                        result += " ∨ ";
                    }

                    if (truthTable[i][j])
                    {
                        result += "¬";
                    }
                    result += variables[j];

                    firstVar = false;
                }
                result += ")";
                firstTerm = false;
            }
        }

        if (result.empty())
        {
            result = "1";
        }

        return result;
    }

    string getDualSDNF() const
    {
        vector<bool> dual = getDualFunction();

        string result;
        int n = variables.size();
        int rows = functionValues.size();
        bool firstTerm = true;

        for (int i = 0; i < rows; i++)
        {
            if (dual[i])
            {
                if (!firstTerm)
                {
                    result += " ∨ ";
                }
                result += "(";

                bool firstVar = true;
                for (int j = 0; j < n; j++)
                {
                    if (!firstVar)
                    {
                        result += " ∧ ";
                    }

                    if (!truthTable[i][j])
                    {
                        result += "¬";
                    }
                    result += variables[j];

                    firstVar = false;
                }
                result += ")";
                firstTerm = false;
            }
        }

        if (result.empty())
        {
            result = "0";
        }

        return result;
    }

    void printTruthTable() const
    {
        cout << "\nTruth Table:" << endl;

        for (const auto& var : variables)
        {
            cout << var << "\t";
        }
        cout << "F" << endl;

        for (size_t i = 0; i < variables.size(); i++)
        {
            cout << "---\t";
        }
        cout << "---" << endl;

        for (size_t i = 0; i < truthTable.size(); i++)
        {
            for (bool val : truthTable[i])
            {
                cout << val << "\t";
            }
            cout << functionValues[i] << endl;
        }
    }

    void printAllInfo()
    {
        cout << "\n=== Boolean Function Analysis ===" << endl;
        cout << "Original formula: " << rawFormula << endl;
        cout << "Variables: ";
        for (const auto& var : variables)
        {
            cout << var << " ";
        }
        cout << endl;

        printTruthTable();

        cout << "\nVariable analysis:" << endl;
        for (size_t i = 0; i < variables.size(); i++)
        {
            cout << variables[i] << ": " << (isEssential[i] ? "essential" : "fictitious") << endl;
        }

        removeFictitiousVariables();

        cout << "\nNormal forms:" << endl;
        cout << "SDNF: " << getSDNF() << endl;
        cout << "SKNF: " << getSKNF() << endl;
        cout << "ANF (Zhegalkin polynomial): " << getANF() << endl;

        cout << "\nDual function:" << endl;
        vector<bool> dual = getDualFunction();
        cout << "Values: ";
        for (bool val : dual)
        {
            cout << val << " ";
        }
        cout << "\nDual function SKNF: " << getDualSKNF() << endl;
        cout << "Dual function SDNF: " << getDualSDNF() << endl;
    }
};

int main(int argc, char* argv[])
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    BooleanFormula formula;

    try
    {
        if (argc >= 2)
        {
            formula.read_formula_from_file(argv[1]);
        }
        else
        {
            formula.read_formula_from_console();
        }

        if (!formula.parse())
        {
            cerr << "Failed to parse formula" << endl;
            return 1;
        }

        formula.buildTruthTable();
        formula.analyzeVariables();
        formula.printAllInfo();
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}