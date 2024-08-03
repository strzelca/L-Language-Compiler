# L Lanaguage Compiler

The L Language Compiler compiles L source to an linkable object, or execute itself in a JIT Compiler Environment.

### Language Syntax (EBNF)

```ebnf
PROGRAM = C;


(* Commands *)
C = nil | Id,"=",E | C,C | D,separator,C | if, "(",E,")", [else, "{",C,"}"] | while, "(",E,")" ,"{",C,"}" | return, E;

(* Expressions *)
E = v | Id | uop, E | E, bop, E | "(",E,")" | Id,"(",ae,")";

(* Declarations *)
D = nil | const, Id,[':',T], "=", E | var, Id,[':',T], "=", E | D,separator,D | function, Id,"(",form,")", ["->", T], "{",C,separator,return E,"}" | extern, E;


(* Formal Arguments and Called Arguments *)
form = nil | const, Id,[':',T], {form} | var, Id,[':',T], {form};
ae   = nil | E,ae;


(* Operators *)
uop = "+" | "-" | "!";
bop = "+" | "-" | "*" | "/" | "%" | "==" | "!=" | ">" | ">=" | "<" | "<=" | "&&" | "||";


(* Separator *)
separator = ';';


(* Numeric and String values *)
letter = "A" | "B" | "C" | "D" | "E" | "F" | "G"
       | "H" | "I" | "J" | "K" | "L" | "M" | "N"
       | "O" | "P" | "Q" | "R" | "S" | "T" | "U"
       | "V" | "W" | "X" | "Y" | "Z" | "a" | "b"
       | "c" | "d" | "e" | "f" | "g" | "h" | "i"
       | "j" | "k" | "l" | "m" | "n" | "o" | "p"
       | "q" | "r" | "s" | "t" | "u" | "v" | "w"
       | "x" | "y" | "z" ;
       
digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;

symbol = "[" | "]" | "{" | "}" | "(" | ")" | "<" | ">"
       | "'" | '"' | "=" | "|" | "." | "," | ";" | "-" 
       | "+" | "*" | "?" | "\n" | "\t" | "\r" | "\f" | "\b" ;

char = letter | digit | symbol | "_" | " " ;

num     = digit, { digit } | digit, { digit },'.',digit, { digit };
str     = '"' , { char - '"' }, '"';
bool    = 'true' | 'false';
v       = num | str | bool;


(* Identifiers *)
Id      = letter , { letter | digit | "_" } ;


(* Types *)
T       = 'Int' | 'Double' | 'Bool' | 'String';
```

### Diagram

![](https://private-user-images.githubusercontent.com/31825825/354805456-aeaf6c46-0c4a-437e-bf4c-83d7c8b1a906.png?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MjI2NzQ2ODIsIm5iZiI6MTcyMjY3NDM4MiwicGF0aCI6Ii8zMTgyNTgyNS8zNTQ4MDU0NTYtYWVhZjZjNDYtMGM0YS00MzdlLWJmNGMtODNkN2M4YjFhOTA2LnBuZz9YLUFtei1BbGdvcml0aG09QVdTNC1ITUFDLVNIQTI1NiZYLUFtei1DcmVkZW50aWFsPUFLSUFWQ09EWUxTQTUzUFFLNFpBJTJGMjAyNDA4MDMlMkZ1cy1lYXN0LTElMkZzMyUyRmF3czRfcmVxdWVzdCZYLUFtei1EYXRlPTIwMjQwODAzVDA4Mzk0MlomWC1BbXotRXhwaXJlcz0zMDAmWC1BbXotU2lnbmF0dXJlPTllNTM1OWI2ZWRhYmZiMTQyZDQzOWZhZDZhMjQ2MWQ2NjZiNmUwMWEzOGRmNWI4MzM4ZDQyZWRkMGMwNWZiYzAmWC1BbXotU2lnbmVkSGVhZGVycz1ob3N0JmFjdG9yX2lkPTAma2V5X2lkPTAmcmVwb19pZD0wIn0.fd1fIa-X6ZkYfvdUZYATsdSu3PxS9Lj_iQfqi1dHkLg)