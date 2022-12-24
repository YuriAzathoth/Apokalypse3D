# Apokalypse3D
Fast and cache-friendly Entity Component System 3D game engine (WIP)

## Requirements
- **CMake** (3.2+)
- **C++ Build Tools**
	- ninja
	- make (not fully tested yet)
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
