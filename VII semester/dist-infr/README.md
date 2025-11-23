# Instant language compiler
## Usage

If you are in the main directory, so actually in which this `README.md` file is created, simply write 

```shell
make
```

and the project will compile to the `insc_jvm` and `insc_llvm` files ready to use:

```shell
./insc_jvm <file to compile to JVM>.ins
```

```shell
./insc_llvm <file to compile to LLVM>.ins
```

After invoking one of the commands above, either .ll & .bc or .j & .class files will be created in the directory in which the initial .ins file was placed.

To later get the output from either JVM or LLVM file you will have to invoke 

```shell 
    java file
    lli file.bc
```

for JVM and LLVM respectively.

Example of programs in Instant language are provided in the directory `examples` directory.

To clean files after compilation you can use: 

```shell
make clean              # It will clean all of the files created during compilation of the project
make cleanBuild         # It will clean the build/ directory created during compilation for ghc files
make cleanExamples      # It will clean files with extenstion in {.class, .j, .ll, .bc} in examples/ directory
make cleanBinaries      # It will clean the created binares - insc_jvm & insc_llvm
```

## Grammar

```cf
Prog. Program ::= [Stmt] ;
SAss. Stmt ::= Ident "=" Exp;
SExp. Stmt ::= Exp ;
separator Stmt ";" ;

ExpAdd.            Exp1   ::= Exp2 "+"  Exp1 ;
ExpSub.            Exp2   ::= Exp2 "-"  Exp3 ;
ExpMul.            Exp3   ::= Exp3 "*"  Exp4 ;
ExpDiv.            Exp3   ::= Exp3 "/"  Exp4 ;
ExpLit.            Exp4   ::= Integer ;
ExpVar.            Exp4   ::= Ident ;
coercions Exp 4;
```

## File structure in the project
 
 * [docs]()
   * DocInstant.txt
   * Instant.cf
   * instant.md
 * [examples]()
 * [lib]()
   * jasmin.jar
 * [src]()
   * [Generated]()
      * AbsInstant.hs
      * ErrM.hs
      * LexInstant.hs
      * ParInstant.hs
      * PrintInstant.hs
      * SkelInstant.hs
   * Compiler.hs
   * JVMCompiler.hs
   * JVMMain.hs
   * JVMUtils.hs
   * LLVMCompiler.hs
   * LLVMMain.hs
   * LLVMUtils.hs
 * .gitignore
 * LICENSE
 * Makefile
 * README.md

## Sources and libs used during programming

- [My own Interpreter](https://github.com/mocar27/xyz-interpreter/tree/main) written for Programming languages and paradigms
- Lab descriptions on Moodle (Lab: JVM, Lab: LLVM) for JVM and LLVM code inheritation & run info
- ChatGPT used for generation of JVM code instructions for variables
- [Jasmin](https://jasmin.sourceforge.net/guide.html)
- [BNFC](https://hackage.haskell.org/package/BNFC) used to generate files `src/Generated/*`
- Libraries used: mtl, Prelude, System.{FilePath, Exit, Process, Environment}, Data.{Set, Map}

## Concluding remarks

*Enjoy*
