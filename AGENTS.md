# AGENTS.md

Guidance for AI coding agents working in this repository. This is the single
source of truth — `CLAUDE.md` imports this file rather than duplicating it.

## Project

`sfrename` — GTK+3 batch file renamer in C23, built with CMake (the only
build system). Runs on GNU/Linux and FreeBSD; designed for Midnight
Commander integration (`sfrename %f` / `sfrename %t` in the user menu) but
works standalone or with files passed as CLI arguments.

## Build

```sh
cmake -S . -B build
cmake --build build
sudo cmake --install build
```

Suggested release build flags (from README.md):

```sh
cmake -S . -B build -DCMAKE_C_COMPILER=gcc -DCMAKE_C_FLAGS="-march=native -O2 -pipe"
```

- Requires CMake >= 3.21, GCC >= 14 or Clang >= 18 (C23), and GTK+ 3 >= 3.22
  (checked via `pkg-config`). Configure errors out on older compilers.
- **Never pass `-std=` in `CMAKE_C_FLAGS`** — `CMakeLists.txt` sets
  `CMAKE_C_STANDARD 23` with extensions on (`-std=gnu23`); a user `-std=`
  lands after it and silently overrides it.
- `uninstall` (`sudo cmake --build build --target uninstall`) and CPack
  (`cd build && cpack --config CPackSourceConfig.cmake`, produces
  `.tar.xz`/`.tar.gz`) replace the old `make uninstall` / `make dist`.
- Everything under `build/`, plus `compile_commands.json`, is gitignored —
  never hand-edit or commit it.

Run the built binary directly:

```sh
./build/sfrename some_file.txt another_file.txt
```

## Tests

GTest (GLib Testing, `glib.h`) — no extra dependency, GLib comes via GTK.

```sh
ctest --test-dir build                                  # all four suites
ctest --test-dir build -R rconfig                       # single suite
./build/test_strfn -p /strfn/get_valid_length/ascii     # single GTest case
```

CTest names drop the `test_` prefix of the source files. The test binaries
are built by the normal `cmake --build build`.

- Tests cover only the GTK-independent logic layer: `strfn.c`, `namefn.c`,
  `rconfig.c`, and the `rdelete`/`rinsovr`/`rreplace`/`rnumber` settings
  structs. GTK-facing code (`sfrename.c`, `rfnames.c`, `rfitem.c`, `dlgs.c`)
  has no automated coverage and relies on manual/UI verification.
- In tests, build `RenData`/`RFnames`/`RFitem` by hand — the constructors
  `rendata_new()` / `rfitem_new_from_gfile()` create real GTK widgets and
  need a display; the functions under test never touch the widget fields.
- There is no linter or typecheck configured; `ctest` is the only automated
  verification.

## Adding a source file — one list to update

`CMakeLists.txt`: the `add_executable(sfrename ...)` source list, plus the
source list of every `add_executable(test_* ...)` target that needs the new
file. That single file is the whole build definition.

## Architecture (outer → inner)

Single GTK+3 application; the UI is one translation unit (`sfrename.c`), with
rename logic and data structures factored into small paired `.c`/`.h`
modules under `src/`. All modules follow the `typedef struct { ... } XName` +
`xname_new()` / `xname_free()` / getter / setter convention — treat them as
lightweight objects with private state.

- `sfrename.c` — `main()` creates one `RenData`, then a `GtkApplication`
  wired to `activate`/`open`/`shutdown` with `G_APPLICATION_HANDLES_OPEN`, so
  argv files arrive via the `open` signal, not `activate`. Both build the
  same main window. On rename, callbacks iterate `RFnames`, run each active
  `RFitem` through the `namefn_*` pipeline, then rename via GIO.
- `rendata.c/h` — top-level state, passed as GTK callback user data: the
  `RFnames` file list, one settings sub-struct per operation (`RDelete`,
  `RInsOvr`, `RReplace`, `RNumber`), and global options (case conversion,
  spaces/underscores, apply to name/ext/both, exit-after-rename,
  directory-selection flags).
