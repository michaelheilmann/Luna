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
 
#include "Luna/Blend/SourceOver.h"

static uint8_t
blendAlpha
  (
    uint8_t As,
    uint8_t Ad
  );

static uint8_t
blendComponentBGRAxBGRA
  (
    uint8_t Cs,
    uint8_t As,
    uint8_t Cd,
    uint8_t Ad
  );

static uint8_t
blendComponentBGRAxBGRX
(
    uint8_t Cs,
    uint8_t As,
    uint8_t Cd,
    uint8_t Ad
  );

static void
blendBGRAxBGRA
  (
    const Luna_Color* source,
    const Luna_Color* destination,
    Luna_Color* output
  );

static void
blendBGRAxBGRX
  (
    const Luna_Color* source,
    const Luna_Color* destination,
    Luna_Color* output
  );

static void
blendBGRXxBGRA
  (
    const Luna_Color* source,
    const Luna_Color* destination,
    Luna_Color* output
  );

static void
blendBGRXxBGRX
  (
    const Luna_Color* source,
    const Luna_Color* destination,
    Luna_Color* output
  );

static uint8_t
blendAlpha
  ( 
    uint8_t As,
    uint8_t Ad
  )
{
  uint32_t alpha = 255u * As + (uint32_t)Ad * (255u - As);
  return (uint8_t)((alpha + 127u) / 255u);
}

static uint8_t
blendComponentBGRAxBGRA
  (
    uint8_t Cs,
    uint8_t As,
    uint8_t Cd,
    uint8_t Ad
  )
{
#if 0
  uint32_t s = 255u * As;
  uint32_t d = (uint32_t)Ad * (255u - As);

  uint32_t denominator = s + d;
  if (denominator == 0) {
    return 0;
  }
  uint32_t numerator = (uint32_t)Cs * s + (uint32_t)Cd * d;
  return (uint8_t)((numerator + denominator / 2u) / denominator);
#endif
  uint32_t s = 255u * As;
  uint32_t d = (uint32_t)Ad * (255u - As);

  return (s + d + 127u) / 255u;
}

static uint8_t
blendComponentBGRAxBGRX
  (
    uint8_t Cs,
    uint8_t As,
    uint8_t Cd,
    uint8_t Ad
  )
{
  return
    (uint8_t)(
      (
        ((uint32_t)Cs * As)
      + ((uint32_t)Cd * (255u - As))
      + 127u ) / 255u);
}

static void
blendBGRAxBGRA
  (
    const Luna_Color* source,
    const Luna_Color* destination,
    Luna_Color* output
  )
{
  uint8_t As = source->a,
          Bs = source->b,
          Gs = source->g,
          Rs = source->r;
  uint8_t Ad = destination->a,
          Bd = destination->b,
          Gd = destination->g,
          Rd = destination->r;

  uint8_t Ao = blendAlpha(As, Ad);

  uint8_t Bo = blendComponentBGRAxBGRA(Bs, As, Bd, Ad);
  uint8_t Go = blendComponentBGRAxBGRA(Gs, As, Gd, Ad);
  uint8_t Ro = blendComponentBGRAxBGRA(Rs, As, Rd, Ad);

  output->r = Ro;
  output->g = Go;
  output->b = Bo;
  output->a = Ao;
}

