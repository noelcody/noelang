#include "noelang.h"

lval* builtin_op(lval* v, char* op) {
  for (int i = 0; i < v->count; i++) {
    if (v->cell[i]->type != LVAL_NUM) {
      lval_del(v);
      return lval_err("op input must be numbers");
    }
  }
  
  lval* first_num = lval_pop(v, 0);
  if ((strcmp(op, "-") == 0) && v->count == 0) {
    first_num->num = -first_num->num;
  }

  // iterate children
  while (v->count > 0) {
    lval* next_num = lval_pop(v, 0);

    if (strcmp(op, "+") == 0) { first_num->num += next_num->num; }
    if (strcmp(op, "-") == 0) { first_num->num -= next_num->num; }
    if (strcmp(op, "*") == 0) { first_num->num *= next_num->num; }
    if (strcmp(op, "/") == 0) {
      if (next_num->num == 0) {
        lval_del(first_num); lval_del(next_num);
        first_num = lval_err("division by zero fail"); break;
      }
      first_num->num /= next_num->num;
    }
    lval_del(next_num);
  }
  lval_del(v); return first_num;
}

lval* builtin_head(lval* a) {
  LASSERT(a, a->count == 1,
    "head takes one arg");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "head takes qexpr");
  LASSERT(a, a->cell[0]->count != 0,
    "head passed empty qexpr");

  lval* v = lval_take(a, 0);  
  while (v->count > 1) { lval_del(lval_pop(v, 1)); }
  return v;
}

lval* builtin_tail(lval* a) {
  LASSERT(a, a->count == 1,
    "tail takes one arg");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "tail takes qexpr");
  LASSERT(a, a->cell[0]->count != 0,
    "tail passed empty qexpr");

  lval* v = lval_take(a, 0);  
  while (v->count > 1) { lval_del(lval_pop(v, 0)); }
  return v;
}

lval* lval_join(lval* x, lval* y) {

  /* For each cell in 'y' add it to 'x' */
  while (y->count) {
    x = lval_add(x, lval_pop(y, 0));
  }

  /* Delete the empty 'y' and return 'x' */
  lval_del(y);  
  return x;
}

lval* builtin_join(lval* a) {
  for (int i = 0; i < a->count; i++) {
    LASSERT(a, a->cell[i]->type == LVAL_QEXPR, "join takes qexpr");
  }

  lval* x = lval_pop(a, 0);

  while (a->count) {
    x = lval_join(x, lval_pop(a, 0));
  }

  lval_del(a);
  return x;
}

lval* builtin_list(lval* a) {
  a->type = LVAL_QEXPR;
  return a;
}

lval* builtin_eval(lval* a) {
  LASSERT(a, a->count == 1,
    "eval takes one arg");
  LASSERT(a, a->cell[0]->type == LVAL_QEXPR,
    "eval takes qexpr");

  lval* x = lval_take(a, 0);
  x->type = LVAL_SEXPR;
  return lval_eval(x);
}

lval* builtin(lval* a, char* func) {
  if (strcmp("join", func) == 0) { return builtin_join(a); }
  if (strcmp("list", func) == 0) { return builtin_list(a); }
  if (strcmp("eval", func) == 0) { return builtin_eval(a); }
  if (strcmp("tail", func) == 0) { return builtin_tail(a); }
  if (strcmp("head", func) == 0) { return builtin_head(a); }
  if (strstr("+-/*", func)) { return builtin_op(a, func); }
  lval_del(a);
  return lval_err("Unknown Function!");
}

lval* lval_eval_sexpr(lval* v) {
  // eval all children to lval
  for (int i = 0; i < v->count; i++) {
    v->cell[i] = lval_eval(v->cell[i]);
  }

  // check lvals for errors
  for (int i = 0; i < v->count; i++) {
    if (v->cell[i]->type == LVAL_ERR) { return lval_take(v, i); }
  }

  if (v->count == 0) { return v; }
  if (v->count == 1) { return lval_take(v, 0); }

  // ensure 1st element is a symbol
  lval* f = lval_pop(v, 0);
  if (f->type != LVAL_SYM) {
    lval_del(f); lval_del(v);
    return lval_err("S-expression does not start with symbol");
  }

  lval* result = builtin(v, f->sym);
  lval_del(f);
  return result;
}

lval* lval_eval(lval* v) {
  // eval sexpr only
  if (v->type == LVAL_SEXPR) { return lval_eval_sexpr(v); }
  return v;
}

