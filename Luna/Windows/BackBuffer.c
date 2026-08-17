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

#include "Luna/Windows/BackBuffer.h"

#include "Luna/Memory.h"

static Luna_Result work(BackBuffer* backBuffer) {
  if (!backBuffer->hDeviceContext) {
    backBuffer->hDeviceContext = CreateCompatibleDC(backBuffer->hWindowDeviceContext);
    if (!backBuffer->hDeviceContext) {
      return Luna_Result_EnvironmentFailed;
    }
  }
  RECT clientRect;
  if (!GetClientRect(backBuffer->hWindow, &clientRect)) {
    return Luna_Result_EnvironmentFailed;
  }

  BITMAPINFO newBitmapInfo = { 0 };

  newBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  newBitmapInfo.bmiHeader.biWidth = clientRect.right - clientRect.left;
  newBitmapInfo.bmiHeader.biHeight = -(clientRect.bottom - clientRect.top); /* Top-down bitmap */
  newBitmapInfo.bmiHeader.biPlanes = 1;
  newBitmapInfo.bmiHeader.biBitCount = 32;
  /* 
   * The pixels of the bitmap are of the form
   * Address + 0 Blue
   * Address + 1 Green
   * Address + 2 Red
   * Address + 3 Unused
   */
  newBitmapInfo.bmiHeader.biCompression = BI_RGB;

  void* newPixels;
  HBITMAP hNewBitmap;
  hNewBitmap =
    CreateDIBSection
      (
        backBuffer->hWindowDeviceContext,
        &newBitmapInfo,
        DIB_RGB_COLORS,
        &newPixels,
        NULL,
        0
      );
  if (!hNewBitmap) {
    return Luna_Result_EnvironmentFailed;
  } 
  if (backBuffer->hOldBitmap) {
    if (!SelectObject(backBuffer->hDeviceContext, backBuffer->hOldBitmap)) {
      DeleteObject(hNewBitmap);
      hNewBitmap = NULL;
      return Luna_Result_EnvironmentFailed;
    }
    backBuffer->hOldBitmap = NULL;
  }
  HBITMAP hNewOldBitmap = SelectObject(backBuffer->hDeviceContext, hNewBitmap);
  if (!hNewOldBitmap) {
    DeleteObject(hNewBitmap);
    hNewBitmap = NULL;
    return Luna_Result_EnvironmentFailed;
  }
  if (backBuffer->hBitmap) {
    DeleteObject(backBuffer->hBitmap);
    backBuffer->hBitmap = NULL;
  }
  backBuffer->hBitmap = hNewBitmap;
  backBuffer->hOldBitmap = hNewOldBitmap;
  backBuffer->pixels = newPixels;
  backBuffer->width = clientRect.right - clientRect.left;
  backBuffer->height = clientRect.bottom - clientRect.top;
  return Luna_Result_Success;
}

static int32_t dummy = 0;

Luna_Result BackBuffer_create(BackBuffer** backBuffer, HWND hWindow) {
  Luna_Result result; 
  BackBuffer* temporary;
  if (result = Luna_allocate(&temporary, sizeof(BackBuffer))) {
    return result;
  }
  temporary->hWindow = hWindow;
  temporary->hWindowDeviceContext = GetDC(hWindow);
  if (!temporary->hWindowDeviceContext) {
    Luna_deallocate(temporary);
    temporary = NULL;
    return Luna_Result_EnvironmentFailed;
  }
  temporary->hDeviceContext = NULL;
  temporary->hBitmap = NULL;
  temporary->hOldBitmap = NULL;
  temporary->pixels = NULL;
  temporary->width = -1;
  temporary->height = -1;
  if (result = work(temporary)) {
    Luna_deallocate(temporary);
    temporary = NULL;
    return result;
  }
  *backBuffer = temporary;
  return Luna_Result_Success;
}

Luna_Result BackBuffer_resize(BackBuffer* backBuffer) {
  return work(backBuffer);
}

Luna_Result BackBuffer_destroy(BackBuffer* backBuffer) {
  if (backBuffer->pixels) {
    backBuffer->pixels = NULL;
  }
  if (backBuffer->hBitmap) {
    SelectObject(backBuffer->hDeviceContext, backBuffer->hOldBitmap);
    backBuffer->hOldBitmap = NULL;
    DeleteObject(backBuffer->hBitmap);
    backBuffer->hBitmap = NULL;
  }
  if (backBuffer->hDeviceContext) {
    DeleteDC(backBuffer->hDeviceContext);
    backBuffer->hDeviceContext = NULL;
  }
  if (backBuffer->hWindowDeviceContext) {
    ReleaseDC(backBuffer->hWindow, backBuffer->hWindowDeviceContext);
    backBuffer->hWindowDeviceContext = NULL;
  }
  return Luna_deallocate(backBuffer);
}
