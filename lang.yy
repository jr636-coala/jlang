%skeleton "lalr1.cc"
//%define parser_class_name {lang}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.error verbose
%locations

%token  PLUSASSIGN "+=" MINUSASSIGN "-=" EQUAL "=="
%token  PLUS "+" MINUS "-" PERC "%" STAR "*" SLASH "/" BSLASH "\\" ASSIGN "=" DOT "." ADDR "&" AND "&&" OR "||" COLON ":"
%token  FN "fn" IF "if" ELSE "else" WHILE "while" LET "let" CONST "const" RETURN "return" POD "pod" NS "::"
%token  IDENTIFIER NUMBER STRING

%left   ','
%left   "=" "+=" "-="
%left   "=="
%left   "||"
%left   "&&"
%left   "+" "-" "%"
%left   "*" "/"
%left   "&"
%left   "(" "["
%right "if" "else"
%%

module:                 statements
statements:             statements statement
|                       %empty

statement:              expression ';'
|                       func_def
|                       pod_def
|                       "if" condition statement_or_body "else" statement_or_body
|                       "if" condition statement_or_body
|                       "while" condition statement_or_body
|                       "return" expr_or_list ';'
|                       "let" typed_identifier "=" expr_or_list ';'
|                       "let" typed_identifier ';'
|                       "const" typed_identifier "=" expr_or_list ';'

condition:              '(' expression ')'

statement_or_body:      statement
|                       '{' statements '}'

func_def:               "fn" optional_identifier '(' param_list ')' '{' statements '}'
|                       "fn" optional_identifier '(' param_list ')' ":" type_mod '{' statements '}'

optional_identifier:    IDENTIFIER
|                       %empty

pod_def:                "pod" IDENTIFIER '{' statements '}'

param_list:             param_list_impl
|                       %empty
param_list_impl:        param_list_impl ',' typed_identifier
|                       typed_identifier

typed_identifier:       IDENTIFIER ":" type_mod
|                       IDENTIFIER

type_mod:               IDENTIFIER type_mod_impl
type_mod_impl:          type_mod_impl "*"
|                       "&"
|                       %empty

expression:             "::" '{' statements '}'
|                       IDENTIFIER '(' expression_list ')'
|                       IDENTIFIER index
|                       pod_access
|                       '(' expression ')'
|                       func_def
|                       expression "+" expression
|                       expression "=" expression
|                       expression "+=" expression
|                       expression "-=" expression
|                       expression "==" expression
|                       expression "-" expression
|                       expression "*" expression
|                       expression "/" expression
|                       expression "%" expression
|                       expression "&&" expression
|                       expression "||" expression
|                       "*" expression  %prec "&"
|                       "&" expression  %prec "&"
|                       "+" expression  %prec "&"
|                       "-" expression  %prec "&"
//|                       IDENTIFIER
|                       NUMBER
|                       STRING

expr_or_list:           '{' expression_list '}'
|                       expression
expression_list:        expression_list_impl
|                       %empty
expression_list_impl:   expression_list_impl ',' expression
|                       expression

pod_access:             pod_access "." IDENTIFIER
|                       IDENTIFIER

index:                  index '[' expression ']'
|                       %empty

%%