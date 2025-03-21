/*
MIT License

Copyright (c) 2020-2025 hirrolot

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
 * @file
 * @brief A slice of some array.
 *
 * Some macros are automatically defined in case they have not been defined before including this
 * header file; these are: #SLICE99_ASSERT, #SLICE99_MEMCMP, #SLICE99_MEMCPY, #SLICE99_MEMMOVE,
 * #SLICE99_STRLEN, #SLICE99_VSPRINTF, #SLICE99_VSNPRINTF, and #SLICE99_SNPRINTF. They represent the
 * corresponding standard library's functions, although actual implementations can differ. If you
 * develop software for a freestanding environment, these macros must be defined beforehand. If you
 * do not want to implement string formatting macros from `stdio.h`, define `SLICE99_DISABLE_STDIO`
 * and Slice99 will not require them from you.
 */

/**
 * @mainpage
 *
 * This library provides <a href="https://en.wikipedia.org/wiki/Array_slicing">array slicing</a>
 * facilities for pure C99.<br>
 *
 * All the documentation is located in slice99.h. See <a
 * href="https://github.com/hirrolot/slice99">our official GitHub repository</a> for a high-level
 * overview.
 */

#ifndef SLICE99_H
#define SLICE99_H

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef SLICE99_ASSERT
#include <assert.h>
/// Like `assert`.
#define SLICE99_ASSERT assert
#endif // SLICE99_ASSERT

#ifndef SLICE99_MEMCMP
#include <string.h>
/// Like `memcmp`.
#define SLICE99_MEMCMP memcmp
#endif

#ifndef SLICE99_MEMCPY
#include <string.h>
/// Like `memcpy`.
#define SLICE99_MEMCPY memcpy
#endif

#ifndef SLICE99_MEMMOVE
#include <string.h>
/// Like `memmove`.
#define SLICE99_MEMMOVE memmove
#endif

#ifndef SLICE99_STRLEN
#include <string.h>
/// Like `strlen`.
#define SLICE99_STRLEN strlen
#endif

#ifndef DOXYGEN_IGNORE

#ifdef __GNUC__
#define SLICE99_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#define SLICE99_WARN_UNUSED_RESULT
#endif // __GNU__

#if defined(__GNUC__) && !defined(__clang__)

#define SLICE99_CONST         __attribute__((const))
#define SLICE99_PURE          __attribute__((pure))
#define SLICE99_ALWAYS_INLINE __attribute__((always_inline))
#define SLICE99_FORMAT_HINT(archetype, string_idx, first_to_check)                                 \
    __attribute__((format(archetype, string_idx, first_to_check)))

#else

#define SLICE99_CONST
#define SLICE99_PURE
#define SLICE99_ALWAYS_INLINE
#define SLICE99_FORMAT_HINT(_archetype, _string_idx, _first_to_check)

#endif

#endif // DOXYGEN_IGNORE

/**
 * The major version number.
 */
#define SLICE99_MAJOR 0

/**
 * The minor version number.
 */
#define SLICE99_MINOR 7

/**
 * The patch version number.
 */
#define SLICE99_PATCH 8

/**
 * Defines the strongly typed slice @p name containing items of type @p T.
 *
 * This macro defines
 *
 * @code
 * typedef struct {
 *     T *ptr;
 *     size_t len;
 * } name;
 * @endcode
 *
 * Also, it specialises all the functions operating on #Slice99. Every function (except for
 * #Slice99_from_str and #Slice99_c_str) is defined by the following rules:
 *
 *  - A function named `Slice99_*` becomes `name_*`.
 *  - `void *` is replaced by `T *`.
 *  - `Slice99` is replaced by `name`.
 *  - `size_t item_size` parameters are removed (e.g., as in #Slice99_new, #Slice99_empty,
 * #Slice99_from_ptrdiff).
 *  - All function preconditions, invariants, and postconditions remain the same.
 *
 * #Slice99_from_str and #Slice99_c_str are derived only for `CharSlice99`.
 *
 * # Examples
 *
 * @code
 * #include <slice99.h>
 *
 * typedef struct {
 *     double x, y;
 * } Point;
 *
 * SLICE99_DEF_TYPED(MyPoints, Point);
 *
 * int main(void) {
 *     MyPoints points = (MyPoints)Slice99_typed_from_array(
 *         (Point[]){{1.5, 32.5}, {12.0, 314.01}, {-134.10, -9.3}});
 *
 *     MyPoints first_two = MyPoints_sub(points, 0, 2);
 *     Point *first = MyPoints_first(points);
 *     bool is_empty = MyPoints_is_empty(points);
 * }
 * @endcode
 */
