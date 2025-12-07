#ifndef _ANNOTATIONS_C_H
#define _ANNOTATIONS_C_H
#include <stdint.h>

#ifndef PERLIO_NOT_STDIO
#include <stdio.h>
#else
#include <nostdio.h>
#endif

unsigned __attribute__ ((noinline)) SIM_BEGIN(int i);
unsigned __attribute__ ((noinline)) SIM_END(int i); 

void __attribute__ ((noinline)) SIM_LOCK(int * i);
void __attribute__ ((noinline)) SIM_UNLOCK(int * i);


unsigned __attribute__ ((noinline)) SIM_BEGIN(int i)
{
      if (i==0) {
          return 0;
      }
#ifndef PERLIO_NOT_STDIO
      printf("sim begin %s\n", __func__);
#else
      PerlIO_printf(Perl_debug_log, "sim begin %s\n", __func__);
#endif
      return 1;
}
unsigned __attribute__ ((noinline)) SIM_END(int i)
{
      if (i==0) {
          return 0;
      }
#ifndef PERLIO_NOT_STDIO
      printf("sim end %s\n", __func__);
#else
      PerlIO_printf(Perl_debug_log, "sim end %s\n", __func__);
#endif
      return 1;
} 

void __attribute__ ((noinline)) SIM_LOCK(int * i)
{
      while(__sync_lock_test_and_set(i, 1));
}

void __attribute__ ((noinline)) SIM_UNLOCK(int * i)
{
      __sync_lock_release(i);
}

#endif
