exeExt := if os() == "windows" { ".exe" } else { "" }
build_dir := 'build'
exe := build_dir / "aoc2015" + exeExt
test_exe := build_dir / "aoc2015-test" + exeExt
cc := 'clang'
src_dir := 'src'
main := src_dir / 'main.c'
test_main := src_dir / 'test.c'
strip_flags := if os() == "windows" {""} else {"-Wl,-s"}
release_flags := "-O3 " + strip_flags

cc_flags := '-D_CRT_SECURE_NO_WARNINGS'

[private]
build-command arg_flags:
    {{cc}} {{cc_flags}} {{arg_flags}} -o {{exe}} {{main}}

build: (build-command "-g")

build-release: (build-command release_flags)

run day:
    {{exe}} {{day}}

test:
    {{cc}} {{cc_flags}} -g -o {{test_exe}} {{test_main}}
    {{test_exe}}