#define SLICE99_DEF_TYPED(name, T)                                                                 \
    typedef struct {                                                                               \
        T *ptr;                                                                                    \
        size_t len;                                                                                \
    } name;                                                                                        \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT name name##_new(                \
        T *ptr, size_t len) {                                                                      \
        const Slice99 result = Slice99_new((void *)ptr, sizeof(T), len);                           \
        return (name)SLICE99_TO_TYPED(result);                                                     \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT name name##_from_ptrdiff(       \
        T *start, T *end) {                                                                        \
        const Slice99 result = Slice99_from_ptrdiff((void *)start, (void *)end, sizeof(T));        \
        return (name)SLICE99_TO_TYPED(result);                                                     \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT name name##_empty(void) {       \
        const Slice99 result = Slice99_empty(sizeof(T));                                           \
        return (name)SLICE99_TO_TYPED(result);                                                     \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT name name##_update_len(         \
        name self, size_t new_len) {                                                               \
        const Slice99 result = Slice99_update_len(SLICE99_TO_UNTYPED(self), new_len);              \
        return (name)SLICE99_TO_TYPED(result);                                                     \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT                                 \
        SLICE99_CONST bool name##_is_empty(name self) {                                            \
        return Slice99_is_empty(SLICE99_TO_UNTYPED(self));                                         \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT SLICE99_CONST size_t            \
        name##_size(name self) {                                                                   \
        return Slice99_size(SLICE99_TO_UNTYPED(self));                                             \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT SLICE99_CONST T *name##_get(    \
        name self, ptrdiff_t i) {                                                                  \
        return (T *)Slice99_get(SLICE99_TO_UNTYPED(self), i);                                      \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT SLICE99_CONST T *name##_first(  \
        name self) {                                                                               \
        return (T *)Slice99_first(SLICE99_TO_UNTYPED(self));                                       \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT SLICE99_CONST T *name##_last(   \
        name self) {                                                                               \
        return (T *)Slice99_last(SLICE99_TO_UNTYPED(self));                                        \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT name name##_sub(                \
        name self, ptrdiff_t start_idx, ptrdiff_t end_idx) {                                       \
        const Slice99 result = Slice99_sub(SLICE99_TO_UNTYPED(self), start_idx, end_idx);          \
        return (name)SLICE99_TO_TYPED(result);                                                     \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT name name##_advance(            \
        name self, ptrdiff_t offset) {                                                             \
        const Slice99 result = Slice99_advance(SLICE99_TO_UNTYPED(self), offset);                  \
        return (name)SLICE99_TO_TYPED(result);                                                     \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT bool name##_primitive_eq(       \
        name lhs, name rhs) {                                                                      \
        return Slice99_primitive_eq(SLICE99_TO_UNTYPED(lhs), SLICE99_TO_UNTYPED(rhs));             \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT bool name##_eq(                 \
        name lhs, name rhs, int (*cmp)(const T *, const T *)) {                                    \
        return Slice99_eq(                                                                         \
            SLICE99_TO_UNTYPED(lhs), SLICE99_TO_UNTYPED(rhs),                                      \
            (int (*)(const void *, const void *))cmp);                                             \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT bool                            \
        name##_primitive_starts_with(name self, name prefix) {                                     \
        return Slice99_primitive_starts_with(                                                      \
            SLICE99_TO_UNTYPED(self), SLICE99_TO_UNTYPED(prefix));                                 \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT bool name##_starts_with(        \
        name self, name prefix, int (*cmp)(const T *, const T *)) {                                \
        return Slice99_starts_with(                                                                \
            SLICE99_TO_UNTYPED(self), SLICE99_TO_UNTYPED(prefix),                                  \
            (int (*)(const void *, const void *))cmp);                                             \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT bool                            \
        name##_primitive_ends_with(name self, name postfix) {                                      \
        return Slice99_primitive_ends_with(SLICE99_TO_UNTYPED(self), SLICE99_TO_UNTYPED(postfix)); \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE SLICE99_WARN_UNUSED_RESULT bool name##_ends_with(          \
        name self, name postfix, int (*cmp)(const T *, const T *)) {                               \
        return Slice99_ends_with(                                                                  \
            SLICE99_TO_UNTYPED(self), SLICE99_TO_UNTYPED(postfix),                                 \
            (int (*)(const void *, const void *))cmp);                                             \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE void name##_copy(name self, name other) {                  \
        Slice99_copy(SLICE99_TO_UNTYPED(self), SLICE99_TO_UNTYPED(other));                         \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE void name##_copy_non_overlapping(name self, name other) {  \
        Slice99_copy_non_overlapping(SLICE99_TO_UNTYPED(self), SLICE99_TO_UNTYPED(other));         \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE void name##_swap(                                          \
        name self, ptrdiff_t lhs, ptrdiff_t rhs, T *restrict backup) {                             \
        Slice99_swap(SLICE99_TO_UNTYPED(self), lhs, rhs, (void *restrict)backup);                  \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE void name##_swap_with_slice(                               \
        name self, name other, T *restrict backup) {                                               \
        Slice99_swap_with_slice(                                                                   \
            SLICE99_TO_UNTYPED(self), SLICE99_TO_UNTYPED(other), (void *restrict)backup);          \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE void name##_reverse(name self, T *restrict backup) {       \
        Slice99_reverse(SLICE99_TO_UNTYPED(self), (void *restrict)backup);                         \
    }                                                                                              \
                                                                                                   \
    inline static SLICE99_ALWAYS_INLINE void name##_split_at(                                      \
        name self, size_t i, name *restrict lhs, name *restrict rhs) {                             \
        SLICE99_ASSERT(lhs);                                                                       \
        SLICE99_ASSERT(rhs);                                                                       \
                                                                                                   \
        Slice99 lhs_untyped = SLICE99_TO_UNTYPED(*lhs), rhs_untyped = SLICE99_TO_UNTYPED(*rhs);    \
                                                                                                   \
        Slice99_split_at(SLICE99_TO_UNTYPED(self), i, &lhs_untyped, &rhs_untyped);                 \
                                                                                                   \
        *lhs = (name)SLICE99_TO_TYPED(lhs_untyped);                                                \
        *rhs = (name)SLICE99_TO_TYPED(rhs_untyped);                                                \
    }                                                                                              \
                                                                                                   \
    struct slice99_priv_trailing_comma

