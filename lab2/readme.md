# [An Efficient Unification Algorithm](https://dl.acm.org/doi/pdf/10.1145/357162.357169)

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
rule := <regex> = <regex>
regex := (<regex><binary><regex>)|char<unary>|(<regex>)<unary> | ε
binary := '|' | ε
unary := '*' | ε
```
Program reads input both from stdin and from file

## Run
./lab2 $1 $2 $3
### Programm arguments
1) Initial regex source [default = stdin] (may be path to file)
2) Rules source [default = stdin] (may be path to file)
3) Timeout: int [default = 10 seconds] (execution maximum duration)
### Example
```sh
./lab2 < tests/1
```
or
```sh
./lab2 tests/1 stdin 3
x* = ((a(b(cd))))*
((a(b(cd)))x*) = ((a(bc)))*
```