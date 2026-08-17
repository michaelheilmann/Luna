# `Luna_Event`
Opaque type of an event.

```
typedef struct Luna_Event Luna_Event;
```




# `Luna_deleteEvent`
Delete an event.

```
Luna_Result Luna_deleteEvent(Luna_Event* event);
```

## Parameters
- `event` A pointer to the event.

## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
The event pointed to by `event` was deleted.

#### Postcondition (Failure)
The environment was not observably modified.




# `Luna_popEvent`
Pop the next event from the event queue.

```
Luna_Result Luna_popEvent(Luna_Event** event);
```

## Parameters
- `event` A pointer to a `Luna_Event*` variable.

## Return value
A `Luna_Result` value indicating success or failure.
In particular, `Luna_Result_IsEmpty` is returned if the event queue is empty.

## Postcondition (Success)
`*event` was assigned a pointer to the next event.
The caller acquired ownership of the event. The caller shall relinquish ownership or shall delete the event when appropriate.

## Postcondition (Failure
The environment was not observably modified.




# `Luna_pushEvent`
Push an event to the event queue.

```
Luna_Result Luna_pushEvent(Luna_Event* event);
```

## Parameters
- `event` A pointer to the event.

## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
The event was pushed to the event queue.
The caller relinquished ownership of the event.

## Postcondition (Failure
The environment was not observably modified.



# Events
## `Luna_EventKind_ApplicationQuitRequested`
If the `kind` field of a `Luna_Event` object is set to this value,
the event indicates that termination of the application has been requested.