/**
 * Converts #Slice99 to a typed representation.
 *
 * @pre @p self must be an expression of type #Slice99.
 *
 * # Examples
 *
 * @code
 * #include <slice99.h>
 *
 * int main(void) {
 *     Slice99 x = Slice99_from_str("abc");
 *     CharSlice99 y = (CharSlice99)SLICE99_TO_TYPED(x);
 *     (void)y;
 * }
 * @endcode
 */
#define SLICE99_TO_TYPED(self)                                                                     \
    { .ptr = (self).ptr, .len = (self).len }

/**
 * Converts the typed slice @p self to #Slice99.
 *
 * @pre @p self must be an expression of type defined by #SLICE99_DEF_TYPED.
 *
 * # Examples
 *
 * @code
 * #include <slice99.h>
 *
 * int main(void) {
 *     CharSlice99 x = CharSlice99_from_str("abc");
 *     Slice99 y = SLICE99_TO_UNTYPED(x);
 *     (void)y;
 * }
 * @endcode
 */
#define SLICE99_TO_UNTYPED(self) Slice99_new((self).ptr, sizeof(*(self).ptr), (self).len)

/**
 * Computes a number of items in an array expression.
 */
#define SLICE99_ARRAY_LEN(...) (sizeof(__VA_ARGS__) / sizeof((__VA_ARGS__)[0]))

/**
 * Copies @p obj to @p buffer, returning the next position of @p buffer to write to.
 *
 * This macro is no different from `memcpy` except for the return value. It is useful when you
 * want to write a sequence of objects to some memory buffer, e.g., to a packet header:
 *
 * @code
 * const uint16_t x = 123;
 * const uint32_t y = 456;
 *
 * header = SLICE99_APPEND(header, x);
 * header = SLICE99_APPEND(header, y);
 * // ...
 * @endcode
 *
 * @param[out] buffer The memory area to write to.
 * @param[in] obj The object (lvalue) that will be copied to @p buffer, byte-by-byte.
 *
 * @return `(void *)((char *)buffer + sizeof(obj))`
 *
 * @pre @p buffer must be capable of holding at least `sizeof(obj)` bytes.
 * @pre @p buffer and @p obj must be non-overlapping.
 *
 * @see #SLICE99_APPEND_ARRAY
 */
#define SLICE99_APPEND(buffer, obj)                                                                \
    ((void *)((char *)SLICE99_MEMCPY((buffer), &(obj), sizeof(obj)) + sizeof(obj)))

/**
 * Copies the array of length @p len accessible through @p ptr into @p buffer, returning the next
 * position of @p buffer to write to.
 *
 * This function has the same requirements and a return value as of #SLICE99_APPEND.
 */
#define SLICE99_APPEND_ARRAY(buffer, ptr, len)                                                     \
    /* clang-format off */ \
    ((void *)((char *)SLICE99_MEMCPY((buffer), (ptr), sizeof((ptr)[0]) * (len)) + \
              sizeof((ptr)[0]) * (len))) /* clang-format on */

#ifdef UINT8_MAX

/**
 * Constructs `U8Slice99` from @p obj.
 *
 * @param[in] obj The object (lvalue) to which the resulting slice will point to.
 *
 * @return An octet slice pointing to @p obj of length `sizeof(obj)`.
 */
#define SLICE99_TO_OCTETS(obj) U8Slice99_new((uint8_t *)&(obj), sizeof(obj))

#endif

/**
 * Constructs a slice from an array expression.
 *
 * The resulting slice will have #Slice99.len equal to the number of items in the array, and
 * #Slice99.item_size equal to the size of each item.
 */
#define Slice99_from_array(...)                                                                    \
    Slice99_new((void *)(__VA_ARGS__), sizeof((__VA_ARGS__)[0]), SLICE99_ARRAY_LEN(__VA_ARGS__))

/**
 * The same as #Slice99_from_array but for typed slices.
 *
 * # Examples
 *
 * @code
 * #include <slice99.h>
 *
 * int main(void) {
 *     IntSlice99 x = (IntSlice99)Slice99_typed_from_array((int[]){1, 2, 3});
 *     (void)x;
 * }
 * @endcode
 */
#define Slice99_typed_from_array(...)                                                              \
    { .ptr = (__VA_ARGS__), .len = SLICE99_ARRAY_LEN(__VA_ARGS__) }

