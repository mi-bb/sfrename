# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## About

Small File Renamer (`sfrename`) is a GTK+3 desktop application (C) for batch
renaming files and directories on GNU/Linux and FreeBSD. It was originally
built to be invoked from Midnight Commander's user menu (`sfrename %f` /
`sfrename %t`), but also runs standalone or with files passed as CLI
arguments.

## Build commands

This is a standard GNU Autotools project.

```sh
./autogen.sh   # regenerates configure/Makefile.in via autoreconf (only needed
                # after editing configure.ac / Makefile.am, or on a fresh clone)
./configure
make
make install
```

Suggested release build flags (from INSTALL):

```sh
./configure CC="gcc" CFLAGS="-march=native -O2 -pipe -std=gnu11"
```

Build dependency: GTK+ 3 >= 3.22 (checked via `pkg-config` in `configure.ac`).

There is no test suite and no linter configured in this repository.

Run the built binary directly, e.g.:

```sh
./src/sfrename some_file.txt another_file.txt
```

Generated build artifacts (`configure`, `Makefile`, `config.h`,
`autom4te.cache/`, etc.) are gitignored — do not hand-edit or commit them;
regenerate with `./autogen.sh` / `./configure` instead.

## Architecture

Single GTK+3 application, single translation unit for the UI (`sfrename.c`),
with rename logic and data structures factored into small paired `.c`/`.h`
modules under `src/`. All modules follow the same convention: a `typedef
struct { ... } XName` plus `xname_new()` / `xname_free()` / getter / setter
functions — treat these as lightweight objects with private state.

Data flow, from outer to inner:

- **`RenData`** (`rendata.c/h`) — top-level state object created once in
  `main()`. Holds the `RFnames` file list plus one settings sub-struct per
  rename operation (`RDelete`, `RInsOvr`, `RReplace`, `RNumber`) and global
  options (case conversion, spaces/underscores, apply-to-name/ext/both,
  exit-after-rename, directory-selection flags). Passed through the GTK
  callbacks as user data.
- **`RFnames`** (`rfnames.c/h`) — the working file list: an array of
  `RFitem`s plus the GTK box widget that displays them. Owns
  select/unselect, remove, restore, and sort operations, each available in
  "selected only" / "all" / "by type" (files/folders/symlinks/hidden)
  variants.
- **`RFitem`** (`rfitem.c/h`) — one file/directory entry: original name
  (`s_org`), pending new name (`s_new`), path, byte/unicode lengths for
  both, file type metadata (symlink/hidden/GFileType), and the row's GTK
  widgets (entry, checkbutton, restore/delete buttons).
- **Per-operation settings structs** — `RDelete` (delete N chars at a
  position), `RInsOvr` (insert/overwrite a string at a position), `RReplace`
  (find/replace substring), `RNumber` (sequential numbering with start value
  and position) — each mirrors one section of the rename dialog.
- **`namefn.c/h`** — applies each settings struct to a single file name
  string (`namefn_delete`, `namefn_insert`, `namefn_overwrite`,
  `namefn_number`, `namefn_upto_lower`, `namefn_spaces_underscores`,
  `namefn_replace`). This is the layer that turns UI state into an actual
  new name.
- **`strfn.c/h`** — lower-level, GTK-independent string primitives (unicode
  length, delete/insert/overwrite/replace at byte position, upper/lower
  case, filename/extension splitting) used by `namefn.c`.
- **`dlgs.c/h`** — auxiliary GTK dialogs (folder chooser, file chooser,
  about dialog).
- **`imgs.c/h`** — embedded/generated image and icon data for the UI.
- **`rdelete.h`, `rendata.h`, `rfitem.h`, `rfnames.h`, `rinsovr.h`,
  `rnumber.h`, `rreplace.h`, `strfn.h`** are heavily Doxygen-commented
  (`@brief` on every struct field and function) — read the header before the
  `.c` file when working in a given module, the contract is usually fully
  documented there.

Application lifecycle (`sfrename.c`): `main()` creates one `RenData`, then a
`GtkApplication` wired to `activate`/`open`/`shutdown` signals
(`G_APPLICATION_HANDLES_OPEN`, so files passed as argv arrive via the `open`
signal, not `activate`). Both `activate` and `open` build the same main
window; UI callbacks read/write the shared `RenData` and, on rename, iterate
`RFnames`, running each active `RFitem` through the `namefn_*` pipeline
before calling into GIO to perform the actual filesystem rename.

`defs.h` centralizes shared constants (window size, app name/version,
default option values) and the `REN_OK` / `REN_NOT_REN` / `REN_EXISTS` /
`REN_NC` rename-result enum returned by the rename operation.

## Conventions

- License headers (GPLv3, `Copyright (C) 2019-2020 Michal Babik`) are
  present at the top of every source file — preserve them when editing
  existing files or creating new ones in `src/`.
- Doxygen-style `/** @brief ... */` comments document every public struct,
  field, and function in headers; match this style for new public API.
- `src/Makefile.am`'s `sfrename_SOURCES` list must be updated when adding or
  removing source files.
