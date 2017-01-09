#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_testKern(void) {
  int num;

  if(argint(0, &num) < 0)
    return -1;
  return testKern(num);
}

int
sys_getpid(void)
{
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

extern int sys_sem_open(void) {
    char *name;
    int init,maxVal;
    cprintf("YEAH ! ! ! !");

    if (argstr(0,&name) < 0 ||
            argint(1,&init)<0 ||
            argint(2,&maxVal)<0)
        return -1;
    cprintf("name : %s\n",name);
  return sem_open(name,init,maxVal);
}

extern int sys_sem_close(void) {
  return sem_close(0);
}
extern int sys_sem_wait(void) {
  return sem_wait(0);
}
extern int sys_sem_try_wait(void) {
  return sem_try_wait(0);
}
extern int sys_sem_signal(void) {
  return sem_signal(0);
}
extern int sys_sem_gat_value(void) {
  return sem_gat_value(0,0,0);
}
extern int sys_sem_unlink(void) {
  return sem_unlink(0);
}
