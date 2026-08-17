# `Luna_openCanvas`
Open the canvas if it is not opened already.

```
Luna_Result Luna_openCanvas();
```

## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
The canvas was

## Postcondition (Failure)
The environment was not observably modified.

# `Luna_closeCanvas`
Close the canvas if it is not closed already.

```
Luna_Result Luna_closeCanvas();
```

## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
The canvas was updated.

## Postcondition (Failure)
The environment was not observably modified.




# `Luna_updateCanvas`
Update the canvas.

```
Luna_Result Luna_updateCanvas();
```

## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
The canvas was updated.

## Postcondition (Failure)
The environment was not observably modified.





# `Luna_getIsCanvasclosed`
Get if the canvas is closed.

```
Luna_Result Luna_getIsCanvasClosed(bool* isCloaed);
```

## Parameters
- `isClosed` A pointer to a `bool` variable.

## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
`*isClosed` was assigned `true` if the canvas was closed and was assigned `false` otherwise.

## Postcondition (Failure)
The environment was not observably modified.




# `Luna_getCanvasSize`
Get the size, in pixels, of the canvas.
This does not include decorations.

```
Luna_Result Luna_getCanvasSize(int32_t* width, int32_t* height);
```

## Parameters
- `width` A pointer to a `int32_t` variable.
- `height` A pointer to a `int32_t` variable.

## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
`*width` was assigned the width, in pixels, of the canvas.
`*height` was assigned the height, in pixels, of the canvas.

## Postcondition (Failure)
The environment was not observably modified.





# Events

## `Luna_EventKind_CanvasOpened`
If the `kind` field of a `Luna_Event` object is set to this value,
the event denotes that the canvas was opened.

## `Luna_createCanvasClosedEvent`
If the `kind` field of a `Luna_Event` object is set to this value,
the event denotes that the canvas was closed.

## `Luna_EventKind_CanvasSizeChanged`
If the `kind` field of a `Luna_Event` object is set to this value,
the event denotes that the size of the canvas has changed.
