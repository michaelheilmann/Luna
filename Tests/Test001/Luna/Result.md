# `Luna_Result`
Type of a value indicating success or failure of an operation. The underlaying type is an unsigned integer. 
The permissible values of this type are specified by symbolic constants of which the names start with `Luna_Result_`.
```
typedef int Luna_Result;
```

## `Luna_Result_Success`
Symbolic constant for a `Luna_Result` value indicating success.

This is the only `Luna_Result_` symbolic constant evaluating to the zero value of the underlaying type, all other `Luna_Result_` symbolic constants evaluate to non-zero value of that type.

```
#define Luna_Result_Success (0)
```

## `Luna_Result_ArgumentValueInvalid`
Symbolic constant for a `Luna_Result` value indicating failure. Reason for that failure was that an invalid argument value was passed.

```
#define Luna_Result_ArgumentValueInvalid (1)
````

## `Luna_Result_AllocationFailed`
Symbolic constant for a `Luna_Result` value indicating failure. Reason for that failure was that an allocation failed.

```
#define Luna_Result_AllocationFailed (2)
```

## `Luna_Result_EnvironmentFailed`
Symbolic constant for a `Luna_Result` value indicating failure. Reason for that failure was that the environmente failed.

```
#define Luna_Result_EnvironmentFailed (3)
```

## `Luna_Result_IsEmpty`
Symbolic constant for a `Luna_Result` value indicating failure. Reason for that failure was that something (e.g., a deque) was empty.

```
#define Luna_Result_IsEmpty (4)
```

## `Luna_Result_IsFull`
Symbolic constant for a `Luna_Result` value indicating failure. Reason for that failure was that something (e.g., a deque) was full.

```
#define Luna_Result_IsFull (5)
```

## `Luna_Result_NotFound`
Symbolic constant for a `Luna_Result` value indicating failure. Reason for that failure was that something (e.g., something that was searched for) was not found.

```
#define Luna_Result_NotFound (6)
```
