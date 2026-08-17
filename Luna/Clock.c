/*
 * The author of this software is Michael Heilmann (contact@michaelheilmann.com).
 * 
 * Copyright(c) 2024-2026 Michael Heilmann (contact@michaelheilmann.com).
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.IN PARTICULAR, NEITHER THE AUTHOR NOR LUCENT MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
 * OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 */
 
#include "Luna/Clock.h"

#if 1
Luna_Result Luna_Clock_initialize(Luna_Clock* clock) {
  if (!clock) {
    return Luna_Result_ArgumentValueInvalid;
  }
  QueryPerformanceFrequency(&clock->frequency);
  QueryPerformanceCounter(&clock->lastUpdate);
  clock->paused = true;
  return Luna_Result_Success;
}
#endif

#if 1
Luna_Result Luna_Clock_uninitialize(Luna_Clock* clock) {
  if (!clock) {
    return Luna_Result_ArgumentValueInvalid;
  }
  return Luna_Result_Success;
}
#endif

#if 1
Luna_Result Luna_Clock_setPaused(Luna_Clock* clock, bool paused) {
  if (!clock) {
    return Luna_Result_ArgumentValueInvalid;
  }
  if (paused) {
    clock->paused = true;
  } else {
    if (clock->paused) {
      QueryPerformanceCounter(&clock->lastUpdate);
      clock->paused = false;
    }
  }
  return Luna_Result_Success;
}

Luna_Result Luna_Clock_getTick(Luna_Clock* clock, double* tick) {
  if (!clock || !tick) {
    return Luna_Result_ArgumentValueInvalid;
  }
  if (clock->paused) {
    *tick = 0.;
  } else {
    LARGE_INTEGER thisUpdate;
    QueryPerformanceCounter(&thisUpdate);

    LONGLONG elapsed = thisUpdate.QuadPart - clock->lastUpdate.QuadPart;

    clock->lastUpdate = thisUpdate;

    *tick = (double)elapsed / (double)clock->frequency.QuadPart;
  }
  return Luna_Result_Success;
}

#endif