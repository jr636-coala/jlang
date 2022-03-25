//
// Created by glover on 08/03/2022.
//

#include "interpreter.hpp"
#include "log.hpp"
#include <memory>


using namespace AST;

std::unordered_map<std::string, Interpreter::compilerfunc_t> Interpreter::compilerFuncs = {};

Type Interpreter::interp() {
    if (!currentScope) delete currentScope;
    currentScope = new Scope;
    run(program);
    // We should have our entry now
    auto c = std::make_unique<Call>();
    c->expression = new Identifier();
    static_cast<Identifier*>(c->expression)->identifier = entry;
    c->arguments = new ExpressionList();
    return run(c.get());
}
Type Interpreter::interpModule() {
    currentScope = new Scope;
    return run(program);;
}

Type Interpreter::run(Node* node) {
    switch(node->type) {
        case NodeType::statementlist:        return run((StatementList*)node);
        case NodeType::call:                 return run((Call*)node);
        case NodeType::functiondefinition:   return run((FunctionDefinition*)node);
        case NodeType::variabledefinition:   return run((VariableDefinition*)node);
        case NodeType::constdefintiion:      return run((ConstDefinition*)node);
        case NodeType::identifier:           return run((Identifier*)node);
        case NodeType::binaryoperator:       return run((BinaryOperator*)node);
        case NodeType::namespacedeclaration: return run((NamespaceDeclaration*)node);
        case NodeType::poddefinition:        return run((PodDefinition*)node);
        case NodeType::nsmemberdeclaration:  return run((NSMemberDeclaration*)node);
        case NodeType::conditionalstatement: return run((ConditionalStatement*)node);
        case NodeType::whilestatement:       return run((WhileStatement*)node);

        case NodeType::number:               return Type(std::stoll(((Number*)node)->number));
        case NodeType::string:               return Type(((String*)node)->string);
    }
    Log::error("Unhandled ast node type \"" + node_type_to_string(node->type) + "\"", node->loc);
}

Type Interpreter::run(StatementList* list) {
    for (auto i = 0; i < list->statements.size(); ++i) {
        const auto statement = list->statements[i];
        if (statement->type == NodeType::returnstatement) return run(((ReturnStatement*)statement)->expression);
        run((Node*)statement);
    }
}

std::vector<Type> Interpreter::resolveExpressionList(ExpressionList* list) {
    std::vector<Type> out;
    out.reserve(list->expressions.size());
    for (auto i = 0; i < list->expressions.size(); ++i) {
        const auto expression = list->expressions[i];
        out.push_back(run(expression));
    }
    return out;
}

Type Interpreter::run(Call* call) {

    auto args = resolveExpressionList(call->arguments);

    const auto& identifier = static_cast<Identifier*>(call->expression)->identifier;

    Type ret;

    if (call->expression->type == NodeType::identifier && identifier[0] == '#') {
        // Call compiler function
        const auto& _call = reinterpret_cast<Identifier*>(call->expression);
        if (compilerFuncs.contains(_call->identifier)) {
            ret = (*compilerFuncs[_call->identifier])(*this, call->loc, args);
        }
    }
    else {
        // Run the non compiler function
        // Does not support args atm
        // Also assume that it is an identifier call
        const auto [func_v, scope] = currentScope->search(identifier);
        if (!func_v.has_value() || func_v.value().type != TypeT::fn)
            Log::error("function \"" + identifier + "\" does not exist", call->loc);
        const auto func = func_v.value().fn->val;

        // Replace scope so that private areas can be accessed (this should be
        // sorted earlier on in a type checker so that we have no need to check)
        auto t_scope = this->currentScope;
        // Create new function scope
        auto f_scope = new Scope();
        f_scope->parentScope = scope;
        for (auto i = 0; i < func->parameters.size(); ++i) {
            f_scope->val[func->parameters[i].second] = args[i];
        }
        this->currentScope = f_scope;
        ret = run(func->body);
        delete f_scope;
        this->currentScope = t_scope;
    }
    return ret;
}

