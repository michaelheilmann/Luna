# Luna — Guide for AI Systems

This document provides architectural and development guidance for artificial intelligence systems and automated tools working with the Luna repository.

It is intentionally different from the public `README.md`. The `README.md` explains how to use Luna. This document explains how to **understand, modify, and extend Luna without violating its design**.

When modifying Luna, treat the conventions and invariants in this document as part of the library's design.

---

## 1. What Luna Is

Luna is a drop-in C library for game and application development.

Luna is written in C but follows an intentionally object-oriented design style. Concepts normally expressed through classes and methods in object-oriented languages are represented through C structures and functions.

For example:

```c
Luna_Result Luna_File_create(
    Luna_File** self,
    char const* path
);

Luna_Result Luna_File_read(
    Luna_File* self,
    void* bytes,
    size_t capacity,
    size_t* outNumberOfBytesRead
);

Luna_Result Luna_File_destroy(
    Luna_File* self
);
```

The API should therefore be understood in terms of:

- objects,
- object lifetime,
- initialization and uninitialization,
- construction and destruction,
- methods,
- ownership,
- explicit error handling.

Do not impose conventions from another C library merely because they are common there. Prefer the conventions already established by Luna.

---

# 2. Before Modifying the Repository

Before making a change:

1. Read the relevant public header.
2. Read the corresponding implementation.
3. Search the repository for existing uses of the affected API or type.
4. Look for an existing implementation of a similar concept.
5. Inspect the tests for the affected functionality.
6. Preserve existing naming, lifetime, ownership, and error-handling conventions.
7. Prefer extending an existing abstraction over introducing a parallel abstraction.
8. Keep public API changes deliberate and consistent.

Do not solve a local problem by introducing a new convention when an existing Luna convention can express the same thing.

When uncertain, examine several existing components before deciding how a new component should look.

---

# 3. Return Values and Errors

Luna functions generally return a `Luna_Result`.

The return value describes whether the operation succeeded or failed.

It is **not normally the primary output value of the operation**.

For example:

```c
Luna_Result Luna_File_create(
    Luna_File** self,
    char const* path
);
```

The `Luna_Result` answers:

> Did the operation succeed?

The `self` parameter answers:

> What object was created?

This distinction is fundamental to the API.

Do not change a function returning `Luna_Result` into a function returning the created object merely because that appears more convenient.

---

# 4. The `self` Convention

Luna uses `self` to identify the object an operation belongs to.

For ordinary operations:

```c
Luna_Result Luna_File_read(
    Luna_File* self,
    ...
);
```

`self` points to an already existing object.

For creation operations:

```c
Luna_Result Luna_File_create(
    Luna_File** self,
    ...
);
```

`self` is a pointer to the variable that will receive the newly created object.

This is intentional.

A creation function can therefore be viewed as the C equivalent of a constructor or `new` operation:

```text
create:
    no object
        |
        v
    initialized object
```

After creation, the same object is passed as `self` to its other operations.

For example:

```c
Luna_File* file = NULL;

Luna_File_create(&file, path);
Luna_File_read(file, ...);
Luna_File_destroy(file);
```

The use of `self` is preferred when the parameter represents the object being created or operated upon.

Do not replace `self` with `outFile`, `object`, `instance`, or similar names merely because the parameter is technically an output parameter.

Auxiliary output values should use an `out`-style name where appropriate:

```c
size_t* outNumberOfBytesRead
```

Thus:

```text
self       The object being operated upon or created.
outX       An auxiliary output value.
inOutX     A value serving as both input and output.
```

---

# 5. Object Lifetime

Luna distinguishes between:

- storage,
- initialization,
- uninitialization,
- creation,
- destruction.

Where an object supports these operations, the conceptual relationship is:

```text
T_initialize
    |
    v
initialized T
    |
    v
T_uninitialize
```

and:

```text
T_create
    |
    v
allocated + initialized T
    |
    v
T_destroy
    |
    v
released storage
```

`initialize` and `uninitialize` operate on object storage supplied by the caller.

`create` and `destroy` normally deal with the complete lifetime of a dynamically allocated object.

Do not assume that initialization allocates memory or that uninitialization releases the object's storage.

The implementation and public API determine the exact ownership semantics for each type.

---

# 6. Ownership

Ownership must always be considered explicitly.

A function returning success after creating an object normally transfers ownership of that object to the caller.

For example:

```c
Luna_File* file = NULL;

Luna_Result result = Luna_File_create(&file, path);

if (result == Luna_Result_Success) {
    /* The caller owns file. */
}
```

The caller is then responsible for the corresponding destruction operation.

Functions receiving:

```c
T* self
```

