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

#include "Luna/Font.h"

#include "Luna/Memory.h"
#include <assert.h>

#if Luna_Configuration_Windows

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef struct Luna_Font_GDIContext {
  HDC dc;
  HFONT font;
  HFONT oldFont;
} Luna_Font_GDIContext;

struct Luna_Font_Glyph {
  uint32_t codePoint;

  int32_t atlasLeft;
  int32_t atlasTop;

  int32_t width;
  int32_t height;

  int32_t bearingX;
  int32_t bearingY;

  int32_t advanceX;
};

struct Luna_Font {
  Luna_Surface* surface;
  /* The glyph of code point c is located at index i := c - firstCodePoint */
  Luna_Font_Glyph* glyphs;
  size_t numberOfGlyphs;
  /* See glyphs. */
  uint32_t firstCodePoint;

  int32_t ascent;
  int32_t descent;
  int32_t lineHeight;
};

#define LUNA_FONT_GLYPH_PADDING 1

static Luna_Result
Luna_Font_GDIContext_initialize
  (
    Luna_Font_GDIContext* context,
    wchar_t const* faceName,
    int32_t pixelHeight
  )
{
  context->dc = CreateCompatibleDC(NULL);

  if (!context->dc) {
    return Luna_Result_EnvironmentFailed;
  }

  context->font =
    CreateFontW
      (
        -pixelHeight,
        0,
        0,
        0,
        FW_NORMAL,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        faceName
      );

  if (!context->font) {
    DeleteDC(context->dc);
    context->dc = NULL;

    return Luna_Result_EnvironmentFailed;
  }

  context->oldFont =
    SelectObject(
      context->dc,
      context->font
    );

  if (context->oldFont == HGDI_ERROR) {
    DeleteObject(context->font);
    DeleteDC(context->dc);

    context->font = NULL;
    context->dc = NULL;

    return Luna_Result_EnvironmentFailed;
  }

  return Luna_Result_Success;
}

static Luna_Result
Luna_Font_GDIContext_uninitialize
  (
    Luna_Font_GDIContext* context
  )
{
  if (context->oldFont) {
    SelectObject(
      context->dc,
      context->oldFont
    );
  }

  if (context->font) {
    DeleteObject(context->font);
    context->font = NULL;
  }

  if (context->dc) {
    DeleteDC(context->dc);
    context->dc = NULL;
  }

  context->oldFont = NULL;

  return Luna_Result_Success;
}

static Luna_Result
Luna_Font_GDIContext_getMetrics
  (
    Luna_Font_GDIContext const* context,
    int32_t* outAscent,
    int32_t* outDescent,
    int32_t* outLineHeight
  )
{
  TEXTMETRICW metrics;

  assert(context);
  assert(outAscent);
  assert(outDescent);
  assert(outLineHeight);

  if (!GetTextMetricsW(context->dc, &metrics)) {
    return Luna_Result_EnvironmentFailed;
  }

  *outAscent = metrics.tmAscent;
  *outDescent = metrics.tmDescent;
  *outLineHeight = metrics.tmHeight;

  return Luna_Result_Success;
}

static MAT2
Luna_Font_GDIContext_getIdentityMatrix
  (
    void
  )
{
  MAT2 matrix;

  matrix.eM11.value = 1;
  matrix.eM11.fract = 0;

  matrix.eM12.value = 0;
  matrix.eM12.fract = 0;

  matrix.eM21.value = 0;
  matrix.eM21.fract = 0;

  matrix.eM22.value = 1;
  matrix.eM22.fract = 0;

  return matrix;
}