/**
 * Constructs a slice from a pointer of a non-`void` type and a length.
 *
 * It is equivalent to #Slice99_new but it automatically computes an item size as `sizeof(*ptr)`.
 *
 * @param[in] ptr The pointer of the resulting slice. Must not point to `void`.
 * @param[in] len The length of the resulting slice.
 */
#define Slice99_from_typed_ptr(ptr, len) Slice99_new(ptr, sizeof(*(ptr)), len)

/**
 * A slice of some array.
 *
 * This structure should not be constructed manually; use #Slice99_new instead.
 *
 * @invariant #ptr must not be `NULL`.
 * @invariant #item_size must be strictly greater than 0.
 */
typedef struct {
    /**
     * The pointer to data.
     */
    void *ptr;

    /**
     * The size of each item in the array addressed by #ptr.
     */
    size_t item_size;

    /**
     * The count of items in the array addressed by #ptr.
     */
    size_t len;
} Slice99;

/**
 * Constructs a slice.
 *
 * @param[in] ptr The value of Slice99#ptr.
 * @param[in] item_size The value of Slice99#item_size.
 * @param[in] len The value of Slice99#len.
 *
 * @pre `ptr != NULL`
 * @pre `item_size > 0`
 */
inline static SLICE99_WARN_UNUSED_RESULT Slice99
Slice99_new(void *ptr, size_t item_size, size_t len) {
    SLICE99_ASSERT(ptr);
    SLICE99_ASSERT(item_size > 0);

    return (Slice99){.ptr = ptr, .item_size = item_size, .len = len};
}

/**
 * Constructs a slice from @p str.
 *
 * @param[in] str Any null-terminated string.
 *
 * @pre `str != NULL`
 */
inline static SLICE99_WARN_UNUSED_RESULT Slice99 Slice99_from_str(char *str) {
    SLICE99_ASSERT(str);

    return Slice99_new(str, sizeof(char), SLICE99_STRLEN(str));
}

/**
 * Constructs a slice residing between @p start (inclusively) and @p end (exclusively).
 *
 * @param[in] start The start position of a returned slice, inclusively.
 * @param[in] end The end position of a returned slice, exlusively.
 * @param[in] item_size The value of Slice99#item_size.
 *
 * @pre `start != NULL`
 * @pre `end != NULL`
 * @pre `((char *)end - (char *)start) >= 0`
 * @pre `(((char *)end - (char *)start)) % item_size == 0`
 */
inline static SLICE99_WARN_UNUSED_RESULT Slice99
Slice99_from_ptrdiff(void *start, void *end, size_t item_size) {
    SLICE99_ASSERT(start);
    SLICE99_ASSERT(end);

    const ptrdiff_t diff = (char *)end - (char *)start;

    SLICE99_ASSERT(diff >= 0);
    SLICE99_ASSERT((size_t)diff % item_size == 0);

    return Slice99_new(start, item_size, (size_t)(diff / (ptrdiff_t)item_size));
}

/**
 * Constructs an empty slice.
 *
 * @param[in] item_size The value of Slice99#item_size.
 *
 * @pre `item_size > 0`
 */
inline static SLICE99_WARN_UNUSED_RESULT Slice99 Slice99_empty(size_t item_size) {
    SLICE99_ASSERT(item_size > 0);
    return Slice99_new("", item_size, 0);
}

/**
 * Updates @p self with the new length @p new_len.
 *
 * @param[in] self The slice whose length will be updated.
 * @param[in] new_len The new length.
 */
inline static SLICE99_WARN_UNUSED_RESULT Slice99 Slice99_update_len(Slice99 self, size_t new_len) {
    return Slice99_new(self.ptr, self.item_size, new_len);
}

/**
 * Checks whether @p self is empty or not.
 *
 * @param[in] self The checked slice.
 *
 * @return `true` if @p self is empty, otherwise `false`.
 */
inline static SLICE99_WARN_UNUSED_RESULT SLICE99_CONST bool Slice99_is_empty(Slice99 self) {
    return self.len == 0;
}

/**
 * Computes a total size in bytes.
 *
 * @param[in] self The slice whose size is to be computed.
 */
inline static SLICE99_WARN_UNUSED_RESULT SLICE99_CONST size_t Slice99_size(Slice99 self) {
    return self.item_size * self.len;
}

/**
 * Computes a pointer to the @p i -indexed item.
 *
 * @param[in] self The slice upon which the pointer will be computed.
 * @param[in] i The index of a desired item. Can be negative.
 *
 * @pre `self.item_size` must be representable as `ptrdiff_t`.
 */
inline static SLICE99_WARN_UNUSED_RESULT SLICE99_CONST void *
Slice99_get(Slice99 self, ptrdiff_t i) {
    return (void *)((char *)self.ptr + i * (ptrdiff_t)self.item_size);
}

/**
 * Computes a pointer to the first item.
 *
 * @param[in] self The slice upon which the pointer will be computed.
 */
inline static SLICE99_WARN_UNUSED_RESULT SLICE99_CONST void *Slice99_first(Slice99 self) {
    return Slice99_get(self, 0);
}

