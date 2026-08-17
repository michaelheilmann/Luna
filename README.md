# Description
Luna is a drop-in C library for game development.

# Usage
To use the library in your project, copy the folder "Luna" from the directory in which this README.md resides in and all its contents to an folder of your choice.
Add that folder to your include directories to ensure that the include files are found.
Add the .c files in the "Luna" folder to the list of your compiled files.

Add the constant `Luna_Configuration_Windows` to the list of preprocessor definitions to use Luna under Windows.
Add the constant `Luna_Configuration_Linux` to the list of preprocessor definitions to use Luna under Linux.

Add the constant `Luna_Configuration_BigEndian` to the list of preprocessor definitions if your system is "big endian".
Add the constant `Luna_Configuration_LittleEndian` to the list of preprocessor definitions if your system is "little endian" (hint: Windows is usually "little endian").

# Usage Example
Assume you have a CMake-based project called game
```
|_ sources <- contains "main.c"
|_ external/luna-1.0 <- contains the folder "Luna"
|_ CMakeLists.txt <- the CMake files
```
Then your `CMakeLists.txt` could look something like the following
```
set(LUNA_SOURCES "")
list(APPEND SOURCES Luna/Canvas.c Luna/Canvas.h)
list(APPEND SOURCES Luna/Clock.c Luna/Clock.h)
list(APPEND SOURCES Luna/Deque.c Luna/Deque.h)
list(APPEND SOURCES Luna/Event.c  Luna/Event.h)
list(APPEND SOURCES Luna/Font.c  Luna/Font.h)
list(APPEND SOURCES Luna/Result.c Luna/Result.h)
list(APPEND SOURCES Luna/Memory.c Luna/Memory.h)
list(APPEND SOURCES Luna/Pixels.c Luna/Pixels.h)
list(APPEND SOURCES Luna/Blend/Copy.c Luna/Blend/Copy.h)
list(APPEND SOURCES Luna/Blend/SourceOver.c Luna/Blend/SourceOver.h)

add_executable(MyGame ${CMAKE_CURRENT_SOURCE_DIR}/sources/main.c ${LUNA_SOURCES})
target_include_directories(MyGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

IF (WIN32)
  add_compile_definitions(Luna_Configuration_Windows)
ELSE()
  add_compile_definitions(Luna_Configuration_Linux)
ENDIF()
```

Your `main.c` could look something like the following
```
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "Luna/Event.h"
#include "Luna/Canvas.h"

static bool g_quit = false;

static Luna_Result onHandleEvents() {
  while (true) {
    Luna_Event* event = NULL;
    Luna_Result result = Luna_Result_Success;
    result = Luna_popEvent(&event);
    if (result == Luna_Result_Success) {
      switch (event->kind) {
        case Luna_EventKind_ApplicationQuitRequested: {
          fprintf(stdout, "received `application quit requested` event\n");
          quit = true;
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
      quit = true;
      return result;
    }
  }
}

static Luna_Result onMain() {
  Luna_Result result = Luna_Result_Success;
  result = Luna_startupEventQueue();
  if (result) {
    return result;
  }
  result = Luna_openCanvas();
  if (result) {
    Luna_shutdownEventQueue();
    return result;
  }
  while (!quit) {
    /* Handle the events. */
    result = handleEvents();
    if (result) {
      quit = true;
      break;
    }
    /* If the "canvas closed" event was not received, ensure we terminate if the canvas was closed. */
    bool isCanvasClosed;
    result = Luna_getIsCanvasClosed(&isCanvasClosed);
    if (result) {
      quit = true;
      break;
    }
    if (isCanvasClosed) {
      Luna_Event* event;
      result = Luna_createApplicationQuitRequestedEvent(&event);
      if (result) {
        quit = true;
        break;
      }
      result = Luna_pushEvent(event);
      if (result) {
        quit = true;
        break;
      }
    }
    result = Luna_updateCanvas();
    if (result) {
      quit = true;
      break;
    }
  }
  Luna_closeCanvas();
  Luna_shutdownEventQueue();;
  return result;
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
  if (onMain()) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
```
