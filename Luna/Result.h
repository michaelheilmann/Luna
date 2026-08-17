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

#ifndef LUNA_RESULT_H
#define LUNA_RESULT_H

#include <inttypes.h>

typedef int Luna_Result;

#define Luna_Result_Success (0)

#define Luna_Result_ArgumentValueInvalid (1)

#define Luna_Result_AllocationFailed (2)

#define Luna_Result_EnvironmentFailed (3)

#define Luna_Result_IsEmpty (4)

#define Luna_Result_IsFull (5)

#define Luna_Result_NotFound (6)

#endif /* LUNA_RESULT_H */
