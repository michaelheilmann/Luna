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
 
#ifndef LUNA_DEQUE_H
#define LUNA_DEQUE_H

#include "Luna/Result.h"

typedef struct Luna_Deque {
  void** elements;
  size_t size;
  size_t capacity;
  size_t front;
  Luna_Result(*acquire)(void*);
  Luna_Result(*relinquish)(void*);
} Luna_Deque;

Luna_Result Luna_Deque_initialize(Luna_Deque* deque);

Luna_Result Luna_Deque_uninitialize(Luna_Deque* deque);

Luna_Result Luna_Deque_grow(Luna_Deque* deque);

Luna_Result Luna_Deque_clear(Luna_Deque* deque);

/* Add an element to the back */
Luna_Result Luna_Deque_pushBack(Luna_Deque* ringBuffer, void* value);

/* Add an element to the front */
Luna_Result Luna_Deque_pushFront(Luna_Deque* ringBuffer, void* value);

/* Remove an element from the back */
Luna_Result Luna_Deque_popBack(Luna_Deque* deque, void** value);

/* Remove an element from the front */
Luna_Result Luna_Deque_popFront(Luna_Deque* deque, void** value);

#endif /* LUNA_DEQUE_H */
