# `Luna_allocate`
Allocate an array of Bytes.
```
Luna_Result Luna_allocate(void** bytes, size_t numberOfBytes);
```

## Parameters
- **`bytes`** A pointer to a `void` variable.
- **`numberOfBytes`** The number of Bytes to allocate.
## Return value
A `Luna_Result` value indicating success or failure.
## Postcondition (Success)
`*bytes` was assigned a pointer to newly allocated array of `numberOfBytes` Bytes.
## Postcondition (Failure)
The environment was not observably modified.




# `Luna_deallocate`
Deallocate an array of Bytes.
```
Luna_Result Luna_deallocate(void* bytes);
```

## Parameters
- `bytes` A pointer to an array of Bytes previously allocated by `Luna_allocate`.

## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
The array of Bytes pointed to by `bytes` was deallocated.

## Postcondition (Failure)
The environment was not observably modified.




# `Luna_reallocate`
Reallocate an array of Bytes.
```
Luna_Result Luna_reallocate(void** bytes, size_t numberOfBytes);
````

## Parameters
- `bytes` A pointer to a `void*` variable. That variable points to an array of Bytes previously allocated `Luna_allocate`.
- `numberOfBytes` The number of Bytes to reallocate the array pointed to by `*bytes` to.

## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
The array pointed to by `*bytes` was reallocated to `numberOfBytes`.
The address of the reallocated array was assigned to `*bytes`.

## Postcondition (Failure)
The environment was not observably modified.