/**
 * Computes a pointer to the last item.
 *
 * @param[in] self The slice upon which the pointer will be computed.
 *
 * @pre `self.len` must be representable as `ptrdiff_t`.
 */
inline static SLICE99_WARN_UNUSED_RESULT SLICE99_CONST void *Slice99_last(Slice99 self) {
    return Slice99_get(self, (ptrdiff_t)self.len - 1);
}

/**
 * Subslicing @p self with [@p start_idx `..` @p end_idx].
 *
 * @param[in] self The original slice.
 * @param[in] start_idx The index at which a new slice will reside, inclusively.
 * @param[in] end_idx The index at which a new slice will end, exclusively.
 *
 * @return A slice with the aforementioned properties.
 *
 * @pre `start_idx <= end_idx`
 */
inline static SLICE99_WARN_UNUSED_RESULT Slice99
Slice99_sub(Slice99 self, ptrdiff_t start_idx, ptrdiff_t end_idx) {
    SLICE99_ASSERT(start_idx <= end_idx);

    return Slice99_from_ptrdiff(
        Slice99_get(self, start_idx), Slice99_get(self, end_idx), self.item_size);
}

/**
 * Advances @p self by @p offset items.
 *
 * @param[in] self The original slice.
 * @param[in] offset The number of items to advance. Can be negative.
 *
 * @return A slice advanced by @p offset items.
 *
 * @pre `offset <= (self).len`
 * @pre `self.len` must be representable as `ptrdiff_t`.
 */
inline static SLICE99_WARN_UNUSED_RESULT Slice99 Slice99_advance(Slice99 self, ptrdiff_t offset) {
    return Slice99_sub(self, offset, (ptrdiff_t)self.len);
}

/**
 * Performs a byte-by-byte comparison of @p lhs with @p rhs.
 *
 * @param[in] lhs The first slice to be compared.
 * @param[in] rhs The second slice to be compared.
 *
 * @return `true` if @p lhs and @p rhs are equal, `false` otherwise.
 */
inline static SLICE99_WARN_UNUSED_RESULT bool Slice99_primitive_eq(Slice99 lhs, Slice99 rhs) {
    return Slice99_size(lhs) != Slice99_size(rhs)
               ? false
               : SLICE99_MEMCMP(lhs.ptr, rhs.ptr, Slice99_size(lhs)) == 0;
}

/**
 * Performs a comparison of @p lhs with @p rhs with a user-supplied comparator.
 *
 * @param[in] lhs The first slice to be compared.
 * @param[in] rhs The second slice to be compared.
 * @param[in] cmp The function deciding whether two items are equal ot not (0 if equal, any other
 * value otherwise).
 *
 * @return `true` if @p lhs and @p rhs are equal, `false` otherwise.
 *
 * @pre `lhs.item_size == rhs.item_size`
 * @pre `cmp != NULL`
 * @pre `lhs.len` and `rhs.len` must be representable as `ptrdiff_t`.
 */
inline static SLICE99_WARN_UNUSED_RESULT bool
Slice99_eq(Slice99 lhs, Slice99 rhs, int (*cmp)(const void *, const void *)) {
    SLICE99_ASSERT(lhs.item_size == rhs.item_size);
    SLICE99_ASSERT(cmp);

    if (lhs.len != rhs.len) {
        return false;
    }

    for (ptrdiff_t i = 0; i < (ptrdiff_t)lhs.len; i++) {
        if (cmp(Slice99_get(lhs, i), Slice99_get(rhs, i)) != 0) {
            return false;
        }
    }

    return true;
}

/**
 * Checks whether @p prefix is a prefix of @p self, byte-by-byte.
 *
 * @param[in] self The slice to be checked for @p prefix.
 * @param[in] prefix The slice to be checked whether it is a prefix of @p self.
 *
 * @return `true` if @p prefix is a prefix of @p self, otherwise `false`.
 *
 * @pre `prefix.len` must be representable as `ptrdiff_t`.
 */
inline static SLICE99_WARN_UNUSED_RESULT bool
Slice99_primitive_starts_with(Slice99 self, Slice99 prefix) {
    return Slice99_size(self) < Slice99_size(prefix)
               ? false
               : Slice99_primitive_eq(Slice99_sub(self, 0, (ptrdiff_t)prefix.len), prefix);
}

/**
 * Checks whether @p prefix is a prefix of @p self with a user-supplied comparator.
 *
 * @param[in] self The slice to be checked for @p prefix.
 * @param[in] prefix The slice to be checked whether it is a prefix of @p self.
 * @param[in] cmp The function deciding whether two items are equal ot not (0 if equal, any other
 * value otherwise).
 *
 * @return `true` if @p prefix is a prefix of @p self, otherwise `false`.
 *
 * @pre `self.item_size == prefix.item_size`
 * @pre `cmp != NULL`
 * @pre `prefix.len` must be representable as `ptrdiff_t`.
 */
inline static SLICE99_WARN_UNUSED_RESULT bool
Slice99_starts_with(Slice99 self, Slice99 prefix, int (*cmp)(const void *, const void *)) {
    SLICE99_ASSERT(self.item_size == prefix.item_size);
    SLICE99_ASSERT(cmp);

    return self.len < prefix.len
               ? false
               : Slice99_eq(Slice99_sub(self, 0, (ptrdiff_t)prefix.len), prefix, cmp);
}

