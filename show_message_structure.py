#!/usr/bin/env python3
"""
Display the complete Jupyter message structure from kernel.
"""

import json

# Load kernel output
with open('/home/eh2889/projects/xeus-stata/kernel_output_capture.json', 'r') as f:
    kernel_data = json.load(f)

print("="*80)
print("JUPYTER MESSAGE STRUCTURE FOR: reg price weight")
print("="*80)
print()

print("Full JSON structure:")
print(json.dumps(kernel_data['regression_output'], indent=2))
print()

print("="*80)
print("MESSAGE BREAKDOWN")
print("="*80)
print()

for idx, msg in enumerate(kernel_data['regression_output']):
    print(f"Message {idx + 1}:")
    print(f"  Type: {msg['type']}")
    print()

    if msg['type'] == 'execute_result':
        print("  MIME types available:")
        for mime_type in msg['data'].keys():
            print(f"    - {mime_type}")
        print()

        print("  Content lengths:")
        for mime_type, content in msg['data'].items():
            print(f"    - {mime_type}: {len(content)} characters")
        print()

        print("  text/plain content (first 200 chars):")
        print("    " + repr(msg['data']['text/plain'][:200]))
        print()

        print("  text/html content (first 300 chars):")
        print("    " + repr(msg['data']['text/html'][:300]))
        print()

print("="*80)
print("COMPARISON WITH sysuse OUTPUT")
print("="*80)
print()

print("sysuse auto, clear output:")
print(json.dumps(kernel_data['sysuse_output'], indent=2))
print()

print("Notice the difference:")
print("  - sysuse: Uses 'stream' message type (stdout)")
print("  - reg:    Uses 'execute_result' message type")
print()

print("This means:")
print("  - Simple text output → stream messages")
print("  - Formatted tables/results → execute_result messages with MIME types")
print()

print("="*80)
print("WHAT EUPORIE RECEIVES")
print("="*80)
print()

print("""
When euporie (or any Jupyter client) runs 'reg price weight', it receives:

1. status message: execution_state = 'busy'
2. execute_input message: echoes the code
3. execute_result message:
   {
     'execution_count': <n>,
     'data': {
       'text/plain': '<table as plain text>',
       'text/html': '<styled HTML table>'
     },
     'metadata': {}
   }
4. status message: execution_state = 'idle'

Euporie then chooses which MIME type to render based on:
- Terminal capabilities
- User preferences
- Available renderers

For text terminals, it typically uses text/plain.
For rich terminals (with HTML support), it could use text/html.
""")

print("="*80)
print("EXACT BYTES (FIRST LINE)")
print("="*80)
print()

text_plain = kernel_data['regression_output'][0]['data']['text/plain']
first_line = text_plain.split('\n')[0]

print(f"First line: {repr(first_line)}")
print()
print("Byte sequence:")
hex_bytes = ' '.join(f'{ord(c):02x}' for c in first_line)
print(f"  {hex_bytes}")
print()
print("Character breakdown:")
for i, char in enumerate(first_line):
    if i < 20 or i >= len(first_line) - 5:
        print(f"  [{i:2d}] {repr(char):5s} (0x{ord(char):02x} = {ord(char):3d})")
    elif i == 20:
        print("  ...")
