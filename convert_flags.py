#!/usr/bin/env python3
"""
Convert palerain_flags from single integer to array-based bitmask.
This script updates all C source files to use the new PALERAIN_OPTION_* macros.
"""

import re
import sys
import os

def convert_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    original = content

    # Pattern 1: palerain_flags |= palerain_option_XXX  ->  PALERAIN_OPTION_SET(palerain_flags, palerain_option_XXX)
    content = re.sub(
        r'palerain_flags\s*\|\=\s*(palerain_option_\w+)',
        r'PALERAIN_OPTION_SET(palerain_flags, \1)',
        content
    )

    # Pattern 2: palerain_flags &= ~palerain_option_XXX  ->  PALERAIN_OPTION_CLEAR(palerain_flags, palerain_option_XXX)
    content = re.sub(
        r'palerain_flags\s*\&\=\s*~\s*(palerain_option_\w+)',
        r'PALERAIN_OPTION_CLEAR(palerain_flags, \1)',
        content
    )

    # Pattern 3: palerain_flags &= palerain_option_XXX  ->  (rare, but handle it)
    content = re.sub(
        r'palerain_flags\s*\&\=\s*(palerain_option_\w+)',
        r'PALERAIN_OPTION_AND(palerain_flags, \1)',  # Note: this won't work directly, need special handling
        content
    )

    # Pattern 4: (palerain_flags & palerain_option_XXX)  ->  PALERAIN_OPTION_TEST(palerain_flags, palerain_option_XXX)
    # But we need to be careful with the parentheses
    content = re.sub(
        r'\(palerain_flags\s*\&\s*(palerain_option_\w+)\)',
        r'PALERAIN_OPTION_TEST(palerain_flags, \1)',
        content
    )

    # Pattern 5: if (palerain_flags & palerain_option_XXX) -> if (PALERAIN_OPTION_TEST(palerain_flags, palerain_option_XXX))
    content = re.sub(
        r'if\s*\(palerain_flags\s*\&\s*(palerain_option_\w+)\)',
        r'if (PALERAIN_OPTION_TEST(palerain_flags, \1))',
        content
    )

    # Pattern 6: !(palerain_flags & palerain_option_XXX) -> !PALERAIN_OPTION_TEST(palerain_flags, palerain_option_XXX)
    content = re.sub(
        r'!\s*\(palerain_flags\s*\&\s*(palerain_option_\w+)\)',
        r'!PALERAIN_OPTION_TEST(palerain_flags, \1)',
        content
    )

    # Pattern 7: palerain_flags & palerain_option_XXX in other contexts (like ternary)
    content = re.sub(
        r'(?<!PALERAIN_OPTION_TEST\()(palerain_flags\s*\&\s*palerain_option_\w+)',
        lambda m: m.group(0).replace('palerain_flags & ', 'PALERAIN_OPTION_TEST(palerain_flags, ').replace(')', ')'),
        content
    )

    # Handle the print statement for flags
    content = re.sub(
        r'snprintf\(palerain_flags_cmd,\s*0x30,\s*"palera1n_flags 0x%" PRIx64 "%" PRIx64",\s*\(uint64_t\)\(palerain_flags\s*>>\s*64\),\s*\(uint64_t\)palerain_flags\);',
        r'snprintf(palerain_flags_cmd, 0x30, "palera1n_flags 0x%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64 "%016" PRIx64", palerain_flags[4], palerain_flags[3], palerain_flags[2], palerain_flags[1], palerain_flags[0]);',
        content
    )

    # Handle any remaining palerain_flags >> 64 patterns
    content = re.sub(
        r'\(uint64_t\)\(palerain_flags\s*>>\s*64\)',
        r'palerain_flags[1]',
        content
    )

    # Handle (uint64_t)palerain_flags (when used alone)
    content = re.sub(
        r'\(uint64_t\)palerain_flags(?!\s*[\[\(\w])',
        r'palerain_flags[0]',
        content
    )

    # Handle palerain_flags == 0 or !palerain_flags
    content = re.sub(
        r'!\s*palerain_flags\b',
        r'!PALERAIN_OPTION_ANY(palerain_flags)',
        content
    )

    content = re.sub(
        r'palerain_flags\s*==\s*0',
        r'!PALERAIN_OPTION_ANY(palerain_flags)',
        content
    )

    content = re.sub(
        r'palerain_flags\s*!=\s*0',
        r'PALERAIN_OPTION_ANY(palerain_flags)',
        content
    )

    if content != original:
        with open(filepath, 'w') as f:
            f.write(content)
        print(f"Updated: {filepath}")
        return True
    return False

def main():
    src_dir = '/Users/adrianoacosta/palera1n-for-usbliter8/src'
    updated = 0
    for filename in os.listdir(src_dir):
        if filename.endswith('.c'):
            filepath = os.path.join(src_dir, filename)
            if convert_file(filepath):
                updated += 1
    print(f"Total files updated: {updated}")

if __name__ == '__main__':
    main()