/**
 * Checks whether @p postfix is a postfix of @p self, byte-by-byte.
 *
 * @param[in] self The slice to be checked for @p postfix.
 * @param[in] postfix The slice to be checked whether it is a postfix of @p self.
 *
 * @return `true` if @p postfix is a postfix of @p self, otherwise `false`.
 *
 * @pre `self.len` and `postfix.len` must be representable as `ptrdiff_t`.
 */
inline static SLICE99_WARN_UNUSED_RESULT bool
Slice99_primitive_ends_with(Slice99 self, Slice99 postfix) {
    return Slice99_size(self) < Slice99_size(postfix)
               ? false
               : Slice99_primitive_eq(
                     Slice99_sub(
                         self, (ptrdiff_t)self.len - (ptrdiff_t)postfix.len, (ptrdiff_t)self.len),
                     postfix);
}

/**
 * Checks whether @p postfix is a postfix of @p self with a user-supplied comparator.
 *
 * @param[in] self The slice to be checked for @p postfix.
 * @param[in] postfix The slice to be checked whether it is a postfix of @p self.
 * @param[in] cmp The function deciding whether two items are equal ot not (0 if equal, any other
 * value otherwise).
 *
 * @return `true` if @p postfix is a postfix of @p self, otherwise `false`.
 *
 * @pre `self.item_size == postfix.item_size`
 * @pre `cmp != NULL`
 * @pre `self.len` and `postfix.len` must be representable as `ptrdiff_t`.
 */
inline static SLICE99_WARN_UNUSED_RESULT bool
Slice99_ends_with(Slice99 self, Slice99 postfix, int (*cmp)(const void *, const void *)) {
    SLICE99_ASSERT(self.item_size == postfix.item_size);
    SLICE99_ASSERT(cmp);

    return self.len < postfix.len
               ? false
               : Slice99_eq(
                     Slice99_sub(
                         self, (ptrdiff_t)self.len - (ptrdiff_t)postfix.len, (ptrdiff_t)self.len),
                     postfix, cmp);
}

/**
 * Copies @p other to the beginning of @p self, byte-by-byte.
 *
 * @param[out] self The location to which the whole @p other will be copied.
 * @param[in] other The slice to be copied to @p self.
 */
inline static void Slice99_copy(Slice99 self, Slice99 other) {
    SLICE99_MEMMOVE(self.ptr, other.ptr, Slice99_size(other));
}

/**
 * The same as #Slice99_copy except that @p self and @p other must be non-overlapping.
 *
 * @pre @p self and @p other must be non-overlapping.
 */
inline static void Slice99_copy_non_overlapping(Slice99 self, Slice99 other) {
    SLICE99_MEMCPY(self.ptr, other.ptr, Slice99_size(other));
}

/**
 * Swaps the @p lhs -indexed and @p rhs -indexed items.
 *
 * @param[out] self The slice in which @p lhs and @p rhs will be swapped.
 * @param[in] lhs The index of the first item.
 * @param[in] rhs The index of the second item.
 * @param[out] backup The memory area of `self.item_size` bytes accessible for reading and writing.
 *
 * @pre `backup != NULL`
 * @pre @p backup must not overlap with `Slice99_get(self, lhs)` and `Slice99_get(self, rhs)`.
 * @pre `Slice99_get(self, lhs)` and `Slice99_get(self, rhs)` must not overlap.
 */
inline static void Slice99_swap(Slice99 self, ptrdiff_t lhs, ptrdiff_t rhs, void *restrict backup) {
    SLICE99_ASSERT(backup);

    SLICE99_MEMCPY((backup), Slice99_get(self, lhs), (self).item_size);
    SLICE99_MEMCPY(Slice99_get(self, lhs), Slice99_get(self, rhs), (self).item_size);
    SLICE99_MEMCPY(Slice99_get(self, rhs), (backup), (self).item_size);
}

/**
 * Swaps all the items in @p self with those in @p other.
 *
 * @param[out] self The first slice to be swapped.
 * @param[out] other The second slice to be swapped.
 * @param[out] backup The memory area of `self.item_size` bytes accessible for reading and writing.
 *
 * @pre `self.len == other.len`
 * @pre `self.item_size == other.item_size`
 * @pre @p backup must not overlap with @p self and @p other.
 * @pre @p self and @p other must not overlap.
 * @pre `self.len` must be representable as `ptrdiff_t`.
 */
inline static void Slice99_swap_with_slice(Slice99 self, Slice99 other, void *restrict backup) {
    SLICE99_ASSERT(self.len == other.len);
    SLICE99_ASSERT(self.item_size == other.item_size);

    for (ptrdiff_t i = 0; i < (ptrdiff_t)self.len; i++) {
        SLICE99_MEMCPY(backup, Slice99_get(self, i), self.item_size);
        SLICE99_MEMCPY(Slice99_get(self, i), Slice99_get(other, i), self.item_size);
        SLICE99_MEMCPY(Slice99_get(other, i), backup, self.item_size);
    }
}

