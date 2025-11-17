#!/usr/bin/env python3
"""
Show hex dump of kernel output to see exact bytes.
"""

import json

# Load kernel output
with open('/home/eh2889/projects/xeus-stata/kernel_output_capture.json', 'r') as f:
    kernel_data = json.load(f)

text_plain = kernel_data['regression_output'][0]['data']['text/plain']

print("="*80)
print("HEX DUMP OF KERNEL OUTPUT (text/plain)")
print("="*80)
print()

# Show first 200 bytes
bytes_data = text_plain.encode('utf-8')
print(f"Total bytes: {len(bytes_data)}")
print(f"Total characters: {len(text_plain)}")
print()

print("First 200 bytes (hex and ASCII):")
print()

for i in range(0, min(200, len(bytes_data)), 16):
    # Hex representation
    hex_part = ' '.join(f'{b:02x}' for b in bytes_data[i:i+16])
    # ASCII representation
    ascii_part = ''.join(chr(b) if 32 <= b < 127 else '.' for b in bytes_data[i:i+16])
    print(f"{i:04x}:  {hex_part:<48}  {ascii_part}")

print()
print("="*80)
print("LINE-BY-LINE BYTE ANALYSIS (first 5 lines)")
print("="*80)
print()

lines = text_plain.split('\n')
for i, line in enumerate(lines[:5]):
    print(f"Line {i}: {len(line)} characters")
    print(f"  Text: {repr(line)}")
    print(f"  Hex:  {' '.join(f'{ord(c):02x}' for c in line[:40])}")
    if len(line) > 40:
        print(f"        ... (truncated)")
    print()

print("="*80)
print("CHECKING FOR SPECIAL CHARACTERS")
print("="*80)
print()

# Check for ANSI codes
if '\x1b' in text_plain:
    print("✗ Contains ANSI escape codes")
else:
    print("✓ No ANSI escape codes")

# Check for tabs
if '\t' in text_plain:
    print("✗ Contains tab characters")
    tab_count = text_plain.count('\t')
    print(f"  Found {tab_count} tabs")
else:
    print("✓ No tab characters")

# Check for carriage returns
if '\r' in text_plain:
    print("✗ Contains carriage returns")
    cr_count = text_plain.count('\r')
    print(f"  Found {cr_count} carriage returns")
else:
    print("✓ No carriage returns")

# Count spaces
space_count = text_plain.count(' ')
newline_count = text_plain.count('\n')
print(f"✓ Total spaces: {space_count}")
print(f"✓ Total newlines: {newline_count}")

print()
print("="*80)
print("HTML OUTPUT (first 500 chars)")
print("="*80)
print()

html_output = kernel_data['regression_output'][0]['data']['text/html']
print(html_output[:500])
print("...")
