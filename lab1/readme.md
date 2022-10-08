# [An Efficient Unification Algorithm](https://dl.acm.org/doi/pdf/10.1145/357162.357169)

## Build
Debug
```sh
mkdir build ; cd build
cmake ../ -DBUILD=DEBUG
cmake --build .
```
Release
```sh
mkdir build ; cd build
cmake ../ -DBUILD=Release
cmake --build .
```
## Input
### Format
```c
constructors= (char(positive-num),)*char(positive-num)
variables = (char,)*char
First term: [term]
Second term: [term]
term := [variable] | [constructor(0)] | [constructor](([term],)*term)
```
Program reads input both from stdin and from file

## Run
```sh
./lab1 < tests/1
```
or
```sh
./lab1 tests/1
```