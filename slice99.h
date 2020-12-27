/**
 * @file
 * A slice of an array.
 */

#ifndef SLICE99_H
#define SLICE99_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * A slice of an array.
 *
 * This structure should not be constructed manually; use #Slice99_new instead.
 *
 * @invariant #ptr shall not be `NULL`.
 * @invariant #item_size shall be greater than 0.
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
 * @pre `ptr != NULL`
 * @pre `item_size > 0`
 */
inline static Slice99 Slice99_new(void *ptr, size_t item_size, size_t len) {
    assert(ptr);
    assert(item_size > 0);

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
    assert(str);

    return Slice99_new(str, sizeof(char), strlen(str));
}

/**
 * Constructs a slice residing between @p start (inclusively) and @p end (exclusively).
 *
 * @param[in] start
 * @param[in] end
 * @param[in] item_size
 *
 * @pre `start != NULL`
 * @pre `end != NULL`
 * @pre `((char *)end - (char *)start) >= 0`
 * @pre `(((char *)end - (char *)start)) % item_size == 0`
 */
inline static Slice99 Slice99_from_ptrdiff(void *start, void *end, size_t item_size) {
    assert(start);
    assert(end);

    const ptrdiff_t diff = (char *)end - (char *)start;

    assert(diff >= 0);
    assert((size_t)diff % item_size == 0);

    return Slice99_new(start, item_size, (size_t)(diff / (ptrdiff_t)item_size));
}

/**
 * Constructs an empty slice.
 *
 * @param[in] item_size
 *
 * @pre `item_size > 0`
 */
inline static Slice99 Slice99_empty(size_t item_size) {
    assert(item_size > 0);
    return Slice99_new("", item_size, 0);
}

/**
 * Checks whether @p self is empty or not.
 *
 * @param[in] self
 *
 * @return `true` if @p self is empty, otherwise `false`.
 */
inline static bool Slice99_is_empty(Slice99 self) {
    return self.len == 0;
}

/**
 * Computes a total size in bytes.
 *
 * @param[in] self
 */
inline static size_t Slice99_size(Slice99 self) {
    return self.item_size * self.len;
}

/**
 * Computes a pointer to the @p i -indexed item.
 *
 * @param[in] self
 * @param[in] i The index of a desired item.
 */
inline static void *Slice99_get(Slice99 self, int i) {
    return (char *)self.ptr + (i * (int)self.item_size);
}

/**
 * Computes a pointer to the first item.
 *
 * @param[in] self
 */
inline static void *Slice99_first(Slice99 self) {
    return Slice99_get(self, 0);
}

/**
 * Computes a pointer to the last item.
 *
 * @param[in] self
 */
inline static void *Slice99_last(Slice99 self) {
    return Slice99_get(self, (int)(self.len - 1));
}

/**
 * Indexing @p self with [@p start_idx `..` @p end_idx].
 *
 * @param[in] self The original slice.
 * @param[in] start_idx The index at which a new slice will reside, inclusively.
 * @param[in] end_idx The index at which a new slice will end, exclusively.
 *
 * @return A slice with the aforementioned properties.
 *
 * @pre `start_idx <= end_idx`
 */
inline static Slice99 Slice99_idx(Slice99 self, int start_idx, int end_idx) {
    assert(start_idx <= end_idx);

    return Slice99_from_ptrdiff(
        Slice99_get(self, start_idx), Slice99_get(self, end_idx), self.item_size);
}

/**
 * Performs a byte-by-byte comparison of @p lhs with @p rhs.
 *
 * @param[in] lhs
 * @param[in] rhs
 *
 * @return `true` if @p lhs and @p rhs are equal, `false` otherwise.
 */
inline static bool Slice99_primitive_eq(Slice99 lhs, Slice99 rhs) {
    return Slice99_size(lhs) != Slice99_size(rhs)
               ? false
               : memcmp(lhs.ptr, rhs.ptr, Slice99_size(lhs)) == 0;
}

