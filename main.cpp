#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <algorithm>
#include <numeric>
#include <optional>
#include <unordered_map>
#include <variant>
#include "parser.hpp"
#include <memory>

using namespace AST;


void printTokens(auto& tokens) {
    for (auto [ match, token ] : tokens) std::cout << match << " : " << token << '\n';
}

struct Variable {
    Variable() : type(TYPE::UNKNOWN) {};
    Variable(const std::string& str) : type(TYPE::STRING), s(str) {};
    Variable(int x) : type(TYPE::INTEGER), i(x) {};
    enum class TYPE { UNKNOWN, STRING, INTEGER, ID } type;
    int i;
    std::string s;
};

class Interpreter;

std::unordered_map<std::string, void(*)(Interpreter&, const std::vector<Variable>&)> compilerFuncs;
struct Scope {
    Scope* parentScope {nullptr};
    std::unordered_map<std::string, FunctionDefinition*> functions;
    std::unordered_map<std::string, Variable> variables;

    std::variant<FunctionDefinition*, Variable, bool> search(std::string str) {
        if (functions.contains(str)) return functions[str];
        if (variables.contains(str)) return variables[str];
        if (!parentScope) return false;
        return parentScope->search(str);
    }
};

class Interpreter {
public:
    Interpreter(auto* program) : program(program) {}

    void run() {
        if (!currentScope) delete currentScope;
        currentScope = new Scope;
        run(program);
        // We should have our entry now
        auto c = std::make_unique<Call>();
        c.get()->expression = new Identifier();
        static_cast<Identifier*>(c.get()->expression)->identifier = entry;
        c.get()->arguments = new ExpressionList();
        run(c.get());
    }

    std::string entry;

private:

    void run(Node* node) {
        switch(node->type) {
            case NodeType::statementlist:
                return run((StatementList*)node);
            case NodeType::call:
                return run((Call*)node);
            case NodeType::functiondefinition:
                return run((FunctionDefinition*)node);
        }
        std::cout << "ERROR : " << node->type << '\n';
    }

    void run(StatementList* list) {
        for (auto i = 0; i < list->statements.size(); ++i) {
            const auto statement = list->statements[i];
            run((Node*)statement);
        }
    }

    std::vector<Variable> resolveExpressionList(ExpressionList* list) {
        std::vector<Variable> out;
        out.reserve(list->expressions.size());
        for (auto i = 0; i < list->expressions.size(); ++i) {
            const auto expression = list->expressions[i];
            if (expression->type == NodeType::string) {
                // Only handling strings atm
                out.push_back(((String*)expression)->string);
            }
            else if (expression->type == NodeType::identifier) {
                auto v = Variable{((Identifier*)expression)->identifier};
                v.type = Variable::TYPE::ID;
                out.push_back(v);
            }
        }
        return out;
    }

    void run(Call* call) {

        auto args = resolveExpressionList(call->arguments);

        if (call->expression->type == NodeType::identifier && reinterpret_cast<Identifier*>(call->expression)->identifier[0] == '#') {
            // Call compiler function
            const auto& _call = reinterpret_cast<Identifier*>(call->expression);
            if (compilerFuncs.contains(_call->identifier)) {
                (*compilerFuncs[_call->identifier])(*this, args);
            }
        }
        else {
            // Run the non compiler function
            // Does not support args atm
            // Also assume that it is an identifier call
            const auto func_v = currentScope->search(static_cast<Identifier*>(call->expression)->identifier);
            if (!std::holds_alternative<FunctionDefinition*>(func_v)) std::cout << "error : function does not exist\n";
            const auto func = std::get<FunctionDefinition*>(func_v);
            run(func->body);
        }
    }

    void run(FunctionDefinition* func) {
        this->currentScope->functions[func->identifier] = func;
    }

    Scope* currentScope;
    StatementList* program;
};

int main(int argc, char** argv) {
    auto tokeniser = Tokeniser("#entry(main);\n"
                               "#output(\"Scuffed henlo\");\n"
                               "fn main() {\n"
                               "#output(\"Hello, World!\");\n"
                               "}");
    auto tokens = tokeniser.getTokens();
    //printTokens(tokens);
    auto parser = Parser(tokens);
    auto ast = parser.parse();
    Parser::printAST(ast);

    // Define compilerFuncs
    compilerFuncs.insert({"#entry", [](auto& inter, const auto& args){
        inter.entry = args[0].s;
        std::cout << args[0].s << " : entry " << '\n';
    }});
    compilerFuncs.insert({"#output", [](auto&, const auto& args){
        for (auto i = 0; i < args.size(); ++i) {
            std::cout << args[i].s;
        }
        std::cout << '\n';
    }});

    auto interpreter = Interpreter(ast);
    interpreter.run();

    return 0;
}
