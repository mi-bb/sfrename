# Changelog

All notable changes to Small File Renamer (`sfrename`) are documented in
this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

Copyright (C) 2019-2026 Michal Babik

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

## [1.2.11] - 2026-07-16

### Added

- "Remember options on exit" checkbox. When enabled, all rename settings are
  saved to a JSON config file on exit and restored on the next launch.
  - `rconfig.c`, `rconfig.h`: new files, a hand-rolled minimal JSON
    writer/parser for saving and restoring rename settings. No new build
    dependency: uses only GLib facilities already pulled in via GTK+3. The
    parser uses a parse-then-apply strategy so a missing or malformed config
    file never partially updates `RenData`.
  - `rendata.c`, `rendata.h`: added `rememberopt` field with matching
    getter/setter.
  - `defs.h`: added `DEF_REMEMBEROPT` default and `RCONFIG_DIR_NAME` /
    `RCONFIG_FILE_NAME` constants for the config file path.
  - `sfrename.c`: added the checkbox next to "Exit after rename". Wired
    `rconfig_load()` into `main()` and `rconfig_save()` into `shutdown()`.
    Every settings widget (delete, insert, overwrite, replace, number,
    case/spaces/apply-to radio groups) now seeds its initial value from the
    loaded `RenData` instead of a hardcoded default.
  - `tests/test_rconfig.c`: new file, GLib-based unit tests for `rconfig.c`
    covering round-trips (defaults, non-defaults, UTF-8 and JSON-escaped
    text), a missing config file, a malformed config file leaving `RenData`
    untouched, and save/load toggling the config file's existence based on
    `rememberopt`.
  - `tests/Makefile.am`, `CMakeLists.txt`: wired `test_rconfig` into
    `make check` and `ctest`.
  - `.gitignore`: ignore the `tests/test_rconfig` binary.

### Changed

- The project is now built as C23 (ISO/IEC 9899:2024) with GNU extensions
  instead of C11. This raises the minimum compiler to GCC 14 or Clang 18.
  - `configure.ac`: added a probe that appends `-std=gnu23` (falling back to
    `-std=gnu2x`, `-std=c23`, `-std=c2x`) when the compiler's default mode is
    older than C23, and fails configuration when no C23 mode is available.
    Autoconf 2.72 has no C23 support of its own, so `AC_PROG_CC` cannot do
    this. `CFLAGS` must no longer carry a `-std=` option.
  - `CMakeLists.txt`: `CMAKE_C_STANDARD` raised from 11 to 23;
    `cmake_minimum_required` raised from 3.10 to 3.21, the first release that
    knows the C23 standard value.
  - `strfn.c` (`string_replace_in`): made the `fp` find-pointer `const char *`.
    C23 makes `strstr()` const-preserving, so assigning its result from a
    `const char *` haystack to a `char *` warned under `-std=gnu23`.
  - `README.md`: documented the C23 requirement and dropped `-std=gnu11` from
    the suggested build flags.
- `configure.ac`: ran `autoupdate`. Replaced the obsolete `AC_GNU_SOURCE` with
  `AC_USE_SYSTEM_EXTENSIONS`, which still defines `_GNU_SOURCE` but also
  enables the other system extension macros. `AC_PREREQ` raised from 2.69 to
  2.72; this only affects regenerating `configure` via `autogen.sh`, not
  building from a release tarball.
