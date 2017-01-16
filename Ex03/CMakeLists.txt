cmake_minimum_required(VERSION 3.6)
project(Ex02)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories( ${MY_SOURCE_DIR}/xv6 )
set(SOURCE_FILES
        xv6/semaphores.h
        xv6/semaphores.c
        xv6/bootasm.S
        xv6/entry.S
        xv6/entryother.S
        xv6/initcode.S
        xv6/swtch.S
        xv6/trapasm.S
        xv6/usys.S
        xv6/asm.h
        xv6/buf.h
        xv6/defs.h
        xv6/elf.h
        xv6/fcntl.h
        xv6/file.h
        xv6/fs.h
        xv6/kbd.h
        xv6/memlayout.h
        xv6/mmu.h
        xv6/mp.h
        xv6/param.h
        xv6/proc.h
        xv6/spinlock.h
        xv6/stat.h
        xv6/syscall.h
        xv6/traps.h
        xv6/types.h
        xv6/user.h
        xv6/x86.h
        xv6/bio.c
        xv6/bootmain.c
        xv6/cat.c
        xv6/console.c
        xv6/echo.c
        xv6/exec.c
        xv6/file.c
        xv6/forktest.c
        xv6/fs.c
        xv6/grep.c
        xv6/ide.c
        xv6/init.c
        xv6/ioapic.c
        xv6/kalloc.c
        xv6/kbd.c
        xv6/kill.c
        xv6/lapic.c
        xv6/ln.c
        xv6/log.c
        xv6/ls.c
        xv6/main.c
        xv6/memide.c
        xv6/mkdir.c
        xv6/mkfs.c
        xv6/mp.c
        xv6/picirq.c
        xv6/pipe.c
        xv6/printf.c
        xv6/proc.c
        xv6/rm.c
        xv6/sh.c
        xv6/spinlock.c
        xv6/stressfs.c
        xv6/string.c
        xv6/syscall.c
        xv6/sysfile.c
        xv6/sysproc.c
        xv6/timer.c
        xv6/trap.c
        xv6/uart.c
        xv6/ulib.c
        xv6/umalloc.c
        xv6/usertests.c
        xv6/vm.c
        xv6/wc.c
        xv6/zombie.c xv6/test.c xv6/pc_def.c xv6/pc.c)
add_executable(Ex02 ${SOURCE_FILES})
#cmake_minimum_required(VERSION 2.8.4)
#project(Ex02)
#
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#
#add_custom_target(Ex02 QEMU=qemu-system-i386 make qemu TOOLPREFIX=i386-elf-)