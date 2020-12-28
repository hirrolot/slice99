/**
 * @file
 * A slice of some array.
 *
 * The macros #SLICE99_ASSERT, #SLICE99_MEMCMP, #SLICE99_MEMCPY, #SLICE99_STRLEN, #SLICE99_QSORT,
 * and #SLICE99_BSEARCH# are automatically defined in case they have not been defined before
 * including this header file. They represent the corresponding standard library's functions, though
 * actual implementations can differ. If you develop software for a freestanding environment, these
 * macros might need to be defined beforehand. Note that #SLICE99_QSORT and #SLICE99_BSEARCH might
 * need to be provided only if you define `SLICE99_INCLUDE_SORT` and `SLICE99_INCLUDE_BSEARCH`,
 * respectively.
 */

/**
 * @mainpage
 *
 * This library provides <a href="https://en.wikipedia.org/wiki/Array_slicing">array slicing</a>
 * facilities for pure C99.<br>
 *
 * All the documentation is located in slice99.h. See <a
 * href="https://github.com/Hirrolot/slice99">our official GitHub repository</a> for a high-level
 * overview.
 */

#ifndef SLICE99_H
#define SLICE99_H

#include <stdbool.h>
#include <stddef.h>

#ifndef SLICE99_ASSERT
#include <assert.h>
/// Like `assert.
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

#ifndef SLICE99_STRLEN
#include <string.h>
/// Like `strlen`.
#define SLICE99_STRLEN strlen
#endif

#ifdef SLICE99_INCLUDE_SORT

#ifndef SLICE99_QSORT
#include <stdlib.h>
/// Like `qsort`.
#define SLICE99_QSORT qsort
#endif

#endif // SLICE99_INCLUDE_SORT

#ifdef SLICE99_INCLUDE_BSEARCH

#ifndef SLICE99_BSEARCH
#include <stdlib.h>
/// Like `bsearch`.
#define SLICE99_BSEARCH bsearch
#endif

#endif // SLICE99_INCLUDE_BSEARCH

/**
 * Computes a number of items in an array expression.
 */
#define Slice99_array_len(...) (sizeof(__VA_ARGS__) / sizeof((__VA_ARGS__)[0]))

/**
 * Constructs a slice from an array expression.
 */
#define Slice99_from_array(...)                                                                    \
    Slice99_new((void *)(__VA_ARGS__), sizeof((__VA_ARGS__)[0]), Slice99_array_len(__VA_ARGS__))

/**
 * A slice of some array.
 *
 * This structure should not be constructed manually; use #Slice99_new instead.
 *
 * @invariant #ptr shall not be `NULL`.
 * @invariant #item_size shall be strictly greater than 0.
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
inline static Slice99 Slice99_new(void *ptr, size_t item_size, size_t len) {
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
inline static Slice99 Slice99_from_str(char *str) {
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
inline static Slice99 Slice99_from_ptrdiff(void *start, void *end, size_t item_size) {
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
inline static Slice99 Slice99_empty(size_t item_size) {
    SLICE99_ASSERT(item_size > 0);
    return Slice99_new("", item_size, 0);
}

/**
 * Checks whether @p self is empty or not.
 *
 * @param[in] self The checked slice.
 *
 * @return `true` if @p self is empty, otherwise `false`.
 */
inline static bool Slice99_is_empty(Slice99 self) {
    return self.len == 0;
}

/**
 * Computes a total size in bytes.
 *
 * @param[in] self The slice whose size is to be computed.
 */
inline static size_t Slice99_size(Slice99 self) {
    return self.item_size * self.len;
}

/**
 * Computes a pointer to the @p i -indexed item.
 *
 * @param[in] self The slice upon which the pointer will be computed.
 * @param[in] i The index of a desired item.
 *
 * @note This subroutine is implemented as a macro due to the absence of a signed `size_t`
 * counterpart (i.e. the index can be negative).
 */
#define Slice99_get(self, i) Slice99_get_cast_type(self, i, size_t)

/**
 * The same as #Slice99_get but explicitly casts `(self).item_size` to @p T.
 */
