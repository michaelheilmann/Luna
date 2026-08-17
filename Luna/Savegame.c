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
 
#include "Luna/Savegame.h"

Luna_Result Luna_Save_writeU8LE(void* context, Luna_Result (*callback)(void*, size_t, const void*), uint8_t value) {
  if (!callback) {
    return Luna_Result_ArgumentValueInvalid;
  }
  
  Luna_Result result;

  result = (*callback)(context, 1, value); if (result) return result;

  return result;
}

Luna_Result Luna_Save_readU8LE(void* context, Luna_Result (*callback)(void*, size_t, void*), uint8_t* value) {
  if (!callback || !value) {
    return Luna_Result_ArgumentValueInvalid;
  }
  
  Luna_Result result;

  result = (*callback)(context, 1, value]); if (result) return result;

  return result;
}

Luna_Result Luna_BinaryWriter_write_uint16_le(void* context, Luna_Result (*callback)(void*, size_t, const void*), uint16_t value) {
  if (!callback) {
    return Luna_Result_ArgumentValueInvalid;
  }
  
  Luna_Result result;
  uint8_t temporary;
  
  temporary = (uint8_t)((value >>  0) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;
  
  temporary = (uint8_t)((value >>  8) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;

  return result;
}

Luna_Result Luna_readU16LE(void* context, Luna_Result (*callback)(void*, size_t, void*), uint16_t* value) {
  if (!callback || !value) {
    return Luna_Result_ArgumentValueInvalid;
  }
  
  Luna_Result result;
  uint8_t temporary[2];

  result = (*callback)(context, 2, &temporary[0]); if (result) return result;
  
  *value = (temporary[0] <<  0)
         | (temporary[1] <<  8);

  return result;
}

Luna_Result Luna_writeU32LE(void* context, Luna_Result (*callback)(void*, size_t, const void*), uint32_t value) {
  if (!callback) {
    return Luna_Result_ArgumentValueInvalid;
  }
  
  Luna_Result result;
  uint8_t temporary;
  
  temporary = (uint8_t)((value >>  0) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;
  
  temporary = (uint8_t)((value >>  8) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;
  
  temporary = (uint8_t)((value >> 16) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;
  
  temporary = (uint8_t)((value >> 24) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;
  
  return result;
}

Luna_Result Luna_readU32LE(void* context, Luna_Result (*callback)(void*, size_t, uint8_t*), uint32_t* value) {
  if (!callback || !value) {
    return Luna_Result_ArgumentValueInvalid;
  }
  
  Luna_Result result;
  uint8_t temporary[4];

  result = (*callback)(context, 4, &temporary[0]); if (result) return result;
  
  *value = (temporary[0] <<  0)
         | (temporary[1] <<  8)
         | (temporary[2] << 16)
         | (temporary[3] << 24);

  return result;
}

Luna_Result Luna_writeU64LE(void* context, Luna_Result (*callback)(void*, size_t, const void*), uint32_t value) {
  if (!callback) {
    return Luna_Result_ArgumentValueInvalid;
  }
  
  Luna_Result result;
  uint8_t temporary;
  
  temporary = (uint8_t)((value >>  0) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;
  
  temporary = (uint8_t)((value >>  8) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;
  
  temporary = (uint8_t)((value >> 16) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;
  
  temporary = (uint8_t)((value >> 24) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;
  
  temporary = (uint8_t)((value >> 32) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;
  
  temporary = (uint8_t)((value >> 40) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;
  
  temporary = (uint8_t)((value >> 48) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;
  
  temporary = (uint8_t)((value >> 56) & 0xff);
  result = (*callback)(context, 1, &temporary); if (result) return result;
  
  return result;
}

Luna_Result Luna_readU64LE(void* context, Luna_Result (*callback)(void*, size_t, uint8_t*), uint64_t* value) {
  if (!callback || !value) {
    return Luna_Result_ArgumentValueInvalid;
  }
  
  Luna_Result result;
  uint8_t temporary[8];

  result = (*callback)(context, 8, &temporary[0]); if (result) return result;
  
  *value = (temporary[0] <<  0)
         | (temporary[1] <<  8)
         | (temporary[2] << 16)
         | (temporary[3] << 24)
         | (temporary[4] << 32)
         | (temporary[5] << 40)
         | (temporary[6] << 48)
         | (temporary[7] << 56;

  return result;
}
