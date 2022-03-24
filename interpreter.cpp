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
        case NodeType::nsmemberdeclaration:
            return run((NSMemberDeclaration*)node);
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
        const auto [func_v, scope] = currentScope->search(identifier);
        if (!func_v.has_value() || func_v.value().type != Variable::TYPE::FUNC)
            Log::error("function \"" + identifier + "\" does not exist", call->loc);
        const auto func = func_v.value().func;

        // Replace scope so that private areas can be accessed (this should be
        // sorted earlier on in a type checker so that we have no need to check)
        auto t_scope = this->currentScope;
        this->currentScope = scope;
        auto ret = run(func->body);
        this->currentScope = t_scope;
        return ret;
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
        case Token::assign: {
            // FIXME horrible
            auto id = (Identifier*)op->l;
            auto [var,scope] = this->currentScope->search(id->identifier); // FIXME Extremely scuffed atm
            if (var.has_value()) {
                scope->variables[id->identifier] = r;
            }
            else {
                Log::warning("Attempt to set undefined variable \"" + l.s + "\"", op->loc);
            }
            return r;
        }
    }
    Log::error("No binary operator \"" + token_to_string(op->op) + "\"", op->loc);
}

Variable Interpreter::run(AST::Identifier *identifier) {
    auto& str = static_cast<Identifier*>(identifier)->identifier;
    auto [_v,_] = currentScope->search(str);
    if (!_v.has_value()) {
        Log::error("Identifier \"" + str + "\" does not exist", identifier->loc);
        exit(0);
    }
    auto v = _v.value();
    return v;
}

// TODO this needs fixing up
Variable Interpreter::run(AST::NamespaceDeclaration *def) {
    auto ns = Variable();
    ns.type = Variable::TYPE::NS;
    ns.ns = new Scope();
    auto this_scope = this->currentScope;
    this->currentScope = ns.ns;
    run(def->statements);
    // Hide everything
    auto ns_inner_scope = Variable();
    ns_inner_scope.type = Variable::TYPE::NS;
    ns_inner_scope.ns = this->currentScope;
    ns_inner_scope.ns->is_ns = true;
    auto ns_scope = new Scope();
    ns_scope->variables[""] = ns_inner_scope;
    this->currentScope = this_scope;
    ns.ns = ns_scope;

    // Demangle names and copy to outer scope the ones we care about
    auto& ns_vars = ns_inner_scope.ns->variables;
    for (auto i = ns_vars.begin(); i != ns_vars.end(); ++i) {
        auto [id, var] = *i;
        if (id[0] == ':') {
            ns_scope->variables[id.substr(2)] = var;
            ns_vars[id.substr(2)] = var;
        }
    }
    return ns;
}

Variable Interpreter::run(AST::NSMemberDeclaration* def) {
    auto var = run(def->expression);
    var.constant = true;
    this->currentScope->variables["::" + def->identifier] = var;
    return var;
}

Variable Interpreter::run(AST::ConstDefinition *def) {
    auto var = run(def->expression);
    var.constant = true;
    this->currentScope->variables[def->identifier] = var;
    return var;
}
