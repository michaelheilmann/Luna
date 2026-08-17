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
 
#ifndef LUNA_SAVEGAME_H
#define LUNA_SAVEGAME_H

#include "Luna/Result.h"

Luna_Result Luna_Save_writeU8LE(void* context, Luna_Result (*callback)(void*, size_t, const void*), uint8_t value);

Luna_Result Luna_Save_readU8LE(void* context, Luna_Result (*callback)(void*, size_t, void*), uint8_t* value);

Luna_Result Luna_BinaryWriter_write_uint16_le(void* context, Luna_Result (*callback)(void*, size_t, const void*), uint16_t value);

Luna_Result Luna_readU16LE(void* context, Luna_Result (*callback)(void*, size_t, void*), uint16_t* value);

Luna_Result Luna_writeU32LE(void* context, Luna_Result (*callback)(void*, size_t, const void*), uint32_t value);

Luna_Result Luna_readU32LE(void* context, Luna_Result (*callback)(void*, size_t, uint8_t*), uint32_t* value);

Luna_Result Luna_writeU64LE(void* context, Luna_Result (*callback)(void*, size_t, const void*), uint32_t value);

Luna_Result Luna_readU64LE(void* context, Luna_Result (*callback)(void*, size_t, uint8_t*), uint64_t* value);

#endif /* LUNA_SAVEGAME_H */
