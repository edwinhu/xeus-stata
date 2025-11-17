#!/usr/bin/env python3
"""
Compare xeus-stata kernel output with raw Stata output.
"""

import json

# Load kernel output
with open('/home/eh2889/projects/xeus-stata/kernel_output_capture.json', 'r') as f:
    kernel_data = json.load(f)

# Get text/plain output from kernel
kernel_output = kernel_data['regression_output'][0]['data']['text/plain']

# Raw Stata output (extracted from log file lines 29-41)
stata_output = """      Source |       SS           df       MS      Number of obs   =        74
-------------+----------------------------------   F(1, 72)        =     29.42
       Model |   184233937         1   184233937   Prob > F        =    0.0000
    Residual |   450831459        72  6261548.04   R-squared       =    0.2901
-------------+----------------------------------   Adj R-squared   =    0.2802
       Total |   635065396        73  8699525.97   Root MSE        =    2502.3

------------------------------------------------------------------------------
       price | Coefficient  Std. err.      t    P>|t|     [95% conf. interval]
-------------+----------------------------------------------------------------
      weight |   2.044063   .3768341     5.42   0.000     1.292857    2.795268
       _cons |  -6.707353    1174.43    -0.01   0.995     -2347.89    2334.475
------------------------------------------------------------------------------"""

print("="*80)
print("KERNEL OUTPUT")
print("="*80)
print(kernel_output)
print()

print("="*80)
print("RAW STATA OUTPUT")
print("="*80)
print(stata_output)
print()

print("="*80)
print("BYTE-BY-BYTE COMPARISON")
print("="*80)

# Split into lines
kernel_lines = kernel_output.split('\n')
stata_lines = stata_output.split('\n')

print(f"Kernel has {len(kernel_lines)} lines")
print(f"Stata has {len(stata_lines)} lines")
print()

# Compare line by line
max_lines = max(len(kernel_lines), len(stata_lines))
differences = []

for i in range(max_lines):
    k_line = kernel_lines[i] if i < len(kernel_lines) else "<MISSING>"
    s_line = stata_lines[i] if i < len(stata_lines) else "<MISSING>"

    if k_line != s_line:
        differences.append((i, k_line, s_line))

if differences:
    print(f"Found {len(differences)} line differences:")
    print()

    for line_num, k_line, s_line in differences:
        print(f"Line {line_num}:")
        print(f"  Kernel: {repr(k_line)}")
        print(f"  Stata:  {repr(s_line)}")
        print()

        # Character-by-character comparison
        if k_line != "<MISSING>" and s_line != "<MISSING>":
            max_len = max(len(k_line), len(s_line))
            char_diffs = []
            for j in range(max_len):
                k_char = k_line[j] if j < len(k_line) else '<END>'
                s_char = s_line[j] if j < len(s_line) else '<END>'
                if k_char != s_char:
                    char_diffs.append((j, k_char, s_char))

            if char_diffs:
                print(f"  Character differences at positions: {[d[0] for d in char_diffs[:5]]}")
                for pos, k_char, s_char in char_diffs[:5]:
                    print(f"    Pos {pos}: kernel={repr(k_char)} stata={repr(s_char)}")
        print()
else:
    print("✓ No differences found! Outputs are identical.")

print("="*80)
print("SPACING ANALYSIS")
print("="*80)

# Analyze the first line to check spacing patterns
if kernel_lines:
    first_line = kernel_lines[0]
    print("First line character analysis:")
    print(repr(first_line))
    print()
    print("Character codes:")
    for i, char in enumerate(first_line[:50]):  # First 50 chars
        print(f"  {i:2d}: {repr(char)} (ord={ord(char)})")

print()
print("="*80)
print("HTML OUTPUT PREVIEW")
print("="*80)
html_output = kernel_data['regression_output'][0]['data']['text/html']
print(html_output[:1000])
