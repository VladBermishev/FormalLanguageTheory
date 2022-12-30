# [Коммутативный образ контекстно-свободной грамматики по алгоритму Пиллинга](https://github.com/TonitaN/FormalLanguageTheory/blob/main/2022/tasks/pilling1973_Parikh_image.pdf)

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
## Run
./lab3 $1
### Programm arguments
1) Grammar source [default = stdin] (may be path to file)
### Example
```sh
./lab3 < tests/1
```
or
```sh
./lab3 tests/1
```
#### Result
```sh
B = (((ab)*a^2b^2+(ab)*a^3)*ab)*a
S = ((ab)*a^2b^2+(ab)*a^3)*b
```