static Luna_Result
Luna_Font_GDIContext_getGlyphMetrics
  (
    Luna_Font_GDIContext const* context,
    uint32_t codePoint,
    Luna_Font_Glyph* glyph,
    size_t* outBitmapSize
  )
{
  GLYPHMETRICS metrics;
  MAT2 matrix;
  DWORD bitmapSize;

  assert(context);
  assert(glyph);
  assert(outBitmapSize);

  matrix =
    Luna_Font_GDIContext_getIdentityMatrix();

  bitmapSize =
    GetGlyphOutlineW(
      context->dc,
      (UINT)codePoint,
      GGO_GRAY8_BITMAP,
      &metrics,
      0,
      NULL,
      &matrix
    );

  if (bitmapSize == GDI_ERROR) {
    return Luna_Result_EnvironmentFailed;
  }

  glyph->codePoint = codePoint;

  glyph->width = (int32_t)metrics.gmBlackBoxX;

  glyph->height = (int32_t)metrics.gmBlackBoxY;

  glyph->bearingX = (int32_t)metrics.gmptGlyphOrigin.x;

  glyph->bearingY = (int32_t)metrics.gmptGlyphOrigin.y;

  glyph->advanceX = (int32_t)metrics.gmCellIncX;

  *outBitmapSize = (size_t)bitmapSize;

  return Luna_Result_Success;
}

static Luna_Result
Luna_Font_GDIContext_getGlyphBitmap
  (
    Luna_Font_GDIContext const* context,
    uint32_t codePoint,
    void* pixels,
    size_t capacity
  )
{
  GLYPHMETRICS metrics;
  MAT2 matrix;
  DWORD bitmapSize;

  assert(context);

  if (capacity > MAXDWORD) {
    return Luna_Result_ArgumentValueInvalid;
  }

  matrix = Luna_Font_GDIContext_getIdentityMatrix();

  bitmapSize =
    GetGlyphOutlineW(
      context->dc,
      (UINT)codePoint,
      GGO_GRAY8_BITMAP,
      &metrics,
      (DWORD)capacity,
      pixels,
      &matrix
    );

  if (bitmapSize == GDI_ERROR) {
    return Luna_Result_EnvironmentFailed;
  }

  if ((size_t)bitmapSize > capacity) {
    return Luna_Result_EnvironmentFailed;
  }

  return Luna_Result_Success;
}

#else

  #error("environment not yet supported")

#endif

static Luna_Result
Luna_Font_getGlyph
  (
    Luna_Font const* self,
    uint32_t codePoint,
    Luna_Font_Glyph const** glyph
  );

static Luna_Result
Luna_Font_getGlyph
  (
    Luna_Font const* self,
    uint32_t codePoint,
    Luna_Font_Glyph const** glyph
  )
{
  uint32_t index;

  if (!self || !glyph) {
    return Luna_Result_ArgumentValueInvalid;
  }

  if (codePoint < self->firstCodePoint) {
    return Luna_Result_ArgumentValueInvalid;
  }

  index = codePoint - self->firstCodePoint;

  if ((size_t)index >= self->numberOfGlyphs) {
    return Luna_Result_ArgumentValueInvalid;
  }

  *glyph = &self->glyphs[index];

  return Luna_Result_Success;
}

