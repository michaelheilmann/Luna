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
 
#include "Luna/Memory.h"

#include <malloc.h>

Luna_Result Luna_allocate(void** bytes, size_t numberOfBytes) {
  if (!bytes) {
    return Luna_Result_ArgumentValueInvalid;
  }
  void* temporary = malloc(numberOfBytes > 0 ? numberOfBytes : 1);
  if (!temporary) {
    return Luna_Result_AllocationFailed;   
  }
  *bytes = temporary;
  return Luna_Result_Success;
}

Luna_Result Luna_deallocate(void* bytes) {
  if (!bytes) {
    return Luna_Result_ArgumentValueInvalid;  
  }
  free(bytes);
  return Luna_Result_Success;
}

Luna_Result Luna_reallocate(void** bytes, size_t numberOfBytes) {
  if (!bytes) {
    return Luna_Result_ArgumentValueInvalid;
  }
  if (!*bytes) {
    return Luna_Result_ArgumentValueInvalid;
  }
  void* temporary = realloc(*bytes, numberOfBytes > 0 ? numberOfBytes : 1);
  if (!temporary) {
    return Luna_Result_AllocationFailed;   
  }
  *bytes = temporary;
  return Luna_Result_Success;
}
