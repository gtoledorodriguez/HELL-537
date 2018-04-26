#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"

char*
strcpy(char *s, char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
    cc = read(0, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, void *vsrc, int n)
{
  char *dst, *src;
  
  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}

int
thread_create(void(*start_routine)(void *, void *), void *arg1, void *arg2)
{
  void  *stack, *real_stack;
  uint page_size = 4096;
  stack  = malloc(page_size * 2);
  if(stack == 0){
	printf(2, "malloc failed");
  	return -1;
  }
  real_stack = (void *)((uint)stack + (page_size - ((uint)stack % page_size)));
  ((uint*)real_stack)[-1] = (uint)stack;
  return clone(start_routine, arg1, arg2, real_stack);
}


int
thread_join(void)
{
  void *stack, *real_stack;
  int pid = join(&real_stack);
  stack = (void *)(((uint *)real_stack)[-1]);
  free(stack);
  return pid;
}

int
FetchAndAdd (uint *ptr)
{
  uint old = *ptr;
  *ptr = old + 1;
  return old;
}

void
lock_acquire(struct lock_t *lock)
{
  int myturn = FetchAndAdd(&lock->ticket);
  while (lock->turn != myturn);
}

void
lock_release(struct lock_t *lock)
{
  FetchAndAdd(&lock->turn);
}

void
lock_init(struct lock_t *lock)
{
  lock->ticket = 0;
  lock->turn = 0;
}