Luna_Result
Luna_Font_construct
  (
    Luna_Font* self,
    wchar_t const* faceName,
    int32_t pixelHeight,
    uint32_t firstCodePoint,
    uint32_t lastCodePoint,
    int32_t atlasWidth
  )
{
  Luna_Result result = Luna_Result_Success;
  Luna_Font_GDIContext gdiContext;

  size_t numberOfGlyphs;

  int32_t atlasHeight;
  int32_t atlasX;
  int32_t atlasY;
  int32_t rowHeight;

  assert(self);

  if (!faceName) {
    return Luna_Result_ArgumentValueInvalid;
  }

  if (pixelHeight <= 0) {
    return Luna_Result_ArgumentValueInvalid;
  }

  if (firstCodePoint > lastCodePoint) {
    return Luna_Result_ArgumentValueInvalid;
  }

  /*
   * The current Windows implementation uses UTF-16 code units
   * and therefore supports only the Basic Multilingual Plane.
   */
  if (lastCodePoint > UINT16_MAX) {
    return Luna_Result_ArgumentValueInvalid;
  }

  if (atlasWidth <= 0) {
    return Luna_Result_ArgumentValueInvalid;
  }

  {
    uint64_t numberOfGlyphs64 = (uint64_t)lastCodePoint - (uint64_t)firstCodePoint + UINT64_C(1);
    if (numberOfGlyphs64 > SIZE_MAX / sizeof(Luna_Font_Glyph)) {
      return Luna_Result_ArgumentValueInvalid;
    }
    numberOfGlyphs =
      (size_t)numberOfGlyphs64;
  }

  /*
   * Initialize the object's members before acquiring resources.
   * Also initialize the GDI context.
   * This makes the failure path safe.
   */
  self->surface = NULL;
  self->glyphs = NULL;
  self->numberOfGlyphs = 0;
  self->firstCodePoint = firstCodePoint;
  self->ascent = 0;
  self->descent = 0;
  self->lineHeight = 0;
  if (result = Luna_Font_GDIContext_initialize(&gdiContext, faceName, pixelHeight)) {
    return result;
  }
  /* After this point, in case of an error, we simply may set result and goto label Error. */

  /* Obtain the global font metrics. */
  if (result = Luna_Font_GDIContext_getMetrics(&gdiContext, &self->ascent, &self->descent, &self->lineHeight)) {
    goto Error;
  }
  /* Allocate the glyph array. */
  if (result = Luna_allocate((void**)&self->glyphs, numberOfGlyphs * sizeof(Luna_Font_Glyph))) {
    goto Error;
  }
  self->numberOfGlyphs = numberOfGlyphs;

  /*  First pass: Obtain glyph metrics and assign each glyph a location in the atlas. */
  atlasHeight = 0;
  atlasX = 0;
  atlasY = 0;
  rowHeight = 0;

  for (size_t i = 0; i < numberOfGlyphs; ++i) {
    Luna_Font_Glyph* glyph =&self->glyphs[i];

    size_t bitmapSize;

    if (result = Luna_Font_GDIContext_getGlyphMetrics(&gdiContext, firstCodePoint + (uint32_t)i, glyph, &bitmapSize)) {
      goto Error;
    }

    /* Glyphs such as the space character can have an advance but no bitmap. */
    if (glyph->width <= 0 ||
        glyph->height <= 0) {
        glyph->atlasLeft = 0;
        glyph->atlasTop = 0;
      continue;
    }

    int32_t requiredWidth = glyph->width + 2 * LUNA_FONT_GLYPH_PADDING;

    int32_t requiredHeight = glyph->height + 2 * LUNA_FONT_GLYPH_PADDING;

    /* Detect signed integer overflow. */
    if (requiredWidth < glyph->width || requiredHeight < glyph->height) {
      result = Luna_Result_ArgumentValueInvalid;
      goto Error;
    }

    /* The glyph must fit into a row of the requested width. */
    if (requiredWidth > atlasWidth) {
      result = Luna_Result_ArgumentValueInvalid;
      goto Error;
    }

    /* Start a new row if the glyph doesn't fit into the current row. */
    if (atlasX > atlasWidth - requiredWidth) {
      if (atlasY > INT32_MAX - rowHeight) {
        result = Luna_Result_ArgumentValueInvalid;
        goto Error;
      }

      atlasY += rowHeight;
      atlasX = 0;
      rowHeight = 0;
    }

    glyph->atlasLeft = atlasX + LUNA_FONT_GLYPH_PADDING;

    glyph->atlasTop = atlasY + LUNA_FONT_GLYPH_PADDING;

    atlasX += requiredWidth;

    if (requiredHeight > rowHeight) {
      rowHeight = requiredHeight;
    }

    if (atlasY > INT32_MAX - rowHeight) {
      result = Luna_Result_ArgumentValueInvalid;
      goto Error;
    }

    atlasHeight = atlasY + rowHeight;
  }

  /* A font with no bitmap-bearing glyphs still gets a valid one-pixel-high atlas. */
  if (atlasHeight <= 0) {
    atlasHeight = 1;
  }

  /* Create the atlas. */
  if (result = Luna_Surface_create(&self->surface, Luna_PixelFormat_BGRA, atlasWidth, atlasHeight)) {
    goto Error;
  }

  /* Initialize the atlas to transparent white. Glyph coverage is stored in the alpha component. */
  {
    Luna_Color color;
    if (result = Luna_Color_setRGBA(&color, 255, 255, 255, 0)) {
      goto Error;
    }

    if (result = Luna_Surface_fill(self->surface, &color, Luna_BlendMode_Copy)) {
      goto Error;
    }
  }
  /* Second pass: Retrieve every glyph bitmap and copy its grayscale coverage into the alpha channel of the atlas. */
  for (size_t i = 0; i < numberOfGlyphs; ++i) {
    Luna_Font_Glyph* glyph = &self->glyphs[i];

    void* bitmap = NULL;
    size_t bitmapSize;

    /* Empty glyphs don't have bitmap data. */
    if (glyph->width <= 0 || glyph->height <= 0) {
      continue;
    }
    /* Query the required temporary bitmap size again. */
    if (result = Luna_Font_GDIContext_getGlyphMetrics(&gdiContext, glyph->codePoint, glyph, &bitmapSize)) {
      goto Error;
    }
    if (bitmapSize == 0) {
      continue;
    }

    if (result = Luna_allocate(&bitmap, bitmapSize)) {
      goto Error;
    }
    if (result = Luna_Font_GDIContext_getGlyphBitmap(&gdiContext, glyph->codePoint, bitmap, bitmapSize)) {
      Luna_deallocate(bitmap);
      bitmap = NULL;
      goto Error;
    }

    /* GGO_GRAY8_BITMAP returns DWORD-aligned rows. */
    size_t sourceStride = ((size_t)glyph->width + 3u) & ~((size_t)3u);

    /* A Luna_Surface stores one 32-bit value per pixel. */
    size_t destinationStride = (size_t)self->surface->width * sizeof(uint32_t);

    uint8_t const* source = (uint8_t const*)bitmap;

    uint8_t* destination = (uint8_t*)self->surface->pixels;

    for (int32_t y = 0; y < glyph->height; ++y) {

      uint8_t const* sourceRow = source + (size_t)y * sourceStride;

      uint8_t* destinationRow = destination 
                              + (size_t)(glyph->atlasTop + y) * destinationStride
                              + (size_t)glyph->atlasLeft * sizeof(Luna_Color);

      for (int32_t x = 0; x < glyph->width; ++x) {
        uint8_t coverage = sourceRow[x];

        /*
          * GGO_GRAY8_BITMAP uses a coverage range of 0..64.
          * Convert it to Luna's normal 8-bit alpha range.
          */
        coverage =
          (uint8_t)(
            ((uint32_t)coverage * 255u + 32u) / 64u
            );

          /*
            * BGRA:
            *
            * B = 255
            * G = 255
            * R = 255
            * A = coverage
            */
        destinationRow[
          (size_t)x * sizeof(Luna_Color) + 0
        ] = 255;

        destinationRow[
          (size_t)x * sizeof(Luna_Color) + 1
        ] = 255;

        destinationRow[
          (size_t)x * sizeof(Luna_Color) + 2
        ] = 255;

        destinationRow[
          (size_t)x * sizeof(Luna_Color) + 3
        ] = coverage;
      }
    }

    Luna_deallocate(bitmap);
    bitmap = NULL;
  }

  Luna_Font_GDIContext_uninitialize(&gdiContext);

  return result;

Error:
  if (self->surface) {
    Luna_Surface_destroy(self->surface);
    self->surface = NULL;
  }
  if (self->glyphs) {
    Luna_deallocate(self->glyphs);
    self->glyphs = NULL;
  }
  Luna_Font_GDIContext_uninitialize(&gdiContext);
  return result;
}

