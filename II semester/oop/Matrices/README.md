# Matrices task

## This is one of two bigger projects I was told to do as a part of Object Oriented Programming course.

## Running a project

The program can be compiled into a single file `jar` using the command
```shell
./gradlew shadowJar
```
the build result is then in the directory `build/libs` and it can be run from the console via a command
```shell
java -jar build/libs/oop-2022-all.jar
```
## Testing

Catalog tests `src/test` can be run by command
```shell
./gradlew test
```
which runs all functions marked with the annotations provided
by JUnit, e.g. `@Test`.