- `rconfig.c/h` — persists `RenData` settings (not the file list) as JSON at
  `$XDG_CONFIG_HOME/sfrename/config.json` (`g_get_user_config_dir()` +
  `RCONFIG_DIR_NAME` / `RCONFIG_FILE_NAME` from `defs.h`). Hand-rolled
  serialization, GLib only, no JSON library. `rconfig_parse()` is
  parse-then-apply: a missing, unreadable, or malformed file leaves
  `RenData` untouched. `rconfig_save()` writes only when "remember options
  on exit" is set, otherwise deletes any existing config file.
- `rfnames.c/h` — working file list: array of `RFitem`s + the GTK box that
  displays them. Owns select/unselect, remove, restore, and sort, each in
  "selected only" / "all" / "by type" (files/folders/symlinks/hidden)
  variants.
- `rfitem.c/h` — one entry: original name (`s_org`), pending new name
  (`s_new`), path, byte/unicode lengths for both, file type metadata
  (symlink/hidden/`GFileType`), and the row's widgets (entry, checkbutton,
  restore/delete buttons).
- Per-operation settings structs, each mirroring one dialog section:
  `RDelete` (delete N chars at a position), `RInsOvr` (insert/overwrite a
  string at a position), `RReplace` (find/replace substring), `RNumber`
  (sequential numbering with start value and position).
- `namefn.c/h` — applies settings structs to a single name string
  (`namefn_delete`, `namefn_insert`, `namefn_overwrite`, `namefn_number`,
  `namefn_upto_lower`, `namefn_spaces_underscores`, `namefn_replace`).
- `strfn.c/h` — GTK-independent string primitives (unicode length,
  delete/insert/overwrite/replace at byte position, case conversion,
  filename/extension splitting) used by `namefn.c`.
- `dlgs.c/h` — auxiliary dialogs (folder chooser, file chooser, about).
- `imgs.c/h` — embedded/generated image and icon data.
- `defs.h` — shared constants (window size, app name/version, config
  file/directory names, default option values) + the `REN_OK` /
  `REN_NOT_REN` / `REN_EXISTS` / `REN_NC` rename-result enum.

## Conventions

- GPLv3 license header (`Copyright (C) 2019-2026 Michal Babik`) at the top of
  every source file — preserve on edit, add to new files in `src/`.
- Headers (`rdelete.h`, `rendata.h`, `rfitem.h`, `rfnames.h`, `rinsovr.h`,
  `rnumber.h`, `rreplace.h`, `strfn.h`) are Doxygen-commented (`/** @brief
  ... */` on every struct field and function) and document the full
  contract — read the header before the `.c` file, and match this style for
  new public API.
- Code uses C23 features freely: `nullptr`, `constexpr` file-scope constants
  in `defs.h`, `auto` in `.c` files, `[[nodiscard]]`, and GCC/Clang
  `__attribute__` annotations on public functions. Match this in new code.
- Commit messages follow [Conventional Commits](https://www.conventionalcommits.org/):
  `<type>(<optional scope>): <description>`, e.g. `fix(rconfig): keep
  defaults on malformed JSON` or `docs: add link to project website in
  README`. Use the standard types (`feat`, `fix`, `docs`, `refactor`,
  `test`, `build`, `chore`, …), write the description in the imperative
  mood with no trailing period, and mark breaking changes with `!` or a
  `BREAKING CHANGE:` footer.
- `CHANGELOG.md` follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)
  and the project uses [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
  Below the license header and preamble comes `## [Unreleased]`, then
  releases as `## [X.Y.Z] - YYYY-MM-DD`, newest first. Changes are grouped
  under `### Added`, `### Changed`, `### Deprecated`, `### Removed`,
  `### Fixed`, `### Security` (only the groups that apply). Record
  user-visible changes, not a copy of the git log.
- New changes go under `## [Unreleased]`. On release, rename it to
  `## [X.Y.Z] - YYYY-MM-DD`, add a fresh empty `## [Unreleased]` above it,
  and update the link references at the bottom of the file: point
  `[Unreleased]` at `compare/vX.Y.Z...HEAD` and add
  `[X.Y.Z]: https://github.com/mi-bb/sfrename/compare/vPREV...vX.Y.Z`.
  Only tagged versions (`v1.2.10` onward) have links; older entries stay
  unlinked.
