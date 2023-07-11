# SpecialGame

My game engine and game made with SDL3.

Space themed game.

![Gameplay](https://github.com/kostahka/SpecialGame/assets/99989412/13c4723e-e141-45d5-9c42-ea65fceb06fc)

## Features
- Graphics
- Sounds
- GUI
- Simple gameplay

## Platforms
![Windows](https://img.shields.io/badge/windows-52b5f7?style=for-the-badge&logo=windows&logoColor=white)
![Linux](https://img.shields.io/badge/linux-%230170FE?style=for-the-badge&logo=linux&logoColor=white)
![Android](https://img.shields.io/badge/android-%2344A833?style=for-the-badge&logo=android&logoColor=white)

## Installation

Game requires:
- [SDL3](http://libsdl.org/) v3.0.0+
- Cmake 3.5+

For android:
- Gradle
- SDK
- NDK

### Build

Linux or windows:
```
mkdir build
cd build
cmake ..
cmake --build .
```
Android:
1) Open folder android-project in Android Studio.
2) Provide path to SDL with variable `sdl3.dir` in local.properties.
3) Provide paths to SDK and NDK if needed.
4) Build.
