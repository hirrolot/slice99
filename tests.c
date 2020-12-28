#include <slice99.h>

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define TEST(name) static void test_##name(void)

// A small eDSL for asserting a slice {
#define ASSERT_SLICE(slice, expected_ptr, expected_item_size, expected_len)                        \
    do {                                                                                           \
        Slice99 slice_ = (slice);                                                                  \
        assert(slice_.ptr == (expected_ptr));                                                      \
        assert(slice_.item_size == (expected_item_size));                                          \
        assert(slice_.len == (expected_len));                                                      \
                                                                                                   \
    } while (0)

#define PTR
#define ITEM_SIZE
#define LEN
// }

// Property-based testing {

// Algebraic stuff {
#define ASSERT_IMPLICATION(cond_1, cond_2)                                                         \
    if (cond_1) {                                                                                  \
        assert(cond_2);                                                                            \
    }                                                                                              \
                                                                                                   \
    do {                                                                                           \
    } while (0)

#define ASSERT_BIDIRECTIONAL_IMPLICATION(lhs, rhs)                                                 \
    ASSERT_IMPLICATION(lhs, rhs);                                                                  \
    ASSERT_IMPLICATION(rhs, lhs);                                                                  \
                                                                                                   \
    do {                                                                                           \
    } while (0)

#define ASSERT_REFLEXIVITY(op, expr) assert(op(expr, expr))

#define ASSERT_SYMMETRICITY(op, lhs, rhs)                                                          \
    ASSERT_BIDIRECTIONAL_IMPLICATION(op(lhs, rhs), op(rhs, lhs))

#define ASSERT_TRANSITIVITY(op, expr_1, expr_2, expr_3)                                            \
    ASSERT_IMPLICATION(op(expr_1, expr_2) && op(expr_2, expr_3), op(expr_1, expr_2))

#define TEST_IS_EQUIVALENCE(name)                                                                  \
    TEST(name##_is_equivalence) {                                                                  \
        test_##name##_is_reflexive();                                                              \
        test_##name##_is_symmetric();                                                              \
        test_##name##_is_transitive();                                                             \
    }                                                                                              \
                                                                                                   \
    _Static_assert(true, "")

#define ASSERT_INVOLUTION(op, eq, expr) assert(eq(op(op(expr)), expr))
// }

// Test cases generation {
static int rand_range(int min, int max) {
    return min + rand() % (max - min + 1);
}

#define GEN_ARRAY(array, len, max_val)                                                             \
    for (size_t l = 0; l < len; l++) {                                                             \
        (array)[l] = rand_range(0, (int)max_val);                                                  \
    }                                                                                              \
                                                                                                   \
    do {                                                                                           \
    } while (0)

#define GEN_INT_ARRAY_SLICES(slices, count)                                                        \
    Slice99 slices[count];                                                                         \
                                                                                                   \
    do {                                                                                           \
        int array[50][count];                                                                      \
        size_t len[count];                                                                         \
                                                                                                   \
        for (size_t k = 0; k < count; k++) {                                                       \
            len[k] = (size_t)rand_range(0, (int)Slice99_array_len(array[0]));                      \
            GEN_ARRAY(array[k], len[k], INT_MAX);                                                  \
            slices[k] = Slice99_new(array[k], sizeof(array[0][0]), len[k]);                        \
        }                                                                                          \
    } while (0)
// }

// } (Property-based testing)

// Auxiliary functions {
static int int_comparator(const void *lhs, const void *rhs) {
    return *(const int *)lhs - *(const int *)rhs;
}

static bool slice_int_eq(Slice99 lhs, Slice99 rhs) {
    return Slice99_eq(lhs, rhs, int_comparator);
}
// }

TEST(from_str) {
    // clang-format off
    const char *str = "abc";
    ASSERT_SLICE(
        Slice99_from_str("abc"),
        PTR str,
        ITEM_SIZE sizeof(char),
        LEN strlen(str)
    );

    str = "";
    ASSERT_SLICE(
        Slice99_from_str(""),
        PTR str,
        ITEM_SIZE sizeof(char),
        LEN strlen(str)
    );
    // clang-format on
}

TEST(from_ptrdiff) {
    int data[] = {1, 2, 3, 4, 5};

    // clang-format off
    ASSERT_SLICE(
        Slice99_from_ptrdiff(data, data, sizeof(int)),
        PTR data,
        ITEM_SIZE sizeof(int),
        LEN 0
    );

    ASSERT_SLICE(
        Slice99_from_ptrdiff(data, data + Slice99_array_len(data),
        sizeof(int)),
        PTR data,
        ITEM_SIZE sizeof(int),
        LEN Slice99_array_len(data)
    );

    ASSERT_SLICE(
        Slice99_from_ptrdiff(data + 1, data + 4, sizeof(int)),
        PTR data + 1,
        ITEM_SIZE sizeof(int),
        LEN 3
    );
    // clang-format on
}

