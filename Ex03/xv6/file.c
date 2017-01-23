//
// File descriptors
//

#include "types.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "file.h"
#include "spinlock.h"
#include "buf.h"

#define BLOCK_COMPLETE 512
struct devsw devsw[NDEV];
struct {
  struct spinlock lock;
  struct file file[NFILE];
} ftable;

char blockfiller[512] = "\0";

void
fileinit(void)
{
    initlock(&ftable.lock, "ftable");
    memset(blockfiller, '_', BLOCK_COMPLETE);
}

// Allocate a file structure.
struct file*
filealloc(void)
{
  struct file *f;

  acquire(&ftable.lock);
  for(f = ftable.file; f < ftable.file + NFILE; f++){
    if(f->ref == 0){
      f->ref = 1;
      release(&ftable.lock);
      return f;
    }
  }
  release(&ftable.lock);
  return 0;
}

// Increment ref count for file f.
struct file*
filedup(struct file *f)
{
  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("filedup");
  f->ref++;
  release(&ftable.lock);
  return f;
}

// Close file f.  (Decrement ref count, close when reaches 0.)
void
fileclose(struct file *f)
{
  struct file ff;

  acquire(&ftable.lock);
  if(f->ref < 1)
    panic("fileclose");
  if(--f->ref > 0){
    release(&ftable.lock);
    return;
  }
  ff = *f;
  f->ref = 0;
  f->type = FD_NONE;
  release(&ftable.lock);
  
  if(ff.type == FD_PIPE)
    pipeclose(ff.pipe, ff.writable);
  else if(ff.type == FD_INODE){
    begin_trans();
    iput(ff.ip);
    commit_trans();
  }
}

// Get metadata about file f.
int
filestat(struct file *f, struct stat *st)
{
  if(f->type == FD_INODE){
    ilock(f->ip);
    stati(f->ip, st);
    iunlock(f->ip);
    return 0;
  }
  return -1;
}

// Read from file f.
int
fileread(struct file *f, char *addr, int n)
{
  int r;

  if(f->readable == 0)
    return -1;
  if(f->type == FD_PIPE)
    return piperead(f->pipe, addr, n);
  if(f->type == FD_INODE){
    ilock(f->ip);
    if((r = readi(f->ip, addr, f->off, n)) > 0)
      f->off += r;
    iunlock(f->ip);
    return r;
  }
  panic("fileread");
}

//PAGEBREAK!
// Write to file f.
int
filewrite(struct file *f, char *addr, int n) {
    int r;

    if (f->writable == 0)
        return -1;
    if (f->type == FD_PIPE)
        return pipewrite(f->pipe, addr, n);
    if (f->type == FD_INODE) {
        // write a few blocks at a time to avoid exceeding
        // the maximum log transaction size, including
        // i-node, indirect block, allocation blocks,
        // and 2 blocks of slop for non-aligned writes.
        // this really belongs lower down, since writei()
        // might be writing a device like the console.
        int max = ((LOGSIZE - 1 - 1 - 2) / 2) * 512;
        int i = 0;
        while (i < n) {
            int n1 = n - i;
            if (n1 > max)
                n1 = max;

            begin_trans();
            ilock(f->ip);
            if ((r = writei(f->ip, addr + i, f->off, n1)) > 0)
                f->off += r;
            iunlock(f->ip);
            commit_trans();

            if (r < 0)
                break;
            if (r != n1)
                panic("short filewrite");
            i += r;
        }
        int spare = BLOCK_COMPLETE - (n % BLOCK_COMPLETE);
        //ADDITION !
        if (f->blockwrite && spare) {
            //cprintf("N is %d Spare is %d \n",n,spare);
            begin_trans();
            ilock(f->ip);
            if ((r = writei(f->ip, blockfiller, f->off, spare)) > 0)
                f->off += r;
            iunlock(f->ip);
            commit_trans();

            if  (r != spare)
                panic("short filewrite");
        }
        return i == n ? n : -1;
    }
    panic("filewrite");
}


int
delete_range(int fd ,int from,int till) {
    int i;
    int startblock = from/512;
    int tillblock = till/512;
    struct file *f = proc->ofile[fd];
    struct inode *ip = f->ip;

    //validate params
    if (from < 0 || till < 0 || from > ip->size || till > ip->size)
        return -2;
    if (from > till)
        return -3;

    int delta = tillblock - startblock + 1 ;
    int endBlock = ip->size/512;
    if (ip->size%512 == 0)
        endBlock--;
    cprintf("from %d \nuntil %d \ndelta %d\n",startblock,tillblock,delta);

    begin_trans();
    ilock(ip);
    int hasNDirectData = ip->addrs[NDIRECT] != 0; //if has indirect, then an update should be needed

    struct buf *bp = 0;
    uint *ndirectArr = 0;

    if (hasNDirectData) {
        cprintf("Has indirect!\n");
        bp = bread(ip->dev, ip->addrs[NDIRECT]);
        ndirectArr = (uint*)bp->data;
    }

    //if all correct run through the range and shift content
    for (i = startblock; i < endBlock + 1; i++) {
        uint *source = getAddressAtBlock(ip,i,ndirectArr);
        uint *target = getAddressAtBlock(ip,i+delta,ndirectArr);
        //swap content
        uint tmp = *source;
        *source = *target;
        *target = tmp;
    }

    int actualDelta = (tillblock - startblock)*BSIZE;
    //if we deleted the end block
    if (till >= ip->size) {
        actualDelta += ip->size % BSIZE;
    }
    else {
        actualDelta += BSIZE;
    }

    ip->size -= actualDelta;
    //finish up..
    iclearaftersize(ip);
    iunlock(ip);
    commit_trans();

    return 1;
}


uint *getAddressAtBlock(struct inode *ip,int blockIndex, uint *a) {
    if (blockIndex < NDIRECT) {
        //cprintf("direct %d %d\n",blockIndex,ip->addrs[blockIndex]);
        return &ip->addrs[blockIndex];
    }
    else {
        blockIndex -= NDIRECT;
        return &a[blockIndex];
    }
}