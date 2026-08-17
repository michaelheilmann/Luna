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

#include <stdlib.h>
#include <stdbool.h> 
#include <stdio.h>
#include <math.h>

#include "Luna/Event.h"
#include "Luna/Canvas.h"
#include "Luna/Pixels.h"
#include "Luna/Clock.h"
#include "Luna/Font.h"

#ifdef Luna_Configuration_Windows

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#endif

static bool g_quit = false;
static Luna_Surface* g_surface = NULL;
static Luna_Font* g_font = NULL;
static Luna_Clock g_clock;

struct {
  struct {
    double x;
    double y;
  } position;
  struct {
    double x;
    double y;
  } movement;
} g_player = { .position.x = 0, .position.y = 0, .movement.x = +1, .movement.y = +1 };

#define WIDTH (80)
#define HEIGHT (80)

#define ROOM_WIDTH 512
#define ROOM_HEIGHT 512

#define PLAYER_WIDTH 64
#define PLAYER_HEIGHT 64

static Luna_Result updateScene() {
  double tick;
  Luna_Clock_getTick(&g_clock, &tick);
  //double dt = GetDeltaTime();
  /* Clamp delta time. For example, if the application was paused in the debugger for several seconds, we don't want the objects to suddenly jump across the screen. */
  if (tick > 0.1) {
    tick = 0.1;
  }

  double dx = g_player.movement.x;
  double dy = g_player.movement.y;
  double l = sqrtf(dx*dx + dy*dy);
  if (l) {
    double m = (1. / l) * 80. * tick;
    dx = dx * m;
    dy = dy * m;
  }
  g_player.position.x += dx;
  g_player.position.y += dy;

  if (g_player.position.x + WIDTH / 2 > ROOM_WIDTH / 2) {
    g_player.movement.x = -1;
  }
  if (g_player.position.y + HEIGHT / 2 > ROOM_HEIGHT / 2) {
    g_player.movement.y = -1;
  }
  if (g_player.position.x - WIDTH / 2 < -ROOM_WIDTH / 2) {
    g_player.movement.x = +1;
  }
  if (g_player.position.y - HEIGHT / 2 < -ROOM_HEIGHT / 2) {
    g_player.movement.y = +1;
  }
  return Luna_Result_Success;
}

static Luna_Result renderScene(Luna_Surface* surface) {
  Luna_Color color;
  Luna_Color_setRGB(&color, 30, 30, 30);
  Luna_Surface_fill(surface, &color, Luna_BlendMode_Copy);

  const int32_t transformX = surface->width / 2;
  const int32_t transformY = surface->height / 2;

  /* Draw the floor of the room we are in. */
  Luna_Color floorColor;
  Luna_Color_setRGB(&floorColor, 80, 80, 80);
  Luna_Surface_fillRectangle(surface, &floorColor, Luna_BlendMode_Copy, transformX - ROOM_WIDTH / 2, transformY - ROOM_HEIGHT / 2, ROOM_WIDTH, ROOM_HEIGHT);

  Luna_Color playerColor;
  Luna_Color_setRGB(&playerColor, 10, 160, 10);
  Luna_Surface_fillRectangle(surface, &playerColor, Luna_BlendMode_Copy,
                             g_player.position.x + transformX - PLAYER_WIDTH / 2,
                             g_player.position.y + transformY - PLAYER_HEIGHT / 2,
                             PLAYER_WIDTH,
                             PLAYER_HEIGHT);

  Luna_Font_drawGlyph(g_font, surface, '9', 64, 64, Luna_BlendMode_SourceOver);

  return Luna_Result_Success;
}

static Luna_Result startupScene() {
  Luna_Result result;
  if (result = Luna_Font_create(&g_font, L"Arial", 24, '0', '9', 60)) {
    return result;
  }
  return Luna_Result_Success;
}

static Luna_Result shutdownScene() {
  if (g_font) {
    Luna_Font_destroy(g_font);
    g_font = NULL;
  }
  return Luna_Result_Success;
}

static Luna_Result App_updateScene() {
  return updateScene();
}

static Luna_Result App_renderScene(int32_t width, int32_t height) {
  Luna_Result result;
  if (!g_surface) {
    if (result = Luna_Surface_create(&g_surface, Luna_PixelFormat_BGRX, width, height)) {
      return result;
    }
  }
  if (g_surface->width != width || g_surface->height != height) {
    Luna_Surface_destroy(g_surface);
    if (result = Luna_Surface_create(&g_surface, Luna_PixelFormat_BGRX, width, height)) {
      return result;
    }

  }

  if (result = renderScene(g_surface)) {
    return result;
  } 
  if (result = Luna_renderToCanvas(g_surface)) {
    return result;
  }
  
  return Luna_Result_Success;
}

static Luna_Result handleEvents() {
  do {
    Luna_Event* event = NULL;
    Luna_Result result = Luna_Result_Success;
    result = Luna_popEvent(&event);
    if (result == Luna_Result_Success) {
      switch (event->kind) {
        case Luna_EventKind_ApplicationQuitRequested: {
          fprintf(stdout, "received `application quit requested` event\n");
          g_quit = true;
        } break;
        case Luna_EventKind_CanvasOpened: {
          fprintf(stdout, "received `canvas opened` event\n");
        } break;
        case Luna_EventKind_CanvasClosed: {
          fprintf(stdout, "received `canvas closed` event\n");
        } break;
        case Luna_EventKind_CanvasSizeChanged: {
          fprintf(stdout, "received `canva size changed` event\n");
        } break;
      }
      Luna_deleteEvent(event);
      event = NULL;
    } else if (result == Luna_Result_IsEmpty) {
      return Luna_Result_Success;
    } else {
      g_quit = true;
      return result;
    }
  } while (true);
}

