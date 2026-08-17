# `Luna_Clock_initialize`
Initialize a clock.


```
Luna_Result Luna_Clock_initialize(Luna_Clock* clock);

```

## Parameters
- `clock` A pointer to the clock.

## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
The clock was initialized. The clock is in "paused" state.

## Postcondition (Failure)
The environment was not observably modified.

## Remarks
#### Clocks and Delta Time

Many applications need to keep track of the passage of time. For example, an application may use time to update animations, move objects, perform periodic work, or calculate how much time has passed between two iterations of its main loop.
A **clock** provides such a mechanism. Conceptually, it observes a source of monotonically increasing time and allows an application to determine how much time has elapsed since the clock was last updated.
The time elapsed between two updates is commonly called **delta time**. If the previous update occurred at time `t_0` and the current update occurs at time `t_1`, the delta time is:

````
Delta t = t_1 - t_0
````

An application can use delta time to make operations independent of the frequency at which its main loop executes. For example, instead of moving an object by a fixed amount once per iteration, the object can be moved according to the elapsed time:

```text
position += velocity × deltaTime
```

This allows the object to move at approximately the same speed even if different iterations of the application take different amounts of time.

### Updating a Clock

A clock stores the time at which it was last updated. When the clock is updated, it obtains the current time from its underlying time source and calculates the difference between the current time and the previously stored time. 
The current time then becomes the reference point for the next update.
Consequently, obtaining the delta time is not merely a passive read operation: calculating it advances the clock's internal state.

#### Pausing a Clock
A clock may also be **paused**. While paused, the clock does not advance from the application's perspective, and updating it produces a delta time of zero.
When the clock is resumed, its reference time must be reset to the current time. Otherwise, the next delta time would include the entire duration for which the clock was paused.

For example:

```text
update        pause                  resume       update
  |-------------|======================|-------------|
                time spent paused
```

The time between `pause` and `resume` should not contribute to the delta time. Resetting the reference time when the clock is resumed ensures that the next update measures only the time that has passed since resumption.
A paused clock therefore represents a **logical notion of time** that may differ from real time. Real time continues to pass while the clock is paused, but the paused clock deliberately ignores that interval.
This makes clocks useful for application state such as pausing a game or animation: the operating system's clock continues normally, while the application's logical clock temporarily stops advancing.




# `Luna_Clock_uninitialize`
Uninitialize a clock.

```
Luna_Result Luna_Clock_uninitialize(Luna_Clock* clock);

```

## Parameters
- `clock` A pointer to the clock.

## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
The clock was uninitialized.

## Postcondition (Failure
The environment was not observably modified.




# `Luna_Clock_setPaused`
Pause a clock.

```
Luna_Result Luna_Clock_setPaused(Luna_Clock* clock, bool paused);

```

## Parameters
- `clock` A pointer to the clock.
- `paused` `true` denotes the desired state is "paused" and `false` denotes the desired state is "running".

## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
If `false` was passed and the clock was in "paused" state, the clock is now in "active" state and was updated.
If `true` was passed and the clock was in "active" state, the clock is now in "paused" state.

## Postcondition (Failure
The environment was not observably modified.




# `Luna_Clock_getTick`
Get the elapsed time`.

```
Luna_Result Luna_Clock_getTick(Luna_Clock* clock, double* tick);

```

## Parameters
- `clock` A pointer to the clock.
- `tick A pointer to a `double` variable.

## Return value
A `Luna_Result` value indicating success or failure.

## Postcondition (Success)
If the clock was in "running" state then `*tick` was assigned the elapsed time since the last "update".
If the clock was in "stopped" state, then `*tick` was assigned `0`.
After that, the clock was "updated".

## Remarks
An "update" of the clock occurs when the clock transitions from "paused" to "running" state in a call

 since the last call to `Luna_Clock_getTick` or since the clock transitioned from "paused" to "running" state, whatever happened last.
Otherwise it was assigned `0`.

## Postcondition (Failure
The environment was not observably modified.
