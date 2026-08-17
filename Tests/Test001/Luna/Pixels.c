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
 
#include "Luna/Pixels.h"

#include "Luna/Memory.h"
#include "Luna/Blend/Copy.h"
#include "Luna/Blend/SourceOver.h"
#include <string.h>
#include <assert.h>

/*
 * Compute the product of a and b.
 * Return "false" if there was no overflow and store the result in "result".
 * Otherwise return "true".
 */
bool mul_overflow_size(size_t a, size_t b, size_t* result);

/*
 * Compute the product of a and b.
 * Return "false" if there was no overflow and store the result in "result".
 * Otherwise return "true".
 */
bool mul_overflow_i32(int32_t a, int32_t b, int32_t* result);

/*
 * Compute the product of a and b.
 * Return "false" if there was no overflow and store the result in "result".
 * Otherwise return "true".
 */
bool mul_overflow_i64(int64_t a, int64_t b, int64_t* result);

bool mul_overflow_size(size_t a, size_t b, size_t* result) {
  if (a != 0 && b > SIZE_MAX / a) {
    return true;
  }

  *result = a * b;
  return false;
}

bool mul_overflow_i32(int32_t a, int32_t b, int32_t* result) {
  if (a == 0 || b == 0) {
    *result = 0;
    return false;
  }

  if (a > 0) {
    if (b > 0) {
      if (a > INT32_MAX / b)
        return true;
    } else {
      if (b < INT32_MIN / a)
        return true;
    }
  } else {
    if (b > 0) {
      if (a < INT32_MIN / b)
        return true;
    } else {
        /*
         * Both negative.
         * Special case: INT32_MIN * -1 overflows.
         */
      if (a < INT32_MAX / b)
        return true;
    }
  }

  *result = a * b;
  return false;
}

bool mul_overflow_i64(int64_t a, int64_t b, int64_t* result) {
  if (a == 0 || b == 0) {
    *result = 0;
    return false;
  }

  if (a > 0) {
    if (b > 0) {
      if (a > INT64_MAX / b)
        return true;
    } else {
      if (b < INT64_MIN / a)
        return true;
    }
  } else {
    if (b > 0) {
      if (a < INT64_MIN / b)
        return true;
    } else {
        /*
         * Both negative.
         * Special case: INT64_MIN * -1 overflows.
         */
      if (a < INT64_MAX / b)
        return true;
    }
  }

  *result = a * b;
  return false;
}

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
  )
{
  /* Sanity check #1. */
  if (width < INT32_C(0) || height < INT32_C(0)) {
    return Luna_Result_ArgumentValueInvalid;
  }
  /* width and height are guaranteed to be non-negative. */
  assert(width >= INT32_C(0));
  assert(height >= INT32_C(0));
  /* Sanity check #2. */
  if (width == INT32_C(0) || height == INT32_C(0) || left >= self->width || top >= self->height) {
    return Luna_Result_Success;
  }
  /* Sanity check #3. */
  if (left == INT32_MIN || top == INT32_MIN) {
    /* As | INT32_MAX | < | INT32_MIN |, width or height would require a value greater than INT32_MAX for that anthing would be drawn. */
    return Luna_Result_Success;
  }
  /* left and top are guaranteed to be smaller than or equal to self->width and self->height, respectively. */
  assert(left <= self->width);
  assert(top <= self->height);
  /* left and top are guaranteed to be greater than INT32_MIN. */
  assert(left >= INT32_MIN);
  assert(top >= INT32_MIN);

  /* Sanity check #4. */
  if (left < 0) {
    if (-left > width) {
      return Luna_Result_Success;
    } else {
      width += left; /* This cannot overflow as width is non-negative and left is negative. */
      left = 0;
    }
  }
  if (top < 0) {
    if (-top > height) {
      return Luna_Result_Success;
    } else {
      height += top; /* This cannot overflow as height is non-negative and top is negative. */
      top = 0;
    }
  } 
  /* left and top are guaranteed to be non-negative. */
  assert(left >= 0);
  assert(top >= 0);

  if (self->width - left < width) {
    /* self->width - left is always non-negative as left <= self->width. */
    width = self->width - left;
  } 
  if (self->height - top < height) {
    /* self->height - top is always non-negative as top <= self->height. */
    height = self->height - top;
  }

  assert(left >= 0 && width <= self->width && left + width <= self->width);
  assert(top >= 0 && height <= self->height && top + height <= self->height);

  switch (blendMode) {
    case Luna_BlendMode_Copy: {
      uint32_t* pixels = (uint32_t*)self->pixels;
      Luna_Color destinationColor;
      destinationColor.pixelFormat = self->pixelFormat;
      int32_t surfaceWidth = self->width;
      for (int32_t y = top; y < top + height; ++y) {
        uint32_t* pixel = pixels + y * surfaceWidth + left;
        for (int32_t x = 0; x < width; ++x) {
          destinationColor.value =  *pixel;
          Luna_BlendModeCopy_do(color, &destinationColor, &destinationColor);
          *pixel = destinationColor.value;
          pixel++;
        }
      }
    } break;
    case Luna_BlendMode_SourceOver: {
      uint32_t* pixels = (uint32_t*)self->pixels;
      Luna_Color destinationColor;
      destinationColor.pixelFormat = self->pixelFormat;
      int32_t surfaceWidth = self->width;
      for (int32_t y = top, n = top + height; y < n; ++y) {
        uint32_t* pixel = pixels  + y * surfaceWidth + left;
        for (int32_t x = 0, m  = width; x < m; ++x) {
          destinationColor.value = *pixel;
          Luna_BlendModeSourceOver_do(color, &destinationColor, &destinationColor);
          *pixel = destinationColor.value;
          pixel++;
        }
      }
    } break;
    default: {
      return Luna_Result_ArgumentValueInvalid;
    } break;
  };
  return Luna_Result_Success;
}

