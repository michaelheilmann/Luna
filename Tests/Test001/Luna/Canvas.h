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
 
#ifndef LUNA_CANVAS_H
#define LUNA_CANVAS_H

#include "Luna/Event.h"
#include <stdbool.h>
typedef struct Luna_Surface Luna_Surface;

#define Luna_EventKind_CanvasOpened (2)

#define Luna_EventKind_CanvasClosed (3)

#define Luna_EventKind_CanvasSizeChanged (4)

Luna_Result Luna_openCanvas();

Luna_Result Luna_closeCanvas();

Luna_Result Luna_updateCanvas();

Luna_Result Luna_getIsCanvasClosed(bool* isClosed);

Luna_Result Luna_getCanvaSize(int32_t* width, int32_t* height);

Luna_Result Luna_renderToCanvas(Luna_Surface* surface);

Luna_Result _Luna_createCanvasOpenedEvent(Luna_Event** event);

Luna_Result _Luna_createCanvasClosedEvent(Luna_Event** event);

Luna_Result _Luna_createCanvasSizeChangedEvent(Luna_Event** event);

#endif /* LUNA_CANVAS_H */