Luna_Result
Luna_Font_destruct
  (
    Luna_Font* self
  )
{
  if (!self) {
    return Luna_Result_ArgumentValueInvalid;
  }

  if (self->surface) {
    Luna_Surface_destroy(self->surface);
    self->surface = NULL;
  }

  if (self->glyphs) {
    Luna_deallocate(self->glyphs);
    self->glyphs = NULL;
  }

  self->numberOfGlyphs = 0;
  self->firstCodePoint = 0;

  self->ascent = 0;
  self->descent = 0;
  self->lineHeight = 0;

  return Luna_Result_Success;
}

Luna_Result
Luna_Font_create
  (
    Luna_Font** self,
    wchar_t const* faceName,
    int32_t pixelHeight,
    uint32_t firstCodePoint,
    uint32_t lastCodePoint,
    int32_t atlasWidth
  )
{
  Luna_Result result;
  Luna_Font* temporary;

  if (!self) {
    return Luna_Result_ArgumentValueInvalid;
  }

  if (result = Luna_allocate((void**)&temporary, sizeof(Luna_Font))) {
    return result;
  }

  if (result = Luna_Font_construct(temporary, faceName, pixelHeight, firstCodePoint, lastCodePoint, atlasWidth)) {
    Luna_deallocate(temporary);
    temporary = NULL;
    return result;
  }
  *self = temporary;
  return Luna_Result_Success;
}

