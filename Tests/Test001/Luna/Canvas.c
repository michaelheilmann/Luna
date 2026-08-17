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
 
#include "Luna/Canvas.h"

#include "Luna/Event.h"
#include "Luna/Memory.h"
#include "Luna/Pixels.h"

#include <stdbool.h>
#include <stdio.h>
#include <assert.h>

#if (!defined(Luna_Configuration_Windows) && !defined(Luna_Configuration_Linux)) || (defined(Luna_Configuration_Windows) && defined(Luna_Configuration_Linux))
  #error("Exactly one of Luna_Configuration_Windows or Luna_Configuration_Linux must be defined.")
#endif

#if defined(Luna_Configuration_Windows)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "resource.h"

#include "Luna/Windows/BackBuffer.h"

static BackBuffer* g_backBuffer = NULL;

static HINSTANCE g_hInstance = NULL;
static ATOM g_aWindowClass = 0;
static HWND g_hWindow = NULL;

static bool g_hasError = false;

static Luna_Result doCreateWindow();

static Luna_Result doDestroyWindow();

static Luna_Result doUpdateWindow();

static LRESULT CALLBACK WindowProc(HWND hWindow, UINT uMessage, WPARAM wParam, LPARAM lParam);

static Luna_Result doCreateWindow() {
  g_hInstance = GetModuleHandle(NULL);
  if (!g_hInstance) {
    fprintf(stderr, "%s failed\n", "GetModuleHandle");
    return Luna_Result_EnvironmentFailed;
  }
  WNDCLASSEX wc;
  wc.cbSize = sizeof(WNDCLASSEX);
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc = WindowProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = g_hInstance;
  wc.hIcon = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
  wc.hCursor = LoadCursor(g_hInstance, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = "Luna-Windows-Window-Class";
  wc.hIconSm = LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
  g_aWindowClass = RegisterClassEx(&wc);
  if (!g_aWindowClass) {
    fprintf(stderr, "%s failed\n", "RegisterClassEx");
    g_hInstance = NULL;
    return Luna_Result_EnvironmentFailed;
  }
  g_hWindow =
    CreateWindowEx
      (
        WS_EX_APPWINDOW,
        "Luna-Windows-Window-Class",
        "Luna",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        g_hInstance,
        NULL
      );
  if (!g_hWindow) {
    fprintf(stderr, "%s failed\n", "CreateWindowEx");
    UnregisterClass("Luna-Widows-Window-Class", g_hInstance);
    g_aWindowClass = 0;
    g_hInstance = NULL;
    return Luna_Result_EnvironmentFailed;
  }

  if (BackBuffer_create(&g_backBuffer, g_hWindow)) {
    DestroyWindow(g_hWindow);
    g_hWindow = NULL;
    UnregisterClass("Luna-Widows-Window-Class", g_hInstance);
    g_aWindowClass = 0;
    g_hInstance = NULL;
    return Luna_Result_EnvironmentFailed;
  }

  ShowWindow(g_hWindow, SW_SHOWDEFAULT);
  UpdateWindow(g_hWindow);

  return Luna_Result_Success;
}

static Luna_Result doDestroyWindow() {
  if (g_backBuffer) {
    BackBuffer_destroy(g_backBuffer);
    g_backBuffer = NULL;
  }
  if (g_hWindow) {
    DestroyWindow(g_hWindow);
    g_hWindow = NULL;
  }
  if (g_aWindowClass) {
    UnregisterClass("Luna-Widows-Window-Class", g_hInstance);
    g_aWindowClass = 0;
  }
  g_hInstance = NULL;
  return Luna_Result_Success;
}

static Luna_Result doUpdateWindow() {
  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
      Luna_Event* event;
      if (Luna_createApplicationQuitRequestedEvent(&event)) {
        g_hasError = true;
        continue;
      }
      if (Luna_pushEvent(event)) {
        Luna_deleteEvent(event);
        g_hasError = true;
        continue;
      }
    } else {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  if (g_hasError) {
    g_hasError = false;
    return Luna_Result_EnvironmentFailed;
  }
  return Luna_Result_Success;
}

static LRESULT CALLBACK WindowProc(HWND hWindow, UINT uMessage, WPARAM wParam, LPARAM lParam) {
  switch (uMessage) {
    case WM_CLOSE: {
      Luna_closeCanvas();
      return 0;
    } break;
    case WM_SIZE: {
      if (BackBuffer_resize(g_backBuffer)) {
        g_hasError = true;
        return 0;
      }
      /* Emit canvas size changed event. */
      Luna_Result result;
      Luna_Event* event;
      result = _Luna_createCanvasSizeChangedEvent(&event);
      if (result) {
        g_hasError = true;
        return 0;
      }
      result = Luna_pushEvent(event);
      if (result) {
        Luna_deleteEvent(event);
        event = NULL;
        g_hasError = true;
        return 0;
      }
    } break;
    case WM_ERASEBKGND: {
      /* Returning 1 prevents Windows from erasing the background before WM_PAINT. Otherwise we might see flickering. */
      return 1;
    }
    case WM_PAINT: {
      /* Create the back buffer. */
      if (!g_backBuffer) {
        if (BackBuffer_create(&g_backBuffer, hWindow)) {
          g_hasError = true;
          return 0;
        }
      }
      PAINTSTRUCT ps;

      HDC windowDC = BeginPaint(hWindow, &ps);

      BitBlt(
        g_backBuffer->hWindowDeviceContext,
        0,
        0,
        g_backBuffer->width,
        g_backBuffer->height,
        g_backBuffer->hDeviceContext,
        0,
        0,
        SRCCOPY
      );

      EndPaint(hWindow, &ps);
      return 0;
    } break;
  }
  return DefWindowProc(hWindow, uMessage, wParam, lParam);
}

#endif

static bool g_initialized = false;

Luna_Result Luna_openCanvas() {
  if (false == g_initialized) {
    Luna_Result result;
    /* Do open. */
    result = doCreateWindow();
    if (result) {
      return result;
    }
    g_initialized = true;
    fprintf(stdout, "canvas opened\n");
    /* We allow this to fail. */
    Luna_Event* event;
    result = _Luna_createCanvasOpenedEvent(&event);
    if (result) {
      g_hasError = true;
      return Luna_Result_Success;
    }
    result = Luna_pushEvent(event);
    if (result) {
      Luna_deleteEvent(event);
      event = NULL;
      g_hasError = true;
      return Luna_Result_Success;
    }
  }
  return Luna_Result_Success;
}

Luna_Result Luna_closeCanvas() {
  if (true == g_initialized) {
    /* Do close. */
    doDestroyWindow();
    g_initialized= false;
    fprintf(stdout, "canvas closed\n");
    /* We allow this to fail. */
    Luna_Result result;
    Luna_Event* event;
    result = _Luna_createCanvasClosedEvent(&event);
    if (result) {
      return Luna_Result_Success;
    }
    result = Luna_pushEvent(event);
    if (result) {
      Luna_deleteEvent(event);
      event = NULL;
      g_hasError = true;
      return Luna_Result_Success;
    }
  }
  return Luna_Result_Success;
}

Luna_Result Luna_updateCanvas() {
  if (true == g_initialized) {
    /* Do update. */
    Luna_Result result = doUpdateWindow();
    if (result) {
      return result;
    }
    if (g_backBuffer) {
      InvalidateRect(
        g_hWindow,
        NULL,
        FALSE // do not erase the background
      );
    }
    fprintf(stdout, "canvas updated\n");
  }
  return Luna_Result_Success;
}

Luna_Result Luna_getIsCanvasClosed(bool* isClosed) {
  *isClosed = !g_initialized;
  return Luna_Result_Success;
}

Luna_Result Luna_getCanvaSize(int32_t* width, int32_t* height) {
  if (false == g_initialized || !width || !height) {
    return Luna_Result_ArgumentValueInvalid;
  }
  RECT rectangle;
  if (!GetClientRect(g_hWindow, &rectangle)) {
    return Luna_Result_EnvironmentFailed;
  }
  *width = rectangle.right - rectangle.left;
  *height = rectangle.bottom - rectangle.top;
  return Luna_Result_Success;
}

Luna_Result Luna_renderToCanvas(Luna_Surface* surface) {
  if (!surface) {
    return Luna_Result_ArgumentValueInvalid;
  }
  int32_t left = 0, top = 0, width = surface->width, height = surface->height;
  if (width > g_backBuffer->width) width = g_backBuffer->width;
  if (height > g_backBuffer->height) height = g_backBuffer->height;

  for (int32_t y = 0; y < height; ++y) {
    for (int32_t x = 0; x < width; ++x) {
      uint32_t* source = (uint32_t*)surface->pixels;
      source += y * surface->width + x;
      uint32_t* target = (uint32_t*)g_backBuffer->pixels;
      target += y * g_backBuffer->width + x;
      *target = *source;
    }
  }
  return Luna_Result_Success;
}

Luna_Result _Luna_createCanvasOpenedEvent(Luna_Event** event) {
  if (!event) {
    return Luna_Result_ArgumentValueInvalid;
  }

  Luna_Result result;
  Luna_Event* temporary = NULL;
  result = Luna_allocate(&temporary, sizeof(Luna_Event));
  if (result) {
    return result;
  }
  temporary->kind = Luna_EventKind_CanvasOpened;
  *event = temporary;
  return Luna_Result_Success;
}

Luna_Result _Luna_createCanvasClosedEvent(Luna_Event** event) {
  if (!event) {
    return Luna_Result_ArgumentValueInvalid;
  }

  Luna_Result result;
  Luna_Event* temporary = NULL;
  result = Luna_allocate(&temporary, sizeof(Luna_Event));
  if (result) {
    return result;
  }
  temporary->kind = Luna_EventKind_CanvasClosed;
  *event = temporary;
  return Luna_Result_Success;

}

Luna_Result _Luna_createCanvasSizeChangedEvent(Luna_Event** event) {
  if (!event) {
    return Luna_Result_ArgumentValueInvalid;
  }

  Luna_Result result;
  Luna_Event* temporary = NULL;
  result = Luna_allocate(&temporary, sizeof(Luna_Event));
  if (result) {
    return result;
  }
  temporary->kind = Luna_EventKind_CanvasSizeChanged;
  *event = temporary;
  return Luna_Result_Success;
}
