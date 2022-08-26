#ifndef __DEBUG_H
#define __DEBUG_H


#include <Arduino.h>
#include "common.h"


#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0 // 0=disabled, 1=errors only, 2=normal, 3=verbose
#endif


static char buf[160] = "";


static void Debug_Init() {
#if DEBUG_LEVEL > 0
  Serial.begin(115200);
#endif
}


static void Debug_Error(const char *fmt, ...)
{
#if DEBUG_LEVEL >= 1
  va_list pargs;
  va_start(pargs, fmt);
  vsnprintf(buf, sizeof(buf), fmt, pargs);
  va_end(pargs);

  Serial.print("Error: ");
  Serial.println(buf);
#endif
}


static void Debug_Msg(const char *fmt, ...)
{
#if DEBUG_LEVEL >= 2
  va_list pargs;
  va_start(pargs, fmt);
  vsnprintf(buf, sizeof(buf), fmt, pargs);
  va_end(pargs);

  Serial.println(buf);
#endif
}


static void Debug_Verbose(const char *fmt, ...)
{
#if DEBUG_LEVEL >= 3
  va_list pargs;
  va_start(pargs, fmt);
  vsnprintf(buf, sizeof(buf), fmt, pargs);
  va_end(pargs);

  Serial.println(buf);
#endif
}


#endif
