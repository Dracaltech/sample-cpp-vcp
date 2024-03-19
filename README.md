# sample-cpp-vcp
Dracal // SDK code sample for C/C++ (Win32) on VCP

## Assumptions

Running this repository requires you to have installed:
- Visual Studio (version >= Community 2022)
  -> _Workloads: Desktop Development with C++_

Optionally, if you want to use the `Makefile` approach you'll need:
- Make (version >= 4.4.1)

This repository makes use of library [**'libcrc'**](https://github.com/lammertb/libcrc/) - which is included as a built object (`libcrc.lib`) in this repository. It is compiled based on the latest commit available at the time of writing ([2021-04-21](https://github.com/lammertb/libcrc/commit/7719e2112a9a960b1bba130d02bebdf58e8701f1)).


## Simple usage

Make sure that in `sample-cpp-vcp.cpp`, variable `dev` corresponds to your local setup (e.g. `COM4`)


### Visual Studio project

1. Open `sample-cpp-vcp.sln`
1. Build > _Build Solution_ (F6)
1. Debug > Start _or Without_ Debugging (F5 / Ctrl+F5)

### Makefile

Build
```
make
```

Run
```
./sample-cpp-vcp.exe
```

## Sample output


```
C:\dev\dracal\sample-cpp-vcp>sample-cpp-vcp.exe
Opening serial port successful
comm_mask: 0

Poll interval set to 1000 ms

VCP-PTH450-CAL E24638 @ 2024-03-18 16:12:20
P = 101294 Pa
T = 23.11 C
H = 61.23 %

VCP-PTH450-CAL E24638 @ 2024-03-18 16:12:21
P = 101293 Pa
T = 23.11 C
H = 61.23 %

VCP-PTH450-CAL E24638 @ 2024-03-18 16:12:22
P = 101293 Pa
T = 23.08 C
H = 61.24 %

VCP-PTH450-CAL E24638 @ 2024-03-18 16:12:23
P = 101293 Pa
T = 23.11 C
H = 61.19 %
^C
C:\dev\dracal\sample-cpp-vcp>
```
