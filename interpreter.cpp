//
// Created by glover on 08/03/2022.
//

#include "interpreter.hpp"
#include "log.hpp"
#include <memory>


using namespace AST;

std::unordered_map<std::string, Interpreter::compilerfunc_t> Interpreter::compilerFuncs = {};

void Interpreter::interp() {
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

Variable Interpreter::run(Node* node) {
    switch(node->type) {
        case NodeType::statementlist:
            return run((StatementList*)node);
        case NodeType::call:
            return run((Call*)node);
        case NodeType::functiondefinition:
            return run((FunctionDefinition*)node);
        case NodeType::variabledefinition:
            return run((VariableDefinition*)node);
        case NodeType::constdefintiion:
            return run((ConstDefinition*)node);
        case NodeType::number:
            return Variable(std::stoi(((Number*)node)->number));
        case NodeType::string:
            return Variable(((String*)node)->string);
        case NodeType::identifier:
            return run((Identifier*)node);
        case NodeType::binaryoperator:
            return run((BinaryOperator*)node);
        case NodeType::namespacedeclaration:
            return run((NamespaceDeclaration*)node);
    }
    Log::error("Unhandled ast node type \"" + node_type_to_string(node->type) + "\"", node->loc);
}

Variable Interpreter::run(StatementList* list) {
    for (auto i = 0; i < list->statements.size(); ++i) {
        const auto statement = list->statements[i];
        //if (statement->type == Token::return) return run((Return*)statment);
        run((Node*)statement);
    }
    return {};
}

std::vector<Variable> Interpreter::resolveExpressionList(ExpressionList* list) {
    std::vector<Variable> out;
    out.reserve(list->expressions.size());
    for (auto i = 0; i < list->expressions.size(); ++i) {
        const auto expression = list->expressions[i];
        out.push_back(run(expression));
    }
    return out;
}

Variable Interpreter::run(Call* call) {

    auto args = resolveExpressionList(call->arguments);

    const auto& identifier = static_cast<Identifier*>(call->expression)->identifier;

    if (call->expression->type == NodeType::identifier && identifier[0] == '#') {
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
        const auto func_v = currentScope->search(identifier);
        if (!func_v.has_value() || func_v.value().type != Variable::TYPE::FUNC)
            Log::error("function" + identifier + "does not exist", call->loc);
        const auto func = func_v.value().func;
        return run(func->body);
    }
    return {};
}

Variable Interpreter::run(FunctionDefinition* func) {
    this->currentScope->functions[func->identifier] = func;
    return {func};
}

Variable Interpreter::run(AST::VariableDefinition* def) {
    auto val =  run(def->expression);
    this->currentScope->variables[def->identifier] = val;
    return val;
}

Variable Interpreter::run(AST::BinaryOperator *op) {
    auto l = run(op->l);
    auto r = run(op->r);
    switch (op->op) {
        // TODO only handling numbers
        case Token::plus: {
            return Variable(l.i + r.i);
        }
        case Token::star: {
            return Variable(l.i * r.i);
        }
    }
    Log::error("No binary operator \"" + token_to_string(op->op) + "\"", op->loc);
}

Variable Interpreter::run(AST::Identifier *identifier) {
    auto& str = static_cast<Identifier*>(identifier)->identifier;
    auto _v = currentScope->search(str);
    if (!_v.has_value()) {
        Log::error("Identifier \"" + str + "\" does not exist", identifier->loc);
        exit(0);
    }
    auto v = _v.value();
    return v;
}

Variable Interpreter::run(AST::NamespaceDeclaration *def) {
    return Variable();
}

Variable Interpreter::run(AST::ConstDefinition *def) {
    return Variable();
}