/**
 * Performs a comparison of @p lhs with @p rhs with a user-supplied comparator.
 *
 * @param[in] lhs
 * @param[in] rhs
 * @param[in] comparator
 *
 * @return `true` if @p lhs and @p rhs are equal, `false` otherwise.
 *
 * @pre `lhs.item_size == rhs.item_size`
 * @pre `comparator != NULL`
 */
inline static bool
Slice99_eq(Slice99 lhs, Slice99 rhs, int (*comparator)(const void *, const void *)) {
    assert(lhs.item_size == rhs.item_size);
    assert(comparator);

    if (lhs.len != rhs.len) {
        return false;
    }

    for (size_t i = 0; i < lhs.len; i++) {
        if (comparator(Slice99_get(lhs, (int)i), Slice99_get(rhs, (int)i)) != 0) {
            return false;
        }
    }

    return true;
}

/**
 * Checks whether @p prefix is a prefix of @p self, byte-by-byte.
 *
 * @param[in] self
 * @param[in] prefix
 *
 * @return `true` if @p prefix is a prefix of @p self, otherwise `false`.
 */
inline static bool Slice99_primitive_starts_with(Slice99 self, Slice99 prefix) {
    return Slice99_size(self) < Slice99_size(prefix)
               ? false
               : Slice99_primitive_eq(Slice99_idx(self, 0, (int)prefix.len), prefix);
}

/**
 * Checks whether @p prefix is a prefix of @p self with a user-supplied comparator.
 *
 * @param[in] self
 * @param[in] prefix
 * @param[in] comparator
 *
 * @return `true` if @p prefix is a prefix of @p self, otherwise `false`.
 *
 * @pre `self.item_size == prefix.item_size`
 * @pre `comparator != NULL`
 */
inline static bool
Slice99_starts_with(Slice99 self, Slice99 prefix, int (*comparator)(const void *, const void *)) {
    assert(self.item_size == prefix.item_size);
    assert(comparator);

    return self.len < prefix.len
               ? false
               : Slice99_eq(Slice99_idx(self, 0, (int)prefix.len), prefix, comparator);
}

/**
 * Checks whether @p postfix is a postfix of @p self, byte-by-byte.
 *
 * @param[in] self
 * @param[in] postfix
 *
 * @return `true` if @p postfix is a postfix of @p self, otherwise `false`.
 */
inline static bool Slice99_primitive_ends_with(Slice99 self, Slice99 postfix) {
    return Slice99_size(self) < Slice99_size(postfix)
               ? false
               : Slice99_primitive_eq(
                     Slice99_idx(self, (int)(self.len - postfix.len), (int)self.len), postfix);
}

/**
 * Checks whether @p postfix is a postfix of @p self with a user-supplied comparator.
 *
 * @param[in] self
 * @param[in] postfix
 * @param[in] comparator
 *
 * @return `true` if @p postfix is a postfix of @p self, otherwise `false`.
 *
 * @pre `self.item_size == postfix.item_size`
 * @pre `comparator != NULL`
 */
inline static bool
Slice99_ends_with(Slice99 self, Slice99 postfix, int (*comparator)(const void *, const void *)) {
    assert(self.item_size == postfix.item_size);
    assert(comparator);

    return self.len < postfix.len
               ? false
               : Slice99_eq(
                     Slice99_idx(self, (int)(self.len - postfix.len), (int)self.len), postfix,
                     comparator);
}

/**
 * Copies @p src to the beginning of @p dst, byte-by-byte.
 *
 * @param[out] dst
 * @param[in] src
 */
inline static void Slice99_copy(Slice99 dst, Slice99 src) {
    memcpy(dst.ptr, src.ptr, Slice99_size(src));
}

/**
 * Prints @p self to @p stream.
 *
 * @param[in] self
 * @param[out] stream
 */
inline static void Slice99_print_to_file(Slice99 self, FILE *stream) {
    fwrite(self.ptr, self.item_size, self.len, stream);
}

/**
 * Prints @p self to `stdout`.
 *
 * @param[in] self
 */
inline static void Slice99_print(Slice99 self) {
    Slice99_print_to_file(self, stdout);
}