#define Slice99_get_cast_type(self, i, T)                                                          \
    ((void *)((char *)(self).ptr + ((i) * (T)(self).item_size)))

/**
 * Computes a pointer to the first item.
 *
 * @param[in] self The slice upon which the pointer will be computed.
 */
inline static void *Slice99_first(Slice99 self) {
    return Slice99_get(self, 0);
}

/**
 * Computes a pointer to the last item.
 *
 * @param[in] self The slice upon which the pointer will be computed.
 */
inline static void *Slice99_last(Slice99 self) {
    return Slice99_get(self, self.len - 1);
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
 * @note This subroutine is implemented as a macro due to the absence of a signed `size_t`
 * counterpart (i.e. the indices can be negative).
 *
 * @pre `start_idx <= end_idx`
 */
#define Slice99_sub(self, start_idx, end_idx)                                                      \
    Slice99_sub_cast_type(self, start_idx, end_idx, size_t)

/**
 * The same as #Slice99_sub but explicitly casts `(self).item_size` to @p T.
 */
#define Slice99_sub_cast_type(self, start_idx, end_idx, T)                                         \
    (SLICE99_ASSERT((start_idx) <= (end_idx)),                                                     \
     Slice99_from_ptrdiff(                                                                         \
         Slice99_get_cast_type((self), (start_idx), T),                                            \
         Slice99_get_cast_type((self), (end_idx), T), (self).item_size))

/**
 * Performs a byte-by-byte comparison of @p lhs with @p rhs.
 *
 * @param[in] lhs The first slice to be compared.
 * @param[in] rhs The second slice to be compared.
 *
 * @return `true` if @p lhs and @p rhs are equal, `false` otherwise.
 */
inline static bool Slice99_primitive_eq(Slice99 lhs, Slice99 rhs) {
    return Slice99_size(lhs) != Slice99_size(rhs)
               ? false
               : SLICE99_MEMCMP(lhs.ptr, rhs.ptr, Slice99_size(lhs)) == 0;
}

/**
 * Performs a comparison of @p lhs with @p rhs with a user-supplied comparator.
 *
 * @param[in] lhs The first slice to be compared.
 * @param[in] rhs The second slice to be compared.
 * @param[in] comparator A function deciding whether two items are equal ot not (0 if equal, any
 * other value otherwise).
 *
 * @return `true` if @p lhs and @p rhs are equal, `false` otherwise.
 *
 * @pre `lhs.item_size == rhs.item_size`
 * @pre `comparator != NULL`
 */
inline static bool
Slice99_eq(Slice99 lhs, Slice99 rhs, int (*comparator)(const void *, const void *)) {
    SLICE99_ASSERT(lhs.item_size == rhs.item_size);
    SLICE99_ASSERT(comparator);

    if (lhs.len != rhs.len) {
        return false;
    }

    for (size_t i = 0; i < lhs.len; i++) {
        if (comparator(Slice99_get(lhs, i), Slice99_get(rhs, i)) != 0) {
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
 */
inline static bool Slice99_primitive_starts_with(Slice99 self, Slice99 prefix) {
    return Slice99_size(self) < Slice99_size(prefix)
               ? false
               : Slice99_primitive_eq(Slice99_sub(self, 0, prefix.len), prefix);
}

/**
 * Checks whether @p prefix is a prefix of @p self with a user-supplied comparator.
 *
 * @param[in] self The slice to be checked for @p prefix.
 * @param[in] prefix The slice to be checked whether it is a prefix of @p self.
 * @param[in] comparator A function deciding whether two items are equal ot not (0 if equal, any
 * other value otherwise).
 *
 * @return `true` if @p prefix is a prefix of @p self, otherwise `false`.
 *
 * @pre `self.item_size == prefix.item_size`
 * @pre `comparator != NULL`
 */
inline static bool
Slice99_starts_with(Slice99 self, Slice99 prefix, int (*comparator)(const void *, const void *)) {
    SLICE99_ASSERT(self.item_size == prefix.item_size);
    SLICE99_ASSERT(comparator);

    return self.len < prefix.len ? false
                                 : Slice99_eq(Slice99_sub(self, 0, prefix.len), prefix, comparator);
}

/**
 * Checks whether @p postfix is a postfix of @p self, byte-by-byte.
 *
 * @param[in] self The slice to be checked for @p postfix.
 * @param[in] postfix The slice to be checked whether it is a postfix of @p self.
 *
 * @return `true` if @p postfix is a postfix of @p self, otherwise `false`.
 */
inline static bool Slice99_primitive_ends_with(Slice99 self, Slice99 postfix) {
    return Slice99_size(self) < Slice99_size(postfix)
               ? false
               : Slice99_primitive_eq(
                     Slice99_sub(self, (self.len - postfix.len), self.len), postfix);
}

/**
 * Checks whether @p postfix is a postfix of @p self with a user-supplied comparator.
 *
 * @param[in] self The slice to be checked for @p postfix.
 * @param[in] postfix The slice to be checked whether it is a postfix of @p self.
 * @param[in] comparator A function deciding whether two items are equal ot not (0 if equal, any
 * other value otherwise).
 *
 * @return `true` if @p postfix is a postfix of @p self, otherwise `false`.
 *
 * @pre `self.item_size == postfix.item_size`
 * @pre `comparator != NULL`
 */
inline static bool
Slice99_ends_with(Slice99 self, Slice99 postfix, int (*comparator)(const void *, const void *)) {
    SLICE99_ASSERT(self.item_size == postfix.item_size);
    SLICE99_ASSERT(comparator);

    return self.len < postfix.len
               ? false
               : Slice99_eq(
                     Slice99_sub(self, self.len - postfix.len, self.len), postfix, comparator);
}

/**
 * Copies @p src to the beginning of @p dst, byte-by-byte.
 *
 * @param[out] dst The location to which the whole @p src will be copied.
 * @param[in] src The slice to be copied to @p dst.
 */
inline static void Slice99_copy(Slice99 dst, Slice99 src) {
    SLICE99_MEMCPY(dst.ptr, src.ptr, Slice99_size(src));
}

#ifdef SLICE99_INCLUDE_IO

#include <stdio.h>

/**
 * Writes @p self to @p stream, byte-by-byte.
 *
 * @param[in] self The slice to be written.
 * @param[out] stream The stream to which @p self will be written.
 *
 * @note Included only if `SLICE99_INCLUDE_IO` is defined.
 */
inline static void Slice99_fwrite(Slice99 self, FILE *stream) {
    fwrite(self.ptr, self.item_size, self.len, stream);
}

/**
 * Writes @p self to `stdout`, byte-by-byte.
 *
 * @param[in] self The slice to be written.
 *
 * @note Included only if `SLICE99_INCLUDE_IO` is defined.
 */
inline static void Slice99_write(Slice99 self) {
    Slice99_fwrite(self, stdout);
}

/**
 * The same as #Slice99_fwrite but places a new line character afterwards.
 *
 * @param[in] self The slice to be written.
 * @param[out] stream The stream to which @p self will be written.
 *
 * @note Included only if `SLICE99_INCLUDE_IO` is defined.
 */
inline static void Slice99_fwrite_ln(Slice99 self, FILE *stream) {
    Slice99_fwrite(self, stream);
    fprintf(stream, "\n");
}

/**
 * The same as #Slice99_write but places a new line character afterwards.
 *
 * @param[in] self The slice to be written.
 *
 * @note Included only if `SLICE99_INCLUDE_IO` is defined.
 */
inline static void Slice99_write_ln(Slice99 self) {
    Slice99_write(self);
    puts("");
}

#endif // SLICE99_INCLUDE_IO

#ifdef SLICE99_INCLUDE_SORT

/**
 * Sorts the items in @p self.
 *
 * @param[out] self The slice whose items will be sorted.
 * @param[in] comparator A function deciding whether two items are equal ot not (0 if equal, any
 * other value otherwise).
 *
 * @note Included only if `SLICE99_INCLUDE_SORT` is defined.
 *
 * @pre `comparator != NULL`
 */
inline static void Slice99_sort(Slice99 self, int (*comparator)(const void *, const void *)) {
    SLICE99_ASSERT(comparator);

    SLICE99_QSORT(self.ptr, self.len, self.item_size, comparator);
}

#endif // SLICE99_INCLUDE_SORT

#ifdef SLICE99_INCLUDE_BSEARCH

/**
 * Performs a binary search in @p self.
 *
 * @param[in] self The slice in which the binary search will be performed.
 * @param[in] key The value to search for.
 * @param[in] comparator A function returning the order of two items (0 if equal, <0 if the first
 * item is lesser than the second, >0 if the first item is greater than the second).
 *
 * @return A pointer to an element in @p self that compares equal to @p key. If it does not exist,
 * `NULL`.
 *
 * @note Included only if `SLICE99_INCLUDE_BSEARCH` is defined.
 *
 * @pre `comparator != NULL`
 */
inline static void *
Slice99_bsearch(Slice99 self, const void *key, int (*comparator)(const void *, const void *)) {
    SLICE99_ASSERT(comparator);

    return SLICE99_BSEARCH(key, self.ptr, self.len, self.item_size, comparator);
}

#endif // SLICE99_INCLUDE_BSEARCH

/**
 * Swaps the @p lhs -indexed and @p rhs -indexed items.
 *
 * @param[out] self The slice in which @p lhs and @p rhs will be swapped.
 * @param[in] lhs The index of the first item.
 * @param[in] rhs The index of the second item.
 * @param[out] temp The memory area of `self.item_size` bytes accessible for reading and writing.
 *
 * @note This subroutine is implemented as a macro due to the absence of a signed `size_t`
 * counterpart (i.e. the indices can be negative).
 *
 * @pre `temp != NULL`
 */
#define Slice99_swap(self, lhs, rhs, temp)                                                         \
    do {                                                                                           \
        SLICE99_ASSERT(temp);                                                                      \
                                                                                                   \
        SLICE99_MEMCPY((temp), Slice99_get(self, lhs), (self).item_size);                          \
        SLICE99_MEMCPY(Slice99_get(self, lhs), Slice99_get(self, rhs), (self).item_size);          \
        SLICE99_MEMCPY(Slice99_get(self, rhs), (temp), (self).item_size);                          \
    } while (0)

/**
 * Swaps all the items in @p self with those in @p other.
 *
 * @param[out] self The first slice to be swapped.
 * @param[out] other The second slice to be swapped.
 * @param[out] temp The memory area of `self.item_size` bytes accessible for reading and writing.
 *
 * @pre `self.len == other.len`
 * @pre `self.item_size == other.item_size`
 */
inline static void Slice99_swap_with_slice(Slice99 self, Slice99 other, void *restrict temp) {
    SLICE99_ASSERT(self.len == other.len);
    SLICE99_ASSERT(self.item_size == other.item_size);

    for (size_t i = 0; i < self.len; i++) {
        SLICE99_MEMCPY(temp, Slice99_get(self, i), self.item_size);
        SLICE99_MEMCPY(Slice99_get(self, i), Slice99_get(other, i), self.item_size);
        SLICE99_MEMCPY(Slice99_get(other, i), temp, self.item_size);
    }
}

/**
 * Reverses the order of items in @p self.
 *
 * @param[out] self The slice to be reversed.
 * @param[out] temp The memory area of `self.item_size` bytes accessible for reading and writing.
 *
 * @pre `temp != NULL`
 */
inline static void Slice99_reverse(Slice99 self, void *restrict temp) {
    SLICE99_ASSERT(temp);

    for (size_t i = 0; i < self.len / 2; i++) {
        Slice99_swap(self, i, self.len - i - 1, temp);
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
 */
inline static void
Slice99_split_at(Slice99 self, size_t i, Slice99 *restrict lhs, Slice99 *restrict rhs) {
    SLICE99_ASSERT(i <= self.len);
    SLICE99_ASSERT(lhs);
    SLICE99_ASSERT(rhs);

    *lhs = Slice99_sub(self, 0, i);
    *rhs = Slice99_sub(self, i, self.len);
}

#endif // SLICE99_H
