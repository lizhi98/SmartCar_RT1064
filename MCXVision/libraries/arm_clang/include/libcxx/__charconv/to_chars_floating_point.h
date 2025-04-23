// -*- C++ -*-
// Modifications copyright (C) 2024 Arm Limited (or its affiliates). All rights reserved.
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP___CHARCONV_TO_CHARS_FLOATING_POINT_H
#define _LIBCPP___CHARCONV_TO_CHARS_FLOATING_POINT_H

#include <__charconv/chars_format.h>
#include <__charconv/to_chars_result.h>
#include <__config>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#  pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER >= 17

_LIBCPP_AVAILABILITY_TO_CHARS_FLOATING_POINT _LIBCPP_EXPORTED_FROM_ABI _LIBCPP_FLOAT_ABI to_chars_result
to_chars(char* __first, char* __last, float __value);

_LIBCPP_AVAILABILITY_TO_CHARS_FLOATING_POINT _LIBCPP_EXPORTED_FROM_ABI _LIBCPP_FLOAT_ABI to_chars_result
to_chars(char* __first, char* __last, double __value);

#ifdef __LIBCPP_HAVE_LONGDOUBLE
_LIBCPP_AVAILABILITY_TO_CHARS_FLOATING_POINT _LIBCPP_EXPORTED_FROM_ABI _LIBCPP_FLOAT_ABI to_chars_result
to_chars(char* __first, char* __last, long double __value);
#endif

_LIBCPP_AVAILABILITY_TO_CHARS_FLOATING_POINT _LIBCPP_EXPORTED_FROM_ABI _LIBCPP_FLOAT_ABI to_chars_result
to_chars(char* __first, char* __last, float __value, chars_format __fmt);

_LIBCPP_AVAILABILITY_TO_CHARS_FLOATING_POINT _LIBCPP_EXPORTED_FROM_ABI _LIBCPP_FLOAT_ABI to_chars_result
to_chars(char* __first, char* __last, double __value, chars_format __fmt);

#ifdef __LIBCPP_HAVE_LONGDOUBLE
_LIBCPP_AVAILABILITY_TO_CHARS_FLOATING_POINT _LIBCPP_EXPORTED_FROM_ABI _LIBCPP_FLOAT_ABI to_chars_result
to_chars(char* __first, char* __last, long double __value, chars_format __fmt);
#endif

_LIBCPP_AVAILABILITY_TO_CHARS_FLOATING_POINT _LIBCPP_EXPORTED_FROM_ABI _LIBCPP_FLOAT_ABI to_chars_result
to_chars(char* __first, char* __last, float __value, chars_format __fmt, int __precision);

_LIBCPP_AVAILABILITY_TO_CHARS_FLOATING_POINT _LIBCPP_EXPORTED_FROM_ABI _LIBCPP_FLOAT_ABI to_chars_result
to_chars(char* __first, char* __last, double __value, chars_format __fmt, int __precision);

#ifdef __LIBCPP_HAVE_LONGDOUBLE
_LIBCPP_AVAILABILITY_TO_CHARS_FLOATING_POINT _LIBCPP_EXPORTED_FROM_ABI _LIBCPP_FLOAT_ABI to_chars_result
to_chars(char* __first, char* __last, long double __value, chars_format __fmt, int __precision);
#endif
#endif // _LIBCPP_STD_VER >= 17

_LIBCPP_END_NAMESPACE_STD

#endif // _LIBCPP___CHARCONV_TO_CHARS_FLOATING_POINT_H