/**
 * The same as #Slice99_print_to_file but places a new line character afterwards.
 *
 * @param[in] self
 * @param[out] stream
 */
inline static void Slice99_print_to_file_ln(Slice99 self, FILE *stream) {
    Slice99_print_to_file(self, stream);
    fprintf(stream, "\n");
}

/**
 * The same as #Slice99_print but places a new line character afterwards.
 *
 * @param[in] self
 */
inline static void Slice99_print_ln(Slice99 self) {
    Slice99_print(self);
    puts("");
}

/**
 * Sorts the items in @p self.
 *
 * @param[out] self
 * @param[in] comparator
 *
 * @pre `comparator != NULL`
 */
inline static void Slice99_sort(Slice99 self, int (*comparator)(const void *, const void *)) {
    assert(comparator);

    qsort(self.ptr, self.len, self.item_size, comparator);
}

/**
 * Performs a binary search in @p self.
 *
 * @param[in] self
 * @param[in] key
 * @param[in] comparator
 *
 * @return A pointer to an element in @p self that compares equal to @p key. If it does not exist,
 * `NULL`.
 *
 * @pre `comparator != NULL`
 */
inline static void *
Slice99_bsearch(Slice99 self, const void *key, int (*comparator)(const void *, const void *)) {
    assert(comparator);

    return bsearch(key, self.ptr, self.len, self.item_size, comparator);
}

/**
 * Swaps the @p lhs -indexed and @p rhs -indexed items.
 *
 * @param[out] self
 * @param[in] lhs The index of the first item.
 * @param[in] rhs The index of the second item.
 * @param[out] temp The memory area of `self.item_size` bytes accessible for reading and writing.
 *
 * @pre `temp != NULL`
 */
inline static void Slice99_swap(Slice99 self, int lhs, int rhs, void *restrict temp) {
    assert(temp);

    memcpy(temp, Slice99_get(self, lhs), self.item_size);
    memcpy(Slice99_get(self, lhs), Slice99_get(self, rhs), self.item_size);
    memcpy(Slice99_get(self, rhs), temp, self.item_size);
}

/**
 * Swaps all the items in @p self with those in @p other.
 *
 * @param[out] self
 * @param[out] other
 * @param[out] temp The memory area of `self.item_size` bytes accessible for reading and writing.
 *
 * @pre `self.len == other.len`
 * @pre `self.item_size == other.item_size`
 */
inline static void Slice99_swap_with_slice(Slice99 self, Slice99 other, void *restrict temp) {
    assert(self.len == other.len);
    assert(self.item_size == other.item_size);

    for (size_t i = 0; i < self.len; i++) {
        memcpy(temp, Slice99_get(self, (int)i), self.item_size);
        memcpy(Slice99_get(self, (int)i), Slice99_get(other, (int)i), self.item_size);
        memcpy(Slice99_get(other, (int)i), temp, self.item_size);
    }
}

/**
 * Reverses the order of items in @p self.
 *
 * @param[out] self
 * @param[out] temp The memory area of `self.item_size` bytes accessible for reading and writing.
 *
 * @pre `temp != NULL`
 */
inline static void Slice99_reverse(Slice99 self, void *restrict temp) {
    assert(temp);

    for (size_t i = 0; i < self.len / 2; i++) {
        Slice99_swap(self, (int)i, (int)(self.len - i - 1), temp);
    }
}

/**
 * Splits @p self into two parts.
 *
 * @param[in] self
 * @param[out] lhs The first part of @p self indexed as [0; @p i).
 * @param[out] rhs The second part of @p self indexed as [@p i; `self.len`).
 *
 * @pre `i <= self.len`
 * @pre `lhs != NULL`
 * @pre `rhs != NULL`
 */
inline static void
Slice99_split_at(Slice99 self, size_t i, Slice99 *restrict lhs, Slice99 *restrict rhs) {
    assert(i <= self.len);
    assert(lhs);
    assert(rhs);

    *lhs = Slice99_idx(self, 0, (int)i);
    *rhs = Slice99_idx(self, (int)i, (int)self.len);
}

#endif // SLICE99_H
