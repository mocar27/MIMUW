# ***Web applications***

Project that we were required to create is **Online SDCC Compiler**.

There were four parts of the project and in each part new functionalities were added to the code.

When running, the project allows user to:
- create an account in the online platform of the SDCC Compiler
- create and delete directories and files
- write code that can be saved and later decompiled to assembly using SDCC compiler
- if desired and set, user can as well save the decompiled file to his own computer
- choose different options of compilation of user's program, eg. different C standard, different processor or different flags

There are almost 60 tests to check the correctness of operations in the application.

Before trying to run or deploy the application, it is required to run this command in main directory:
```shell
python3 manage.py collectstatic
```
