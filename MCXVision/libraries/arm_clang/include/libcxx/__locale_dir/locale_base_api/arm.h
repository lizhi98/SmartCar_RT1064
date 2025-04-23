/* locale_arm.h
 *
 * Copyright 2024 Arm Limited. All rights reserved.
 */

#ifndef _LIBCPP___LOCALE_LOCALE_BASE_API_ARM_H
#define _LIBCPP___LOCALE_LOCALE_BASE_API_ARM_H

#include <cstdlib>
#include <clocale>

typedef void* locale_t;

// Patch over newlib's lack of extended locale support
typedef void *locale_t;
static inline locale_t duplocale(locale_t) {
  return NULL;
}

static inline void freelocale(locale_t) {
}

static inline locale_t newlocale(int, const char *, locale_t) {
  return NULL;
}

static inline locale_t uselocale(locale_t) {
  return NULL;
}

// Our LC_FOO values are bit masks already, so we can define the _MASK
// versions to just have the same values.
#define LC_COLLATE_MASK  (LC_COLLATE)
#define LC_CTYPE_MASK    (LC_CTYPE)
#define LC_MONETARY_MASK (LC_MONETARY)
#define LC_NUMERIC_MASK  (LC_NUMERIC)
#define LC_TIME_MASK     (LC_TIME)
#define LC_ALL_MASK (LC_COLLATE_MASK|\
                     LC_CTYPE_MASK|\
                     LC_MONETARY_MASK|\
                     LC_NUMERIC_MASK|\
                     LC_TIME_MASK)

// Indicate that wcsnrtombs, mbsnrtowcs, asprintf, vasprintf are
// defined under different names by Arm Compiler for Embedded's libraries to avoid
// impinging on user namespace
#define _ARM_LIB_NAMESPACE_GUARD

#include <string.h>
#include <wchar.h>
#include <__support/xlocale/__posix_l_fallback.h>
#include <__support/xlocale/__strtonum_fallback.h>

#endif // _LIBCPP___LOCALE_LOCALE_BASE_API_ARM_H
