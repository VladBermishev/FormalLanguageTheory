

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
