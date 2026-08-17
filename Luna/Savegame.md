


# `Luna_writeU8LE`
This function writes an uint8_t to a target. The Bytes are sent to the target in little endian order.
```
Luna_writeU8LE(void* context, Luna_Result(*callback)(void*, size_t, const void*), uint8_t value);
```

## Parameters
- `context` An opaque context. Passed as the 1st argument to the callback.
- `callback` A pointer to a callback writing an array of Bytes.
   The callback receives as its 1st argument the value of `context`. The 2nd argument is the number of Bytes to write, the 3rd argument is a pointer to an array of that number of Bytes.
   The callbackk returns `Luna_Result_Success` if all Bytes were written successfully. Otherwise it returns another `Luna_Result value`.
- `value` The value to be written.
## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
All Bytes were written.

## Postcondition (Failure)
The target may have received certain values.




# `Luna_readU8LE`
This function writes an uint8_t to a target. The Bytes are sent to the target in little endian order.
```
Luna_Savegame_writeU8LE(void* context, Luna_Result(*callback)(void*, size_t, void*), uint8_t* value);
```

## Parameters
- `context` An opaque context. Passed as the 1st argument to the callback.
- `callback` A pointer to a callback writing an array of Bytes.
   The callback receives as its 1st argument the value of `context`. The 2nd argument is the number of Bytes to read, the 3rd argument is a pointer to an array of that number of Bytes or greater.
   The callbackk returns `Luna_Result_Success` if all Bytes were read successfully. Otherwise it returns another `Luna_Result value`.
- `value` The value to be written.
## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
All Bytes were written.

## Postcondition (Failure)
The target may have received certain values.




# `Luna_writeU16LE`
This function writes an uint16_t to a target. The Bytes are sent to the target in little endian order.
```
Luna_writeU16LE(void* context, Luna_Result(*callback)(void*, size_t, const void*), uint16_t value);
```

## Parameters
- `context` An opaque context. Passed as the 1st argument to the callback.
- `callback` A pointer to a callback writing an array of Bytes.
   The callback receives as its 1st argument the value of `context`. The 2nd argument is the number of Bytes to write, the 3rd argument is a pointer to an array of that number of Bytes.
   The callbackk returns `Luna_Result_Success` if all Bytes were written successfully. Otherwise it returns another `Luna_Result value`.
- `value` The value to be written.
## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
All Bytes were written.

## Postcondition (Failure)
The target may have received certain values.




# `Luna_readU16LE`
This function writes an uint8_t to a target. The Bytes are sent to the target in little endian order.
```
Luna_Savegame_writeU8LE(void* context, Luna_Result(*callback)(void*, size_t, void*), uint16_t* value);
```

## Parameters
- `context` An opaque context. Passed as the 1st argument to the callback.
- `callback` A pointer to a callback writing an array of Bytes.
   The callback receives as its 1st argument the value of `context`. The 2nd argument is the number of Bytes to read, the 3rd argument is a pointer to an array of that number of Bytes or greater.
   The callbackk returns `Luna_Result_Success` if all Bytes were read successfully. Otherwise it returns another `Luna_Result value`.
- `value` The value to be written.
## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
All Bytes were written.

## Postcondition (Failure)
The target may have received certain values.




# `Luna_writeU32LE`
This function writes an uint32_t to a target. The Bytes are sent to the target in little endian order.
```
Luna_writeU16LE(void* context, Luna_Result(*callback)(void*, size_t, const void*), uint32_t value);
```

## Parameters
- `context` An opaque context. Passed as the 1st argument to the callback.
- `callback` A pointer to a callback writing an array of Bytes.
   The callback receives as its 1st argument the value of `context`. The 2nd argument is the number of Bytes to write, the 3rd argument is a pointer to an array of that number of Bytes.
   The callbackk returns `Luna_Result_Success` if all Bytes were written successfully. Otherwise it returns another `Luna_Result value`.
- `value` The value to be written.
## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
All Bytes were written.

## Postcondition (Failure)
The target may have received certain values.




# `Luna_readU32LE`
This function writes an uint8_t to a target. The Bytes are sent to the target in little endian order.
```
Luna_Savegame_writeU32LE(void* context, Luna_Result(*callback)(void*, size_t, void*), uint32_t* value);
```

## Parameters
- `context` An opaque context. Passed as the 1st argument to the callback.
- `callback` A pointer to a callback writing an array of Bytes.
   The callback receives as its 1st argument the value of `context`. The 2nd argument is the number of Bytes to read, the 3rd argument is a pointer to an array of that number of Bytes or greater.
   The callbackk returns `Luna_Result_Success` if all Bytes were read successfully. Otherwise it returns another `Luna_Result value`.
- `value` The value to be written.
## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
All Bytes were written.

## Postcondition (Failure)
The target may have received certain values.




# `Luna_writeU64LE`
This function writes an uint16_t to a target. The Bytes are sent to the target in little endian order.
```
Luna_writeU64LE(void* context, Luna_Result(*callback)(void*, size_t, const void*), uint64_t value);
```

## Parameters
- `context` An opaque context. Passed as the 1st argument to the callback.
- `callback` A pointer to a callback writing an array of Bytes.
   The callback receives as its 1st argument the value of `context`. The 2nd argument is the number of Bytes to write, the 3rd argument is a pointer to an array of that number of Bytes.
   The callbackk returns `Luna_Result_Success` if all Bytes were written successfully. Otherwise it returns another `Luna_Result value`.
- `value` The value to be written.
## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
All Bytes were written.

## Postcondition (Failure)
The target may have received certain values.




# `Luna_readU64LE`
This function writes an uint8_t to a target. The Bytes are sent to the target in little endian order.
```
Luna_Savegame_writeU64LE(void* context, Luna_Result(*callback)(void*, size_t, void*), uint64_t* value);
```

## Parameters
- `context` An opaque context. Passed as the 1st argument to the callback.
- `callback` A pointer to a callback writing an array of Bytes.
   The callback receives as its 1st argument the value of `context`. The 2nd argument is the number of Bytes to read, the 3rd argument is a pointer to an array of that number of Bytes or greater.
   The callbackk returns `Luna_Result_Success` if all Bytes were read successfully. Otherwise it returns another `Luna_Result value`.
- `value` The value to be written.
## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
All Bytes were written.

## Postcondition (Failure)
The target may have received certain values.
