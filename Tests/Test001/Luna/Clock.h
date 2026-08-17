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
 
#ifndef LUNA_CLOCK_H
#define LUNA_CLOCK_H

#include "Luna/Result.h"
#include <stdbool.h>

#ifdef Luna_Configuration_Windows

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef struct Luna_Clock {
  LARGE_INTEGER frequency;
  LARGE_INTEGER lastUpdate;
  bool paused;
} Luna_Clock;

#else
  #error("environment not yet supported")
#endif

Luna_Result Luna_Clock_initialize(Luna_Clock* clock);

Luna_Result Luna_Clock_uninitialize(Luna_Clock* clock);

Luna_Result Luna_Clock_setPaused(Luna_Clock* clock, bool paused);

Luna_Result Luna_Clock_getTick(Luna_Clock* clock, double* tick);

#endif /* LUNA_CLOCK_H */