Luna_Result
Luna_Surface_fill
  (
    Luna_Surface* self,
    Luna_Color const* color,
    Luna_BlendMode blendMode
  )
{
  switch (blendMode) {
    case Luna_BlendMode_Copy: {
      uint32_t* pixels = (uint32_t*)self->pixels;
      Luna_Color destinationColor;
      destinationColor.pixelFormat = self->pixelFormat;
      int32_t surfaceWidth = self->width;
      for (int32_t y = 0; y < self->height; ++y) {
        uint32_t* pixel = pixels + y * self->width + 0;
        for (int32_t x = 0; x < self->width; ++x) {
          destinationColor.value = *pixel;
          Luna_BlendModeCopy_do(color, &destinationColor, &destinationColor);
          *pixel = destinationColor.value;
          pixel++;
        }
      }
    } break;
    case Luna_BlendMode_SourceOver: {
      uint32_t* pixels = (uint32_t*)self->pixels;
      Luna_Color destinationColor;
      destinationColor.pixelFormat = self->pixelFormat;
      int32_t surfaceWidth = self->width;
      for (int32_t y = 0; y < self->height; ++y) {
        uint32_t* pixel = pixels + y * self->width + 0;
        for (int32_t x = 0; x < self->width; ++x) {
          destinationColor.value = *pixel;
          Luna_BlendModeSourceOver_do(color, &destinationColor, &destinationColor);
          *pixel = destinationColor.value;
          pixel++;
        }
      }
    } break;
    default: {
      return Luna_Result_ArgumentValueInvalid;
    } break;
  };
  return Luna_Result_Success;
}

static Luna_Result
destroySurface
  (
    Luna_Surface* self
  )
{
  if (!self) {
    return Luna_Result_ArgumentValueInvalid;
  }
  Luna_deallocate(self->pixels);
  self->pixels = NULL;
  Luna_deallocate(self);
  self = NULL;
  return Luna_Result_Success;
}

static Luna_Result
destroyForeignSurface
  (
    Luna_Surface* self
  )
{
  if (!self) {
    return Luna_Result_ArgumentValueInvalid;
  }
  Luna_deallocate(self);
  self = NULL;
  return Luna_Result_Success;
}