TEST(is_empty) {
    assert(!Slice99_is_empty(Slice99_from_array((int[]){1, 2, 3})));
    assert(Slice99_is_empty(Slice99_new("abc", 1, 0)));
}

TEST(size) {
    assert(Slice99_size(Slice99_from_array((int[]){1, 2, 3})) == sizeof(int) * 3);
    assert(Slice99_size(Slice99_new("abc", 1, 0)) == 0);
}

TEST(get) {
    int data[] = {1, 2, 3};
    Slice99 slice = Slice99_from_array(data);

    assert(Slice99_get(slice, 0) == &data[0]);
    assert(Slice99_get(slice, 1) == &data[1]);
    assert(Slice99_get(slice, 2) == &data[2]);

    assert((int *)Slice99_get(slice, 3) - 1 == &data[2]);
    assert((int *)Slice99_get(slice, -1) + 1 == &data[0]);
}

TEST(first) {
    int data[] = {1, 2, 3};
    assert(Slice99_first(Slice99_from_array(data)) == &data[0]);
}

TEST(last) {
    int data[] = {1, 2, 3};
    assert(Slice99_last(Slice99_from_array(data)) == &data[2]);
}

TEST(sub) {
    int data[] = {1, 2, 3, 4, 5};

    // clang-format off
    ASSERT_SLICE(
        Slice99_sub(Slice99_from_array(data), 0, 0),
        PTR data,
        ITEM_SIZE sizeof(int),
        LEN 0
    );

    ASSERT_SLICE(
        Slice99_sub(Slice99_from_array(data), 0, 3),
        PTR data,
        ITEM_SIZE sizeof(int),
        LEN 3
    );

    ASSERT_SLICE(
        Slice99_sub(Slice99_from_array(data), 2, 4),
        PTR data + 2,
        ITEM_SIZE sizeof(int),
        LEN 2
    );

    ASSERT_SLICE(
        Slice99_sub(Slice99_new(data + 2, sizeof(int), 3), -2, 1),
        PTR data,
        ITEM_SIZE sizeof(int),
        LEN 3
    );

    ASSERT_SLICE(
        Slice99_sub(Slice99_new(data + 3, sizeof(int), 2), -2, -1),
        PTR data + 1,
        ITEM_SIZE sizeof(int),
        LEN 1
    );
    // clang-format on
}

// Slice99_primitive_eq {
TEST(primitive_eq_basic) {
    char str[] = "12345";

    Slice99 slice_1 = Slice99_from_array((int[]){1, 2, 3, 4, 5}),
            slice_2 = Slice99_from_array((int[]){1, 2, 3, 4, 5}),
            slice_3 = Slice99_from_array((int[]){6, 7, 8});

    assert(Slice99_primitive_eq(slice_1, slice_2));

    assert(!Slice99_primitive_eq(slice_1, slice_3));
    assert(!Slice99_primitive_eq(Slice99_from_str(str), slice_1));
}

TEST(primitive_eq_is_reflexive) {
    for (size_t i = 0; i < 100; i++) {
        GEN_INT_ARRAY_SLICES(slices, 1);
        ASSERT_REFLEXIVITY(Slice99_primitive_eq, slices[0]);
    }
}

TEST(primitive_eq_is_symmetric) {
    for (size_t i = 0; i < 100; i++) {
        GEN_INT_ARRAY_SLICES(slices, 2);
        ASSERT_SYMMETRICITY(Slice99_primitive_eq, slices[0], slices[1]);
    }
}

TEST(primitive_eq_is_transitive) {
    for (size_t i = 0; i < 100; i++) {
        GEN_INT_ARRAY_SLICES(slices, 3);
        ASSERT_TRANSITIVITY(Slice99_primitive_eq, slices[0], slices[1], slices[2]);
    }
}

TEST_IS_EQUIVALENCE(primitive_eq);

TEST(primitive_eq) {
    test_primitive_eq_basic();
    test_primitive_eq_is_equivalence();
}
// } (Slice99_primitive_eq)

// Slice99_eq {
TEST(eq_basic) {
    Slice99 slice_1 = Slice99_from_array((int[]){1, 2, 3, 4, 5}),
            slice_2 = Slice99_from_array((int[]){1, 2, 3, 4, 5}),
            slice_3 = Slice99_from_array((int[]){6, 7, 8});

    assert(Slice99_primitive_eq(slice_1, slice_2));
    assert(!Slice99_primitive_eq(slice_1, slice_3));
}

