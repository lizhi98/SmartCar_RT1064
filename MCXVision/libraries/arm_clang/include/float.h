/* float.h: ANSI 'C' (X3J11 Oct 88) library header, section 2.2.4.2 */
/* Copyright (C) Codemist Ltd, 1988                             */
/* Copyright 1991 ARM Limited. All rights reserved.             */
/* version 0.01 */

/*
 * RCS $Revision$
 * Checkin $Date$
 * Revising $Author: drodgman $
 */

#ifndef __float_h
#define __float_h
#define __ARMCLIB_VERSION 6230001

  #ifndef __FLOAT_DECLS
  #define __FLOAT_DECLS

    #undef __CLIBNS

    #ifdef __cplusplus
      namespace std {
      #define __CLIBNS std::
        extern "C" {
    #else
      #define __CLIBNS
    #endif  /* __cplusplus */

/* IEEE version: the following values are taken from the above ANSI draft.  */
/* The ACORN FPE (v17) is known not to precisely implement IEEE arithmetic. */

#define FLT_RADIX     2
    /* radix of exponent representation */
#ifdef __FP_FENV_ROUNDING
extern unsigned __ieee_status(unsigned,unsigned); /* from fenv.h */
#define FLT_ROUNDS ((1+(__CLIBNS __ieee_status(0, 0)>>22))&3)
#else
#define FLT_ROUNDS    1
#endif
    /*
     * The rounding mode for floating-point addition is characterised by the
     * value of FLT_ROUNDS:
     *  -1 : indeterminable.
     *   0 : towards zero.
     *   1 : to nearest.
     *   2 : towards positive infinity.
     *   3 : towards negative infinity.
     *   ? : any other is implementation-defined.
     */

#define FLT_MANT_DIG        24
#define DBL_MANT_DIG        53
#if defined(__aarch64__)
#  define LDBL_MANT_DIG       113
#else
#  define LDBL_MANT_DIG       53
#endif
    /* number of base-FLT_RADIX digits in the floating point mantissa */
#if !defined(__STRICT_ANSI__) || (defined(__STDC_VERSION__) && 199901L <= __STDC_VERSION__) || (defined(__cplusplus) && 201103L <= __cplusplus)
#define DECIMAL_DIG 17
    /* number of decimal digits that the widest floating point type
       can be rounded to and back again without changing the value */
#ifdef __FP_FAST
#  define FLT_EVAL_METHOD (-1)
#else
#  define FLT_EVAL_METHOD 0
#endif
    /*
     * The use of evaluation formats is characterized by the value of
     * FLT_EVAL_METHOD:
     *  -1 : indeterminable.
     *   0 : evaluate all operations and constants just to the range
     *       and precision of the type.
     *   1 : evaluate operations and constants of type float and
     *       double to the range and precision of the double type,
     *       evaluate long double operations and constants to the
     *       range and precision of the long double type.
     *   2 : evaluate all opertations and constants to the range and
     *       precision of the long double type.
     */
#endif

#if !defined(__STRICT_ANSI__) || (defined(__STDC_VERSION__) && 201112L <= __STDC_VERSION__) || (defined(__cplusplus) && 201703L <= __cplusplus)
#define FLT_DECIMAL_DIG     9
#define DBL_DECIMAL_DIG     17
#if defined(__aarch64__)
#define LDBL_DECIMAL_DIG    36
#else
#define LDBL_DECIMAL_DIG    17
#endif
    /* number of decimal digits that the corresponding floating point type
       can be rounded to and back again without changing the value */

#define FLT_TRUE_MIN        0x1.0p-149F
#define DBL_TRUE_MIN        0x1.0p-1074
#if defined(__aarch64__)
#define LDBL_TRUE_MIN       0x1.0p-16494L
#else
#define LDBL_TRUE_MIN       0x1.0p-1074
#endif
    /*  minimum positive floating-point number */

#define FLT_HAS_SUBNORM     1
#define DBL_HAS_SUBNORM     1
#define LDBL_HAS_SUBNORM    1
    /*
     * The presence or absence of subnormal numbers:
     *  -1 : indeterminable.
     *   0 : absent (type does not support subnormal numbers).
     *   1 : present (type does support subnormal numbers).
     */
#endif

/* The values that follow are not achieved under Acorn's FPE version 17  */
/* but they should be correct in due course!                             */

#define FLT_DIG      6
#define DBL_DIG      15
#if defined(__aarch64__)
#define LDBL_DIG     33
#else
#define LDBL_DIG     15
#endif
    /* number of decimal digits of precision */

#define FLT_MIN_EXP  (-125)
#define DBL_MIN_EXP  (-1021)
#if defined(__aarch64__)
#define LDBL_MIN_EXP (-16381)
#else
#define LDBL_MIN_EXP (-1021)
#endif
    /* minimum negative integer such that FLT_RADIX raised to that power */
    /* minus 1 is a normalised floating-point number. */

#define FLT_MIN_10_EXP  (-37)
#define DBL_MIN_10_EXP  (-307)
#if defined(__aarch64__)
#define LDBL_MIN_10_EXP (-4931)
#else
#define LDBL_MIN_10_EXP (-307)
#endif
    /* minimum negative integer such that 10 raised to that power is in the */
    /* range of normalised floating-point numbers. */

#define FLT_MAX_EXP  128
#define DBL_MAX_EXP  1024
#if defined(__aarch64__)
#define LDBL_MAX_EXP 16384
#else
#define LDBL_MAX_EXP 1024
#endif
    /* maximum integer such that FLT_RADIX raised to that power minus 1 is a */
#define FLT_MAX_10_EXP  38
#define DBL_MAX_10_EXP  308
#if defined(__aarch64__)
#define LDBL_MAX_10_EXP 4932
#else
#define LDBL_MAX_10_EXP 308
#endif
    /* maximum integer such that 10 raised to that power is in the range of */
    /* representable finite floating-point numbers. */

#define FLT_MAX  3.40282347e+38F
#define DBL_MAX  1.79769313486231571e+308
#if defined(__aarch64__)
#define LDBL_MAX 1.18973149535723176508575932662800702e+4932L
#else
#define LDBL_MAX 1.79769313486231571e+308L
#endif
    /* maximum representable finite floating-point number. */

#define FLT_EPSILON         1.19209290e-7F
#define DBL_EPSILON         2.2204460492503131e-16
#if defined(__aarch64__)
#define LDBL_EPSILON        1.92592994438723585305597794258492732e-34L
#else
#define LDBL_EPSILON        2.2204460492503131e-16L
#endif
    /* minimum positive floating point number x such that 1.0 + x != 1.0 */

#define FLT_MIN  1.175494351e-38F
#define DBL_MIN  2.22507385850720138e-308
#if defined(__aarch64__)
#define LDBL_MIN 3.36210314311209350626267781732175260e-4932L
#else
#define LDBL_MIN 2.22507385850720138e-308L
#endif
    /* minimum normalised positive floating-point number. */

/*
 * The Microsoft <float.h> extensions.
 */
#ifdef __cplusplus
extern "C" {
#endif
unsigned _controlfp(unsigned, unsigned);
unsigned _clearfp(void);
unsigned _statusfp(void);
#ifdef __cplusplus
}
#endif


/*
 * Because the _EM_ constants are shared between _controlfp (masks)
 * and _statusfp (sticky bits), we adopt the convention that
 * _controlfp will shift its arguments left by 8 bits before using
 * them.
 */

#define _MCW_EM         0x001F
#define _EM_INVALID     0x0001
#define _EM_ZERODIVIDE  0x0002
#define _EM_OVERFLOW    0x0004
#define _EM_UNDERFLOW   0x0008
#define _EM_INEXACT     0x0010

#define _MCW_RC         0xC000
#define _RC_CHOP        0xC000
#define _RC_UP          0x4000
#define _RC_DOWN        0x8000
#define _RC_NEAR        0x0000

/*
 * _FPE_ constants passed as the hidden second argument to SIGFPE
 * handlers.
 */
#define _FPE_INVALID     0x04000000
#define _FPE_ZERODIVIDE  0x08000000
#define _FPE_OVERFLOW    0x10000000
#define _FPE_UNDERFLOW   0x20000000
#define _FPE_INEXACT     0x40000000


    #ifdef __cplusplus
        }  /* extern "C" */
      }  /* namespace std */
    #endif
  #endif /* __FLOAT_DECLS */

  #if defined(__cplusplus) && !defined(__FLOAT_NO_EXPORTS)
    using ::std::_controlfp;
    using ::std::_clearfp;
    using ::std::_statusfp;
  #endif

#endif /* __float_h */

/* end of float.h */