Type Interpreter::run(FunctionDefinition* func) {
    return this->currentScope->val[func->identifier] = Type(func);;
}

Type Interpreter::run(AST::VariableDefinition* def) {
    return this->currentScope->val[def->identifier] = run(def->expression);
}

Type Interpreter::run(AST::BinaryOperator *op) {
    auto l = run(op->l);
    auto r = run(op->r);
    if (l.type == TypeT::unknown || r.type == TypeT::unknown)
        Log::error("Unknown types for binary operator", op->loc);
    switch (op->op) {
        case Token::plus: return Type::plus(l, r);
        case Token::minus: return Type::minus(l, r);
        case Token::star: return Type::star(l, r);
        case Token::slash: return Type::slash(l, r);
        case Token::assign: {
            // FIXME horrible
            auto id = (Identifier*)op->l;
            auto [_,scope] = this->currentScope->search(id->identifier); // FIXME Extremely scuffed atm
            scope->val[id->identifier] = r;
            return r;
        }
        case Token::plusequal: {
            auto id = (Identifier*)op->l;
            auto [_, scope] = this->currentScope->search(id->identifier);
            return Type::plusEqual(scope->val[id->identifier], r);
        }
        case Token::minusequal: {
            auto id = (Identifier*)op->l;
            auto [_, scope] = this->currentScope->search(id->identifier);
            return Type::minusEqual(scope->val[id->identifier], r);
        }
    }
    Log::error("No binary operator \"" + token_to_string(op->op) + "\"", op->loc);
    exit(0);
}

Type Interpreter::run(AST::Identifier *identifier) {
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
Type Interpreter::run(AST::NamespaceDeclaration *def) {
    auto ns = Type(TypeT::ns);
    ns.ns->parentScope = this->currentScope;
    auto this_scope = this->currentScope;
    this->currentScope = ns.ns;
    run(def->statements);
    // Hide everything
    this->currentScope = this_scope;

    // Demangle names and copy to outer scope the ones we care about
    auto& ns_vars = ns.ns->val;
    for (auto i = ns_vars.begin(); i != ns_vars.end(); ++i) {
        auto [id, var] = *i;
        if (id[0] == ':') {
            ns_vars[id.substr(2)] = var;
        }
    }
    return ns;
}
Type Interpreter::run(AST::PodDefinition *def) {
    auto pod = Type(TypeT::pod);
    pod.pod->val.name = def->name;
    pod.pod->val.content.parentScope = this->currentScope;
    auto this_scope = this->currentScope;
    this->currentScope = &pod.pod->val.content;
    run(def->body);
    // Hide everything
    this->currentScope = this_scope;

    // Demangle names and copy to outer scope the ones we care about
    auto& pod_vars = pod.pod->val.content.val;
    for (auto i = pod_vars.begin(); i != pod_vars.end(); ++i) {
        auto [id, var] = *i;
        if (id[0] == ':') {
            pod_vars[id.substr(2)] = var;
        }
    }
    return pod;
}

Type Interpreter::run(AST::NSMemberDeclaration* def) {
    auto var = run(def->expression);
    var.constant = true;
    this->currentScope->val["::" + def->identifier] = var;
    return var;
}

Type Interpreter::run(AST::ConstDefinition *def) {
    auto var = run(def->expression);
    var.constant = true;
    this->currentScope->val[def->identifier] = var;
    return var;
}

Type Interpreter::run(AST::ConditionalStatement* def) {
    auto condition = run(def->condition);
    if (valueIsTrue(condition)) {
        return run(def->_true);
    }
    else if (def->_else) {
        return run(def->_else);
    }
    return {};
}

Type Interpreter::run(AST::WhileStatement* def) {
    while(valueIsTrue(run(def->condition))) run(def->body);
    return {};
}

bool Interpreter::valueIsTrue(Type val) {
    bool met = false;
    switch (val.type) {
        case TypeT::i64: met = *val.i64; break;
        case TypeT::string: met = val.string->val.length(); break;
    }
    return met;
}
