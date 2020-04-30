#!/bin/bash

if [[ ! -f "compile_commands.json" ]]; then
    echo "No 'compile_commands.json' file found. Aborting."
    exit 1
fi

python3 ../tools/run-clang-tidy.py -clang-tidy-binary=clang-tidy-10 -clang-apply-replacements-binary=clang-apply-replacements-10 -quiet -header-filter='liballocator/lib/.*' -export-fixes=errors.yml
