# JShell

JShell is a posix(ish) toy shell with most of the commonly used features and minimal dependencies (only readline to be exact).

This started as a "Build your own shell" challenge by [codecrafters](https://app.codecrafters.io/courses/shell/overview) to explore c and learn more about stuff like parsers and memory management. Currently, the end goal is to make it fully POSIX compliant and add some more QOL features.

# Quickstart
- This project uses Makefile and stores release binary in `build/release/shell` so you can just run below commands:
```
git clone https://github.com/jaydamani/JShell
cd JShell
make
build/release/shell
```
> [!NOTE]
> Currently, only unix systems are supported.

# Highlights
- Persistent history accross setions[^*]
- Autocomplete for command names and file paths
- Redirect output with stderr and chaining support
```
cmd arg1 arg2 > out 2> err_our
```
[^*]: history is stored in path from HISTFILE environment variables
