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

#ifndef LUNA_WINDOWS_BACKBUFFER_H
#define LUNA_WINDOWS_BACKBUFFER_H

#include "Luna/Result.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef struct BackBuffer {
  HWND hWindow;
  HDC hWindowDeviceContext;

  HDC hDeviceContext;

  HBITMAP hBitmap;
  void* pixels;

  LONG width;
  LONG height;

  HBITMAP hOldBitmap;
} BackBuffer;

Luna_Result BackBuffer_create(BackBuffer** backBuffer, HWND hWindow);

Luna_Result BackBuffer_resize(BackBuffer* backBuffer);

Luna_Result BackBuffer_destroy(BackBuffer* backBuffer);

#endif /* LUNA_WINDOWS_BACKBUFFER_H */
