# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [unreleased]

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
   - `Slice99_update_ptr`, `Slice99_update_item_size`.

## [0.3.0] - 2021-03-27

### Added

 - `SLICE99_(MAJOR|MINOR|PATCH)` representing the corresponding components of a library version.

### Removed

 - Sorting facilities: `Slice99_sort`, `SLICE99_INCLUDE_SORT`, `SLICE99_QSORT`.
 - Binary search facilities: `Slice99_bsearch`, `SLICE99_INCLUDE_BSEARCH`, `SLICE99_BSEARCH`.

### Changed

 - `Slice99_array_len` -> `SLICE99_ARRAY_LEN`.

## [0.2.1] - 2021-02-21

### Added

 - `Slice99_pack_to_(u8|u16|u32|u64)` to pack a slice into an integer.

## [0.2.0] - 2021-02-11

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

## [0.1.0] - 2021-01-02

### Added

 - This excellent project.