static Luna_Result startupEngine() {
  Luna_Result result = Luna_Result_Success;
  if (result = Luna_startupEventQueue()) {
    return result;
  }
  if (result = Luna_openCanvas()) {
    Luna_shutdownEventQueue();
    return result;
  }
  if (result = Luna_Clock_initialize(&g_clock)) {
    Luna_closeCanvas();
    Luna_shutdownEventQueue();
    return result;
  }
  if (result = Luna_Surface_create(&g_surface, Luna_PixelFormat_BGRX, 64, 64)) {
    Luna_Clock_uninitialize(&g_clock);
    Luna_closeCanvas();
    Luna_shutdownEventQueue();
    return result;
  }
  if (result = Luna_Clock_setPaused(&g_clock, false)) {
    Luna_Surface_destroy(g_surface);
    g_surface = NULL;
    Luna_Clock_uninitialize(&g_clock);
    Luna_closeCanvas();
    Luna_shutdownEventQueue();
    return result;
  }
  return Luna_Result_Success;
}

static Luna_Result shutdownEngine() {
  Luna_Surface_destroy(g_surface);
  g_surface = NULL;
  Luna_Clock_uninitialize(&g_clock);
  Luna_closeCanvas();
  Luna_shutdownEventQueue();
  return Luna_Result_Success;
}

static Luna_Result onMain() {
  Luna_Result result = Luna_Result_Success;
  if (result = startupEngine()) {
    return result;
  }
  if (result = startupScene()) {
    shutdownEngine();
    return result;
  }
  while (!g_quit) {
    int32_t width, height;
    if (result = Luna_getCanvaSize(&width, &height)) {
      g_quit = true;
      break;
    }
    App_renderScene(width, height);
    App_updateScene();
    // Handle the events.
    result = handleEvents();
    if (result) {
      g_quit = true;
    }
    // If the "canvas closed" event was not received, ensure we terminate if the canvas was closed.
    bool isCanvasClosed;
    if (result = Luna_getIsCanvasClosed(&isCanvasClosed)) {
      g_quit = true;
      break;
    }
    if (isCanvasClosed) {
      Luna_Event* event;
      if (result = Luna_createApplicationQuitRequestedEvent(&event)) {
        g_quit = true;
        break;
      }
      if (result = Luna_pushEvent(event)) {
        g_quit = true;
        break;
      }
    }
    if (result = Luna_updateCanvas()) {
      g_quit = true;
      break;
    }
  }
  shutdownScene();
  shutdownEngine();
  return result;
}

static Luna_Result test1() {
  Luna_Color color;
  Luna_Color_setRGBA(&color, 255, 128, 64, 0);
  if (color.r != 255 || color.g != 128 || color.b != 64 || color.a != 0) {
    return Luna_Result_EnvironmentFailed;
  }
  return Luna_Result_Success;
}

static Luna_Result test2() {
  Luna_Font* font = NULL;
  Luna_Result result = Luna_Result_Success;
  if (result = Luna_Font_create(&font, L"Arial", 12, '0', '9', 8)) {
    return result;
  }
  Luna_Font_Metrics metrics;
  if (result = Luna_Font_getMetrics(font, &metrics)) {
    Luna_Font_destroy(font);
    font = NULL;
    return result;
  }
  for (uint32_t i = '0', n = '9'; i <= n; ++i) {
    Luna_Font_GlyphMetrics glyphMetrics;
    if (result = Luna_Font_getGlyphMetrics(font, i, &glyphMetrics)) {
      Luna_Font_destroy(font);
      font = NULL;
      return result;
    }
  }
  if (result = Luna_Font_destroy(font)) {
    return result;
  }
  return result;
}

#include "Luna/Blend/SourceOver.h"

static Luna_Result test3() {
  Luna_Color source;
  Luna_Color destination;
  Luna_Color output;

  Luna_Color_setRGBA(
    &source,
    255,
    255,
    255,
    255
  );

  Luna_Color_setRGB(
    &destination,
    0,
    0,
    0
  );

  Luna_BlendModeSourceOver_do(
    &source,
    &destination,
    &output
  );
  if (output.r != 255 || output.g != 255 || output.b != 255 || output.a != 255) {
    return Luna_Result_EnvironmentFailed;
  }
  return Luna_Result_Success;
}

#if defined(Luna_Configuration_Windows)

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

int WINAPI WinMain(
  _In_ HINSTANCE hInstance,
  _In_opt_  HINSTANCE hPrevInstance,
  _In_ LPSTR lpCmdLine,
  _In_ int nShowCmd
)

#else

int main(int argc, char** argv)

#endif

{
  if (test1()) {
    return EXIT_FAILURE;
  }
  if (test2()) {
    return EXIT_FAILURE;
  }
  if (test3()) {
    return EXIT_FAILURE;
  }
  if (onMain()) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