/**
 * Reverses the order of items in @p self.
 *
 * @param[out] self The slice to be reversed.
 * @param[out] backup The memory area of `self.item_size` bytes accessible for reading and writing.
 *
 * @pre `backup != NULL`
 * @pre `self.len` must be representable as `ptrdiff_t`.
 */
inline static void Slice99_reverse(Slice99 self, void *restrict backup) {
    SLICE99_ASSERT(backup);

    for (ptrdiff_t i = 0; i < (ptrdiff_t)self.len / 2; i++) {
        Slice99_swap(self, i, (ptrdiff_t)self.len - i - 1, backup);
    }
}

/**
 * Splits @p self into two parts.
 *
 * @param[in] self The slice to be splitted into @p lhs and @p rhs.
 * @param[in] i The index at which @p self will be splitted.
 * @param[out] lhs The first part of @p self indexed as [0; @p i).
 * @param[out] rhs The second part of @p self indexed as [@p i; `self.len`).
 *
 * @pre `i <= self.len`
 * @pre `lhs != NULL`
 * @pre `rhs != NULL`
 * @pre `self.len` and @p i must be representable as `ptrdiff_t`.
 */
inline static void
Slice99_split_at(Slice99 self, size_t i, Slice99 *restrict lhs, Slice99 *restrict rhs) {
    SLICE99_ASSERT(i <= self.len);
    SLICE99_ASSERT(lhs);
    SLICE99_ASSERT(rhs);

    *lhs = Slice99_sub(self, 0, (ptrdiff_t)i);
    *rhs = Slice99_sub(self, (ptrdiff_t)i, (ptrdiff_t)self.len);
}

/**
 * Copies @p self to @p out and appends '\0' to the end.
 *
 * @param[in] self The slice that will be copied.
 * @param[in] out The memory area to which @p self and the null character will be copied.
 *
 * @return The pointer @p out.
 *
 * @pre @p out must be capable of writing `Slice99_size(self) + 1` bytes.
 * @pre @p out must not overlap with @p self.
 */
inline static char *Slice99_c_str(Slice99 self, char out[restrict]) {
    SLICE99_ASSERT(out);

    SLICE99_MEMCPY(out, self.ptr, Slice99_size(self));
    out[Slice99_size(self)] = '\0';
    return out;
}

SLICE99_DEF_TYPED(CharSlice99, char);
SLICE99_DEF_TYPED(SCharSlice99, signed char);
SLICE99_DEF_TYPED(UCharSlice99, unsigned char);

SLICE99_DEF_TYPED(ShortSlice99, short);
SLICE99_DEF_TYPED(UShortSlice99, unsigned short);

SLICE99_DEF_TYPED(IntSlice99, int);
SLICE99_DEF_TYPED(UIntSlice99, unsigned int);

SLICE99_DEF_TYPED(LongSlice99, long);
SLICE99_DEF_TYPED(ULongSlice99, unsigned long);
SLICE99_DEF_TYPED(LongLongSlice99, long long);
SLICE99_DEF_TYPED(ULongLongSlice99, unsigned long long);

SLICE99_DEF_TYPED(FloatSlice99, float);
SLICE99_DEF_TYPED(DoubleSlice99, double);
SLICE99_DEF_TYPED(LongDoubleSlice99, long double);

SLICE99_DEF_TYPED(BoolSlice99, _Bool);

// Unsigned integers {
#ifdef UINT8_MAX
SLICE99_DEF_TYPED(U8Slice99, uint8_t);
#endif

#ifdef UINT16_MAX
SLICE99_DEF_TYPED(U16Slice99, uint16_t);
#endif

#ifdef UINT32_MAX
SLICE99_DEF_TYPED(U32Slice99, uint32_t);
#endif

#ifdef UINT64_MAX
SLICE99_DEF_TYPED(U64Slice99, uint64_t);
#endif
// } (Unsigned integers)

// Signed integers {
#ifdef INT8_MAX
SLICE99_DEF_TYPED(I8Slice99, int8_t);
#endif

#ifdef INT16_MAX
SLICE99_DEF_TYPED(I16Slice99, int16_t);
#endif

#ifdef INT32_MAX
SLICE99_DEF_TYPED(I32Slice99, int32_t);
#endif

#ifdef INT64_MAX
SLICE99_DEF_TYPED(I64Slice99, int64_t);
#endif
// } (Signed integers)

/**
 * The same as #Slice99_from_str.
 */
inline static SLICE99_WARN_UNUSED_RESULT CharSlice99 CharSlice99_from_str(char *str) {
    return (CharSlice99)SLICE99_TO_TYPED(Slice99_from_str(str));
}

/**
 * The same as #Slice99_c_str.
 */
inline static char *CharSlice99_c_str(CharSlice99 self, char out[restrict]) {
    return Slice99_c_str(SLICE99_TO_UNTYPED(self), out);
}

/**
 * Makes a null-terminated string out of `CharSlice99` using `alloca`.
 *
 * The same as #CharSlice99_c_str, except that the second parameter is allocated using `alloca`. Do
 * not use this macro for big strings to avoid stack overflow.
 */
