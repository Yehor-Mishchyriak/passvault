# Passvault roadmap

## Schedule and target

- Work **1–2 hours every other day**. Session 1 is your next workday; Session N falls on project day `2N - 1`.
- Plan: **64 sessions, 64–128 hours, about 18 weeks**. Every eighth session is a buffer. This is a budget, not a deadline; split any unfinished session across additional workdays.
- Each session: implement its checklist, run its completion check, inspect the diff, commit the working change. Stop at two hours and record the next step beside the unfinished item.
- Check an item only when its behavior works. A filename or WIP comment is not an implementation.
- Target: a local encrypted vault with a command-line interface, an interactive prompt, and a keyboard-driven TUI on macOS and Linux, including SSH terminals.
- Scope: one user, local filesystems, one writer at a time. No cloud sync, browser extension, clipboard integration, shared HPC/network storage, or multi-user access in this release.
- Use fake credentials during development. A release number does not establish security; record review results and unresolved limitations.

## Already completed

- [x] Implemented in-memory add, edit, delete, display, list, help, and quit.
- [x] Added unique, case-sensitive names; rejected empty names/secrets; allowed empty usernames.
- [x] Added duplicate/missing-name errors, deletion confirmation, and cancellation.
- [x] Checked input failures; interrupted add/edit/delete operations leave existing entries unchanged.
- [x] Added positive listing-count validation, trailing-character rejection, and an empty-vault message.
- [x] Updated help to describe the implemented commands.
- [x] Created `src/`, `tests/`, `tests/fixtures/`, and `docs/` with module placeholders.
- [x] Ran temporary compilation, behavior, and sanitizer checks during development.

**Current state:** implementation remains in `src/main.cpp`. CMake, other modules, committed tests, fixtures, security docs, and changelog are placeholders. The `.bin` fixtures contain text markers, not encrypted data. Persistence, encryption, hidden input, command-line subcommands, and TUI are not implemented.

## File responsibilities

| File(s) | Responsibility |
| --- | --- |
| `src/main.cpp` | Startup, argument dispatch, exit status. |
| `src/entry.hpp` | Entry data fields. |
| `src/vault.hpp/.cpp` | In-memory operations and validation; no terminal or filesystem calls. |
| `src/cli.hpp/.cpp` | Interactive commands, subcommands, output, help. |
| `src/session.hpp/.cpp` | Locked/unlocked state, pending edits, create/open/save/close. |
| `src/serialization.hpp/.cpp` | Entries ↔ validated JSON in memory. |
| `src/crypto.hpp/.cpp` | Library initialization, key derivation, authenticated encryption, key cleanup. |
| `src/vault_format.hpp/.cpp` | Encode/decode and bound the encrypted file envelope. |
| `src/storage.hpp/.cpp` | File reads, exclusive creation, locks, safe replacement, permissions. |
| `src/terminal.hpp/.cpp` | Hidden input and terminal-state restoration. |
| `src/tui.hpp/.cpp` | Full-screen interface calling the same session/vault operations. |
| `tests/*_tests.cpp` | Repeatable tests; add CLI/terminal/TUI tests as those modules grow. |
| `tests/fixtures/` | Real fake-data fixtures with documented purposes. |
| `CMakeLists.txt` | Build, dependency versions, tests, install target. |
| `docs/file-format.md` | Payload/envelope layout, versions, limits, compatibility. |
| `docs/security.md` | Threat model, secret handling, review status, limitations. |

Headers stay beside source files. Add a shared error or key-buffer header only when more than one module needs it. Do not add generic `utils` modules without a specific responsibility.

## Phase 1 — Separate and test the existing application

### 01 — Establish a reproducible build

- [ ] Implement CMake for the current `src/main.cpp`; require C++20 and enable compiler warnings.
- [ ] Update `.gitignore` for build output, local vaults, temporary saves, and editor artifacts; keep fake fixtures trackable.
- [ ] Add configure/build/run commands to README; state that storage is currently memory-only.

