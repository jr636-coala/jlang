//
// Created by glover on 08/03/2022.
//

#include "interpreter.hpp"
#include "log.hpp"
#include <memory>


using namespace AST;

std::unordered_map<std::string, Interpreter::compilerfunc_t> Interpreter::compilerFuncs = {};

TypeVal Interpreter::interp() {
    if (!currentScope) delete currentScope;
    currentScope = new Scope;
    return run(program);
    // We should have our entry now
    //auto c = std::make_unique<Call>();
    //c->expression = new Identifier();
    //static_cast<Identifier*>(c->expression)->identifier = entry;
    //c->arguments = new ExpressionList();
    //return run(c.get());
}
TypeVal Interpreter::interpModule() {
    currentScope = new Scope;
    return run(program);
}

TypeVal Interpreter::run(Node node) {
    switch(node.type) {
        case Node::Type::addr:          return TypeVal();
        case Node::Type::andd:          return TypeVal();
        case Node::Type::call:          return TypeVal();
        case Node::Type::constt:        return TypeVal();
        case Node::Type::deref:         return TypeVal();
        case Node::Type::equal:         return TypeVal();
        case Node::Type::func_def:      return TypeVal();
        case Node::Type::identifier:    return TypeVal();
        case Node::Type::iff:           return run_if(node);
        case Node::Type::index:         return TypeVal();
        case Node::Type::let:           return TypeVal();
        case Node::Type::list:          return TypeVal();
        case Node::Type::ns:            return TypeVal();
        case Node::Type::orr:           return TypeVal();
        case Node::Type::pod_access:    return TypeVal();
        case Node::Type::pod_def:       return TypeVal();
        case Node::Type::returnn:       return TypeVal();
        case Node::Type::whilee:        return TypeVal();

        case Node::Type::assign:
        case Node::Type::plusassign:
        case Node::Type::minusassign:   return TypeVal();

        case Node::Type::minus:
        case Node::Type::plus:
        case Node::Type::star:
        case Node::Type::slash:
        case Node::Type::perc:          return run_binOp(node);

        case Node::Type::number:               return TypeVal(std::stoll((node.value)));
        case Node::Type::string:               return TypeVal(node.value);
    }
    Log::error("Unhandled ast node type \"" + Node::type_to_string(node.type) + "\"", node.loc);
}

TypeVal Interpreter::run_list(Node list) {
    for (auto i = 0; i < list.params.size(); ++i) {
        const auto statement = list.params[i];
        if (statement.type == Node::Type::returnn) return run(statement);
        run(statement);
    }
}

TypeVal Interpreter::run_call(Node call) {

    std::vector<TypeVal> args;
    const auto& arg_exps = call.params[0];
    args.reserve(arg_exps.params.size());
    for (auto i = 0; i < arg_exps.params.size(); ++i) args[i] = run(arg_exps.params[i]); 
    TypeVal ret;

    if (call.identifier.name[0] == '#') {
        // Call compiler function
        if (compilerFuncs.contains(call.identifier.name)) {
            ret = (*compilerFuncs[call.identifier.name])(*this, call.loc, args);
        }
    }
    else {
        // Run the non compiler function
        // Does not support args atm
        // Also assume that it is an identifier call
        const auto [func_v, scope] = currentScope->search(call.identifier.name);
        if (!func_v.has_value() || func_v.value().type != TypeT::fn)
            Log::error("function \"" + call.identifier.name + "\" does not exist", call.loc);
        const auto func = func_v.value().fn->val;

        // Replace scope so that private areas can be accessed (this should be
        // sorted earlier on in a type checker so that we have no need to check)
        auto t_scope = this->currentScope;
        // Create new function scope
        auto f_scope = new Scope();
        f_scope->parentScope = scope;
        const auto& fn_params = func.params[0].params;
        for (auto i = 0; i < fn_params.size(); ++i) {
            f_scope->val[fn_params[i].identifier.name] = args[i];
        }
        this->currentScope = f_scope;
        ret = run(func.params[1]);
        delete f_scope;
        this->currentScope = t_scope;
    }
    return ret;
}