- The source now uses C23 features rather than merely compiling as C23. No
  behavioural change is intended; the on-disk config format is unchanged and a
  config file written by an earlier version still loads.
  - `defs.h`: the numeric and string `#define`s became `constexpr` objects.
    `FN_LEN` is deliberately `constexpr int`, not `size_t`: it is compared
    against signed values in `rconfig.c`, and an unsigned type would silently
    make those unsigned comparisons. `APP_NAME`/`APP_VER` stay macros because
    they are pasted with adjacent string literals, which a `constexpr` array
    cannot do. String constants use the array form, since C23 requires a
    `constexpr` pointer to be initialised with a null pointer constant.
  - `defs.h`: the anonymous rename-result enum is now
    `typedef enum RenResult : int_fast8_t`, and `sfrename.c` carries the value
    in a `RenResult` instead of an `int_fast8_t`. The switch over it lost its
    unreachable `default:`, so `-Wswitch` now reports a missing case.
    `imgs.h`: `IconImg` likewise gained a fixed underlying type.
  - All 196 `NULL`s became `nullptr`. This matters most at the five variadic
    sentinels (`dlgs.c`, `rconfig.c`, `tests/test_rconfig.c` and the
    `g_test_init` calls), where `nullptr_t` is pointer-sized by definition.
  - Internal `gboolean` became `bool`: the JSON parser helpers and public API
    in `rconfig.{c,h}`, the `b_slink`/`b_hidden` fields and `rfitem_is_*` /
    `rfitem_get_checked` predicates in `rfitem.h`, `rinsovr_empty` and
    `rreplace_empty_*` (previously `int`), and `RenData::renexit` /
    `RenData::rememberopt`. `gboolean` is kept wherever it crosses a GTK
    boundary, notably the `event_win_key_press` signal handler, whose return
    type is fixed by the marshaller. `RenData::uplo`, `spaces`, `applyto` and
    `dirsel` stay `int8_t`: they are multi-valued, and `dirsel` is a bitmask.
  - `rfnames.c`: unified the predicate function-pointer type on
    `bool (*)(const RFitem *)`. The forward declarations previously said
    `int (*fun)` while the definitions said `gboolean (*fun)`; this only
    compiled because `gboolean` is `int`.
  - `__attribute__ ((unused))` became `[[maybe_unused]]` at all five sites.
    `[[nodiscard]]` was added to the nine allocating constructors plus
    `string_get_valid_length` and `rfnames_append_gfile`, where discarding the
    result leaks or is meaningless. The `malloc`, `nonnull`, `returns_nonnull`
    and `pure` attributes stay as `__attribute__`, having no C23 equivalent.
  - The seven `*_init` functions collapsed to `*obj = (T){}`, removing three
    `memset` calls. `rfitem_init` previously left `org_len`/`org_u8len`
    indeterminate until they were assigned later; they are now zeroed.
  - `auto` replaces the type name in a dozen single declarations whose
    initialiser already names the type. The aligned multi-declaration blocks
    in `strfn.c` keep their explicit types, which serve as documentation.
  - `imgs.c`: the nine SVGs became `static const char[]` instead of
    `const char *`, so their length is `sizeof - 1` at compile time rather
    than nine runtime `strlen` calls; `rev` is now `constexpr` (it was the one
    data array that was not even `const`); and the hand-maintained
    `rev_size`/`about_img_size` constants were dropped in favour of `sizeof`,
    which cannot drift.
  - `namefn.c`, `strfn.c`, `tests/test_strfn.c`: the ten 14-field positional
    `ProcessData` initialisers became designated initialisers, and the
    repeated `rendata_get_r*()` calls inside them are hoisted into a local.

### Fixed

- `rconfig.c` (`rconfig_parse`): "select files from directory" options were
  lost whenever the config was reloaded. `dirsel` is a bitmask of the
  `FOLDER_SELECT_*` flags, but the range check clamped it with
  `dirsel ? 1 : 0`, so any combination collapsed to `FOLDER_SELECT_FILES`.
  It is now validated against the real flag range. The flag constants moved
  from `dlgs.h` to `defs.h` (joining the other shared constants) so that
  `rconfig.c` can use them without pulling in GTK, and gained a
  `FOLDER_SELECT_ALL` mask. `tests/test_rconfig.c`: added regression tests
  covering every `dirsel` value, out-of-range values, and unknown-key
  skipping.
- `rfnames.c` (`rfnames_free`): freed each `RFitem` but never the `RFitem **`
  array holding them, leaking it on every `RFnames` teardown.
- `rfnames.c` (`rfnames_sort`): dropped a `g_list_free (gl_items1)` that
  always freed nothing, since the loop above had already walked `gl_items1`
  to `nullptr` and `gl_items` owns every node the concatenation produced.
- `strfn.c` (`string_replace_in`): removed a dead store to `ui_len` that was
  unconditionally overwritten on the next line.
- `strfn.c` (`string_add_number`): `sprintf` into a fixed 20-byte buffer
  became `snprintf`.
- `rfitem.c` (`rfitem_label_set_markup`): replaced the unchecked
  `strcpy` + `strcat` chain into a 100-byte buffer with a single `snprintf`.
  The old code did not actually overflow (the longest markup it can produce
  is 57 bytes), but nothing enforced that; the output is byte-identical for
  every flag and file-type combination.
- `rconfig.c` (`skip_value`): stopped reserving a 1020-byte stack buffer used
  only as a discard sink. `parse_json_string` now accepts a null output
  pointer, meaning "parse and validate, but discard".