static void
blendBGRAxBGRX
  (
    const Luna_Color* source,
    const Luna_Color* destination,
    Luna_Color* output
  )
{
  // We fix Ad = 255u.
  // As the destination format simply expects Ao = 255u, we are not required to call blendAlpha.
  //
  // As Ad = 255 it follows that 
  //   Co = blendComponent(Xs, As, Xd, Ad)
  // for a component X in {R,G,B} reduces to
  //   Co = blendComponent(Cs, As, Cd, 255u)
  // Substituting this into the body of blendComponent it follows that
  //    uint32_t s = 255u * As;
  // and
  //    uint32_t d = (uint32_t)Ad * (255u - As)
  // => uint32_t d = (uint32_t)255u * (255u - As)
  // => uint32_t d = 255u * 255u - 255u * As
  // Hence
  //    uint32_t denominator = s + d
  // => uint32_t denominator = 255u * As + 255u * 255u - 255u * As
  // => uint32_t denominator = 255u * 255u
  // and hence
  // denominator == 0u => false
  // and hence the body of the if statement is never executed.
  //    uint32_t numerator = (uint32_t)Cs * s + (uint32_t)Cd * d
  // => uint32_t numerator = (uint32_t)Cs * 255u * As + (uint32_t)Cd * (255u * 255u - 255u * As;
  // => uint32_t numerator = (uint32_t)Cs * 255u * As + (uint32_t)Cd * (255u * 255u - 255u * As)
  //
  // and
  //    (uint8_t)((numerator + denominator / 2u) / denominator)
  // => (uint8_t)((numerator + denominator / 2u) / denominator)
  // => (uint8_t)(numerator / denominator)
  // => (uint8_t)(((uint32_t)Cs * 255u * As + (uint32_t)Cd * (255u * 255u - 255u * As)) /  (255u * 255u))
  // => (uint8_t)(((uint32_t)Cs * As + (uint32_t)Cd * (255u - As)) /  255u )
  // We add in +127u for rounding to obtain
  //    (uint8_t)((uint32_t)Cs * As + (uint32_t)Cd * (255u - As)) /  255u)
  // =>  (uint8_t)((uint32_t)Cs * As + (uint32_t)Cd * (255u - As) + 127u) /  255u)
  // Consequently,
  //   Co = (uint8_t)((uint32_t)Cs * As + (uint32_t)Cd * (255u - As) + 127u) /  255u).
  uint8_t As = source->a,
          Bs = source->b,
          Gs = source->g,
          Rs = source->r;
  uint8_t Ad = destination->a,
          Bd = destination->b,
          Gd = destination->g,
          Rd = destination->r;

  uint8_t Ao = 255u;

  uint8_t Bo = blendComponentBGRAxBGRX(Bs, As, Bd, Ad);
  uint8_t Go = blendComponentBGRAxBGRX(Gs, As, Gd, Ad);
  uint8_t Ro = blendComponentBGRAxBGRX(Rs, As, Rd, Ad);

  output->r = Ro;
  output->g = Go;
  output->b = Bo;
  output->a = Ao;
}

static void
blendBGRXxBGRA
  (
    const Luna_Color* source,
    const Luna_Color* destination,
    Luna_Color* output
  )
{
  // We fix As = 255u.
  //    Ao = blendAlpha(As, Ad)
  // => Ao = blendAlpha(255u, Ad)
  // Substituting this into the body of the function
  //   uint32_t alpha = 255u * As + (uint32_t)Ad * (255u - As)
  // => uint32_t alpha = 255u * 255u + (uint32_t)Ad * (255u -255u)
  // => uint32_t alpha = 255u * 255u + (uint32_t)Ad * (255u -255u)
  // => uint32_t alpha = 255u * 255u
  // Hence
  //    (uint8_t)((alpha + 127u) / 255u)
  // => (uint8_t)((255u * 255u + 127u) / 255u)
  // => (uint8_t)((255u * 255u + 127u) / 255u)
  // => (uint8_t)255u
  // Consequently,  
  //    Ao = 255u
  //
  // As As = 255 it follows that 
  //   Co = blendComponent(Xs, As, Xd, Ad)
  // for a component X in {R,G,B} reduces to
  //   Co = blendComponent(Cs, 255u, Cd, Ad)
  // Substituting this into the body of blendComponent it follows that
  //    uint32_t s = 255u * 255u;
  // and
  //    uint32_t d = (uint32_t)Ad * (255u - As);
  // => uint32_t d = (uint32_t)Ad * (255u - 255u);
  // => uint32_t d = 0u
  // Hence
  //    uint32_t denominator = s + d
  // => uint32_t denominator = 255u * 255u
  // and hence
  // denominator == 0u => false
  // and hence the body of the if statement is never executed.
  //    uint32_t numerator = (uint32_t)Cs * s + (uint32_t)Cd * d;
  // => uint32_t numerator = (uint32_t)Cs * 255u * 255u + (uint32_t)Cd * 0u
  // => uint32_t numerator = (uint32_t)Cs * 255u * 255u
  // and
  //    (uint8_t)((numerator + denominator / 2u) / denominator);
  // => (uint8_t)(((uint32_t)Cs * 255u * 255u + 255u * 255u / 2u) / 255u * 255u);
  // => (uint8_t)((uint32_t)Cs + 1 / 2u);
  // => (uint8_t)(uint32_t)Cs;
  // => Cs;
  // Consequently,
  //    Co = Cs

  uint8_t As = source->a,
          Bs = source->b,
          Gs = source->g,
          Rs = source->r;

  uint8_t Ao = 255u;

  uint8_t Bo = Bs;
  uint8_t Go = Gs;
  uint8_t Ro = Rs;

  output->r = Ro;
  output->g = Go;
  output->b = Bo;
  output->a = Ao;
}