do not implicitly acquire ownership merely because they receive the pointer.

Do not introduce hidden ownership transfers.

Do not add reference counting, copying, or automatic lifetime management unless the abstraction explicitly requires it.

When modifying an existing type, inspect its implementation and documentation before making assumptions about ownership.

---

# 7. Failure Semantics

Every fallible operation must have clearly defined failure behavior.

When adding or modifying an API, consider:

- What happens when an argument is `NULL`?
- What happens when allocation fails?
- What happens when an operating-system operation fails?
- What state does the object have after failure?
- What happens to output parameters?
- Can the operation be safely retried?
- Who owns resources acquired before the failure?

Do not silently ignore errors.

Do not convert an error into success merely because an operation can continue.

Do not leave partially initialized objects in a state in which public functions can mistake them for valid objects.

When an operation consists of multiple resource-acquisition steps, failure must clean up resources acquired by earlier steps.

---

# 8. Resource Management

Luna frequently wraps operating-system or externally managed resources.

Examples include:

- files,
- windows,
- graphics resources,
- fonts,
- surfaces,
- memory allocations,
- synchronization primitives.

A Luna object should provide a clear abstraction boundary around such resources.

Resource acquisition and release should be paired.

For example:

```text
initialize  <-> uninitialize
create      <-> destroy
```

When implementing initialization involving several resources, structure the implementation so that every failure path releases resources already acquired.

Prefer small initialization helpers when they make ownership and cleanup clearer.

Avoid large functions that simultaneously perform:

- argument validation,
- allocation,
- object initialization,
- operating-system resource acquisition,
- configuration,
- cleanup.

---

# 9. API Naming

Luna names public symbols using the `Luna_` prefix.

Types and enumerations use the established Luna naming convention.

Functions associated with an object should use the object's type as part of their name.

Conceptually:

```c
Luna_File_create(...)
Luna_File_initialize(...)
Luna_File_read(...)
Luna_File_write(...)
Luna_File_uninitialize(...)
Luna_File_destroy(...)
```

Do not introduce unrelated naming schemes for new components.

Names should describe the abstraction rather than the implementation detail.

For example, a public API should generally not expose an implementation-specific name when the abstraction is actually a `Luna_Font`.

---

# 10. Public API vs. Implementation

Keep the public API small.

Implementation details should remain private unless users genuinely need to depend on them.

When introducing an internal helper, prefer a `static` function over exposing another public function.

Before adding a public symbol, ask:

> Does a user of Luna need to know that this exists?

If the answer is no, it probably belongs in the implementation rather than the public API.

Do not expose operating-system implementation details unnecessarily.

For example, a Windows implementation may use:

```text
HDC
HFONT
HBITMAP
HWND
```

internally without making these handles part of Luna's portable abstraction.

---

# 11. Platform Abstraction

Luna may use platform-specific implementations internally while exposing a platform-independent public API where practical.

Platform-specific code should remain behind the appropriate abstraction boundary.

Do not spread platform-specific types through otherwise portable components unless the public API explicitly intends to be platform-specific.

When adding platform support:

1. Identify the platform-independent abstraction.
2. Identify the platform-specific implementation.
3. Keep platform-specific code localized.
4. Preserve the same observable semantics across platforms where possible.

Platform differences that cannot be hidden should be documented explicitly.

---

# 12. Memory

Memory allocation is part of Luna's error model.

Allocation failure must be treated as a normal possible failure condition.

Do not assume that allocation succeeds.

When calculating allocation sizes, take integer overflow into account.

For example, code equivalent to:

```c
size_t size = count * elementSize;
```

must not be written without considering whether the multiplication can overflow.

Prefer checked arithmetic where the operands are not already known to be safe.

When reallocating or growing a data structure, do not overwrite the only valid pointer before a potentially failing allocation operation has succeeded.

---

# 13. Integer and Size Types

Use types according to their semantic meaning.

Examples:

```text
size_t       sizes, capacities, byte counts, array indices
uint8_t      explicitly 8-bit unsigned data
uint32_t     explicitly 32-bit unsigned data
int32_t      explicitly 32-bit signed data
```

Do not use a type merely because it happens to be large enough on the current platform.

When serializing binary data, explicitly define byte order rather than relying on host endianness.

---

# 14. Cleanup and Failure Paths

Luna code should make cleanup obvious.

A useful structure for complex initialization is:

```text
validate arguments
        |
        v
initialize object state
        |
        v
acquire resource A
        |
        v
acquire resource B
        |
        v
acquire resource C
        |
        v
success
```

If resource C fails:

```text
release C if necessary
release B
release A
uninitialize object
return error
```

