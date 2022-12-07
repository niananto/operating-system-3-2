## xv6-riscv  

Prerequisite to running xv6: https://pdos.csail.mit.edu/6.828/2022/tools.html

Code-base of xv6: https://github.com/mit-pdos/xv6-riscv

Explanation of source code (Not required for this course, but you may want to go through it):
https://www.youtube.com/watch?v=fWUJKH0RNFE&list=PLbtzT1TYeoMhTPzyTZboW_j7TPAnjv9XB

### Setup (Ubuntu)
> sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu 

> git clone https://github.com/mit-pdos/xv6-riscv.git  

Go into the `xv6-riscv/` directory and then
> make qemu

To clean the qemu build anytime
> make clean

To exit from the shell, press CTRL+A then X

### Running Programs
This is so simple an OS that we need to `make qemu` everytime to see changes in the programs we wrote

Every c program should have these -
```
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
```

- Create a `.c` file in the `user/` directory of xv6. For example `myprog.c`
- Add it to the list of `UPROGS` in `Makefile` like this `$U/_myprog\`. Don't forget the underscore
- Run it from the terminal like this
  >myprog