Luna_Result
Luna_Surface_create
  (
    Luna_Surface** self,
    Luna_PixelFormat pixelFormat,
    int32_t width,
    int32_t height
  )
{
  if (!self) {
    return Luna_Result_ArgumentValueInvalid;
  }
  int32_t numberOfPixels;
  if (mul_overflow_i32(width, height, &numberOfPixels)) {
    return Luna_Result_ArgumentValueInvalid;
  }
  size_t numberOfBytes;
  if (numberOfPixels < 0 || numberOfPixels > SIZE_MAX || mul_overflow_size((size_t)numberOfPixels, sizeof(uint32_t), &numberOfBytes)) {
    return Luna_Result_ArgumentValueInvalid;
  }
  Luna_Result result;
  Luna_Surface* temporary = NULL;
  if (result = Luna_allocate(&temporary, sizeof(Luna_Surface))) {
    return result;
  }
  temporary->pixelFormat = pixelFormat;
  temporary->width = width;
  temporary->height = height;
  if (result = Luna_allocate((void**)&temporary->pixels, numberOfBytes)) {
    Luna_deallocate(temporary);
    temporary = NULL;
    return result;
  }

  // Fill with opaque black.  
  uint32_t* pixels = (uint32_t*)temporary->pixels;
  Luna_Color destinationColor;
  Luna_Color black;
  Luna_Color_setRGB(&black, 0, 0, 0);
  destinationColor.pixelFormat = temporary->pixelFormat;
  int32_t surfaceWidth = temporary->width;
  for (int32_t y = 0; y < temporary->height; ++y) {
    uint32_t* pixel = pixels + y * temporary->width + 0;
    for (int32_t x = 0; x < temporary->width; ++x) {
      destinationColor.value = *pixel;
      Luna_BlendModeCopy_do(&black, &destinationColor, &destinationColor);
      *pixel = destinationColor.value;
      pixel++;
    }
  }

  temporary->destroy = destroySurface;
  *self = temporary;
  return Luna_Result_Success;
}

Luna_Result
Luna_Surface_createForeign
  (
    Luna_Surface** self,
    Luna_PixelFormat pixelFormat,
    int32_t width,
    int32_t height,
    void* pixels
  )
{
  if (!self || !pixels) {
    return Luna_Result_ArgumentValueInvalid;
  }
  int32_t numberOfPixels;
  if (mul_overflow_i32(width, height, &numberOfPixels)) {
    return Luna_Result_ArgumentValueInvalid;
  }
  size_t numberOfBytes;
  if (numberOfPixels < 0 || numberOfPixels > SIZE_MAX || mul_overflow_size((size_t)numberOfPixels, sizeof(uint32_t), &numberOfBytes)) {
    return Luna_Result_ArgumentValueInvalid;
  }
  Luna_Result result;
  Luna_Surface* temporary = NULL;
  if (result = Luna_allocate(&temporary, sizeof(Luna_Surface))) {
    return result;
  }
  temporary->pixelFormat = pixelFormat;
  temporary->width = width;
  temporary->height = height;
  temporary->pixels = pixels;
  temporary->destroy = destroyForeignSurface;
  *self = temporary;
  return Luna_Result_Success;
}

Luna_Result
Luna_Surface_destroy
  (
    Luna_Surface* self
  )
{
  if (!self) {
    return Luna_Result_ArgumentValueInvalid;
  }
  if (self->destroy) {
    self->destroy(self);
  }
  return Luna_Result_Success;
}

