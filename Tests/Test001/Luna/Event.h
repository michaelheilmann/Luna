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
 
#ifndef LUNA_EVENT_H
#define LUNA_EVENT_H

#include "Luna/Result.h"

#define Luna_EventKind_ApplicationQuitRequested (1)

typedef struct Luna_Event Luna_Event;

struct Luna_Event {
  uint8_t kind;
  union {
    struct {
      uint8_t dummy;
    } applicationQuitRequested;
    struct {
      uint8_t dummy;
    } canvasOpenedMessage;
    struct {
      uint8_t dummy;
    } canvasClosed;
    struct {
      uint8_t dummy;
    } canvasSizeChanged;
  };
};

Luna_Result Luna_startupEventQueue();

Luna_Result Luna_shutdownEventQueue();

Luna_Result Luna_deleteEvent(Luna_Event* event);

Luna_Result Luna_popEvent(Luna_Event** event);

Luna_Result Luna_pushEvent(Luna_Event* event);

Luna_Result Luna_createApplicationQuitRequestedEvent(Luna_Event** event);

#endif /* LUNA_EVENT_H */