TEST(eq_is_reflexive) {
    for (size_t i = 0; i < 100; i++) {
        GEN_INT_ARRAY_SLICES(slices, 1);
        ASSERT_REFLEXIVITY(slice_int_eq, slices[0]);
    }
}

TEST(eq_is_symmetric) {
    for (size_t i = 0; i < 100; i++) {
        GEN_INT_ARRAY_SLICES(slices, 2);
        ASSERT_SYMMETRICITY(slice_int_eq, slices[0], slices[1]);
    }
}

TEST(eq_is_transitive) {
    for (size_t i = 0; i < 100; i++) {
        GEN_INT_ARRAY_SLICES(slices, 3);
        ASSERT_TRANSITIVITY(slice_int_eq, slices[0], slices[1], slices[2]);
    }
}

TEST_IS_EQUIVALENCE(eq);

TEST(eq) {
    test_eq_basic();
    test_eq_is_equivalence();
}
// } (Slice99_eq)

TEST(primitive_starts_with) {
    int data[] = {1, 2, 3, 4, 5};
    Slice99 slice = Slice99_from_array(data);

    assert(Slice99_primitive_starts_with(slice, Slice99_sub(slice, 0, 0)));
    assert(Slice99_primitive_starts_with(slice, Slice99_sub(slice, 0, 3)));

    assert(!Slice99_primitive_starts_with(slice, Slice99_sub(slice, 1, 2)));
}

TEST(starts_with) {
    int data[] = {1, 2, 3, 4, 5};
    Slice99 slice = Slice99_from_array(data);

    assert(Slice99_starts_with(slice, Slice99_sub(slice, 0, 0), int_comparator));
    assert(Slice99_starts_with(slice, Slice99_sub(slice, 0, 3), int_comparator));

    assert(!Slice99_starts_with(slice, Slice99_sub(slice, 1, 3), int_comparator));
}

TEST(primitive_ends_with) {
    int data[] = {1, 2, 3, 4, 5};
    Slice99 slice = Slice99_from_array(data);

    assert(Slice99_primitive_ends_with(slice, Slice99_sub(slice, 0, 0)));
    assert(Slice99_primitive_ends_with(slice, Slice99_sub(slice, 3, (int)slice.len)));

    assert(!Slice99_primitive_ends_with(slice, Slice99_sub(slice, 1, 4)));
}

TEST(ends_with) {
    int data[] = {1, 2, 3, 4, 5};
    Slice99 slice = Slice99_from_array(data);

    assert(Slice99_ends_with(slice, Slice99_sub(slice, 0, 0), int_comparator));
    assert(Slice99_ends_with(slice, Slice99_sub(slice, 3, (int)slice.len), int_comparator));

    assert(!Slice99_ends_with(slice, Slice99_sub(slice, 1, 4), int_comparator));
}

TEST(copy) {
    int data[] = {1, 2, 3, 4, 5};
    int copied[Slice99_array_len(data)];

    Slice99_copy(Slice99_from_array(copied), Slice99_from_array(data));
    assert(memcmp(data, copied, sizeof(data)) == 0);
    memset(copied, 0, sizeof(copied));

    Slice99_copy(Slice99_new(copied, 1, 0), Slice99_from_array(data));
    assert(memcmp(data, copied, sizeof(data)) == 0);
    memset(copied, 0, sizeof(copied));
}

TEST(sort) {
    int data[] = {123};
    Slice99 slice = Slice99_new(data, sizeof(char), 0);
    Slice99_sort(slice, int_comparator);
    assert(memcmp(data, (const int[]){123}, sizeof(data)) == 0);

    slice = Slice99_from_array((int[]){62, -15, 60, 0, -19019, 145});
    Slice99_sort(slice, int_comparator);
    assert(memcmp(slice.ptr, (const int[]){-19019, -15, 0, 60, 62, 145}, Slice99_size(slice)) == 0);
}

TEST(bsearch) {
    Slice99 slice = Slice99_from_array((int[]){1, 2, 3, 4, 5, 6, 7});

    int key = 5;
    assert(*(int *)Slice99_bsearch(slice, &key, int_comparator) == key);

    key = 101;
    assert(Slice99_bsearch(slice, &key, int_comparator) == NULL);
}

TEST(swap) {
    int temp;

    Slice99 slice = Slice99_from_array((int[]){1, 2, 3, 4, 5});

    Slice99_swap(slice, 1, 3, &temp);
    assert(*(int *)Slice99_get(slice, 1) == 4);
    assert(*(int *)Slice99_get(slice, 3) == 2);
}