**Check:** configure into `build/`, compile without warnings, run `/help` and `/quit`.

### 02 — Extract types and define operation results

- [ ] Move `Entry` into `entry.hpp`; put `Vault` in `vault.hpp`; add header guards or `#pragma once`.
- [ ] Define explicit results for duplicate name, missing entry, and invalid fields; use `std::optional` or a pointer for absent lookup results.
- [ ] Remove the `"NULL"` sentinel and reserved-word workaround; allow the literal string `"NULL"` as ordinary data.

**Check:** add, find, and delete an entry named `NULL`; no missing-entry path depends on a magic string.

### 03 — Extract add and lookup

- [ ] Implement add and lookup in `vault.cpp`; take field values as arguments and return results without printing or reading input.
- [ ] Keep validation in the vault layer: nonempty name/secret, optional username, unique name.
- [ ] Update the existing prompt handlers to call these operations.

**Check:** add two entries; reject duplicates and empty required fields without changing the vault.

### 04 — Extract edit, delete, and listing

- [ ] Move edit/delete/list behavior into `vault.cpp`; keep confirmation and count prompts in the interface.
- [ ] Make edits validate all proposed fields before modifying the stored entry.
- [ ] Return listing data without secrets; sort names for predictable display while retaining map lookup.

**Check:** edit one of two entries; the other stays unchanged. Cancel deletion; both remain. Confirm deletion; only the selected entry disappears.

### 05 — Finish the interface split

- [ ] Move command parsing, prompts, help, and handlers into `cli.cpp`; expose the entry point in `cli.hpp`.
- [ ] Reduce `main.cpp` to startup and calling the interface; remove the unused `None` path and ineffective final assignment.
- [ ] Keep input failure handling consistent; add direct includes for every library facility used.

**Check:** the current interactive workflow still works after linking multiple `.cpp` files.

### 06 — Commit vault tests

- [ ] Configure a test executable and CTest; implement `tests/vault_tests.cpp` with a test framework or checks that stay active in release builds.
- [ ] Cover valid add/edit/delete/find, duplicates, missing entries, empty required fields, empty usernames, and `NULL` values.
- [ ] Assert failed operations preserve the existing records.

**Check:** `ctest --test-dir build --output-on-failure` runs the tests and fails when an invariant is deliberately broken, then passes after reverting that break.

### 07 — Commit interface regressions and CI

- [ ] Add scripted prompt tests for spaces, unknown commands, confirmation cancellation, invalid counts, and EOF at each input boundary.
- [ ] Add macOS/Linux CI builds and CTest; add a sanitizer build where supported.
- [ ] Write the first changelog entry for the verified in-memory functionality.

**Check:** a fresh checkout builds and tests in CI; incomplete input never commits an operation.

### 08 — Buffer and refactoring checkpoint

- [ ] Finish open Phase 1 items; remove stale WIP markers only from implemented files.
- [ ] Verify vault tests run without terminal input and no UI code lives in `vault.cpp`.
- [ ] Record remaining failures before moving on.

**Check:** build and committed tests pass; README instructions match the repository.

## Phase 2 — Define and validate the data format

### 09 — Write the security and compatibility contract

- [ ] Fill `docs/security.md`: protect a locked file against offline reading and tampering; exclude compromised hosts, keyloggers, terminal capture, and rollback to an older valid vault.
- [ ] Specify case-sensitive names, UTF-8 field policy, optional username, single-line fields for v1, and name+username listing without secrets.
- [ ] Choose and document maximum field bytes, entry count, serialized bytes, and vault-file bytes; define rejection behavior.

**Check:** each limit and excluded threat has a concrete written rule.

### 10 — Define the plaintext payload

- [ ] Add an established JSON library through CMake with a fixed reviewed version; document how to update it.
- [ ] Define a versioned payload schema in `docs/file-format.md` containing entry name, username, and secret.
- [ ] Replace `empty_vault.json`, `valid_entries.json`, and `special_characters.json` placeholders with fake data.