Avoid duplicated cleanup logic when a small helper or structured cleanup path makes the ownership relationships clearer.

At the same time, do not introduce unnecessarily complicated abstractions merely to eliminate a few lines of cleanup.

Clarity is more important than cleverness.

---

# 15. Performance

Luna is intended for applications such as games, where some operations may occur in performance-critical loops.

Do not optimize blindly.

First determine whether an operation is expected to be:

- initialization-time,
- frame-time,
- per-object,
- per-draw,
- per-pixel,
- per-event,
- or otherwise performance-sensitive.

Avoid unnecessary allocations and system calls in hot paths.

Prefer predictable data access and simple control flow where performance matters.

However, do not sacrifice correctness or API clarity for speculative micro-optimizations.

When optimizing, preserve the semantics of the existing API.

---

# 16. Graphics and Rendering

Graphics abstractions should distinguish between:

- logical objects,
- CPU-side data,
- GPU/platform resources,
- transient rendering state.

Do not expose implementation details of a graphics backend through a generic Luna abstraction unless required.

For software rendering, be explicit about:

- pixel format,
- row layout,
- dimensions,
- clipping,
- bounds,
- ownership of pixel memory.

For font rendering, distinguish between:

- font resources,
- glyph metrics,
- glyph bitmaps,
- atlases,
- rendering surfaces.

Do not conflate a font with its rendered glyph data.

---

# 17. Data Structures

Data structures should define their invariants explicitly.

For dynamically growing structures, consider:

- current size,
- capacity,
- maximum representable capacity,
- allocation overflow,
- growth failure,
- empty-state behavior,
- removal semantics,
- ownership of contained objects.

Do not assume that a container can grow indefinitely.

Capacity calculations must be checked for integer overflow.

Operations should behave consistently at boundary conditions such as:

```text
empty container
single element
full capacity
maximum capacity
allocation failure
```

---

# 18. Thread Safety

Do not assume that a Luna object is thread-safe unless its API or implementation explicitly guarantees it.

Adding synchronization to an object is not automatically an improvement.

When thread safety is required, define:

- which operations may execute concurrently,
- which state is protected,
- who owns the synchronization primitive,
- whether callbacks occur while locks are held,
- whether destruction may race with other operations.

Avoid introducing hidden global locks.

---

# 19. Assertions

Assertions may be used to document and detect programmer errors and internal invariants.

Do not use assertions as a substitute for handling expected runtime failures.

For example:

```text
Invalid caller argument that can legitimately occur
    -> return Luna_Result_ArgumentValueInvalid

Internal invariant that should be impossible to violate
    -> assertion may be appropriate
```

An out-of-memory condition is not an assertion failure merely because the programmer would prefer allocation to succeed.

---

# 20. Error Handling Style

Do not write code that assumes only one failure mode exists when the API can return several.

Prefer:

```c
Luna_Result result = Luna_File_create(&file, path);
if (result != Luna_Result_Success) {
    return result;
}
```

when the exact error is not relevant.

When a particular error requires special handling:

```c
if (result == Luna_Result_OutOfMemory) {
    ...
} else if (result != Luna_Result_Success) {
    ...
}
```

Do not silently discard a `Luna_Result` from an operation that can fail unless ignoring the result is explicitly valid.

---

# 21. Tests

Tests are part of the specification.

When modifying behavior:

1. Find existing tests for the affected component.
2. Preserve existing behavior unless the change intentionally modifies it.
3. Add tests for newly introduced behavior.
4. Test failure cases where practical.
5. Test boundary conditions.

Particularly important boundary conditions include:

- `NULL` arguments,
- zero sizes,
- empty collections,
- one-element collections,
- maximum values,
- allocation failure,
- invalid state,
- repeated initialization/destruction,
- platform-specific failures.

Do not modify tests merely to make an implementation pass unless the expected behavior itself has changed.

---

# 22. Avoid Speculative API Design

Do not add APIs "just in case".

A new public function increases the long-term compatibility burden of the library.

Prefer:

```text
small API
+
strong semantics
+
composable operations
```

over:

```text
large API
+
many convenience functions
+
ambiguous semantics
```

If two operations can be expressed cleanly using an existing primitive, prefer the existing primitive.

---

# 23. Avoid Unnecessary Abstractions

Do not introduce an abstraction simply because another language or framework uses one.

For example, do not introduce:

- virtual dispatch,
- interfaces,
- generic object systems,
- reference counting,
- elaborate allocator hierarchies,
- callback frameworks,

unless there is a concrete requirement for them.

Luna uses C deliberately.

Use C's strengths:

- explicit ownership,
- explicit lifetime,
- predictable memory layout,
- simple function calls,
- static typing,
- straightforward compilation.

