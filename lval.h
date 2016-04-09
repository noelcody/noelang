#include "mpc/mpc.h"

struct lval;
struct lenv;
typedef struct lval lval;
typedef struct lenv lenv;
typedef lval*(*lbuiltin)(lenv*, lval*);

enum { LVAL_ERR, LVAL_NUM, LVAL_SYM, LVAL_SEXPR, LVAL_QEXPR, LVAL_FUN };

struct lval {
  int type;
  long num;
  
  char* err;
  char* sym;
  lbuiltin fun;

  // subtree
  struct lval** cell;
  int count;
};


struct lenv {
  int count;
  char** syms;
  lval** vals;
};

#define LASSERT(args, cond, err) \
  if (!(cond)) { lval_del(args); return lval_err(err); }

lval* lval_read_num(mpc_ast_t* mpc_with_num);
lval* lval_num(long x);
lval* lval_err(char* m);
lval* lval_sym(char* s);
lval* lval_sexpr(void);
lval* lval_qexpr(void);
lval* lval_fun(lbuiltin fun);

void lval_del(lval* v);
lval* lval_add(lval* parent, lval* new_child);
lval* lval_read(mpc_ast_t* t);

void lval_expr_print(lval* v, char open, char close);
void lval_print(lval* v);
void lval_println(lval* v);

lval* lval_pop(lval* v, int i);
lval* lval_take(lval* v, int i);
lval* lval_copy(lval* v);