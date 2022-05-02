# Apokalypse3D
Fast and cache-friendly 3D game engine (WIP)

## Requirements
- **CMake** (2.8.12+)
- **C++ Build Tools**
	- ninja
	- make
- **C++ Compiller**
	- **Linux** gcc, clang
	- **W32** mingw, clang
	- **OSX** clang

## Clonning the repository
```bash
> git clone https://github.com/YuriAzathoth/Apokalypse3D.git --recursive 
```

## How to build
- **Linux/OSX/MSYS**
```bash
> cd ~/Apokalypse3D
> cmake . -G "Ninja" -B build
> cd ./build
> ninja all
```
- **W32**
```cmd
> cd Apokalypse3D
> cmake . -G "Ninja" -B build
> cd build
> ninja all
```