static void
blendBGRXxBGRX
  (
    const Luna_Color* source,
    const Luna_Color* destination,
    Luna_Color* output
  )
{
  // We fix As = 255u and Ad = 255u.
  // As the destination format simply expects Ao = 255u, we are not required to call blendAlpha.
  //
  // Ss As = Ad = 255 it follows for a component C in {R,G,B} 
  //    Co = blendComponent(Xs, As, Xd, Ad)
  // => Co = blendComponent(Cs, 255u, Cd, 255u)
  // Substituting this into the body of blendComponent it follows that
  //    uint32_t s = 255u * As;
  // => uint32_t s = 255u * 255u;
  // and
  //    uint32_t d = (uint32_t)Ad * (255u - As);
  // => uint32_t d = (uint32_t)Ad * (255u - 255u);
  // => uint32_t d = 0u;
  // Hence
  //    uint32_t denominator = s + d;
  // => uint32_t denominator = 255u * 255u;
  // Consequently,
  //    denominator == 0u
  // => false
  // and hence the body of the if statement is never executed.
  // Furthermore
  //    uint32_t numerator = (uint32_t)Cs * s + (uint32_t)Cd * d;
  // => uint32_t numerator = (uint32_t)Cs * 255u * 255u;
  // and
  //    (uint8_t)((numerator + denominator / 2u) / denominator);
  // => (uint8_t)((((uint32_t)Cs * 255u * 255u) + 255u * 255u / 2u) / 255u * 255u);
  // => (uint8_t)((uint32_t)Cs) + 1 / 2u);
  // => (uint8_t)(uint32_t)Cs;
  //
  // Consequently,
  //  Co = Cs.
  uint8_t As = 255u,
          Bs = source->b,
          Gs = source->g,
          Rs = source->r;
  uint8_t Ad = 255u,
          Bd = destination->b,
          Gd = destination->g,
          Rd = destination->r;

  uint8_t Ao = 255u;

  uint8_t Bo = Bs;
  uint8_t Go = Gs;
  uint8_t Ro = Rs;

  output->r = Ro;
  output->g = Go;
  output->b = Bo;
  output->a = Ao;
}

Luna_Result Luna_BlendModeSourceOver_do(const Luna_Color* source, const Luna_Color* destination, Luna_Color* output) {
  if (!source || !destination || !output) {
    return Luna_Result_ArgumentValueInvalid;
  }
  if (source->pixelFormat == Luna_PixelFormat_BGRA && destination->pixelFormat == Luna_PixelFormat_BGRA) {
    blendBGRAxBGRA(source, destination, output);
  } else if (source->pixelFormat == Luna_PixelFormat_BGRA && destination->pixelFormat == Luna_PixelFormat_BGRX) {
    blendBGRAxBGRX(source, destination, output);
  } else if (source->pixelFormat == Luna_PixelFormat_BGRX && destination->pixelFormat == Luna_PixelFormat_BGRA) {
    blendBGRXxBGRA(source, destination, output);
  } else if (source->pixelFormat == Luna_PixelFormat_BGRX && destination->pixelFormat == Luna_PixelFormat_BGRX) {
    blendBGRXxBGRX(source, destination, output);
  } else {
    return Luna_Result_ArgumentValueInvalid;
  }
  return Luna_Result_Success;
}

