#include <slice99.h>

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <assert_algebraic.h>

// Auxiliary stuff {

#define TEST(name) static void test_##name(void)

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

static Slice99 gen_int_slice(void) {
    const size_t len = rand() % 10;
    Slice99 data = Slice99_new(malloc(len * sizeof(int)), sizeof(int), len);

    for (ptrdiff_t i = 0; i < (ptrdiff_t)len; i++) {
        *(int *)Slice99_get(data, i) = rand() % INT_MAX;
    }

    return data;
}

static int cmp_ints(const void *lhs, const void *rhs) {
    return *(const int *)lhs - *(const int *)rhs;
}

static bool slice_int_eq(Slice99 lhs, Slice99 rhs) {
    return Slice99_eq(lhs, rhs, cmp_ints);
}
// } (Auxiliary stuff)

TEST(from_str) {
    // clang-format off
    {
        char *str = "";
        ASSERT_SLICE(
            Slice99_from_str(str),
            PTR str,
            ITEM_SIZE sizeof(char),
            LEN strlen(str)
        );

        str = "abc";
        ASSERT_SLICE(
            Slice99_from_str(str),
            PTR str,
            ITEM_SIZE sizeof(char),
            LEN strlen(str)
        );
    }

    {
        char *str = "";
        ASSERT_SLICE(
            SLICE99_TO_UNTYPED(CharSlice99_from_str(str)),
            PTR str,
            ITEM_SIZE sizeof(char),
            LEN strlen(str)
        );

        str = "abc";
        ASSERT_SLICE(
            SLICE99_TO_UNTYPED(CharSlice99_from_str(str)),
            PTR str,
            ITEM_SIZE sizeof(char),
            LEN strlen(str)
        );
    }
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
        Slice99_from_ptrdiff(data, data + SLICE99_ARRAY_LEN(data),
        sizeof(int)),
        PTR data,
        ITEM_SIZE sizeof(int),
        LEN SLICE99_ARRAY_LEN(data)
    );

    ASSERT_SLICE(
        Slice99_from_ptrdiff(data + 1, data + 4, sizeof(int)),
        PTR data + 1,
        ITEM_SIZE sizeof(int),
        LEN 3
    );
    // clang-format on
}

TEST(from_typed_ptr) {
    int data[] = {1, 2, 3, 4, 5};
    Slice99 slice = Slice99_from_typed_ptr((int *)data, 5);

    // clang-format off
    ASSERT_SLICE(
        slice,
        PTR data,
        ITEM_SIZE sizeof(int),
        LEN 5
    );
    // clang-format on
}

