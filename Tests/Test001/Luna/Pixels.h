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
 
#ifndef LUNA_PIXELS_H
#define LUNA_PIXELS_H

#include "Luna/Result.h"
#include <inttypes.h>
#include <stdbool.h>

/* An enumeration of blend modes. When combining two input pixel values, then the blend mode determines how a source pixel is composited with a destination pixel value. */
typedef enum Luna_BlendMode {

  /*
   * The source pixel replaces the destination pixel value.
   * 
   * RGBA -> RGBA
   * (Or,Og,Ob,Oa) = (Sr,Sg,Sb,Sa)
   *
   * RGBA -> RGBX
   * (Or, Og, Ob, 255) = (Sr, Sg, Sb, Sa)
   *
   * RGBX -> RGBA
   * (Or, Og, Ob, 255) = (Sr, Sg, Sb, 255)
   *
   * RGBX -> RGBX
   * (Or, Og, Ob, 255) = (Sr, Sg, Sb, 255)
   */
  Luna_BlendMode_Copy,

  /*
   * Copy the source pixel over the destination pixel using the alpha components of both pixels.
   * 
   * BGRA -> BGRA
   * source.
   * if src.a and / or dst.a is not zero 
   * out.a = src.a + dst.a * (255 - src.a) if src.a and / or dst.a is not zero 
   * out.rgb = (src.rgb * src.a + dst.rgb * dst.a * (255 - src.a)) / dst.a
   *
   * if both src.a and dst.a are zero
   * out.a = 0
   * out.rgb = (0,0,0)
   *
   * BGRX -> BGRA
   * BGRX assumes an alpha of 255. Hence te formula for BGRA -> BGRA reduces to
   * out.a = src.a + dst.a * (255 - 255) = src.a
   * out.rgb = (src.rgb * src.a + dst.rgb * dst.a * (255 - src.a)) / out.a
   *
   * BGRA -> BGRX
   * as dst.a
   * out.a = src.a + dst.a * (255 - src.a) | as dst.a = 255
  * 
   * out.rgb = (src.rgb * src.a + dst.rgb * dst.a * (255 - src.a)) / dst.a
   */
  Luna_BlendMode_SourceOver,

} Luna_BlendMode;

typedef enum Luna_PixelFormat {
  /** @internal */
  Luna_PixelFormat_BGRX = 1,

  /** @internal */
  Luna_PixelFormat_BGRA = 3
} Luna_PixelFormat;

typedef struct Luna_Color {
  Luna_PixelFormat pixelFormat;
  union {
    uint32_t value;
    struct {
      uint8_t b, g, r, a;
    };
  };
} Luna_Color;

static inline Luna_Result
Luna_Color_setRGB
  (
    Luna_Color* color,
    uint8_t r,
    uint8_t g,
    uint8_t b
  )
{
  color->pixelFormat = Luna_PixelFormat_BGRX;
  color->value = (((uint32_t)b) << 0) | (((uint32_t)g) << 8) | (((uint32_t)r) << 16) | (((uint32_t)255) << 24);
  return Luna_Result_Success;
}

static inline Luna_Result
Luna_Color_setRGBA
  (
    Luna_Color* color,
    uint8_t r,
    uint8_t g,
    uint8_t b,
    uint8_t a
  )
{
  color->pixelFormat = Luna_PixelFormat_BGRA;
  color->value = (((uint32_t)b) << 0) | (((uint32_t)g) << 8) | (((uint32_t)r) << 16) | (((uint32_t)a) << 24);
  return Luna_Result_Success;
}

static inline Luna_Result
Luna_Color_setPacked
  (
    Luna_Color* color,
    Luna_PixelFormat pixelFormat,
    uint32_t value
  )
{ 
  color->pixelFormat = pixelFormat;
  color->value = value;
  return Luna_Result_Success;
}


typedef struct Luna_Surface Luna_Surface;

struct Luna_Surface {
  void* pixels;
  Luna_PixelFormat pixelFormat;
  int32_t width;
  int32_t height;
  Luna_Result (*destroy)(Luna_Surface*);
};

Luna_Result
Luna_Surface_fillRectangle
  (
    Luna_Surface* self,
    Luna_Color const* color,
    Luna_BlendMode blendMode,
    int32_t left,
    int32_t top,
    int32_t width,
    int32_t height
  );

Luna_Result
Luna_Surface_fill
  (
    Luna_Surface* self,
    Luna_Color const* color,
    Luna_BlendMode blendMode
  );

Luna_Result
Luna_Surface_create
  (
    Luna_Surface** self,
    Luna_PixelFormat pixelFormat,
    int32_t width,
    int32_t height
  );

Luna_Result
Luna_Surface_createForeign
  (
    Luna_Surface** self,
    Luna_PixelFormat pixelFormat,
    int32_t width,
    int32_t height,
    void* pixels
  );

Luna_Result
Luna_Surface_destroy
  (
    Luna_Surface* self
  );

Luna_Result
Luna_Surface_copyRectangle
  (
    Luna_Surface* self,
    int32_t destinationLeft,
    int32_t destinationTop,
    Luna_Surface const* source,
    int32_t sourceLeft,
    int32_t sourceTop,
    int32_t sourceWidth,
    int32_t sourceHeight,
    Luna_BlendMode blendMode
  );

#endif /* LUNA_PIXELS_H */
