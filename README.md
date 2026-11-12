# MyVault 🔐

A simple encrypted password vault written in pure C — built as a PBL project
for *Introduction to Programming Using C (22B21MA111)*.

## Features

- Master password protects the vault
- Add / list / view / delete password entries (site, username, password)
- Entries are **encrypted** with an XOR cipher before being written to `vault.dat`
- Wrong master password = unreadable garbage (try it!)

## Build & Run

```sh
gcc -std=c99 -Wall -O2 -o myvault myvault.c
./myvault
```

## ⚠️ Honest security note

XOR "encryption" is very weak and this project stores data locally in plain
reach. This is a **learning project** demonstrating file I/O and basic crypto
ideas — do not store real passwords in it.

## What it demonstrates (PBL syllabus mapping)

| Course topic | Where in the code |
|---|---|
| Pointers & pointer arithmetic | `xor_buf()` cycles a key over a buffer; planned v2: traversal via `struct entry *p` |
| Dynamic memory allocation for structures | v2: vault grows with `realloc` of `struct entry *` |
| Dynamic 2D array | v2: allocated pointer-to-pointer grid |
| Pass by reference | functions modify caller data via pointers (`int *count`) |
| Binary file I/O | `fwrite` / `fread` with `"wb"` mode |
| Structures & arrays of structs | `struct entry items[]` |

## Project structure

```
myvault/
├── myvault.c      # the whole program, fully commented
└── README.md
```

`vault.dat` is created at runtime and stores your encrypted entries.
