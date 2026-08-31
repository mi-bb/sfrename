# AGENTS.md

Guidance for AI coding agents working in this repository. Fuller architecture
notes also live in `CLAUDE.md` — keep the two in sync when structure changes.

## Project

`sfrename` — GTK+3 batch file renamer in C23, built with CMake (the only
build system). Runs on GNU/Linux and FreeBSD; designed for Midnight
Commander integration but works standalone.

## Build

```sh
cmake -S . -B build
cmake --build build
sudo cmake --install build
```

- Requires CMake >= 3.21, GCC >= 14 or Clang >= 18 (C23), and GTK+ 3 >= 3.22.
- **Never pass `-std=` in `CMAKE_C_FLAGS`** — `CMakeLists.txt` sets
  `CMAKE_C_STANDARD 23` with extensions on (`-std=gnu23`); a user `-std=`
  lands after it and silently overrides it.
- `uninstall` (`cmake --build build --target uninstall`) and CPack
  (`cpack --config CPackSourceConfig.cmake` in `build/`) replace the old
  `make uninstall` / `make dist`.
- Everything under `build/` is gitignored — never hand-edit or commit it.

## Tests

GTest (GLib Testing) — no extra dependency, GLib comes via GTK.

```sh
ctest --test-dir build                # all tests
ctest --test-dir build -R strfn       # single test suite
```

CTest names drop the `test_` prefix of the source files. The test binaries
are built by the normal `cmake --build build`.

- Tests cover only the GTK-independent logic layer: `strfn.c`, `namefn.c`,
  the settings structs, and `rconfig.c`. GTK-facing code (`sfrename.c`,
  `rfnames.c`, `rfitem.c`, `dlgs.c`) has no automated coverage.
- In tests, build `RenData`/`RFnames`/`RFitem` by hand — the constructors
  `rendata_new()` / `rfitem_new_from_gfile()` create real GTK widgets and
  need a display.
- There is no linter or typecheck configured; `ctest` is the only automated
  verification.

## Adding a source file — one list to update

`CMakeLists.txt`: the `add_executable(sfrename ...)` source list, plus the
source list of every `add_executable(test_* ...)` target that needs the new
file. That single file is the whole build definition.

## Architecture (outer → inner)

- `sfrename.c` — single UI translation unit; `main()` creates one `RenData`,
  wraps it in a `GtkApplication` with `G_APPLICATION_HANDLES_OPEN`, so argv
  files arrive via the `open` signal, not `activate`.
- `rendata.c/h` — top-level state: file list + one settings sub-struct per
  operation (`RDelete`, `RInsOvr`, `RReplace`, `RNumber`) + global options.
- `rconfig.c/h` — persists `RenData` settings as JSON under
  `$XDG_CONFIG_HOME/sfrename/` (hand-rolled serialization, GLib only).
  Parse-then-apply: malformed config leaves `RenData` untouched.
- `rfnames.c/h` — working file list (array of `RFitem`s + GTK box).
- `rfitem.c/h` — one entry: original/pending name, path, metadata, widgets.
- `namefn.c/h` — applies settings structs to a name string.
- `strfn.c/h` — GTK-independent string primitives used by `namefn.c`.
- `defs.h` — shared constants + `REN_OK`/`REN_NOT_REN`/`REN_EXISTS`/`REN_NC`
  result enum.

All modules follow the `typedef struct { ... } XName` + `xname_new()` /
`xname_free()` / getter / setter convention.

## Conventions

- GPLv3 license header (`Copyright (C) 2019-2026 Michal Babik`) at the top of
  every source file — preserve on edit, add to new files in `src/`.
- Headers are Doxygen-commented (`@brief` on every struct field and
  function) and document the full contract — read the header before the
  `.c` file.
- Code uses C23 features (e.g. `nullptr`, `auto` in `.c` files) with GLib.