## [1.2.10] - 2026-07-13

### Added

- `.gitignore`: new file, ignore autotools build artifacts.
- `CLAUDE.md`: new file, project documentation for Claude Code.
- `CMakeLists.txt`: new file, CMake build environment as an alternative to
  Autotools. Builds sfrename against GTK+3 (>=3.22) and installs the binary,
  desktop file, and pixmap icon.
- `tests/test_strfn.c`, `tests/test_namefn.c`: new files, GLib-based (GTest)
  unit tests for `strfn.c` and `namefn.c`. Cover the GTK-independent logic
  layer only; `RenData`/`RFnames`/`RFitem` fixtures are built by hand rather
  than via their real constructors, since those create GTK widgets and need
  a display.
- `tests/test_rsettings.c`: new file, GLib-based unit tests for `rdelete.c`,
  `rinsovr.c`, `rreplace.c`, and `rnumber.c`. These structs hold no GTK
  widgets, so they are exercised through their real
  `rXXX_new()`/`rXXX_free()` constructors. Covers defaults, getter/setter
  round-trips, UTF-8 byte/char length accounting, `FN_LEN` truncation, and
  the `empty_*` predicates.
- `tests/Makefile.am`, `CMakeLists.txt`: wired `test_strfn`/`test_namefn`/
  `test_rsettings` into `make check` and `ctest`.

### Fixed

- `rfnames.c` (`rfnames_delete_at_pos`): free the `RFitem**` array before
  dropping the pointer when the last item is removed, instead of leaking
  it. Fixed the cleanup loop on `realloc()` failure iterating one slot past
  the live range and double-freeing a stale/duplicate `RFitem`.
- `sfrename.c` (`event_click_add_folder_files`): free the selected folder
  path even when all "select ..." checkboxes are unchecked (`i_opt == 0`),
  not only when files were actually added.

## [1.2.9] - 2020-11-15

### Changed

- `imgs.c`: changed some icons to SVG.
- Some code updates.

## [1.2.8] - 2020-06-05

### Added

- `rfnames.c` (`rfnames_restore_all_files`, `rfnames_restore_all_folders`,
  `rfnames_restore_all_symlinks`, `rfnames_restore_all_hidden`): new
  functions for restoring original file names.
- `sfrename.c` (`create_toolbar`): added toolbar entries for the different
  restoring options.

## [1.2.7] - 2020-04-28

### Added

- `rfitem.h`/`rfitem.c`: `RFitem` gained the length of `s_new` in bytes
  (`new_len`) and unicode characters (`new_u8len`); set in
  `rfitem_set_snew`.
  - `rfitem_set_sorg_from_snew`, `rfitem_set_snew_from_sorg`: new functions
    to copy `s_new` string and length to `s_org` and vice versa.
- `rinsovr.h`/`rinsovr.c`: `RInsOvr` gained the length of `s_text` (total
  `len` and unicode `u8len`); set in `rinsovr_set_text`.
  - `rinsovr_get_len`, `rinsovr_get_u8len`: new length getters.

### Changed

- `strfn.h`: `ProcessData` structure gained a string to modify (`s_srcdst`)
  and the length of strings; all callers updated.

## [1.2.6] - 2020-04-18

### Added

- `data/sfrename.desktop`: new desktop entry.
- `data/sfrename.png`: new pixmap icon.

## [1.2.5] - 2020-03-19

### Added

- `defs.h`: added default options `DEF_UPLO`, `DEF_SPACES`, `DEF_APPLTO`,
  `DEF_RENEXT`, `DEF_DIRSEL`.
- `rendata.c` (`rendata_new`): setting default options from `defs.h`; new
  getters/setters (`rendata_get_rdelete`, `rendata_get_rinsert`,
  `rendata_get_roverwr`, `rendata_get_rreplace`, `rendata_get_rnumber`,
  `rendata_get_uplo`/`rendata_set_uplo`, `rendata_get_spaces`/
  `rendata_set_spaces`, `rendata_get_applyto`/`rendata_set_applyto`,
  `rendata_get_renexit`/`rendata_set_renexit`, `rendata_get_dirsel`/
  `rendata_set_dirsel`) for the `RenData` structure.
- `rfitem.c`: new getters and setters `rfitem_get_snew`/`rfitem_set_snew`,
  `rfitem_get_sorg`/`rfitem_set_sorg`, `rfitem_get_spth`/`rfitem_set_spth`
  for the `RFitem` structure.
