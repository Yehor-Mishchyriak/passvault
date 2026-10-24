# Documentation

We keep Passvault's documentation sources, including Markdown, configuration files, and assets, in `docs/`. Generated HTML and other documentation artifacts live under `build/docs/` and are excluded from Git.

As Passvault's interfaces become available, we'll add guides for each audience:

- `cli/`: command-line installation, commands, and examples.
- `python/`: Python installation, usage examples, and public API documentation.
- `tui/`: terminal UI navigation and workflows.
- `maintainers/`: architecture, contributing, building, and testing.

These guide directories are planned. For now, we provide a generated C++ reference for maintainers.

## Generate the C++ reference

With Doxygen installed, run from the project root:

```sh
doxygen docs/Doxyfile
```

You can read the generated reference at `build/docs/html/index.html`. The configuration resolves its input and output paths relative to the project root.

