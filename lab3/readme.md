# Коммутативный образ контекстно-свободной грамматики по алгоритму Пиллинга

## Build
Debug
```sh
mkdir build ; cd build
cmake ../ -DBUILD=DEBUG
cmake --build . --target lab2 -j 3
```
Release
```sh
mkdir build ; cd build
cmake ../ -DBUILD=Release
cmake --build . --target lab2 -j 3
```
## Input
### Format
```c
<grammar> ::= <rule>+
<rule> ::= <nterm> -> (<term>|<nterm>)*
<term> ::= {A-z}|{0-9}
<nterm> ::= [{A-z}+{0-9}*]
```
## Roadmap
 - epsilon and chain rules
 - arden's algorithm (50%)
 - kleeny star problem