- `rinsovr.c` (`rinsovr_empty`): new function to check whether the insert
  string is empty.
- `rreplace.c` (`rreplace_empty_from`, `rreplace_empty_to`): new functions
  for checking the "from" and "to" strings.

### Changed

- `namefn.c` (`name_delete_chars`, `name_overwrite_string`,
  `name_number_string`, `name_to_upcase_lowercase`,
  `name_spaces_underscores`, `name_replace_strings`): changed access to
  the `RenData`, `RDelete`, `RInsOvr`, `RReplace`, `RNumber` parameters.
- `strfn.c` (`string_get_valid_length`): renamed from `get_valid_length`.
- `rdelete.c/.h`, `rinsovr.c/.h`, `rreplace.c/.h`, `rnumber.c/.h`: `RDelete`,
  `RInsOvr`, `RReplace`, `RNumber` structure and functions moved here from
  `rendata.c`.

## [1.2.4] - 2020-03-01

### Added

- `rfnames.c` (`rfnames_unselect_files`): new function to unselect files on
  the list.
  - `rfnames_select_unselect`: new function to select or unselect items
    depending on the result of a predicate function.
  - `rfnames_unselect_folders`, `rfnames_unselect_symlinks`,
    `rfnames_unselect_hidden`: new functions to unselect folders, symlinks,
    and hidden files/directories on the list.
  - `rfnames_select_files`, `rfnames_select_folders`,
    `rfnames_select_symlinks`, `rfnames_select_hidden`: now call
    `rfnames_select_unselect` to select items.
  - `rfnames_remove`: new function to remove items depending on the result
    of a predicate function; `rfnames_remove_selected`,
    `rfnames_remove_all_files`, `rfnames_remove_all_folders`,
    `rfnames_remove_all_symlinks`, `rfnames_remove_all_hidden` now call it.
- `rfitem.c` (`rfitem_is_file`, `rfitem_is_folder`, `rfitem_is_symlink`,
  `rfitem_is_hidden`): new functions checking whether an `RFitem` is a
  file/folder/symlink/hidden item.
- `imgs.h`, `imgs.c`: added `W_ICON_SELECT_BW`, `W_ICON_SELECT_BL` to the
  `IconImg` enum for the b/w and blue select icons; `get_image` updated to
  return them.
- `sfrename.c` (`create_toolbar`): added menu items for unselecting files,
  folders, hidden files/dirs, and symlinks.
- Added tool items for unselecting files, folders, symlinks and hidden
  files/dirs in the select button menu.

### Changed

- `dlgs.c` (`about_app_dialog`): updated the link to the author's page.
- `rendata.c` (`rendata_init`): default `i_opt` value set to 1.
- `strfn.c` (`string_replace_in`, `string_insert_string`,
  `string_overwrite_string`): some changes in pointer iteration.
- Source code updates.

## [1.2.3] - 2020-02-18

### Added

- `rendata.h` (`RenData::i_opt`): new value on the `RenData` structure,
  used to store the selected options in the folder-select dialog.
- After adding files from a directory, the program remembers the add
  options (files/directories/hidden, etc.).

### Changed

- `dlgs.c` (`add_files_folder_dialog`): changed to set toggle buttons based
  on the `i_opt` value.
- `sfrename.c` (`event_click_add_folder_files`): reads `i_opt` from the
  `RenData` item, checks whether it is less than 1 and sets it to 1 if so
  (so "add files" stays checked in the dialog by default), and only scans
  the folder if `i_opt` is greater than 0. Stores `i_opt` back on the
  `RenData` item after scanning the folder. Changed the input parameter
  from `RFnames` to `RenData` to get access to `i_opt`; all callers
  updated.
- `autogen.sh`: removed the `--force` option so it no longer overwrites the
  `INSTALL` file.
- Updated `autogen.sh` in repositories so it doesn't overwrite the
  `INSTALL` file.
- Some comment updates.

## [1.2.2] - 2020-02-10

### Added

- `sfrename.c` (`event_click_add_files`, `event_click_add_folder_files`,
  `open`): added setting focus on the first entry.
- Setting focus on the first file name entry after starting with files
  passed as arguments, or after adding files to an empty list.

## [1.2.1] - 2020-02-05

### Added

