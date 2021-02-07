# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [unreleased]

### Added

 - `Slice99_copy`
 - `Slice99_c_str`
 - `Slice99_find`
 - `Slice99_for_each`
 - `Slice99_from_typed_ptr`
 - `Slice99_advance`
 - Structure updating functions: `Slice99_update_ptr`, `Slice99_update_item_size`, `Slice99_update_len`.
 - An optional slice: `Slice99Maybe`, `Slice99Maybe_just`, `Slice99Maybe_nothing`.

### Changed

 - Rename `Slice99_copy` to `Slice99_copy_non_overlapping`.
 - Use `ptrdiff_t` as an index. Reify the macros `Slice99_get` and `Slice99_get_cast_type`, `Slice99_sub` and `Slice99_sub_cast_type`, `Slice99_swap` into the corresponding functions: `Slice99_get`, `Slice99_sub`, `Slice99_swap`.

## [0.1.0] - 2020-01-02

### Added

 - This excellent project.