#define CharSlice99_alloca_c_str(self) CharSlice99_c_str((self), alloca((self).len + 1))

#ifndef SLICE99_DISABLE_STDIO

#ifndef SLICE99_VSPRINTF
#include <stdio.h>
/// Like `vsprintf`. Defined only if it has not been defined previously **and**
/// `SLICE99_DISABLE_STDIO` is **not** defined.
#define SLICE99_VSPRINTF vsprintf
#endif

#ifndef SLICE99_VSNPRINTF
#include <stdio.h>
/// Like `vsnprintf`. Defined only if it has not been defined previously **and**
/// `SLICE99_DISABLE_STDIO` is **not** defined.
#define SLICE99_VSNPRINTF vsnprintf
#endif

#ifndef SLICE99_SNPRINTF
#include <stdio.h>
/// Like `snprintf`. Defined only if it has not been defined previously **and**
/// `SLICE99_DISABLE_STDIO` is **not** defined.
#define SLICE99_SNPRINTF snprintf
#endif

#ifndef DOXYGEN_IGNORE

// Doxygen fails to parse function declarations with function-like macro invocations.

#define SLICE99_FORMAT_HINT_2_0 SLICE99_FORMAT_HINT(printf, 2, 0)
#define SLICE99_FORMAT_HINT_2_3 SLICE99_FORMAT_HINT(printf, 2, 3)
#define SLICE99_FORMAT_HINT_3_0 SLICE99_FORMAT_HINT(printf, 3, 0)
#define SLICE99_FORMAT_HINT_3_4 SLICE99_FORMAT_HINT(printf, 3, 4)

#endif // DOXYGEN_IGNORE

/**
 * Prints a formatted string to @p out and returns the corresponding character slice.
 *
 * Defined only if `SLICE99_DISABLE_STDIO` is **not** defined.
 *
 * @param[out] out The buffer that must be capable of holding all characters that will be written by
 * #SLICE99_VSPRINTF.
 * @param[in] fmt The `printf`-like format string.
 * @param[in] list The variadic function arguments reified into `va_list`.
 *
 * @return A character slice constructed from @p out.
 *
 * @pre `out != NULL`
 * @pre `fmt != NULL`
 */
inline static SLICE99_WARN_UNUSED_RESULT SLICE99_FORMAT_HINT_2_0 CharSlice99
CharSlice99_vfmt(char out[restrict], const char *restrict fmt, va_list list) {
    SLICE99_ASSERT(out);
    SLICE99_ASSERT(fmt);

    SLICE99_VSPRINTF(out, fmt, list);
    return CharSlice99_from_str(out);
}

/**
 * The #CharSlice99_vfmt twin.
 *
 * Defined only if `SLICE99_DISABLE_STDIO` is **not** defined.
 */
inline static SLICE99_WARN_UNUSED_RESULT SLICE99_FORMAT_HINT_2_3 CharSlice99
CharSlice99_fmt(char out[restrict], const char *restrict fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const CharSlice99 result = CharSlice99_vfmt(out, fmt, ap);
    va_end(ap);
    return result;
}

/**
 * The same as #CharSlice99_vfmt but writes at most `bufsz - 1` characters.
 *
 * Defined only if `SLICE99_DISABLE_STDIO` is **not** defined.
 */
inline static SLICE99_WARN_UNUSED_RESULT SLICE99_FORMAT_HINT_3_0 CharSlice99
CharSlice99_vnfmt(char out[restrict], size_t bufsz, const char *restrict fmt, va_list list) {
    SLICE99_VSNPRINTF(out, bufsz, fmt, list);
    return CharSlice99_from_str(out);
}

/**
 * The #CharSlice99_vnfmt twin.
 *
 * Defined only if `SLICE99_DISABLE_STDIO` is **not** defined.
 */
inline static SLICE99_WARN_UNUSED_RESULT SLICE99_FORMAT_HINT_3_4 CharSlice99
CharSlice99_nfmt(char out[restrict], size_t bufsz, const char *restrict fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    const CharSlice99 result = CharSlice99_vnfmt(out, bufsz, fmt, ap);
    va_end(ap);
    return result;
}

#ifndef DOXYGEN_IGNORE

#undef SLICE99_FORMAT_HINT_2_0
#undef SLICE99_FORMAT_HINT_2_3
#undef SLICE99_FORMAT_HINT_3_0
#undef SLICE99_FORMAT_HINT_3_4

#endif // DOXYGEN_IGNORE

/**
 * Printfs a formatted string to a character slice using `alloca`.
 *
 * The same as #CharSlice99_fmt, except that the first parameter is allocated using `alloca`. Do not
 * use this macro for big strings to avoid stack overflow.
 *
 * Defined only if `SLICE99_DISABLE_STDIO` is **not** defined.
 */
#define CharSlice99_alloca_fmt(fmt, ...)                                                           \
    CharSlice99_fmt(alloca(SLICE99_SNPRINTF(NULL, 0, (fmt), __VA_ARGS__) + 1), (fmt), __VA_ARGS__)

#endif // SLICE99_DISABLE_STDIO

#endif // SLICE99_H
