/*
 * MIT License
 *
 * Copyright (c) 2020 Temirkhan Myrzamadi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <slice99.h>

#include <assert.h>

int main(void) {
    Slice99 str = Slice99_from_str("hello world");

    // Accessors {
    assert(*(char *)Slice99_first(str) == 'h');
    assert(*(char *)Slice99_last(str) == 'd');
    assert(*(char *)Slice99_get(str, 4) == 'o');
    // }

    // Subslicing {
    assert(Slice99_primitive_eq(Slice99_sub(str, 3, 9), Slice99_from_str("lo wor")));
    // }

    // Predicates {
    assert(!Slice99_is_empty(str));
    assert(Slice99_primitive_starts_with(str, Slice99_from_str("hello")));
    assert(Slice99_primitive_ends_with(str, Slice99_from_str("world")));
    // }

    // Mutators {
    Slice99 data = Slice99_from_array((int[]){5, 8, 1, 9});
    int temp;

    Slice99_swap(data, 1, 3, &temp);
    assert(*(int *)Slice99_get(data, 1) == 9);
    assert(*(int *)Slice99_get(data, 3) == 8);

    data = Slice99_from_array((int[]){1, 2, 3});
    Slice99_reverse(data, &temp);
    assert(Slice99_primitive_eq(data, Slice99_from_array((int[]){3, 2, 1})));
    // }

    // ... and more!
}
