# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [unreleased]

## [0.3.0] - 2021-03-27

### Added

 - `SLICE99_MAJOR`, `SLICE99_MINOR`, `SLICE99_PATCH`

### Removed

 - `Slice99_sort`, `SLICE99_INCLUDE_SORT`, `SLICE99_QSORT`
 - `Slice99_bsearch`, `SLICE99_INCLUDE_BSEARCH`, `SLICE99_BSEARCH`

### Changed

 - `Slice99_array_len` -> `SLICE99_ARRAY_LEN`

## [0.2.1] - 2021-02-21

### Added

 - `Slice99_pack_to_u8`, `Slice99_pack_to_u16`, `Slice99_pack_to_u32`, `Slice99_pack_to_u64`

## [0.2.0] - 2021-02-11

### Added

 - `Slice99_copy`
 - `Slice99_c_str`
 - `Slice99_find`
 - `Slice99_for_each`
 - `Slice99_from_typed_ptr`
 - `Slice99_advance`
 - Structure updating functions: `Slice99_update_ptr`, `Slice99_update_item_size`, `Slice99_update_len`.
 - An optional slice: `Slice99Maybe`, `Slice99Maybe_just`, `Slice99Maybe_nothing`.
 - Tell a compiler to warn about an unused result of certain functions (`SLICE99_WARN_UNUSED_RESULT`).
 - Tell a compiler that certain functions are constant (`SLICE99_CONST`).

### Changed

 - Rename `Slice99_copy` to `Slice99_copy_non_overlapping`.
 - Use `ptrdiff_t` as an index. Reify the macros `Slice99_get(_cast_type)`, `Slice99_sub(_cast_type)`, and `Slice99_swap` into the corresponding functions: `Slice99_get`, `Slice99_sub`, `Slice99_swap`.

## [0.1.0] - 2021-01-02

### Added

 - This excellent project.
