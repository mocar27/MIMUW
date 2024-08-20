[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/RQ2rP42y)
# sokoban-5

Opis zadania w pliku ZADANIE.md. W pliku README.md prosze dodac opis swojego rozwiązania.

Spora część kodu z poprzedniego zadania została usunięta na potrzeby tego zadania, cześć została przerobiona, niemniej jednak najważniejsze zmiany to:

1. Opisanie typu Picture za pomocą:

```haskell
type DrawFun = Integer -> Integer -> Char
type Picture = DrawFun -> DrawFun
blank = id
(&) = (.)

```

2. Activity zostało zmienione na TerminalActivity
3. Użyte znaki do reprezentowania obiektów:

```haskell
wall -> '#'
ground -> ' '
storage -> '.'
box -> '$'
terminalPlayer -> '@'
```