**Check:** all valid fixtures parse; special-character fixtures include quotes, backslashes, spaces, and non-ASCII text.

### 11 — Implement serialization

- [ ] Encode a vault into JSON bytes in `serialization.cpp`; keep plaintext in memory.
- [ ] Produce a deterministic entry order for repeatable tests.
- [ ] Enforce output limits before producing an oversized payload.

**Check:** empty and populated vaults serialize with the documented schema and retain every field.

### 12 — Implement validated deserialization

- [ ] Decode into a temporary vault; reject unsupported schema versions, duplicate names, wrong types, missing fields, invalid fields, and exceeded limits.
- [ ] Report structured errors without including secret values or raw JSON in messages.
- [ ] Return the complete validated vault only after the entire payload succeeds.

**Check:** malformed input cannot partially replace the active vault.

### 13 — Finish serialization fixtures and tests

- [ ] Replace remaining JSON WIP markers with the invalid cases indicated by their filenames; add unsupported-schema and over-limit cases.
- [ ] Test encode/decode round trips, boundary sizes, duplicate names, special characters, and every invalid fixture.
- [ ] Decide and test whether unknown fields are rejected or ignored.

**Check:** all serialization tests run under CTest without file persistence or encryption.

### 14 — Specify the encrypted envelope

- [ ] Document magic bytes, format version, algorithm IDs, KDF parameters, salt, nonce, ciphertext length, and byte order.
- [ ] Define exact header bytes to authenticate as additional data; do not serialize raw C++ structs.
- [ ] Define minimum/maximum KDF costs and length limits checked before allocation or key derivation; reject unsupported algorithms and trailing bytes.

**Check:** the document describes how to parse an envelope without depending on compiler layout or native integer size.

### 15 — Implement envelope parsing and encoding

- [ ] Implement `vault_format.cpp` with checked lengths and overflow-safe boundary checks.
- [ ] Replace malformed-header, truncated-vault, unsupported-version, and excessive-KDF fixtures with actual bytes; document each expected error.
- [ ] Test truncation at header boundaries and inconsistent declared lengths.

**Check:** invalid headers fail before invoking crypto or allocating attacker-selected sizes.

### 16 — Buffer and format checkpoint

- [ ] Finish format tests and resolve ambiguous schema/header rules.
- [ ] Add `tests/fixtures/README.md` describing fake values, expected results, and which encrypted fixtures still await implementation.
- [ ] Check all parser errors for accidental payload disclosure.

**Check:** payload and envelope tests pass; the format document matches the code.

## Phase 3 — Encrypt and handle secret input

### 17 — Integrate libsodium

- [ ] Add libsodium to CMake; document supported dependency versions and installation commands.
- [ ] Initialize the library once and handle initialization failure.
- [ ] Add a crypto test target and library-backed random-byte generation.

**Check:** macOS/Linux CI can initialize libsodium and run the crypto test target.

### 18 — Derive keys from passwords

- [ ] Implement Argon2id key derivation with `crypto_pwhash`; record the actual algorithm ID and parameters in the envelope.
- [ ] Generate a random salt when creating a vault; validate stored parameters before deriving a key.
- [ ] Measure unlock time and memory on supported machines; choose documented defaults within the accepted limits.

