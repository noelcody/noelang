#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>
#include "noelang.h"

int main(int argc, char** argv) { 
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Symbol = mpc_new("symbol");
  mpc_parser_t* Sexpr = mpc_new("sexpr");
  mpc_parser_t* Qexpr = mpc_new("qexpr");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* NoeLang = mpc_new("noelang");

  mpca_lang(MPCA_LANG_DEFAULT,
  "\
    number : /-?[0-9]+/ ;\
    symbol : /[a-zA-Z0-9_+\\-*\\/\\\\=<>!&]+/ ;         \
    sexpr : '(' <expr>* ')';\
    qexpr : '{' <expr>* '}';\
    expr : <number> | <symbol> | <sexpr> | <qexpr> ;\
    noelang : /^/ <expr>* /$/ ;\
  ",
  Number, Symbol, Sexpr, Qexpr, Expr, NoeLang);
   
  while (1) {
    char* input = readline("noelang> ");
    
    add_history(input);
    
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, NoeLang, &r)) {
      lval* x = lval_eval(lval_read(r.output));
      lval_println(x);
      lval_del(x);
    }
    free(input);
  }

  mpc_cleanup(5, Number, Symbol, Sexpr, Qexpr, Expr, NoeLang);
  return 0;
}