Luna_Result
Luna_Font_destroy
  (
    Luna_Font* self
  )
{
  Luna_Result result;

  if (!self) {
    return Luna_Result_ArgumentValueInvalid;
  }
  Luna_Font_destruct(self);
  return Luna_deallocate(self);
}

Luna_Result
Luna_Font_getMetrics
  (
    Luna_Font const* self,
    Luna_Font_Metrics* metrics
  )
{ 
  if (!self || !metrics) {
    return Luna_Result_ArgumentValueInvalid;
  }

  metrics->ascent = self->ascent;

  metrics->descent = self->descent;

  metrics->lineHeight = self->lineHeight;

  return Luna_Result_Success;
}

Luna_Result
Luna_Font_getGlyphMetrics
  (
    Luna_Font const* self,
    uint32_t codePoint,
    Luna_Font_GlyphMetrics* metrics
  )
{
  Luna_Font_Glyph const* glyph;

  if (!self || !metrics) {
    return Luna_Result_ArgumentValueInvalid;
  }

  Luna_Result result;
  if (result = Luna_Font_getGlyph(self, codePoint, &glyph)) {
    return result;
  }

  metrics->width = glyph->width;
  metrics->height = glyph->height;
  metrics->bearingX = glyph->bearingX;
  metrics->bearingY = glyph->bearingY;
  metrics->advanceX = glyph->advanceX;

  return Luna_Result_Success;
}

Luna_Result
Luna_Font_drawGlyph
  (
    Luna_Font const* self,
    Luna_Surface* destination,
    uint32_t codePoint,
    int32_t x,
    int32_t y,
    Luna_BlendMode blendMode
  )
{ 
  Luna_Font_Glyph const* glyph;
  Luna_Result result;
  int32_t destinationLeft;
  int32_t destinationTop;

  if (!self || !destination) {
    return Luna_Result_ArgumentValueInvalid;
  }

  result =
    Luna_Font_getGlyph(
      self,
      codePoint,
      &glyph
    );

  if (result) {
    return result;
  }

  if (glyph->bearingX > 0) {
    if (x > INT32_MAX - glyph->bearingX) {
      return Luna_Result_ArgumentValueInvalid;
    }
  } else if (glyph->bearingX < 0) {
    if (x < INT32_MIN - glyph->bearingX) {
      return Luna_Result_ArgumentValueInvalid;
    }
  }

  destinationLeft =
    x + glyph->bearingX;

  if (glyph->bearingY > 0) {
    if (y < INT32_MIN + glyph->bearingY) {
      return Luna_Result_ArgumentValueInvalid;
    }
  } else if (glyph->bearingY < 0) {
    if (y > INT32_MAX + glyph->bearingY) {
      return Luna_Result_ArgumentValueInvalid;
    }
  }

  destinationTop =
    y - glyph->bearingY;

  return Luna_Surface_copyRectangle(
    destination,
    destinationLeft,
    destinationTop,
    self->surface,
    glyph->atlasLeft,
    glyph->atlasTop,
    glyph->width,
    glyph->height,
    blendMode
  );
}