Luna_Result
Luna_Surface_copyRectangle
  (
    Luna_Surface* self,
    int32_t destinationLeft,
    int32_t destinationTop,
    Luna_Surface const* source,
    int32_t sourceLeft,
    int32_t sourceTop,
    int32_t width,
    int32_t height,
    Luna_BlendMode blendMode
  )
{
  if (!self || !source) {
    return Luna_Result_ArgumentValueInvalid;
  }
  if (width < 0 || height < 0) {
    return Luna_Result_ArgumentValueInvalid;
  }

  if (width == 0 || height == 0) {
    return Luna_Result_Success;
  }

  /*
   * Clip the source rectangle against the source surface.
   */
  if (sourceLeft < 0) {
    if (sourceLeft == INT32_MIN || -sourceLeft >= width) {
      return Luna_Result_Success;
    }

    width += sourceLeft;
    destinationLeft -= sourceLeft;
    sourceLeft = 0;
  }

  if (sourceTop < 0) {
    if (sourceTop == INT32_MIN || -sourceTop >= height) {
      return Luna_Result_Success;
    }

    height += sourceTop;
    destinationTop -= sourceTop;
    sourceTop = 0;
  }

  if (sourceLeft >= source->width ||
    sourceTop >= source->height) {
    return Luna_Result_Success;
  }

  if (source->width - sourceLeft < width) {
    width = source->width - sourceLeft;
  }

  if (source->height - sourceTop < height) {
    height = source->height - sourceTop;
  }

  /*
   * Clip the destination rectangle against the destination surface.
   */
  if (destinationLeft < 0) {
    if (destinationLeft == INT32_MIN ||
      -destinationLeft >= width) {
      return Luna_Result_Success;
    }

    sourceLeft -= destinationLeft;
    width += destinationLeft;
    destinationLeft = 0;
  }

  if (destinationTop < 0) {
    if (destinationTop == INT32_MIN ||
      -destinationTop >= height) {
      return Luna_Result_Success;
    }

    sourceTop -= destinationTop;
    height += destinationTop;
    destinationTop = 0;
  }

  if (destinationLeft >= self->width ||
    destinationTop >= self->height) {
    return Luna_Result_Success;
  }

  if (self->width - destinationLeft < width) {
    width = self->width - destinationLeft;
  }

  if (self->height - destinationTop < height) {
    height = self->height - destinationTop;
  }

  assert(sourceLeft >= 0);
  assert(sourceTop >= 0);
  assert(destinationLeft >= 0);
  assert(destinationTop >= 0);
  assert(width >= 0);
  assert(height >= 0);

  /*
   * A copy between identical surfaces can use memmove(), preserving
   * the existing fast path and its overlap semantics.
   */
  if (blendMode == Luna_BlendMode_Copy &&
    self == source) {
    size_t const stride =
      (size_t)self->width * sizeof(uint32_t);
    size_t const rowSize =
      (size_t)width * sizeof(uint32_t);

    uint8_t const* sourcePixels =
      (uint8_t const*)source->pixels;
    uint8_t* destinationPixels =
      (uint8_t*)self->pixels;

    for (int32_t y = 0; y < height; ++y) {
      uint8_t const* sourceRow =
        sourcePixels +
        ((size_t)sourceTop + (size_t)y) * stride +
        (size_t)sourceLeft * sizeof(uint32_t);

      uint8_t* destinationRow =
        destinationPixels +
        ((size_t)destinationTop + (size_t)y) * stride +
        (size_t)destinationLeft * sizeof(uint32_t);

      memmove(destinationRow, sourceRow, rowSize);
    }

    return Luna_Result_Success;
  }

  /*
   * Determine the traversal direction for an overlapping copy from
   * the same surface. This is the pixel-wise equivalent of memmove().
   */
  int32_t yStart = 0;
  int32_t yEnd = height;
  int32_t yStep = 1;

  int32_t xStart = 0;
  int32_t xEnd = width;
  int32_t xStep = 1;

  if (self == source) {
    if (destinationTop > sourceTop) {
      yStart = height - 1;
      yEnd = -1;
      yStep = -1;
    } else if (destinationTop == sourceTop &&
      destinationLeft > sourceLeft) {
      xStart = width - 1;
      xEnd = -1;
      xStep = -1;
    }
  }

  uint32_t const* sourcePixels =
    (uint32_t const*)source->pixels;
  uint32_t* destinationPixels =
    (uint32_t*)self->pixels;

  for (int32_t y = yStart; y != yEnd; y += yStep) {
    uint32_t const* sourceRow =
      sourcePixels +
      ((size_t)sourceTop + (size_t)y) * (size_t)source->width +
      (size_t)sourceLeft;

    uint32_t* destinationRow =
      destinationPixels +
      ((size_t)destinationTop + (size_t)y) * (size_t)self->width +
      (size_t)destinationLeft;

    for (int32_t x = xStart; x != xEnd; x += xStep) {
      Luna_Color sourceColor;
      sourceColor.pixelFormat = source->pixelFormat;
      sourceColor.value = sourceRow[x];

      Luna_Color destinationColor;
      destinationColor.pixelFormat = self->pixelFormat;
      destinationColor.value = destinationRow[x];

      Luna_Color outputColor;
      outputColor.pixelFormat = self->pixelFormat;

      Luna_Result result;

      switch (blendMode) {
        case Luna_BlendMode_Copy: {
          result =
            Luna_BlendModeCopy_do(
              &sourceColor,
              &destinationColor,
              &outputColor
            );
        } break;

        case Luna_BlendMode_SourceOver: {
          result =
            Luna_BlendModeSourceOver_do(
              &sourceColor,
              &destinationColor,
              &outputColor
            );
        } break;

        default: {
          return Luna_Result_ArgumentValueInvalid;
        } break;
      }

      if (result != Luna_Result_Success) {
        return result;
      }

      destinationRow[x] = outputColor.value;
    }
  }

  return Luna_Result_Success;
}