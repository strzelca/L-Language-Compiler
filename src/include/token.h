#ifndef TOKEN_H
#define TOKEN_H

enum Token {
  tok_eof = -1,

  // commands
  // C ::= Id = E | C;C | if (E) {C} [else {C}] | while (E) {C} | D;C | return E
  tok_id = -2,
  tok_if = -3,
  tok_while = -4,
  tok_return = -5,

  // expressions
  // E ::= v | Id | uop E | E bop E | (E) | Id(ae)
  tok_val = -6,
  tok_uop = -7,
  tok_bop = -8,
  tok_call = -9,

  // declarations
  // D ::= nil | const Id[:T] = E | var Id[:T] = E | D;D | function Id(form) ->
  // T {C; return E} | form = ae
  tok_const = -10,
  tok_var = -11,
  tok_fun = -12,
  tok_form = -13

  // form
  // form ::= const Id: T, form | var Id: T, form

  // ae
  // ae ::= E, ae
};

#endif