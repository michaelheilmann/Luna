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
 
#ifndef LUNA_FONT_H
#define LUNA_FONT_H

#include "Luna/Pixels.h"

typedef struct Luna_Font Luna_Font;

typedef struct Luna_Font_Metrics Luna_Font_Metrics;

typedef struct Luna_Font_GlyphMetrics Luna_Font_GlyphMetrics;

typedef struct Luna_Font_Glyph Luna_Font_Glyph;

struct Luna_Font_Metrics {
  int32_t ascent;
  int32_t descent;
  int32_t lineHeight;
};

struct Luna_Font_GlyphMetrics {
  int32_t bearingX;
  int32_t bearingY;
  int32_t width;
  int32_t height;
  int32_t advanceX;
};

Luna_Result
Luna_Font_create
  (
    Luna_Font** self,
    wchar_t const* faceName,
    int32_t pixelHeight,
    uint32_t firstCodePoint,
    uint32_t lastCodePoint,
    int32_t atlasWidth
  );

Luna_Result
Luna_Font_destroy
  (
    Luna_Font* self
  );

Luna_Result
Luna_Font_getMetrics
  (
    Luna_Font const* self,
    Luna_Font_Metrics* metrics
  );

Luna_Result
Luna_Font_getGlyphMetrics
  (
    Luna_Font const* self,
    uint32_t codePoint,
    Luna_Font_GlyphMetrics* metrics
  );

Luna_Result
Luna_Font_drawGlyph
  (
    Luna_Font const* self,
    Luna_Surface* destination,
    uint32_t codePoint,
    int32_t x,
    int32_t y,
    Luna_BlendMode blendMode
  );

#endif /* LUNA_FONT_H */