TEST(swap_with_slice) {
    int temp;

    Slice99 lhs = Slice99_from_array((int[]){1, 2, 3, 4, 5}),
            rhs = Slice99_from_array((int[]){6, 7, 8, 9, 0});

    Slice99_swap_with_slice(lhs, rhs, &temp);

    assert(memcmp(lhs.ptr, (const int[]){6, 7, 8, 9, 0}, Slice99_size(lhs)) == 0);
    assert(memcmp(rhs.ptr, (const int[]){1, 2, 3, 4, 5}, Slice99_size(rhs)) == 0);
}

// Slice99_reverse {
TEST(reverse_basic) {
    int temp;

    char empty_str[] = "";
    Slice99 slice = Slice99_new(empty_str, 1, 0);
    Slice99_reverse(slice, &temp);
    assert(memcmp(slice.ptr, empty_str, Slice99_size(slice)) == 0);

    slice = Slice99_from_array((int[]){1, 2, 3, 4, 5});
    Slice99_reverse(slice, &temp);
    assert(memcmp(slice.ptr, (const int[]){5, 4, 3, 2, 1}, Slice99_size(slice)) == 0);
}

static Slice99 slice_rev_aux(Slice99 slice) {
    int temp;
    Slice99_reverse(slice, &temp);
    return slice;
}

TEST(reverse_involutive) {
    for (size_t i = 0; i < 100; i++) {
        GEN_INT_ARRAY_SLICES(slices, 1);

        int saved_array[Slice99_size(slices[0])];
        memcpy(saved_array, slices[0].ptr, sizeof(saved_array));
        Slice99 saved_slice = Slice99_from_array(saved_array);

        ASSERT_INVOLUTION(slice_rev_aux, Slice99_primitive_eq, saved_slice);
    }
}

TEST(reverse) {
    test_reverse_basic();
    test_reverse_involutive();
}
// } (Slice99_reverse)

// Slice99_split_at {
TEST(split_at_empty_slice) {
    Slice99 slice = Slice99_empty(1), lhs, rhs;
    Slice99_split_at(slice, 0, &lhs, &rhs);

    // clang-format off
    ASSERT_SLICE(
        lhs,
        PTR slice.ptr,
        ITEM_SIZE slice.item_size,
        LEN slice.len
    );
    ASSERT_SLICE(
        rhs,
        PTR slice.ptr,
        ITEM_SIZE slice.item_size,
        LEN slice.len
    );
    // clang-format on
}

TEST(split_at_non_empty_slice) {
    Slice99 slice = Slice99_from_array((int[]){1, 2, 3, 4, 5}), lhs, rhs;
    Slice99_split_at(slice, 2, &lhs, &rhs);

    // clang-format off
    ASSERT_SLICE(
        lhs,
        PTR slice.ptr,
        ITEM_SIZE slice.item_size,
        LEN 2
    );
    ASSERT_SLICE(
        rhs,
        PTR (int *)slice.ptr + 2,
        ITEM_SIZE slice.item_size,
        LEN 3
    );
    // clang-format on
}

TEST(split_at_beginning) {
    Slice99 slice = Slice99_from_array((int[]){1, 2, 3, 4, 5}), lhs, rhs;
    Slice99_split_at(slice, 0, &lhs, &rhs);

    // clang-format off
    ASSERT_SLICE(
        lhs,
        PTR slice.ptr,
        ITEM_SIZE slice.item_size,
        LEN 0
    );
    ASSERT_SLICE(
        rhs,
        PTR slice.ptr,
        ITEM_SIZE slice.item_size,
        LEN slice.len
    );
    // clang-format on
}

TEST(split_at_end) {
    Slice99 slice = Slice99_from_array((int[]){1, 2, 3, 4, 5}), lhs, rhs;
    Slice99_split_at(slice, 5, &lhs, &rhs);

    // clang-format off
    ASSERT_SLICE(
        lhs,
        PTR slice.ptr,
        ITEM_SIZE slice.item_size,
        LEN slice.len
    );
    ASSERT_SLICE(
        rhs,
        PTR (int *)slice.ptr + 5,
        ITEM_SIZE slice.item_size,
        LEN 0
    );
    // clang-format on
}

TEST(split_at) {
    test_split_at_empty_slice();
    test_split_at_non_empty_slice();
    test_split_at_beginning();
    test_split_at_end();
}
// } (Slice99_split_at)

int main(void) {
    srand((unsigned)time(NULL));

    test_from_str();
    test_from_ptrdiff();
    test_is_empty();
    test_size();
    test_get();
    test_first();
    test_last();
    test_sub();
    test_primitive_eq();
    test_eq();
    test_primitive_starts_with();
    test_starts_with();
    test_primitive_ends_with();
    test_ends_with();
    test_copy();
    test_sort();
    test_bsearch();
    test_swap();
    test_swap_with_slice();
    test_reverse();
    test_split_at();

    puts("All the tests have passed!");
}
