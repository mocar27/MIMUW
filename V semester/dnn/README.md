# Xyz-Interpreter

Project initiated using Stack.

## Description

In the `example-programs/bad/` directory there are of course bad program definitions, so we can check that
Xyz-Interpreter supports static type control and execution errors, so programs that throw errors or do not compile
were moved to the `bad/` directory instead of `good/` directory.

Static typing will be also proven whether it is working in bad example programs, as if the program passes 
TypeChecker it will evaluate and run the program, so bad examples will have programs that won't pass 
the static type checking.

## Usage

If you are in the main directory, so actually in which this `README.md` file is created, simply write 

```shell
make
```

and the project will compile to the `interpreter` file ready to use:

```shell
./interpreter <provide file directory here (optional, otherwise it will parse program actively providing in the input)>
```

Example of `bad` and `good` program examples are provided in the directory `example-programs/bad/` and `example-programs/good/` respectively.

## Files

*Main* file is the main file taking the input and passing it to interpreter.

*Interpreter* file is the file that runs Parser & Lexer on given input, 
then if it returns success it runs TypeChecker, and if the result is successful
it runs the Evaluator. 

## Some nuances / changes to initial grammar

It is not permitted to create Void function. Function must have a type and must return something.

For evaluator to be successful in given input program there has to be `main` function defined.

**Unfortunately, due to my minor oversight, recursion is not supported (actually it is, but it's infinite), as there was a little change in the language grammar, that makes functions return always something, and this change created oversight in which return in `if` and `while` statements is not supported, so recursion will be infinite. I left the infinite examples of recursion in `example-programs/good/06-recursion.xyz**

## Grammar

```cf
-- Programs ------------------------------------------------

entrypoints  Program ;

MyProgram.   Program ::= [Stmt] ;

separator    Arg "," ;

ArgVal.      Arg ::= Type Ident ;

ArgRef.      Arg ::= "var" Type Ident ;

-- Statements ----------------------------------------------

separator    nonempty Item "," ;

NoInit.      Item ::= Ident ; 

Init.        Item ::= Ident "=" Expr ;

separator    Stmt "" ;

StmtBlock.   Block ::= "{" [Stmt] "}" ;

FnBlock.     FunBlock ::= "{" [Stmt] Rtrn "}" ;

Empty.       Stmt ::= ";" ;

Decl.        Stmt ::= Type [Item] ";" ;

Assign.      Stmt ::= Ident "=" Expr  ";" ;

Ret.         Rtrn ::= "return" Expr ";" ;

If.          Stmt ::= "if" "(" Expr ")" Block  ;

IfElse.      Stmt ::= "if" "(" Expr ")" Block "else" Block  ;

While.       Stmt ::= "while" "(" Expr ")" Block ;

FunctionDef. Stmt ::= Type "function" Ident "(" [Arg] ")" FunBlock ;

StmtExp.     Stmt ::= Expr  ";" ;

-- Types ---------------------------------------------------

separator    Type "," ;

Integer.     Type ::= "Integer" ;

String.      Type ::= "String" ;

Boolean.     Type ::= "Boolean" ;

Function.    Type ::= Type "(" [Type] ")" ;

-- Expressions ---------------------------------------------

separator    Expr "," ;

ExpVar.      Expr6 ::= Ident ;

ExpLitInt.   Expr6 ::= Integer ;

ExpString.   Expr6 ::= String ;

ExpLitTrue.  Expr6 ::= "true" ;

ExpLitFalse. Expr6 ::= "false" ;

ExpApp.      Expr6 ::= Ident "(" [Expr] ")" ;

ExpNeg.      Expr5 ::= "-" Expr6 ;

ExpNot.      Expr5 ::= "!" Expr6 ;

ExpMul.      Expr4 ::= Expr4 MulOp Expr5 ;

ExpAdd.      Expr3 ::= Expr3 AddOp Expr4 ;

ExpRel.      Expr2 ::= Expr2 RelOp Expr3 ;

ExpAnd.      Expr1 ::= Expr2 "&&" Expr1 ;

ExpOr.       Expr ::= Expr1 "||" Expr ;

ExpLambda.   Expr ::= "(" [Arg] ")" "=>" Type FunBlock ; 

coercions    Expr 6 ;

-- Operators -----------------------------------------------

Plus.        AddOp ::= "+" ;

Minus.       AddOp ::= "-" ;

Multi.       MulOp ::= "*" ;

Div.         MulOp ::= "/" ;

Mod.         MulOp ::= "%" ;

LThan.       RelOp ::= "<" ;

Leq.         RelOp ::= "<=" ;

GThan.       RelOp ::= ">" ;

Geq.         RelOp ::= ">=" ;

Eq.          RelOp ::= "==" ;

NEq.         RelOp ::= "!=" ;

-- Comments ------------------------------------------------

comment    "//" ;

comment    "/*" "*/" ;

```

## Sources used during programming

- My own laboratory assignments codes
- Provided lab descriptions on Moodle (Monads 1., Monads 2., Lab Haskell-Extra, Types)
- https://wiki.haskell.org/wikiupload/c/c6/ICMI45-paper-en.pdf
- https://learnyouahaskell.com/chapters
- https://jameshfisher.com/2018/03/06/an-interpreter-in-haskell/ 
- https://bor0.wordpress.com/2019/03/15/writing-a-simple-evaluator-and-type-checker-in-haskell/
- https://pappasbrent.com/blog/2022/06/11/writing-an-interpreter-in-haskell.html

## Concluding remarks

*Enjoy*
