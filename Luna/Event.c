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

#include "Luna/Event.h"

#include "Luna/Memory.h"
#include "Luna/Deque.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

static Luna_Deque* g_deque = NULL;

static uint32_t g_referenceCount = UINT32_C(0);

static Luna_Result doStartupEventQueue();

static Luna_Result doShutdownEventQueue();

/* An intra-process mutex. Only threads of this process may use it. */
static CRITICAL_SECTION g_mutex = { 0 };

static Luna_Result doStartupEventQueue() {
  Luna_Result result;
  if (result = Luna_allocate((void**)&g_deque, sizeof(Luna_Deque))) {
    return result;
  }
  if (result = Luna_Deque_initialize(g_deque)) {
    Luna_deallocate(g_deque);
    g_deque = NULL;
    return result;
  }
  if (!InitializeCriticalSectionAndSpinCount(&g_mutex, 4000)) {
    Luna_Deque_uninitialize(g_deque);
    Luna_deallocate(g_deque);
    g_deque = NULL;
    return result;
  }
  return Luna_Result_Success;
}

static Luna_Result doShutdownEventQueue() {
  Luna_Deque_uninitialize(g_deque);
  Luna_deallocate(g_deque);
  g_deque = NULL;
  DeleteCriticalSection(&g_mutex);
  return Luna_Result_Success;
}

Luna_Result Luna_startupEventQueue() {
  if (g_referenceCount == 0) {
    Luna_Result result = doStartupEventQueue();
    if (result) {
      return result;
    }
  }
#ifdef _DEBUG
  if (g_referenceCount == UINT32_MAX) {
    return Luna_Result_EnvironmentFailed;
  }
#endif
  g_referenceCount++;
  return Luna_Result_Success;
}

Luna_Result Luna_shutdownEventQueue() {
#ifdef _DEBUG
  if (g_referenceCount == 0) {
    return Luna_Result_EnvironmentFailed;
  }
#endif
  if (g_referenceCount == 1) {
    doShutdownEventQueue();
  }
  g_referenceCount--;
  return Luna_Result_Success;
}

Luna_Result Luna_deleteEvent(Luna_Event* event) {
  if (!event) {
    return Luna_Result_ArgumentValueInvalid;
  }
  return Luna_deallocate(event);
}

Luna_Result Luna_popEvent(Luna_Event** event) {
  if (!event) {
    return Luna_Result_ArgumentValueInvalid;
  }
  return Luna_Deque_popFront(g_deque, event);
}

Luna_Result Luna_pushEvent(Luna_Event* event) {
   return Luna_Deque_pushBack(g_deque, (void*)event);
}

Luna_Result Luna_createApplicationQuitRequestedEvent(Luna_Event** event) {
  if (!event) {
    return Luna_Result_ArgumentValueInvalid;
  }
  Luna_Result result;
  Luna_Event* temporary = NULL;
  result = Luna_allocate(&temporary, sizeof(Luna_Event));
  if (result) {
    return result;
  }
  temporary->kind = Luna_EventKind_ApplicationQuitRequested;
  *event = temporary;
  return Luna_Result_Success;
}
