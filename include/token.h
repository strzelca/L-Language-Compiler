#ifndef TOKEN_H
#define TOKEN_H

enum Token {
  tok_eof = -1,

  // commands
  // C ::= Id = E | C;C | if (E) {C} [else {C}] | while (E) {C} | D;C | return E
  tok_id = -2,
  tok_if = -3,
  tok_else = -4,
  tok_while = -5,
  tok_return = -6,

  // expressions
  // E ::= v | Id | uop E | E bop E | (E) | Id(ae)
  tok_val = -7,
  tok_uop = -8,
  tok_bop = -8,
  tok_call = -10,

  // declarations
  // D ::= nil | const Id[:T] = E | var Id[:T] = E | D;D | function Id(form) ->
  // T {C; return E} | form = ae
  tok_const = -11,
  tok_var = -12,
  tok_fun = -13,
  tok_form = -14,

  // form
  // form ::= const Id: T, form | var Id: T, form

  // extern
  // extern ::= extern Id(form);
  tok_extern = -15

  // ae
  // ae ::= E, ae
};

#endif