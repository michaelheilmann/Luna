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

#include "Luna/Deque.h"

#include "Luna/Memory.h"

/* The minimal capacity of the deque. */
#define _minimal_capacity ((size_t)(0))

static_assert(_minimal_capacity == ((size_t)(0)), "assertion _minimal_capacity == ((size_t)(0)) violated");


/* The maximal capacity of the deque. */
#define _maximal_capacity (SIZE_MAX / sizeof(void*))

static_assert(_minimal_capacity <= _maximal_capacity, "assertion _minimal_capacity <= _maximal_capacity violated");

/* The initial capacity of the deque. */
#define _initial_capacity ((size_t)(8))

static_assert(_minimal_capacity <= _initial_capacity && _initial_capacity <= _maximal_capacity, "assertion _minimal_capacity <= _initial_capacity && _initial_capacity <= _maximal_capacity violated");

/* Convert a logical index into a physical array index */
static size_t _index(Luna_Deque* deque, size_t index);

static size_t _index(Luna_Deque* deque, size_t index) {
  return index < deque->capacity - deque->front
       ? deque->front + index
       : index - (deque->capacity - deque->front);
}

Luna_Result Luna_Deque_initialize(Luna_Deque* deque) {
  Luna_Result result;
  result = Luna_allocate((void**)&deque->elements, _initial_capacity * sizeof(void*));
  if (result) {
    return result;
  }
  deque->capacity = _initial_capacity;
  deque->size = 0;
  deque->front = 0;
  deque->acquire = NULL;
  deque->relinquish = NULL;
  return Luna_Result_Success;
}

Luna_Result Luna_Deque_uninitialize(Luna_Deque* deque) {
  Luna_Deque_clear(deque);
  Luna_deallocate(deque->elements);
  deque->elements = NULL;
  deque->capacity = 0;
  return Luna_Result_Success;
}

Luna_Result Luna_Deque_grow(Luna_Deque* deque) {
  size_t oldCapacity = deque->capacity;
  size_t newCapacity = oldCapacity * 2;
  if (oldCapacity > _maximal_capacity / 2) {
    newCapacity = _maximal_capacity;
  } else {
    newCapacity = oldCapacity ? oldCapacity * 2 : 8;
  }
  Luna_Result result;
  void** newElements;
  result = Luna_allocate((void**)&newElements, newCapacity * sizeof(void*));
  if (result) {
    return result;
  }

  /* Copy elements in logical order: front -> ... -> back */
  for (size_t i = 0; i < deque->size; ++i)
    newElements[i] = deque->elements[_index(deque, i)];

  Luna_deallocate(deque->elements);
  deque->elements = newElements;
  deque->capacity = newCapacity;
  deque->front = 0;

  return Luna_Result_Success;
}

Luna_Result Luna_Deque_clear(Luna_Deque* deque) {
  while (deque->size) {
    void* value = deque->elements[deque->front];
    if (deque->relinquish) {
      deque->relinquish(value);
    }
    deque->front = (deque->front + 1) % deque->capacity;
    deque->size--;
  }
  return Luna_Result_Success;
}

Luna_Result Luna_Deque_pushBack(Luna_Deque* deque, void* value) {
  if (deque->size == deque->capacity) {
    Luna_Result result = Luna_Deque_grow(deque);
    if (result) {
      return result;
    }
  }

  if (deque->acquire) {
    Luna_Result result = deque->acquire(value);
    if (result) {
      return result;
    }
  }

  size_t index = _index(deque, deque->size);
  deque->elements[index] = value;
  deque->size++;

  return Luna_Result_Success;
}

Luna_Result Luna_Deque_pushFront(Luna_Deque* deque, void* value) {
  if (deque->size == deque->capacity) {
    Luna_Result result = Luna_Deque_grow(deque);
    if (result) {
      return result;
    }
  }

  if (deque->acquire) {
    Luna_Result result = deque->acquire(value);
    if (result) {
      return result;
    }
  }
    
  if (deque->front == 0) {
    deque->front = deque->capacity - 1;
  } else {
    deque->front--;
  }
  deque->elements[deque->front] = value;
  deque->size++;

  return Luna_Result_Success;
}

Luna_Result Luna_Deque_popBack(Luna_Deque* deque, void** value) {
  if (!deque || !value)
    return Luna_Result_ArgumentValueInvalid;
  if (deque->size == 0)
    return Luna_Result_IsEmpty;

  size_t index = _index(deque, deque->size - 1);

  void* _value = deque->elements[index];
  deque->size--;

  if (deque->relinquish) {
    deque->relinquish(_value);
  }

  *value = _value;

  return Luna_Result_Success;
}

Luna_Result Luna_Deque_popFront(Luna_Deque* deque, void** value) {
  if (!deque || !value)
    return Luna_Result_ArgumentValueInvalid;
  if (deque->size == 0)
    return Luna_Result_IsEmpty;


  void* _value = deque->elements[deque->front];
  deque->front = (deque->front + 1) % deque->capacity;
  deque->size--;

  if (deque->relinquish) {
    deque->relinquish(_value);
  }

  *value = _value;

  return Luna_Result_Success;
}