**Check:** the same password/salt/parameters derive the same key; invalid parameters and allocation failures return controlled errors. Follow the [key-derivation API](https://doc.libsodium.org/password_hashing/default_phf).

### 19 — Encrypt and decrypt bytes

- [ ] Wrap libsodium's XChaCha20-Poly1305 AEAD; generate a fresh random nonce on every encryption.
- [ ] Authenticate the exact envelope header as additional data and encrypt the serialized payload.
- [ ] Return plaintext only after authentication succeeds; handle every library error.

**Check:** round trips preserve the payload; a wrong key, altered header, or altered ciphertext fails. Follow the [AEAD documentation](https://doc.libsodium.org/secret-key_cryptography/aead/chacha20-poly1305/xchacha20-poly1305_construction).

### 20 — Add crypto regression fixtures

- [ ] Replace `valid_encrypted_vault.bin` and `tampered_ciphertext.bin` markers with real test ciphertext; document the fake test password.
- [ ] Add wrong-password, empty/truncated ciphertext, changed nonce, and changed authenticated-header tests.
- [ ] Ensure tests with fixed inputs never introduce fixed randomness into production encryption.

**Check:** the valid fixture decrypts; all tampered fixtures fail without returning entries.

### 21 — Control sensitive-buffer lifetime

- [ ] Introduce an owned, noncopyable key buffer with cleanup on destruction and defined move behavior.
- [ ] Wipe key/password buffers with supported library facilities; check any memory-locking failures instead of claiming protection that was not obtained.
- [ ] Reduce unnecessary plaintext copies; document remaining `std::string`, JSON, swap, and crash-dump limitations.

**Check:** normal/error exits release sensitive buffers; code review confirms cleanup paths. Use the [secure-memory API](https://doc.libsodium.org/memory_management).

### 22 — Hide password and secret input

- [ ] Implement terminal input with echo disabled in `terminal.cpp`; restore the saved terminal settings on scope exit.
- [ ] Route secret/master-password prompts through this helper; reject unsupported non-terminal secret input with a clear error for v1.
- [ ] Add a pseudoterminal test that checks terminal echo flags and verifies the typed secret is not echoed.

**Check:** secret input is hidden; ordinary command input remains visible; EOF and exceptions restore echo.

### 23 — Handle terminal interruptions and output

- [ ] Handle Ctrl-C and termination requests through a safe shutdown path; keep unsafe I/O/allocation out of signal handlers.
- [ ] Test interrupted secret input and terminal restoration on macOS/Linux; document that SIGKILL cannot run cleanup.
- [ ] Escape control characters in displayed names/usernames; keep secrets out of routine errors and help. Require explicit reveal and show a scrollback notice.

**Check:** cancellation restores the terminal; injected escape characters cannot control the screen through ordinary listings.

### 24 — Buffer and crypto checkpoint

- [ ] Finish crypto/terminal tests; inspect every password, key, plaintext, and error path.
- [ ] Add negative tests for boundary values discovered during implementation.
- [ ] Update security documentation with implemented guarantees and remaining limitations.

**Check:** tests pass; secrets have no command-line argument, environment-variable, or routine-log path.

## Phase 4 — Store encrypted vaults safely

### 25 — Implement bounded file reads

- [ ] Read binary vault files with a maximum size; handle missing files, permissions, short reads, and non-regular files.
- [ ] Define and implement a symlink policy; validate the opened file descriptor rather than relying only on a prior path check.
- [ ] Keep storage independent of JSON and encryption.

**Check:** fixture reads match bytes exactly; invalid paths and oversized files produce specific errors.

### 26 — Create a new vault without overwriting

- [ ] Create new files exclusively with owner-only permissions on macOS/Linux.
- [ ] Check all write/close errors and handle partial writes; remove only temporary files created by the current attempt.
- [ ] Add tests for existing destinations, permission failures, and creation failure.

**Check:** attempting creation over any existing file leaves its bytes unchanged.

### 27 — Implement atomic replacement

- [ ] Create a uniquely named, owner-only temporary file in the destination directory and write encrypted bytes only.
- [ ] Check writes and close; replace the original only after the temporary write succeeds.
- [ ] Add controlled failures before replacement; preserve the original vault and clean up owned temporary files.

**Check:** each pre-replacement failure leaves the previous vault byte-for-byte intact.

### 28 — Define durability behavior

- [ ] Add the required file/directory synchronization steps for the supported local filesystems.
- [ ] Distinguish a failure before replacement from a durability failure after replacement; do not claim the old file survived if rename already succeeded.
- [ ] Inject write, sync, close, and rename failures; document filesystem/power-loss limits.

**Check:** tests distinguish unchanged-old-file outcomes from replaced-but-durability-uncertain outcomes.

### 29 — Prevent concurrent writers

- [ ] Implement an OS-managed exclusive lock on a stable sidecar file before reading or creating a vault; hold it for the session.
- [ ] Do not unlink/recreate the lock file while sessions may use it; handle permissions and release locks on exit.
- [ ] Test two processes targeting the same vault; document the supported path/alias policy and exclude network filesystems.

**Check:** the second process fails clearly before reading/editing a concurrently opened vault; process exit releases the lock.

### 30 — Implement create and unlock sessions

- [ ] Implement `session.cpp`: acquire lock, read/validate envelope, derive key, authenticate, decode, then expose an unlocked vault.
- [ ] Create a vault using a confirmed master password, a new salt, and an encrypted empty payload.
- [ ] Keep a failed open locked; never write after a wrong password or failed validation.

**Check:** create → close → open works; wrong passwords and corrupt files leave disk bytes unchanged.

### 31 — Implement save and close sessions

- [ ] Track whether successful mutations create unsaved changes; clear that state only after a confirmed successful save.
- [ ] Connect serialization → encryption → envelope → storage replacement; use a fresh nonce for each save.
- [ ] Add save/discard/cancel handling for ordinary close; define EOF/interruption behavior explicitly, including possible loss of unsaved edits.

**Check:** save → restart restores edits; failed saves remain visibly unsaved and do not report success.

### 32 — Buffer and persistence checkpoint

- [ ] Finish session/storage tests, including full create/add/edit/delete/save/reopen sequences.
- [ ] Test failure paths with fake credentials and assert earlier saved data remains recoverable.
- [ ] Update README and changelog for encrypted local persistence.

**Check:** a fresh process can reopen the vault; no plaintext vault or plaintext temporary file is written.

## Phase 5 — Finish the command-line product

### 33 — Define the CLI contract

- [ ] Specify subcommands: `init`, `list`, `show`, `add`, `edit`, `delete`, `passwd`, `backup`, `shell`, `tui`, plus `--help`, `--version`, and `--vault PATH`.
- [ ] Implement argument parsing for the first batch: `init`, `list`, `show`, `shell`; reject unknown options and missing arguments.
- [ ] Define stdout/stderr and exit codes; prohibit passwords/secrets as arguments or environment variables.

**Check:** help describes actual implemented subcommands; malformed invocations exit without opening or modifying a vault.

### 34 — Connect read commands and interactive sessions

- [ ] Connect `init`, `list`, `show`, and `shell` to the session layer; keep authentication prompts out of ordinary result output.
- [ ] Retain existing slash commands in `shell`; add explicit save/lock/quit behavior with pending-change handling.
- [ ] Make `show` an explicit secret-reveal operation; define redirected-output policy rather than accidentally exposing secrets to a pipe.

**Check:** separate CLI processes can create a vault, open it, and list/show a fake entry with documented exit codes.

### 35 — Connect mutation commands

- [ ] Add `add`, `edit`, and `delete` subcommands using terminal prompts for secret fields and explicit deletion confirmation.
- [ ] Save successful one-shot mutations before reporting success; make cancellation leave disk unchanged.
- [ ] Test failures and non-terminal invocation; do not hang waiting for unavailable interactive input.

**Check:** a command's exit status distinguishes success, cancellation, invalid input, and operational failure as documented.

### 36 — Add search and consistent output

- [ ] Add name/username filtering to listing; never search or display secret contents implicitly.
- [ ] Use deterministic sorting, a clear empty-result message, and optional count limits consistently in CLI and shell.
- [ ] Handle long/non-ASCII names and control characters; add a plain-output mode suitable for scripts without color codes.

**Check:** filtering returns only expected entries and produces no secret values.

### 37 — Change the master password

- [ ] Authenticate the current vault, confirm the new password, generate a fresh salt, and derive a new key.
- [ ] Save a newly encrypted file through the normal safe replacement path; replace the active key only after the save outcome is resolved.
- [ ] Test failure before/after replacement; document that old backups still use the old password.

**Check:** after success the new password opens the current vault and the old one does not; pre-replacement failures preserve the old vault.

### 38 — Add encrypted backup and recovery instructions

- [ ] Implement backup to a new destination without overwriting; copy only committed encrypted bytes with restrictive permissions.
- [ ] Define behavior when the interactive session has unsaved changes: save first or explicitly back up the last saved version.
- [ ] Document recovery by verifying/opening a backup at a separate path before manually replacing a working vault.

**Check:** a backup opens in a fresh process; the recovery exercise never overwrites the original during verification.

### 39 — Add generated secrets

- [ ] Generate passwords with library randomness and unbiased character selection; enforce a bounded requested length.
- [ ] Allow generated values in add/edit without printing them automatically; preserve explicit reveal behavior.
- [ ] Test length/alphabet/error handling; do not present simple randomness tests as a security proof.

**Check:** generated secrets save/reopen correctly and never appear in routine status output.

### 40 — Buffer and CLI checkpoint

- [ ] Complete CLI integration tests: subcommands, shell, exit codes, save failures, redirection policy, and Ctrl-D/Ctrl-C.
- [ ] Update command help, README examples, and changelog to match the current executable.
- [ ] Remove any documented command that has no working implementation.

**Check:** complete the documented workflow from a fresh temporary directory using fake credentials only.

## Phase 6 — Build the TUI on the same application logic

### 41 — Select and integrate the TUI library

- [ ] Evaluate one maintained library against keyboard input, password fields, resizing, SSH behavior, licensing, and macOS/Linux support; record the choice.
- [ ] Pin its version in CMake; implement `passvault tui` with a minimal screen and clean exit.
- [ ] Keep the CLI build usable without TUI dependencies through a CMake option.

**Check:** open/close the TUI locally and over SSH; the shell's terminal state is restored.

### 42 — Add lock/unlock screens

- [ ] Add vault-path selection and masked master-password input; reuse the session API.
- [ ] Display wrong-password/file errors without exposing raw payloads; support cancellation.
- [ ] Clear password widgets on success, failure, and screen changes.

**Check:** failed unlock never exposes entries or modifies the vault; successful unlock opens the entry view.

### 43 — Add the entry list and details view

- [ ] Implement keyboard selection, scrolling, stable sorted order, and name/username search.
- [ ] Show metadata in the details panel; keep secrets masked by default.
- [ ] Add empty-vault and no-results states; preserve selection safely after refresh.

**Check:** navigate and filter a fake vault larger than the screen without displaying secrets.

### 44 — Add entry forms

- [ ] Implement add/edit forms with optional username, required name/secret, validation errors, and cancel.
- [ ] Reuse vault validation; keep drafts separate until confirmation succeeds.
- [ ] Mask secret fields and integrate the generator without automatically revealing generated values.

**Check:** invalid/cancelled forms do not change entries; accepted forms mark the session unsaved.

### 45 — Add delete and reveal controls

- [ ] Add a confirmation dialog for deletion with a safe default and keyboard cancellation.
- [ ] Add an explicit reveal action with visible reveal state; remask on navigation, lock, or timeout.
- [ ] Clear secret-bearing widgets when leaving the screen; document terminal/screen-capture limitations.

**Check:** cancel preserves the entry; deletion selects a valid remaining row; revealed data does not carry into another entry's panel.

### 46 — Add save, lock, and exit flows

- [ ] Display an unsaved-change indicator; add save and save/discard/cancel dialogs.
- [ ] Add manual lock that resolves pending edits, clears entries/keys from the session, and returns to unlock.
- [ ] Add configurable inactivity locking with a documented pending-edit policy; never silently discard unsaved changes.

**Check:** failed saves keep the session open and unsaved; lock/reopen requires the master password again.

### 47 — Finish terminal behavior and TUI tests

- [ ] Add keyboard-help overlay, usable focus order, visible selection without relying only on color, and a narrow-terminal fallback.
- [ ] Handle resize, long/non-ASCII text, interruptions, and terminal reconnect/EOF paths.
- [ ] Add controller tests for screen transitions and pseudoterminal smoke tests for startup/exit/restoration.

**Check:** run the same add/edit/delete/save/lock workflow locally and over SSH at small and large terminal sizes.

### 48 — Buffer and TUI checkpoint

- [ ] Fix the TUI issues found in Session 47; keep business rules in shared modules.
- [ ] Verify CLI and TUI can alternately open the same saved vault and preserve all fields.
- [ ] Add TUI instructions and keyboard reference to README.

**Check:** neither interface requires its own encryption, validation, or storage implementation.

## Phase 7 — Harden and test the complete application

### 49 — Consolidate secret-handling checks

- [ ] Audit input, errors, stdout/stderr, debug logs, widgets, temporary files, and crash paths for secret exposure.
- [ ] Add fake-secret marker tests that assert routine commands/errors never emit the marker.
- [ ] Document unavoidable plaintext lifetime and display exposure; remove unsupported security claims.

**Check:** only explicitly requested secret display emits the marker; secure-input tests separately verify echo behavior.

### 50 — Stress the parsers

- [ ] Add a bounded fuzz/random-input harness for envelope and payload decoding; run it with sanitizers.
- [ ] Exercise truncation, invalid lengths, integer boundaries, duplicate fields/entries, unsupported versions, and resource limits.
- [ ] Turn every discovered failure into a small regression fixture and test.

**Check:** malformed data fails within the documented resource bounds without crashes or partial vault replacement.

### 51 — Exercise storage failure and recovery

- [ ] Expand fault injection for disk-full/short-write, sync/close/rename failure, temporary-file cleanup, and permission failures.
- [ ] Run process-kill tests before and after replacement; distinguish these from actual power-loss testing.
- [ ] Verify lock contention and backup recovery in separate processes.

**Check:** every tested failure has a documented recoverable outcome; no success message accompanies an unresolved save failure.

### 52 — Test installation platforms

- [ ] Run a fresh build and full test suite on macOS and Linux; record compiler/library versions.
- [ ] Test CLI and TUI over SSH with a non-admin user and a supported local vault directory.
- [ ] Check owner-only permissions after create/save/backup and behavior under restrictive/default umasks.

**Check:** documented platform workflows work without administrator privileges at runtime.

### 53 — Measure responsiveness

- [ ] Benchmark unlock, save, list/search, and TUI navigation at empty, typical, and maximum supported vault sizes.
- [ ] Record CPU/memory and KDF settings; adjust limits or UI feedback based on measurements without silently weakening KDF settings.
- [ ] Verify large inputs cannot bypass limits or freeze ordinary navigation indefinitely.

**Check:** publish measured results and limitations rather than unsupported performance targets.

### 54 — Review security-sensitive changes

- [ ] Review KDF bounds, header authentication, nonce generation, key ownership, file replacement, locks, and terminal cleanup against tests and library documentation.
- [ ] Request an independent review if available; prepare the format/security docs and focused source files for that reviewer.
- [ ] Record findings and review status; fix confirmed data-loss/authentication/secret-exposure defects before release.

**Check:** there is a written review record. If independent review is unavailable, explicitly label the release unaudited.

### 55 — Reconcile behavior and documentation

- [ ] Update `docs/security.md` and `docs/file-format.md` from the actual implementation, including password loss, backups, rollback, filesystem limits, and compatibility.
- [ ] Add a vulnerability-reporting route in `SECURITY.md` using a real maintainer-approved contact/channel.
- [ ] Audit README/help against implemented commands, flags, defaults, and failure behavior.

**Check:** no placeholder promises or undocumented format decisions remain in release documentation.

### 56 — Buffer and release-candidate checkpoint

- [ ] Resolve remaining Phase 7 findings; rerun only affected tests plus the complete release suite once fixes are done.
- [ ] Freeze v1 scope; move optional additions into a post-release list.
- [ ] Record any blocker with a reproduction and an explicit next action.

**Check:** no known unresolved data-loss, authentication-bypass, or unintended-secret-exposure defect is accepted as a cosmetic issue.

## Phase 8 — Package, validate, and release

### 57 — Make installation repeatable

- [ ] Add CMake install/uninstall guidance and a configurable install prefix; verify the installed executable outside the repository.
- [ ] Document dependency installation and supported build options, including CLI-only builds.
- [ ] Make version information available from both interfaces without unlocking a vault.

**Check:** build → install → run works from a clean checkout with README commands.

### 58 — Package and document dependencies

- [ ] Produce macOS/Linux release artifacts or reproducible source-build instructions for the supported configurations.
- [ ] Verify library licensing; update `CREDITS.md` and include required third-party notices.
- [ ] Generate artifact checksums and record exact dependency versions; exclude debug dumps, local vaults, and credentials.

**Check:** inspect package contents and run the packaged executable on a clean test environment.

### 59 — Write the final user guide

- [ ] Document create/unlock/add/edit/delete/search/reveal/save/lock/quit, password change, backup, and recovery with fake examples.
- [ ] Add TUI keyboard reference, error explanations, forgotten-password behavior, and unsupported-use cases.
- [ ] Add development commands for build/test/sanitizers and fixture regeneration.

**Check:** someone can follow the guide without reading source code or this roadmap.

### 60 — Run the release acceptance workflow

- [ ] On macOS and Linux: create a vault, add two entries, edit one, cancel then confirm deletion, save, close, reopen, and verify fields.
- [ ] Repeat through CLI and TUI; verify wrong passwords, corrupted copies, empty inputs, EOF, and failed saves.
- [ ] Change the password, create a backup, and recover that backup into a separate path.

**Check:** save the commands/results in a release checklist; every required case passes.

### 61 — Test upgrade and compatibility behavior

- [ ] Open the oldest supported test vault and confirm unsupported versions are rejected without modification.
- [ ] Document how users back up before updating; preserve fixture compatibility for future releases.
- [ ] Verify a CLI-only build and a TUI-enabled build read/write the same format.

**Check:** compatibility tests are part of CI, not only a manual release note.

### 62 — Prepare the release candidate

- [ ] Replace the changelog placeholder with implemented features, supported platforms, known limitations, and review status.
- [ ] Remove unused WIP placeholders from the release or explicitly mark them outside v1 scope; do not ship fake fixture markers as valid data.
- [ ] Run the final build/test/package checks from a clean checkout and prepare a candidate tag.

**Check:** candidate contents match the documented feature set; repository status contains no accidental artifacts or secrets.

### 63 — Trial the candidate

- [ ] Use only fake/disposable secrets through several normal CLI/TUI sessions and restarts.
- [ ] Record confusing prompts, navigation issues, failed saves, and installation problems; fix release blockers.
- [ ] Confirm the published security wording matches the actual review/testing evidence.

**Check:** all required acceptance cases still pass after candidate fixes.

### 64 — Buffer and v1.0 release

- [ ] Finish outstanding release blockers; defer cosmetic additions instead of skipping correctness checks.
- [ ] Tag and publish v1.0 with release notes, artifacts/build instructions, checksums, and review status.
- [ ] Create a short maintenance list: dependency updates, security reports, compatibility tests, and prioritized follow-up bugs.

**Done:** a fresh user can install, create an encrypted vault, use CLI or TUI, save/reopen it, change its password, and recover a backup using the documentation.

## After v1.0 — separate scope, no deadline

- [ ] Add multiline notes/recovery codes with an explicit payload-version decision and migration tests.
- [ ] Add tags, richer search, or optional shell completion based on actual use.
- [ ] Evaluate clipboard support only with a documented exposure/cleanup policy.
- [ ] Treat sync, browser integration, shared storage, and multi-user support as new designs with separate threat models.
