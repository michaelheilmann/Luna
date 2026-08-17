# `Luna_Color`

A `Luna_Color` is a color value.


# `Luna_Color_setRGB`

Assign a `Luna_Color` value from the specified red, green, and blue component values.

```c
Luna_Result
Luna_Color_setRGB
  (
    Luna_Color* self,
    uint8_t red,
    uint8_t green,
    uint8_t blue
  );
```

## Parameters

- `self` A pointer to the color value.
- `red` The red component value.
- `green` The green component value.
- `blue` The blue component value.

## Return value

A `Luna_Result` value indicating success or failure.

## Postcondition (Success)

The color value was assigned from the specified component values.

## Postcondition (Failure)

The environment was not observably modified.


# `Luna_Color_setRGBA`

Assign a `Luna_Color` value from the specified red, green, blue, and alpha component values.

```c
Luna_Result
Luna_Color_setRGBA
  (
    Luna_Color* self,
    uint8_t red,
    uint8_t green,
    uint8_t blue,
    uint8_t alpha
  );
```

## Parameters

- `self` A pointer to the color value.
- `red` The red component value.
- `green` The green component value.
- `blue` The blue component value.
- `alpha` The alpha component value.

## Return value

A `Luna_Result` value indicating success or failure.

## Postcondition (Success)

The color value was assigned from the specified component values.

## Postcondition (Failure)

The environment was not observably modified.


# `Luna_Surface`

A `Luna_Surface` represents a rectangular array of pixels.

The pixels are stored consecutively in memory, one row after another, from top to bottom. Within each row, pixels are stored from left to right.

The `pixelFormat` member specifies the representation of the pixels.


# `Luna_Surface_create`

Create a surface of the specified width and height.

```c
Luna_Result
Luna_Surface_create
  (
    Luna_Surface** self,
    uint8_t flags,
    int32_t width,
    int32_t height
  );
```

## Parameters

- `self` A pointer to a `Luna_Surface*` variable.
- `flags` Must specify a supported pixel format.
- `width` The width of the surface.
- `height` The height of the surface.

## Return value

A `Luna_Result` value indicating success or failure.

## Postcondition (Success)

`*self` was assigned a pointer to the surface.

The caller acquired ownership of the surface. The caller shall relinquish ownership of the surface by destroying it when appropriate.

The surface is filled with opaque black.

## Postcondition (Failure)

The environment was not observably modified.


# `Luna_Surface_destroy`

Destroy a surface.

```c
Luna_Result
Luna_Surface_destroy
  (
    Luna_Surface* self
  );
```

## Parameters

- `self` A pointer to the surface.

## Return value

A `Luna_Result` value indicating success or failure.

## Postcondition (Success)

The surface pointed to by `self` was destroyed.

## Postcondition (Failure)

The environment was not observably modified.


# `Luna_Surface_fillRectangle`

Fill a rectangular region of a surface with a color.

The rectangle is clipped to the bounds of the surface. A rectangle that lies completely outside the surface has no effect.

```c
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
```

## Parameters

- `self` A pointer to the surface.
- `color` A pointer to the color with which the rectangle is filled.
- `blendMode` The blend mode.
- `left` The horizontal coordinate of the left edge of the rectangle.
- `top` The vertical coordinate of the top edge of the rectangle.
- `width` The width of the rectangle.
- `height` The height of the rectangle.

The rectangle is specified in surface coordinates. Its left and top edges are inclusive. The rectangle has the specified `width` and `height`.

`width` and `height` must not be negative. If either is zero, the operation has no effect.

If the rectangle extends beyond the surface, the part outside the surface is ignored. Negative `left` and `top` coordinates are therefore permitted. A rectangle that does not intersect the surface has no effect.

## Return value

A `Luna_Result` value indicating success or failure.

## Postcondition (Success)

Every pixel within the intersection of the specified rectangle and the surface was assigned the specified color using the specified blend mode.

If the specified rectangle does not intersect the surface, the surface was not modified.

## Postcondition (Failure)

The environment was not observably modified.


# `Luna_Surface_fill`

Fill the entire surface with a color.

```c
Luna_Result
Luna_Surface_fill
  (
    Luna_Surface* self,
    Luna_Color const* color,
    Luna_BlendMode blendMode
  );
```

## Parameters

- `self` A pointer to the surface.
- `color` A pointer to the color with which the surface is filled.
- `blendMode` The blend mode.

## Return value

A `Luna_Result` value indicating success or failure.

Returns `Luna_Result_ArgumentValueInvalid` if `self` or `color` is `NULL`, or if `blendMode` is not supported.

## Postcondition (Success)

Every pixel of the surface was assigned the specified color using the specified blend mode.

## Postcondition (Failure)

The environment was not observably modified.


# `Luna_Surface_copyRectangle`

Composite a rectangular region of a source surface onto a rectangular region of a destination surface.

The source rectangle is specified by `sourceLeft`, `sourceTop`, `width`, and `height`.

The destination rectangle is specified by `destinationLeft`, `destinationTop`, `width`, and `height`.

The source and destination surfaces may use different pixel formats. Pixel format conversion is performed as part of the operation.

The `blendMode` parameter specifies how each source pixel is combined with the corresponding destination pixel.

The source and destination surfaces may be the same surface. Overlapping source and destination rectangles are supported.

If either rectangle extends beyond its respective surface, the rectangles are clipped so that corresponding source and destination pixels continue to correspond.

If clipping results in an empty rectangle, the function succeeds without modifying the destination surface.

```c
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
  );
```

## Parameters

- `self` A pointer to the destination surface.
- `destinationLeft` The horizontal coordinate of the upper-left corner of the destination rectangle.
- `destinationTop` The vertical coordinate of the upper-left corner of the destination rectangle.
- `source` A pointer to the source surface.
- `sourceLeft` The horizontal coordinate of the upper-left corner of the source rectangle.
- `sourceTop` The vertical coordinate of the upper-left corner of the source rectangle.
- `width` The width of both rectangles.
- `height` The height of both rectangles.
- `blendMode` The blend mode used to combine source and destination pixels.

The source and destination rectangles have the same dimensions.

`width` and `height` must not be negative. If either is zero, the operation has no effect.

Negative source and destination coordinates are permitted. Portions of either rectangle that lie outside their respective surfaces are ignored.

## Return value

A `Luna_Result` value indicating success or failure.

Returns `Luna_Result_ArgumentValueInvalid` if `self` or `source` is `NULL`, if `width` or `height` is negative, or if `blendMode` is not supported.

## Postcondition (Success)

Every pixel within the resulting source rectangle was combined with the corresponding pixel within the resulting destination rectangle using the specified blend mode.

If clipping resulted in an empty rectangle, the destination surface was not modified.

If `source` and `self` refer to the same surface and the source and destination rectangles overlap, the result is as if the original source pixels had been preserved before any destination pixels were modified.

## Postcondition (Failure)

The environment was not observably modified.