- `imgs.c`, `imgs.h`: new files with graphics for the toolbar buttons.
- `sfrename.c` (`create_file_name_entries`): added forgotten check for
  file existence.
  - (`create_toolbar`): changed to load pixbufs into tool items.
- `rfitem.c` (`rfitem_label_set_markup`): new function to set label markup
  based on the type of file/dir; called from `rfitem_new_from_gfile`.

### Changed

- `rfnames.c` (`rfnames_sort`): improved sorting.

## [1.2.0] - 2020-02-02

### Added

- Toolbar with buttons to add, select, and remove files.
- The program can now be run without files passed as arguments.
- Checkboxes to select which files to apply changes to.
- Labels showing the file type.
- Buttons on each item to restore the original name or delete the item.

### Changed

- Source code updates.

## [1.1.7] - 2020-01-20

### Added

- `cres.c`: new file with functions to create or resize a dynamic array.
- `rfnames.c`: `RFnames` structure and functions moved here (out of
  `sfrename.c`).
- `rendata.c` (`rdelete_init`, `rinsovr_init`, `rreplace_init`,
  `rnumber_init`): new functions with the initializations moved out of
  `rendata_init`.
  - `rendata_new`: new function to create a `RenData` item.
- Renaming files from different directories in one batch is now available.

### Changed

- `rendata.c` (`rendata_free`): changed, it now calls `rfnames_free`.
- `sfrename.c`: removed the `RFiles` structure and its functions and used
  `RenData` instead.
  - `create_file_name_entries`: changed the way structures and entries are
    created.
  - `file_check_and_rename`: changed to work with files from different
    directory paths.
- `strfn.c` (`string_replace_in`, `string_insert_string`,
  `string_overwrite_string`): small updates to improve performance.
- Cosmetic code updates.

## [1.1.6] - 2019-12-09

### Added

- `sfrfn.c`, `strfn.h`: new files for string-modify functions.
- `namefn.c`, `namefn.h`: new files for functions to modify file name
  strings.
- Configuration and build scripts.

### Changed

- `sfrename.c`: renamed some values for better readability.
  - `get_valid_length`, `string_replace_in`, `string_delete_chars`,
    `string_insert_string`, `string_overwrite_string`, `string_add_number`,
    `string_to_lower`, `string_to_upper`, `string_extract_name_ext`,
    `string_combine_name_ext`, `string_process_filename`: moved to
    `strfn.c`.
  - `name_delete_chars`, `name_insert_string`, `name_overwrite_string`,
    `name_number_string`, `name_to_upcase_lowercase`,
    `name_spaces_underscores`, `name_replace_strings`: moved to
    `namefn.c`.
  - `create_window`: changed the way the app version string is passed.
- Source code updates.

## [1.1.5] - 2019-12-02

### Added

- Option for numbering files.
- New text-overwrite option.

### Changed

- `sfrename.c`: new `ProcessData` structure used by the functions that
  manipulate strings.
  - `file_names_update_changes`: inserted calls to the overwrite/number
    functions.
  - `string_overwrite_string`: new function to overwrite text in a string.
  - `string_add_number`: new function for adding numbers to a string.
  - `create_overwrite_string_box`: new function to create widgets for
    string overwrite.
  - `create_number_string_box`: new function to create widgets for
    numbering strings.
  - `event_overwrite_pos_changed`, `event_overwrite_string_entry_changed`:
    new functions for string overwrite.
  - `event_toggle_number_names`, `event_number_start_changed`,
    `event_number_pos_changed`: new functions for string numbering.
- Some code updates.

## [1.1.4] - 2019-12-01

### Changed

- Small performance updates.
- `sfrename.c` (`string_insert_string`): changed string iteration to work
  using an index instead of pointer arithmetic, for simpler end-of-string
  checking. `strncpy` changed to `memcpy` throughout.
  - (`string_process_filename`): changed the size of `f_ext` for better
    compatibility with the string-processing functions.

## [1.1.3] - 2019-11-28

### Changed

- `sfrename.c` (`string_insert_string`): slightly changed the function that
  inserts strings into file names.
  - (`string_replace_in`): slightly changed the function that replaces
    strings in file names.
  - (`string_to_lower`, `string_to_upper`): added string length validation.
- Some code updates.

## [1.1.2] - 2019-10-10

### Removed

- `sfrename.c` (`free_stuff`): removed a no-longer-needed print command.

---

The project's history predates this changelog's version tracking; the
first commit (`sfrename.c`, 2019-01-25) was the initial release.
