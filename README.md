# Slice99
[![CI](https://github.com/Hirrolot/slice99/workflows/C/C++%20CI/badge.svg)](https://github.com/Hirrolot/slice99/actions)
[![docs](https://img.shields.io/badge/docs-github.io-blue)](https://hirrolot.github.io/slice99/slice99_8h.html)

This library provides [array slicing] facilities for pure C99.

[array slicing]: https://en.wikipedia.org/wiki/Array_slicing

[[`examples/demo.c`](examples/demo.c)] [ [Playground >>](https://godbolt.org/z/ef3j66) ]
```c
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
    int backup;

    Slice99_swap(data, 1, 3, &backup);
    assert(*(int *)Slice99_get(data, 1) == 9);
    assert(*(int *)Slice99_get(data, 3) == 8);

    data = Slice99_from_array((int[]){1, 2, 3});
    Slice99_reverse(data, &backup);
    assert(Slice99_primitive_eq(data, Slice99_from_array((int[]){3, 2, 1})));
    // }

    // ... and more!
}
```

The main type is [`Slice99`]. It represents a pointer to some array accompanied with an array item size and its length. The utilities `Slice99_*` are used to manipulate this type.

[`Slice99`]: https://hirrolot.github.io/slice99/structSlice99.html

## Motivation

Normally, we pass a data pointer with its length as separate arguments:

```c
void foo(size_t len, uint8_t buffer[static len]) { /* ... */ }
```

However, this interface is notoriously easy to misuse by passing an invalid length or a null pointer for `buffer`. Moreover, programmers frequently tend to perform specific operations on `buffer` and `len`, leading to even more bugs and code clutter:

```c
// Advance the buffer by HEADER_SIZE.
buffer += HEADER_SIZE;
len -= HEADER_SIZE;
```

Slice99 reifies such patterns into functions (`Slice99_advance` below), thus assisting in code safety and clarity:

```c
void foo(Slice99 buffer) {
    // ...
    buffer = Slice99_advance(buffer, HEADER_SIZE);
    // ...
}
```

Another use case of Slice99 is zero-copy parsers: you can return slices from your parser pointing to actual data, without `malloc`ing and `memcpy`ing just to append `'\0'` each time.

## Installation

Slice99 consists of just one header `slice99.h` and nothing else; therefore, the only thing you need to tell your compiler is to add `slice99` to include directories.

If you use CMake, the recommended way is either [`FetchContent`] or [`add_subdirectory`], e.g.:

[`FetchContent`]: https://cmake.org/cmake/help/latest/module/FetchContent.html
[`add_subdirectory`]: https://cmake.org/cmake/help/latest/command/add_subdirectory.html

```cmake
include(FetchContent)

FetchContent_Declare(
    slice99
    URL https://github.com/Hirrolot/slice99/archive/refs/tags/v1.2.3.tar.gz // v1.2.3
)

FetchContent_MakeAvailable(slice99)

target_link_libraries(MyProject slice99)
```

Using [`add_subdirectory`]:

```cmake
add_subdirectory(slice99)
target_link_libraries(MyProject slice99)
```

In the latter case, I encourage you to download Slice99 as a [Git submodule] to be able to update it with `git submodule update --remote` when necessary.

[Git submodule]: https://git-scm.com/book/en/v2/Git-Tools-Submodules

Happy hacking!

## Typed slices

You can define a strongly typed slice with `SLICE99_DEF_TYPED`:

```c

#include <slice99.h>

typedef struct {
    double x, y;
} Point;

SLICE99_DEF_TYPED(MyPoints, Point);

int main(void) {
    MyPoints points = (MyPoints)Slice99_typed_from_array(
        (Point[]){{1.5, 32.5}, {12.0, 314.01}, {-134.10, -9.3}});

    MyPoints first_two = MyPoints_sub(points, 0, 2);
    Point *first = MyPoints_first(points);
    bool is_empty = MyPoints_is_empty(points);
}
```

`SLICE99_DEF_TYPED(MyPoints, Point)` generates a slice named `MyPoints` of the following structure:

```c
typedef struct {
    Point *ptr;
    size_t len;
} MyPoints;
```

It also generates `inline static` functions like `MyPoints_sub` that type-check their arguments. These functions merely desugar to their untyped `Slice99_*` counterparts.

### Fundamental types

This library automatically defines typed slices of several fundamental types. I recommend using them instead of untyped `Slice99`, if possible. For more information, see the docs.

## Projects using Slice99

 - [Hirrolot/smolrtsp](https://github.com/Hirrolot/smolrtsp) --  A small, portable, extensible RTSP 1.0 implementation in C99.

## Release procedure

 1. Update the `PROJECT_NUMBER` field in `Doxyfile`.
 2. Update `SLICE99_MAJOR`, `SLICE99_MINOR`, and `SLICE99_PATCH` in `slice99.h`. 
 3. Run `./scripts/docs.sh` to generate newest docs.
 4. Update `CHANGELOG.md`.
 5. Release the project in [GitHub Releases].

[GitHub Releases]: https://github.com/Hirrolot/slice99/releases

## FAQ

### Q: Can I use this library to develop bare-metal software?

A: Yes, see the [docs](https://hirrolot.github.io/slice99/slice99_8h.html#details).
