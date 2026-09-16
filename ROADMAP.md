# Passvault: from first program to encrypted vault

Everything below is **planned**, not implemented. Check items off only after the
behavior works. These are future changelogs: when you release a version, copy its
completed items into `CHANGELOG.md` with a date.

Your first MVP is **v0.1.0**, targeted at 2–3 hours. **v1.0.0** is the later milestone
where the pieces form an encrypted local vault. A version number is not a security
certification. Use fake credentials throughout development.

## Start here: v0.1.0 — a working terminal session

**Goal:** launch the program, add two fake secrets, list their names, retrieve one,
and quit. Entries live only in memory and disappear when the program exits.

### Planned changelog

- [ ] Added an interactive prompt with `help`, `add`, `list`, `get`, and `quit`.
- [ ] Added entries containing a unique name, a username, and a secret.
- [ ] Added prompts for each field after `add`; `get` prompts for an exact name.
- [ ] Added name-only listing; secrets appear only when explicitly retrieved.
- [ ] Added messages for unknown commands, missing entries, and duplicate names.
- [ ] Added clean exit on end-of-input (Ctrl-D on macOS/Linux).

### Keep the implementation small

Start with **one `main.cpp`**. Use:

- `std::string`: a string of characters for commands and fields.
- An `Entry` struct: one record grouping the three fields.
- `std::vector<Entry>`: a growable array holding your records.
- `std::getline`: read an entire input line, including spaces.
- A loop and `if`/`else` branches to handle commands.
- A simple loop over entries to find a matching name.

Require nonempty names and secrets; allow an empty username. Names are unique and
case-sensitive. A duplicate `add` must leave the existing entry unchanged. If
input ends halfway through `add`, exit without adding an incomplete entry.

Read the command on its own line, then prompt for its arguments. You do not need
to parse commands such as `add github user password` yet.

Your first action: create `main.cpp`, print `passvault> `, and read one line. Compile
and run that before adding anything else. This machine has Apple Clang available;
after creating your source file, build from the project directory with:

```sh
clang++ -std=c++20 -Wall -Wextra -Wpedantic -g main.cpp -o passvault
./passvault
```

### A 2–3 hour session

| Time | Small target |
| --- | --- |
| 0–20 min | Compile; make the prompt, `help`, and `quit` work. |
| 20–50 min | Define `Entry`; implement `add` and keep entries in a vector. |
| 50–80 min | Implement `list` and exact-name `get`. |
| 80–110 min | Handle duplicates, empty fields, unknown commands, and Ctrl-D. |
| 110–150 min | Run the checks below, fix issues, and document how to build. |
| 150–180 min | Buffer for learning and debugging; ship the completed slice. |

These are estimates, not deadlines. If you get stuck, reduce the slice to
`add`, `list`, and `quit`, and move `get` into v0.1.1. Do not spend this session on
encryption, files, CMake, classes with inheritance, or a polished terminal UI.

### Done when you can demonstrate this

1. Start with no entries: `list` reports an empty vault.
2. Add `example-site`, username `alice`, secret `fake password 123`.
3. Add a second entry; `list` prints both names without their secrets.
4. Retrieve `example-site`; the secret still contains its spaces.
5. Try adding `example-site` again; the original entry is unchanged.
6. Retrieve a missing name and enter an unknown command; both give useful errors.
7. Quit and restart; the vault is empty, as documented.
8. Try Ctrl-D at the command prompt and during `add`; neither loops forever.

**Ship:** a buildable program, a short README example using fake values, and a
v0.1.0 changelog entry. Inspect your Git diff before committing; never commit real
secrets or generated vault files.

## v0.2.0 — edit and delete entries

**Learn:** functions, references, and modifying a vector. Aim for one short session.

### Planned changelog

- [ ] Added `edit` for username and secret, keeping the entry name fixed.
- [ ] Added `remove` with an explicit confirmation prompt.
- [ ] Moved repeated lookup and command handling into small named functions.
- [ ] Added repeatable checks for the entry operations, separate from terminal input.

**Done when:** editing changes only the selected record; cancelling removal changes
nothing; deleting the first, middle, last, or only entry works; missing names are
handled. Everything still lives in memory.

## v0.3.0 — encode and decode the vault in memory

**Learn:** serialization (turning records into bytes) and parsing (reading them back).
Budget more than one session if a library or build setup is new to you.

### Planned changelog

- [ ] Added a versioned data format for entries using an established JSON library.
- [ ] Added conversion from entries to a string and back, without writing a file.
- [ ] Added validation for missing fields, wrong field types, and duplicate names.
- [ ] Added a minimal CMake build once managing source files/dependencies warrants it.

**Done when:** encoding and then decoding preserves every field, including quotes,
backslashes, and spaces. Invalid input reports an error and leaves the current
entries unchanged. Keep plaintext serialization in memory; it will become the
input to encryption.

## v0.4.0 — encrypt and decrypt a test payload

**Learn:** using a cryptographic library and checking every failure result.
Treat this as several small sessions, not another 2-hour deadline.

### Planned changelog

- [ ] Integrated libsodium and checked library initialization.
- [ ] Derived an encryption key from a master password using `crypto_pwhash`.
- [ ] Encrypted/decrypted the serialized vault with authenticated encryption.
- [ ] Defined a versioned envelope containing the derivation algorithm, parameters,
      salt, nonce, and ciphertext needed to reopen it.