TypeVal Interpreter::run_funcDef(Node func) {
    return this->currentScope->val[func.identifier.name] = TypeVal(/*func*/);
}

TypeVal Interpreter::run_varDef(Node def) {
    return this->currentScope->val[def.identifier.name] = run(def.params[0]);
}

TypeVal Interpreter::run_binOp(Node op) {
    auto l = run(op.params[0]);
    auto r = run(op.params[1]);
    if (l.type == TypeT::unknown || r.type == TypeT::unknown)
        Log::error("Unknown types for binary operator", op.loc);
    switch (op.type) {
        case Node::Type::plus: return TypeVal::plus(l, r);
        case Node::Type::minus: return TypeVal::minus(l, r);
        case Node::Type::star: return TypeVal::star(l, r);
        case Node::Type::slash: return TypeVal::slash(l, r);
        case Node::Type::assign: {
            // FIXME horrible
            auto [_,scope] = this->currentScope->search(op.params[0].identifier.name); // FIXME Extremely scuffed atm
            scope->val[op.params[0].identifier.name] = r;
            return r;
        }
        case Node::Type::plusassign: {
            auto [_, scope] = this->currentScope->search(op.params[0].identifier.name);
            return TypeVal::plusEqual(scope->val[op.params[0].identifier.name], r);
        }
        case Node::Type::minusassign: {
            auto [_, scope] = this->currentScope->search(op.params[0].identifier.name);
            return TypeVal::minusEqual(scope->val[op.params[0].identifier.name], r);
        }
    }
    Log::error("No binary operator \"" + Node::type_to_string(op.type) + "\"", op.loc);
    exit(0);
}

TypeVal Interpreter::run_identifier(Node identifier) {
    auto [_v,_] = currentScope->search(identifier.identifier.name);
    if (!_v.has_value()) {
        Log::error("Identifier \"" + identifier.identifier.name + "\" does not exist", identifier.loc);
        exit(0);
    }
    auto v = _v.value();
    return v;
}

// TODO this needs fixing up
TypeVal Interpreter::run_nsDef(Node def) {
    auto ns = TypeVal(TypeT::ns);
    ns.ns->parentScope = this->currentScope;
    auto this_scope = this->currentScope;
    this->currentScope = ns.ns;
    for (const auto& x : def.params) run(x);
    // Hide everything
    this->currentScope = this_scope;

    // Demangle names and copy to outer scope the ones we care about
    /*auto& ns_vars = ns.ns->val;
    for (auto i = ns_vars.begin(); i != ns_vars.end(); ++i) {
        auto [id, var] = *i;
        if (id[0] == ':') {
            ns_vars[id.substr(2)] = var;
        }
    }*/
    return ns;
}
TypeVal Interpreter::run_podDef(Node def) {
    return {};
    /*
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
    return pod;*/
}

TypeVal Interpreter::run_nsmemDec(Node def) {
    auto var = run(def.params[0]);
    var.constant = true;
    this->currentScope->val["::" + def.identifier.name] = var;
    return var;
}

TypeVal Interpreter::run_constDef(Node def) {
    auto var = run(def.params[0]);
    var.constant = true;
    this->currentScope->val[def.identifier.name] = var;
    return var;
}

TypeVal Interpreter::run_if(Node def) {
    auto condition = run(def.params[0]);
    if (valueIsTrue(condition)) {
        return run(def.params[1]);
    }
    else if (def.params.size() > 2) {
        return run(def.params[2]);
    }
    return {};
}

TypeVal Interpreter::run_while(Node def) {
    while(valueIsTrue(run(def.params[0]))) run(def.params[1]);
    return {};
}

bool Interpreter::valueIsTrue(TypeVal val) {
    bool met = false;
    switch (val.type) {
        case TypeT::i64: met = *val.i64; break;
        case TypeT::string: met = val.string->val.length(); break;
    }
    return met;
}
