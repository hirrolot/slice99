# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## unreleased

### Added

 - String formatting support:
   - The `CharSlice99_(v)(n)fmt` functions that make `CharSlice99` out of a formatted string.
   - The `CharSlice99_alloca_fmt` convenience macro.
   - The `SLICE99_DISABLE_STDIO` macro setting.
   - The `SLICE99_VSPRINTF`, `SLICE99_VSNPRINTF`, and `SLICE99_SNPRINTF` macros.

## 0.7.5 - 2022-02-26

### Fixed

 - Segfault: allocate space for a null terminator in `CharSlice99_alloca_c_str`.

## 0.7.4 - 2022-02-26

### Added

 - `CharSlice99_alloca_c_str`, which allocates a C-string using `alloca`.

### Fixed

 - Surround the `ptr` parameter of `Slice99_from_typed_ptr` with parentheses to avoid macro expansion issues.

## 0.7.3 - 2022-02-25

### Fixed

 - Return a `void *` pointer from `SLICE99_APPEND(_ARRAY)`.

## 0.7.2 - 2021-12-09

### Fixed

 - Specify `C` as the project language in `CMakeLists.txt`. Previously, CMake detected C++ and required a C++ compiler in order to compile the project.

## 0.7.1 - 2021-12-01

### Added

 - Add the root `CMakeLists.txt` to be able to use CMake with [`FetchContent`] or [`add_subdirectory`].

[`FetchContent`]: https://cmake.org/cmake/help/latest/module/FetchContent.html
[`add_subdirectory`]: https://cmake.org/cmake/help/latest/command/add_subdirectory.html

## 0.7.0 - 2021-09-07

### Added

 - `SLICE99_APPEND(_ARRAY)` to write an object/array to a memory buffer.
 - `SLICE99_TO_OCTETS` to convert an object to `U8Slice99`.

### Changed

 - `Slice99_typed_from_array(T, x)` => `(T)Slice99_typed_from_array(x)` [**BC**].

### Fixed

 - Show `(I8|I16|I32|I64)Slice99` type definitions in the docs.

## 0.6.0 - 2021-08-21

### Changed

 - `SLICE99_TO_TYPED(x, T)` => `(T)SLICE99_TO_TYPED(x)` [**BC**].

## 0.5.1 - 2021-08-20

### Added

 - `CharSlice99_from_str` & `CharSlice99_c_str` as equivalents of the corresponding `Slice99` functions.

## 0.5.0 - 2021-06-24

### Removed

 - `Slice99_pack_to_(u8|u16|u32|u64)` and `Slice99_pack_to_(u8|u16|u32|u64)_beginning` due to too specific domain.

## 0.4.1 - 2021-06-20

### Added

 - The `Slice99_pack_to_(u8|u16|u32|u64)_beginning` functions. They are like `Slice99_pack_to_(u8|u16|u32|u64)` but only take the beginning of a slice.
 - Specify `__attribute__((always_inline))` on `SLICE99_DEF_TYPED`-generated functions if compiling on GCC.
 - Typed slices of fundamental types:
   - `CharSlice99` of `char`.
   - `SCharSlice99` of `signed char`.
   - `UCharSlice99` of `unsigned char`.
   - `ShortSlice99` of `short`.
   - `UShortSlice99` of `unsigned short`.
   - `IntSlice99` of `int`.
   - `UIntSlice99` of `unsigned int`.
   - `LongSlice99` of `long`.
   - `ULongSlice99` of `unsigned long`.
   - `LongLongSlice99` of `long long`.
   - `ULongLongSlice99` of `unsigned long long`.
   - `FloatSlice99` of `float`.
   - `DoubleSlice99` of `double`.
   - `LongDoubleSlice99` of `long double`.
   - `BoolSlice99` of `_Bool`.
   - `U8Slice99` of `uint8_t` (if supported).
   - `U16Slice99` of `uint16_t` (if supported).
   - `U32Slice99` of `uint32_t` (if supported).
   - `U64Slice99` of `uint64_t` (if supported).
   - `I8Slice99` of `int8_t` (if supported).
   - `I16Slice99` of `int16_t` (if supported).
   - `I32Slice99` of `int32_t` (if supported).
   - `I64Slice99` of `int64_t` (if supported).

### Fixed

 - Do not specify `SLICE99_PURE` for `Slice99_pack_to_(u8|u16|u32|u64)` as they use `SLICE99_ASSERT`.

## 0.4.0 - 2021-05-30

### Added

 - Strongly typed slices:
   - `SLICE99_DEF_TYPED` to generate a typed slice.
   - `SLICE99_TO_TYPED` to convert an untyped slice into a typed one.
   - `SLICE99_TO_UNTYPED` to convert a typed slice into an untyped one.
   - `Slice99_typed_from_array` as a typed slice counterpart of `Slice99_from_array`.

### Removed

 - Useless higher-order functions:
   - `Slice99_for_each`, `Slice99_find`.
 - I/O functions:
   - `Slice99_fwrite(_ln)`, `Slice99_write(_ln)`.
 - Field updating functions:
   - `Slice99_update_(ptr|item_size)`.
 - `Slice99Maybe` facilities:
   - The `Slice99Maybe` type.
   - The `Slice99Maybe_(just|nothing)`, functions.

## 0.3.0 - 2021-03-27

### Added

 - `SLICE99_(MAJOR|MINOR|PATCH)` representing the corresponding components of a library version.

### Removed

 - Sorting facilities: `Slice99_sort`, `SLICE99_INCLUDE_SORT`, `SLICE99_QSORT`.
 - Binary search facilities: `Slice99_bsearch`, `SLICE99_INCLUDE_BSEARCH`, `SLICE99_BSEARCH`.

### Changed

 - `Slice99_array_len` -> `SLICE99_ARRAY_LEN`.

## 0.2.1 - 2021-02-21

### Added

 - `Slice99_pack_to_(u8|u16|u32|u64)` to pack a slice into an integer.

## 0.2.0 - 2021-02-11

### Added

 - `Slice99_copy` to copy one slice to the beginning of another one.
 - `Slice99_c_str` to construct a null-terminated C string.
 - `Slice99_find` to find an element in a slice.
 - `Slice99_for_each` to walk through all elements in a slice.
 - `Slice99_from_typed_ptr` to obtain a slice from a properly typed pointer.
 - `Slice99_advance` to advance a slice by N bytes.
 - `Slice99_update_(ptr|item_size|len)` to update a specific field.
 - An optional slice `Slice99Maybe` with value constructors `Slice99Maybe_just`, `Slice99Maybe_nothing`.
 - Tell a compiler to warn about an unused result of certain functions.
 - Tell a compiler that certain functions are constant.

### Changed

 - `Slice99_copy` -> `Slice99_copy_non_overlapping`.
 - Use `ptrdiff_t` as an index. Reify these macros into the corresponding functions:
   - `Slice99_get(_cast_type)` -> `Slice99_get`.
   - `Slice99_sub(_cast_type)` -> `Slice99_sub`.
   - `Slice99_swap` -> `Slice99_swap`.

## 0.1.0 - 2021-01-02

### Added

 - This excellent project.