TEST(update_len) {
    Slice99 slice = Slice99_from_str("abc");
    const size_t new_len = 1;

    // clang-format off
    ASSERT_SLICE(
        Slice99_update_len(slice, new_len),
        PTR slice.ptr,
        ITEM_SIZE slice.item_size,
        LEN new_len
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

TEST(advance) {
    int data[] = {1, 2, 3, 4, 5};

    // clang-format off
    ASSERT_SLICE(
        Slice99_advance(Slice99_from_array(data), 0),
        PTR data,
        ITEM_SIZE sizeof(int),
        LEN 5
    );

    ASSERT_SLICE(
        Slice99_advance(Slice99_from_array(data), 1),
        PTR data + 1,
        ITEM_SIZE sizeof(int),
        LEN 4
    );

    ASSERT_SLICE(
        Slice99_advance(Slice99_from_array(data), 5),
        PTR data + 5,
        ITEM_SIZE sizeof(int),
        LEN 0
    );

    ASSERT_SLICE(
        Slice99_advance(Slice99_new(data + 1, sizeof(int), 4), -1),
        PTR data,
        ITEM_SIZE sizeof(int),
        LEN 5
    );
    // clang-format on
}

TEST(primitive_eq_basic) {
    char str[] = "12345";

    Slice99 slice_1 = Slice99_from_array((int[]){1, 2, 3, 4, 5}),
            slice_2 = Slice99_from_array((int[]){1, 2, 3, 4, 5}),
            slice_3 = Slice99_from_array((int[]){6, 7, 8});

    assert(Slice99_primitive_eq(slice_1, slice_2));

    assert(!Slice99_primitive_eq(slice_1, slice_3));
    assert(!Slice99_primitive_eq(Slice99_from_str(str), slice_1));
}

TEST(primitive_eq_is_equivalence) {
    for (size_t i = 0; i < 100; i++) {
        Slice99 slices[] = {
            gen_int_slice(),
            gen_int_slice(),
            gen_int_slice(),
        };

        ASSERT_EQUIVALENCE(Slice99_primitive_eq, slices[0], slices[1], slices[2]);

        free(slices[0].ptr);
        free(slices[1].ptr);
        free(slices[2].ptr);
    }
}

TEST(primitive_eq) {
    test_primitive_eq_basic();
    test_primitive_eq_is_equivalence();
}

TEST(eq_basic) {
    Slice99 slice_1 = Slice99_from_array((int[]){1, 2, 3, 4, 5}),
            slice_2 = Slice99_from_array((int[]){1, 2, 3, 4, 5}),
            slice_3 = Slice99_from_array((int[]){6, 7, 8});

    assert(Slice99_primitive_eq(slice_1, slice_2));
    assert(!Slice99_primitive_eq(slice_1, slice_3));
}

TEST(eq_is_equivalence) {
    for (size_t i = 0; i < 100; i++) {
        Slice99 slices[] = {
            gen_int_slice(),
            gen_int_slice(),
            gen_int_slice(),
        };

        ASSERT_EQUIVALENCE(slice_int_eq, slices[0], slices[1], slices[2]);

        free(slices[0].ptr);
        free(slices[1].ptr);
        free(slices[2].ptr);
    }
}

TEST(eq) {
    test_eq_basic();
    test_eq_is_equivalence();
}

TEST(primitive_starts_with_basic) {
    Slice99 slice = Slice99_from_array((int[]){1, 2, 3, 4, 5});

    assert(Slice99_primitive_starts_with(slice, Slice99_sub(slice, 0, 0)));
    assert(Slice99_primitive_starts_with(slice, Slice99_sub(slice, 0, 3)));

    assert(!Slice99_primitive_starts_with(slice, Slice99_sub(slice, 1, 2)));
    assert(!Slice99_primitive_starts_with(slice, Slice99_sub(slice, 3, 5)));
}

TEST(primitive_starts_with_partial_order) {
    for (size_t i = 0; i < 100; i++) {
        Slice99 slices[] = {
            gen_int_slice(),
            gen_int_slice(),
            gen_int_slice(),
        };

        ASSERT_PARTIAL_ORDER(
            Slice99_primitive_starts_with, Slice99_primitive_eq, slices[0], slices[1], slices[2]);

        free(slices[0].ptr);
        free(slices[1].ptr);
        free(slices[2].ptr);
    }
}

TEST(primitive_starts_with_empty_slice_is_min) {
    for (size_t i = 0; i < 100; i++) {
        Slice99 slice = gen_int_slice();

        assert(Slice99_primitive_starts_with(slice, Slice99_empty(sizeof(int))));

        free(slice.ptr);
    }
}

TEST(primitive_starts_with) {
    test_primitive_starts_with_basic();
    test_primitive_starts_with_partial_order();
    test_primitive_starts_with_empty_slice_is_min();
}

#define STARTS_WITH(slice, prefix) Slice99_starts_with(slice, prefix, cmp_ints)

TEST(starts_with_basic) {
    Slice99 slice = Slice99_from_array((int[]){1, 2, 3, 4, 5});

    assert(STARTS_WITH(slice, Slice99_sub(slice, 0, 0)));
    assert(STARTS_WITH(slice, Slice99_sub(slice, 0, 3)));

    assert(!STARTS_WITH(slice, Slice99_sub(slice, 1, 2)));
    assert(!STARTS_WITH(slice, Slice99_sub(slice, 3, 5)));
}

TEST(starts_with_partial_order) {
    for (size_t i = 0; i < 100; i++) {
        Slice99 slices[] = {
            gen_int_slice(),
            gen_int_slice(),
            gen_int_slice(),
        };

#define EQ(slice, other) Slice99_eq(slice, other, cmp_ints)

        ASSERT_PARTIAL_ORDER(STARTS_WITH, EQ, slices[0], slices[1], slices[2]);

#undef EQ

        free(slices[0].ptr);
        free(slices[1].ptr);
        free(slices[2].ptr);
    }
}

TEST(starts_with_empty_slice_is_min) {
    for (size_t i = 0; i < 100; i++) {
        Slice99 slice = gen_int_slice();

        assert(STARTS_WITH(slice, Slice99_empty(sizeof(int))));

        free(slice.ptr);
    }
}

TEST(starts_with) {
    test_starts_with_basic();
    test_starts_with_partial_order();
    test_starts_with_empty_slice_is_min();
}

#undef STARTS_WITH

TEST(primitive_ends_with_basic) {
    Slice99 slice = Slice99_from_array((int[]){1, 2, 3, 4, 5});

    assert(Slice99_primitive_ends_with(slice, Slice99_sub(slice, 0, 0)));
    assert(Slice99_primitive_ends_with(slice, Slice99_sub(slice, 3, (ptrdiff_t)slice.len)));

    assert(!Slice99_primitive_ends_with(slice, Slice99_sub(slice, 1, 4)));
    assert(!Slice99_primitive_ends_with(slice, Slice99_sub(slice, 0, 3)));
}

TEST(primitive_ends_with_partial_order) {
    for (size_t i = 0; i < 100; i++) {
        Slice99 slices[] = {
            gen_int_slice(),
            gen_int_slice(),
            gen_int_slice(),
        };

        ASSERT_PARTIAL_ORDER(
            Slice99_primitive_ends_with, Slice99_primitive_eq, slices[0], slices[1], slices[2]);

        free(slices[0].ptr);
        free(slices[1].ptr);
        free(slices[2].ptr);
    }
}

TEST(primitive_ends_with_empty_slice_is_max) {
    for (size_t i = 0; i < 100; i++) {
        Slice99 slice = gen_int_slice();

        assert(Slice99_primitive_ends_with(slice, Slice99_empty(sizeof(int))));

        free(slice.ptr);
    }
}

TEST(primitive_ends_with) {
    test_primitive_ends_with_basic();
    test_primitive_ends_with_partial_order();
    test_primitive_ends_with_empty_slice_is_max();
}

#define ENDS_WITH(slice, postfix) Slice99_ends_with(slice, postfix, cmp_ints)

TEST(ends_with_basic) {
    Slice99 slice = Slice99_from_array((int[]){1, 2, 3, 4, 5});

    assert(ENDS_WITH(slice, Slice99_sub(slice, 0, 0)));
    assert(ENDS_WITH(slice, Slice99_sub(slice, 3, (ptrdiff_t)slice.len)));

    assert(!ENDS_WITH(slice, Slice99_sub(slice, 1, 4)));
    assert(!ENDS_WITH(slice, Slice99_sub(slice, 0, 3)));
}

TEST(ends_with_partial_order) {
    for (size_t i = 0; i < 100; i++) {
        Slice99 slices[] = {
            gen_int_slice(),
            gen_int_slice(),
            gen_int_slice(),
        };

#define EQ(slice, other) Slice99_eq(slice, other, cmp_ints)

        ASSERT_PARTIAL_ORDER(ENDS_WITH, EQ, slices[0], slices[1], slices[2]);

#undef EQ

        free(slices[0].ptr);
        free(slices[1].ptr);
        free(slices[2].ptr);
    }
}

TEST(ends_with_empty_slice_is_max) {
    for (size_t i = 0; i < 100; i++) {
        Slice99 slice = gen_int_slice();

        assert(ENDS_WITH(slice, Slice99_empty(sizeof(int))));

        free(slice.ptr);
    }
}

TEST(ends_with) {
    test_ends_with_basic();
    test_ends_with_partial_order();
    test_ends_with_empty_slice_is_max();
}

#undef ENDS_WITH

TEST(copy) {
    int data[] = {1, 2, 3, 4, 5};
    int copied[SLICE99_ARRAY_LEN(data)];

    Slice99_copy(Slice99_from_array(copied), Slice99_from_array(data));
    assert(memcmp(data, copied, sizeof(data)) == 0);
    memset(copied, 0, sizeof(copied));

    // Check that copy occurs even if a destination slice is empty.
    Slice99_copy(Slice99_new(copied, sizeof(int), 0), Slice99_from_array(data));
    assert(memcmp(data, copied, sizeof(data)) == 0);
    memset(copied, 0, sizeof(copied));
}

TEST(copy_non_overlapping) {
    Slice99 slice = Slice99_from_array((int[]){1, 2, 3, 4, 5});

    Slice99_copy(slice, Slice99_sub(slice, 1, (ptrdiff_t)slice.len));
    assert(memcmp(slice.ptr, (const int[]){2, 3, 4, 5}, sizeof(int) * 4) == 0);

    // Check that copy occurs even if a destination slice is empty.
    int data[] = {1, 2, 3, 4, 5};
    int copied[SLICE99_ARRAY_LEN(data)];
    Slice99_copy(Slice99_new(copied, sizeof(int), 0), Slice99_from_array(data));
    assert(memcmp(data, copied, sizeof(data)) == 0);
}

TEST(swap) {
    int backup;

    Slice99 slice = Slice99_from_array((int[]){1, 2, 3, 4, 5});

    Slice99_swap(slice, 1, 3, &backup);
    assert(*(int *)Slice99_get(slice, 1) == 4);
    assert(*(int *)Slice99_get(slice, 3) == 2);
}

TEST(swap_with_slice) {
    int backup;

    Slice99 lhs = Slice99_from_array((int[]){1, 2, 3, 4, 5}),
            rhs = Slice99_from_array((int[]){6, 7, 8, 9, 0});

    Slice99_swap_with_slice(lhs, rhs, &backup);

    assert(memcmp(lhs.ptr, (const int[]){6, 7, 8, 9, 0}, Slice99_size(lhs)) == 0);
    assert(memcmp(rhs.ptr, (const int[]){1, 2, 3, 4, 5}, Slice99_size(rhs)) == 0);
}

TEST(reverse_basic) {
    int backup;

    char empty_str[] = "";
    Slice99 slice = Slice99_new(empty_str, 1, 0);
    Slice99_reverse(slice, &backup);
    assert(memcmp(slice.ptr, empty_str, Slice99_size(slice)) == 0);

    slice = Slice99_from_array((int[]){1, 2, 3, 4, 5});
    Slice99_reverse(slice, &backup);
    assert(memcmp(slice.ptr, (const int[]){5, 4, 3, 2, 1}, Slice99_size(slice)) == 0);
}

static Slice99 slice_rev_aux(Slice99 slice) {
    int backup;
    Slice99_reverse(slice, &backup);
    return slice;
}

TEST(reverse_involutive) {
    for (size_t i = 0; i < 100; i++) {
        Slice99 slice = gen_int_slice();

        int *saved_array;
        if ((saved_array = malloc(Slice99_size(slice))) == NULL) {
            abort();
        }

        memcpy(saved_array, slice.ptr, Slice99_size(slice));
        Slice99 saved_slice = Slice99_new(saved_array, sizeof(int), slice.len);

        ASSERT_INVOLUTIVE(slice_rev_aux, Slice99_primitive_eq, saved_slice);

        free(slice.ptr);
        free(saved_array);
    }
}

TEST(reverse) {
    test_reverse_basic();
    test_reverse_involutive();
}

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

TEST(to_c_str) {
    {
        Slice99 slice = Slice99_from_array((char[]){'a', 'b', 'c'});
        assert(strcmp(Slice99_c_str(slice, (char[4]){0}), "abc") == 0);
    }

    {
        CharSlice99 slice = Slice99_typed_from_array(CharSlice99, (char[]){'a', 'b', 'c'});
        assert(strcmp(CharSlice99_c_str(slice, (char[4]){0}), "abc") == 0);
    }
}

TEST(write_to_buffer) {
    char buffer[8] = {0};
    void *buffer_ptr = buffer;

    char data1[] = {'a', 'b', 'c'}, data2[] = {'d', 'e', 'f'};

    buffer_ptr = Slice99_write_to_buffer(Slice99_from_array(data1), buffer_ptr);
    assert(buffer + sizeof(data1) == buffer_ptr);
    assert(memcmp(buffer, data1, sizeof data1) == 0);

    buffer_ptr = Slice99_write_to_buffer(Slice99_from_array(data2), buffer_ptr);
    assert(buffer + sizeof(data1) + sizeof(data2) == buffer_ptr);
    assert(memcmp(buffer + sizeof(data1), data2, sizeof data2) == 0);
}

TEST(u8_from_ints) {
#define TEST_FROM_INT(T, shortcut)                                                                 \
    do {                                                                                           \
        T n = 123;                                                                                 \
        U8Slice99 result = U8Slice99_from_##shortcut(&n);                                          \
        assert(sizeof(T) == result.len);                                                           \
        assert(memcmp((const void *)&n, (const void *)result.ptr, sizeof(T)) == 0);                \
    } while (0)

    TEST_FROM_INT(uint8_t, u8);
    TEST_FROM_INT(uint16_t, u16);
    TEST_FROM_INT(uint32_t, u32);
    TEST_FROM_INT(uint64_t, u64);

    TEST_FROM_INT(int8_t, i8);
    TEST_FROM_INT(int16_t, i16);
    TEST_FROM_INT(int32_t, i32);
    TEST_FROM_INT(int64_t, i64);

#undef TEST_FROM_INT
}

typedef struct {
    int x, y;
} Point;

SLICE99_DEF_TYPED(MyPoints, Point);

TEST(def_typed) {
#define TYPECHECK(fn, expected_type)                                                               \
    do {                                                                                           \
        expected_type = fn;                                                                        \
        (void)_;                                                                                   \
    } while (0)

    TYPECHECK(MyPoints_new, MyPoints(*_)(Point *, size_t));
    TYPECHECK(MyPoints_from_ptrdiff, MyPoints(*_)(Point *, Point *));
    TYPECHECK(MyPoints_empty, MyPoints(*_)(void));
    TYPECHECK(MyPoints_update_len, MyPoints(*_)(MyPoints, size_t));
    TYPECHECK(MyPoints_is_empty, bool (*_)(MyPoints));
    TYPECHECK(MyPoints_size, size_t(*_)(MyPoints));
    TYPECHECK(MyPoints_get, Point * (*_)(MyPoints, ptrdiff_t));
    TYPECHECK(MyPoints_first, Point * (*_)(MyPoints));
    TYPECHECK(MyPoints_last, Point * (*_)(MyPoints));
    TYPECHECK(MyPoints_sub, MyPoints(*_)(MyPoints, ptrdiff_t, ptrdiff_t));
    TYPECHECK(MyPoints_advance, MyPoints(*_)(MyPoints, ptrdiff_t));

    TYPECHECK(MyPoints_primitive_eq, bool (*_)(MyPoints, MyPoints));
    TYPECHECK(MyPoints_eq, bool (*_)(MyPoints, MyPoints, int (*)(const Point *, const Point *)));

    TYPECHECK(MyPoints_primitive_starts_with, bool (*_)(MyPoints, MyPoints));
    TYPECHECK(
        MyPoints_starts_with, bool (*_)(MyPoints, MyPoints, int (*)(const Point *, const Point *)));

    TYPECHECK(MyPoints_primitive_ends_with, bool (*_)(MyPoints, MyPoints));
    TYPECHECK(
        MyPoints_ends_with, bool (*_)(MyPoints, MyPoints, int (*)(const Point *, const Point *)));

    TYPECHECK(MyPoints_copy, void (*_)(MyPoints, MyPoints));
    TYPECHECK(MyPoints_copy_non_overlapping, void (*_)(MyPoints, MyPoints));
    TYPECHECK(MyPoints_swap, void (*_)(MyPoints, ptrdiff_t, ptrdiff_t, Point * restrict));
    TYPECHECK(MyPoints_swap_with_slice, void (*_)(MyPoints, MyPoints, Point * restrict));
    TYPECHECK(MyPoints_reverse, void (*_)(MyPoints, Point * restrict));
    TYPECHECK(
        MyPoints_split_at, void (*_)(MyPoints, size_t, MyPoints * restrict, MyPoints * restrict));
    TYPECHECK(MyPoints_write_to_buffer, Point * (*_)(MyPoints, Point * restrict));

#undef TYPECHECK

    // Slice99_typed_from_array
    {
        MyPoints points =
            Slice99_typed_from_array(MyPoints, (Point[]){{1, 32}, {12, 314}, {-134, -9}});
        (void)points;
    }
}

TEST(fundamentals) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"

    (void)CharSlice99_new((char[]){'a', 'b', 'c'}, 3);
    (void)SCharSlice99_new((signed char[]){'a', 'b', 'c'}, 3);
    (void)UCharSlice99_new((unsigned char[]){'a', 'b', 'c'}, 3);

    (void)ShortSlice99_new((short[]){1, 2, 3}, 3);
    (void)UShortSlice99_new((unsigned short[]){1, 2, 3}, 3);

    (void)IntSlice99_new((int[]){1, 2, 3}, 3);
    (void)UIntSlice99_new((unsigned int[]){1, 2, 3}, 3);

    (void)LongSlice99_new((long[]){1, 2, 3}, 3);
    (void)ULongSlice99_new((unsigned long[]){1, 2, 3}, 3);
    (void)LongLongSlice99_new((long long[]){1, 2, 3}, 3);
    (void)ULongLongSlice99_new((unsigned long long[]){1, 2, 3}, 3);

    (void)FloatSlice99_new((float[]){1, 2, 3}, 3);
    (void)DoubleSlice99_new((double[]){1, 2, 3}, 3);
    (void)LongDoubleSlice99_new((long double[]){1, 2, 3}, 3);

    (void)BoolSlice99_new((_Bool[]){1, 2, 3}, 3);

    (void)U8Slice99_new((uint8_t[]){1, 2, 3}, 3);
    (void)U16Slice99_new((uint16_t[]){1, 2, 3}, 3);
    (void)U32Slice99_new((uint32_t[]){1, 2, 3}, 3);
    (void)U64Slice99_new((uint64_t[]){1, 2, 3}, 3);

    (void)I8Slice99_new((int8_t[]){1, 2, 3}, 3);
    (void)I16Slice99_new((int16_t[]){1, 2, 3}, 3);
    (void)I32Slice99_new((int32_t[]){1, 2, 3}, 3);
    (void)I64Slice99_new((int64_t[]){1, 2, 3}, 3);

#pragma GCC diagnostic pop
}

TEST(to_typed) {
    char *str = "abc";
    const Slice99 x = Slice99_from_str(str);
    CharSlice99 y = (CharSlice99)SLICE99_TO_TYPED(x);

    assert(x.ptr == y.ptr);
    assert(x.len == y.len);
}

TEST(to_untyped) {
    // clang-format off
    char *str = "abc";
    ASSERT_SLICE(
        SLICE99_TO_UNTYPED(CharSlice99_from_str(str)),
        PTR str,
        ITEM_SIZE sizeof(char),
        LEN strlen(str)
    );
    // clang-format on
}

int main(void) {
    srand((unsigned)time(NULL));

    test_from_str();
    test_from_ptrdiff();
    test_from_typed_ptr();
    test_update_len();
    test_is_empty();
    test_size();
    test_get();
    test_first();
    test_last();
    test_sub();
    test_advance();
    test_primitive_eq();
    test_eq();
    test_primitive_starts_with();
    test_starts_with();
    test_primitive_ends_with();
    test_ends_with();
    test_copy();
    test_copy_non_overlapping();
    test_swap();
    test_swap_with_slice();
    test_reverse();
    test_split_at();
    test_to_c_str();
    test_write_to_buffer();
    test_u8_from_ints();

    test_def_typed();
    test_fundamentals();
    test_to_typed();
    test_to_untyped();

    puts("All the tests have passed!");
}
