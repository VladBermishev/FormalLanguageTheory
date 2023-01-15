## Dependencies
libgraphviz
```sh
sudo apt-get install libgraphviz-dev
```
## Build
Debug
```sh
mkdir build ; cd build
cmake ../ -DBUILD=DEBUG
cmake --build . --target lab5 -j 3
```
Release
```sh
mkdir build ; cd build
cmake ../ -DBUILD=Release
cmake --build . --target lab5 -j 3
```
## Run
./lab5 $1
### Programm arguments
1) Grammar source [default = stdin] (may be path to file)
### Example
```sh
./lab5 < tests/1
```
or
```sh
./lab5 tests/1
```
#### Result
![ER-model](https://github.com/VladBermishev/FormalLanguageTheory_Labs/blob/master/lab5/tests/1-result/ER_model.svg "ER-model")
![Relation-model](https://github.com/VladBermishev/FormalLanguageTheory_Labs/blob/master/lab5/tests/1-result/Relational_model.svg "Relation-model")

|Relationship |              |Cardinality   |     |     |
|-------------|--------------|--------------|-----|-----|
|Parent       | Child        |Type          | MAX | MIN |
|User         | User_User    |Identifying   | N-1 | 0-1 |
|User_User    | User         |Identifying   | 1-N | 1-0 |
|User         | Credentials  |Identifying   | 1-1 | 1-1 |
|User         | Message      |Identifying   | N-1 | 0-1 |
|User         | Photo        |Nonidentifying| N-1 | 0-1 |
|Message      | Message_Photo |Identifying   | N-1 | 0-1 |
|Message_Photo| Photo        |Identifying   | 1-N | 1-1 |
|Message      | User         |Nonidentifying| 1-1 | 1-1 |