- [ ] Added rejection of incorrect passwords and modified ciphertext.

A salt is random input to password-based key derivation. A nonce is a per-encryption
value that must not repeat with the same key. Authenticated encryption checks for
tampering as well as hiding the contents. Use library-generated randomness, a new
salt when establishing a new password-derived key, and a fresh nonce on every
encryption. Do not implement your own cipher or use `std::hash` to derive a key.

Read the official [key derivation API](https://doc.libsodium.org/password_hashing/default_phf)
and [authenticated encryption API](https://doc.libsodium.org/secret-key_cryptography/secretbox)
when implementing this stage. Follow their parameter and error-handling requirements.

**Done when:** the right password recovers the original records; a wrong password,
modified ciphertext, malformed header, or unsupported format fails without
returning entries. Validate and bound header sizes and derivation costs before
using them. Keep this exercise in memory until these checks pass.

## v0.5.0 — save and reopen an encrypted local vault

**Learn:** binary file I/O, filesystem errors, and preserving existing data.
Split this into creation, loading, and replacement sessions.

### Planned changelog

- [ ] Added explicit vault creation that refuses to overwrite an existing file.
- [ ] Added opening an existing vault with a prompted master password.
- [ ] Added saving the encrypted envelope; no plaintext temporary files.
- [ ] Added replacement through a temporary file in the same directory, checking
      writes and close before renaming it over the old vault.
- [ ] Added owner-only permissions for vault and temporary files on macOS/Linux.
- [ ] Added clear missing-file, invalid-file, and save-failure errors.
- [ ] Documented single-process, local-filesystem use; shared HPC filesystem behavior
      and concurrent writers are outside this release's scope.

**Done when:** entries survive a restart, opening with a wrong password never
overwrites anything, and a simulated write failure leaves the previous vault usable.
A failed save must not print success. Atomic replacement alone is not a promise
of durability after power loss; document this limitation until addressed and tested.

## v0.6.0 — safer terminal interaction

**Learn:** terminal state, cleanup on scope exit, and explicit secret exposure.

### Planned changelog

- [ ] Disabled terminal echo for master-password and secret entry on supported systems.
- [ ] Restored terminal settings on normal exit and handled error/interrupt paths.
- [ ] Kept secrets out of command-line arguments, routine logs, and error messages.
- [ ] Made revealing a stored secret an explicit action with a scrollback notice.
- [ ] Reduced unnecessary secret copies and wiped dedicated key buffers using the
      library's supported facilities; documented remaining memory limitations.

**Done when:** typed secrets are hidden, terminal echo returns after tested failures
and interruptions, and ordinary listing/error output exposes no secret values.
Do not claim that clearing a `std::string` securely erases its previous contents.

## v1.0.0 — a documented encrypted-vault learning release

**Goal:** stabilize the existing workflow. Add no large new features here.

### Planned changelog

- [ ] Documented installation, build, initialization, unlock, add, list, get, edit,
      remove, and save behavior using fake credentials.
- [ ] Added a repeatable test command covering record operations, serialization,
      wrong passwords, tampering, truncated files, and failed saves.
- [ ] Verified the build and basic terminal workflow on macOS and Linux over SSH.
- [ ] Documented encrypted backup and tested restoring a copy without overwriting
      the working vault.
- [ ] Documented file-format version handling and refusal of unsupported versions.
- [ ] Documented the threat model: intended to protect a locked vault file; does not
      protect against a compromised host, keylogger, or secrets deliberately revealed.
- [ ] Documented that a forgotten master password has no recovery mechanism.
- [ ] Reviewed the cryptographic integration and storage failure paths, recording
      unresolved issues and whether any independent review has occurred.

**Done when:** a fresh checkout builds using the README, the checks pass, and a full
create → add → save → restart → unlock → retrieve workflow works. This remains an
unaudited learning project; do not treat completing a checklist as proof that it is
ready to hold important credentials.

## After v1.0 — pick one feature at a time

These are optional directions, not commitments:

| Version | Planned change | Small completion check |
| --- | --- | --- |
| v1.1 | Search names and attach tags. | Results match expected entries without exposing secrets. |
| v1.2 | Generate passwords using library cryptographic randomness. | Requested lengths and allowed character rules hold. |
| v1.3 | Change the master password. | New password opens the replacement; old password fails; failed replacement preserves the old file. |
| v1.4 | Support multiline notes and recovery codes. | Save/reopen preserves line breaks exactly. |
| v1.5 | Add command-style CLI operations for developer/SSH workflows. | Errors have meaningful exit codes; secrets still never appear in arguments. |

Defer cloud sync, browser extensions, clipboard integration, multi-user sharing,
and shared HPC storage until the local workflow is reliable and their requirements
are understood. They each introduce substantial separate design work.

## How to keep shipping

1. Pick the next unchecked behavior in the current version.
2. Write down one example of its expected input and output.
3. Implement it yourself; compile and try that example immediately.
4. Check a failure case, then make a focused commit.
5. Release when that version's completion checks pass. Move unfinished scope forward.

Use patch versions for small repairs, such as v0.2.1 for a deletion bug. Later
milestones can span multiple sessions and commits. You do not need to understand
the whole roadmap before starting: **today's job is only v0.1.0.**
