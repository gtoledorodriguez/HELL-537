#include "types.h"
#include "user.h"
#include "stat.h"
#include "pstat.h"

#define check(exp, msg) if(exp) {} else {\
   printf(1, "%s:%d check (" #exp ") failed: %s\n", __FILE__, __LINE__, msg);\
   exit();}

int pid[3];

void
spin()
{
  int i = 0;
  int j = 0;
  int k = 0;
  for(i = 0; i<50000; ++i){
    for (j = 0; j < 1000000; ++j){
      k = j % 10;
      k = k+1;
    }
  }
}

void
watchChild(int tickets)
{
  pid[tickets/50] = getpid();
  settickets(tickets);
  spin();
  exit();
}

int 
main(int argc, char *argv[])
{
  //int pid_par = getpid();
  settickets(10000);
  struct pstat ps_parent;
  for( int i = 3; i > 0; i--){
    if(fork() == 0){
      watchChild(i*50);
    }
  }
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_Initial *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_20 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_40 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_60 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_80 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_100 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_120 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_140 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_160 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_180 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_200 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_220 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_240 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_260 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_280 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }
  sleep(20);
  check(getpinfo(&ps_parent) == 0, "getpinfo");
  printf(1,"\n***** PInfo_300 *****\n");
  for(int i = 0; i < NPROC; i++) {
    if(ps_parent.inuse[i]){
      printf(1,  "pid: %d tickets: %d ticks: %d\n", ps_parent.pid[i], ps_parent.tickets[i], ps_parent.ticks[i]);
    }
  }

  for(int i = 3; i>0; i--){
    kill(pid[i]);
  }
  
  exit();
}