---

# 24. Match Existing Design Before Inventing New Design

When adding a new type, first find the closest existing type.

For example, when adding a new resource type, inspect existing resource types for:

```text
type naming
create naming
destroy naming
initialize naming
uninitialize naming
self parameter
error handling
ownership
allocation
cleanup
```

The new type should feel as though it was designed at the same time as the existing types.

Consistency is more valuable than personal preference.

---

# 25. Do Not Infer Semantics From Names Alone

Names are important, but they are not sufficient documentation.

Before changing code, inspect:

- declarations,
- definitions,
- callers,
- tests,
- comments,
- related abstractions.

For example, do not assume that:

```c
Luna_X_destroy(...)
```

necessarily means `free()`.

It may release an operating-system resource, uninitialize state, or perform both operations.

Determine the actual ownership model from the implementation and documentation.

---

# 26. Repository Structure

The repository currently contains the main Luna library under:

```text
Luna/
```

and tests under:

```text
Tests/
```

The build system is provided through:

```text
CMakeLists.txt
```

The top-level `README.md` describes Luna's use as a drop-in library and provides build/integration examples.

When looking for an implementation, start with the corresponding file in `Luna/`.
Ignore the contents of `Tests/` for now.

When looking for expected behavior, inspect:
1. the public header,
2. the associated Markdown documentation,
3. the implementation,
4. the tests.

For a component named `Foo`, first look for
```
Luna/Foo.h
Luna/Foo.c
Luna/Foo.md
```

---

# 27. Making Changes

When implementing a change, prefer this sequence:

```text
Understand
    |
    v
Find existing pattern
    |
    v
Define desired semantics
    |
    v
Change public API only if necessary
    |
    v
Implement
    |
    v
Handle all failure paths
    |
    v
Test
    |
    v
Review consistency
```

Before considering a change complete, ask:

- Does the API follow Luna's naming conventions?
- Is ownership obvious?
- Is the lifetime obvious?
- Are failures represented by `Luna_Result`?
- Are output parameters named consistently?
- Is `self` used for the object?
- Are allocations checked?
- Are integer overflows impossible or checked?
- Are resources cleaned up on every failure path?
- Does the implementation match existing Luna patterns?
- Did the change introduce unnecessary public API?

---

# 28. Most Important Rule

When modifying Luna, **do not merely make the code work**.

Make the code work **as Luna**.

A correct Luna implementation is one that:

1. behaves correctly,
2. handles failure correctly,
3. has clear ownership and lifetime semantics,
4. follows the existing API conventions,
5. preserves the abstraction boundaries,
6. remains understandable to a C programmer,
7. does not introduce unnecessary complexity.

When an implementation is technically correct but conflicts with the established design, prefer the established design.

If the established design itself appears inconsistent, identify the inconsistency first and consider whether the better solution is to improve the convention consistently rather than introducing another exception.

---

# 29. Guidance for AI-Generated Changes

AI systems should not assume that the first plausible implementation is the correct Luna implementation.

Before producing code:

- inspect related Luna code,
- identify established patterns,
- verify assumptions against callers and tests,
- preserve existing semantics,
- explicitly consider ownership and failure paths.

AI-generated code should be reviewed as if it were written by a new contributor who does not yet know the library.

In particular, AI systems frequently make these mistakes:

- inventing APIs that do not match the existing design,
- returning objects directly instead of using `Luna_Result`,
- confusing output parameters with `self`,
- forgetting cleanup on partial initialization,
- assuming allocation cannot fail,
- introducing unnecessary abstractions,
- leaking platform-specific details into portable APIs,
- changing semantics merely to make implementation easier,
- duplicating functionality that already exists elsewhere.

Avoid these patterns.

The repository itself is the ultimate source of truth when this document and an implementation disagree. If the implementation appears to contradict this document, investigate whether the documentation or implementation is outdated before making changes.

---

# 30. Summary

The mental model for Luna is:

```text
                    Luna
                      |
          +-----------+-----------+
          |                       |
      Objects                 Results
          |                       |
     +----+----+              success/failure
     |         |
 lifetime    methods
     |
 create/destroy
 initialize/uninitialize
```

The central API principle is:

```c
Luna_Result operation(...);
```

means:

> The function's return value communicates the result of the operation.

And:

```c
T* self
```

means:

> This is the object the operation operates on.

While:

```c
T** self
```

in a creation function means:

> Create the object and establish `self` as the resulting object.

Additional output values are named explicitly:

```c
T* self,
size_t* outSize
```

This distinction should be preserved throughout the library.

**When in doubt: follow the existing Luna pattern before inventing a new one.**