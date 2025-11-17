#!/usr/bin/env python3
"""
Capture exact output from xeus-stata kernel for regression command.
"""

import jupyter_client
import time
import json
import sys

def capture_kernel_output():
    """Start xeus-stata kernel and capture regression output."""

    print("Starting xeus-stata kernel...")
    km = jupyter_client.KernelManager(kernel_name='xeus-stata')
    km.start_kernel()
    kc = km.client()
    kc.start_channels()

    # Wait for kernel to be ready
    print("Waiting for kernel to be ready...")
    try:
        kc.wait_for_ready(timeout=10)
        print("Kernel ready!")
    except RuntimeError:
        print("Kernel failed to start!")
        return

    def execute_and_capture(code):
        """Execute code and capture all outputs."""
        print(f"\n{'='*80}")
        print(f"Executing: {code}")
        print('='*80)

        msg_id = kc.execute(code)
        outputs = []

        # Collect all messages
        while True:
            try:
                msg = kc.get_iopub_msg(timeout=5)
                msg_type = msg['header']['msg_type']
                content = msg['content']

                print(f"\nMessage type: {msg_type}")

                if msg_type == 'status' and content['execution_state'] == 'idle':
                    break

                if msg_type == 'stream':
                    print(f"Stream ({content['name']}):")
                    print(repr(content['text']))  # Show raw bytes/escapes
                    outputs.append({
                        'type': 'stream',
                        'name': content['name'],
                        'text': content['text']
                    })

                elif msg_type == 'execute_result':
                    print("Execute result:")
                    print(f"Data keys: {content['data'].keys()}")
                    outputs.append({
                        'type': 'execute_result',
                        'data': content['data']
                    })

                    # Show each MIME type
                    for mime_type, data in content['data'].items():
                        print(f"\n--- {mime_type} ---")
                        print(repr(data))  # Show raw representation

                elif msg_type == 'display_data':
                    print("Display data:")
                    print(f"Data keys: {content['data'].keys()}")
                    outputs.append({
                        'type': 'display_data',
                        'data': content['data']
                    })

                    # Show each MIME type
                    for mime_type, data in content['data'].items():
                        print(f"\n--- {mime_type} ---")
                        print(repr(data))

                elif msg_type == 'error':
                    print("Error:")
                    print(f"ename: {content['ename']}")
                    print(f"evalue: {content['evalue']}")
                    print(f"traceback: {content['traceback']}")
                    outputs.append({
                        'type': 'error',
                        'ename': content['ename'],
                        'evalue': content['evalue'],
                        'traceback': content['traceback']
                    })

            except Exception as e:
                print(f"Timeout or error getting message: {e}")
                break

        return outputs

    # Execute commands
    print("\n" + "="*80)
    print("COMMAND 1: Load dataset")
    print("="*80)
    outputs1 = execute_and_capture('sysuse auto, clear')

    print("\n" + "="*80)
    print("COMMAND 2: Run regression")
    print("="*80)
    outputs2 = execute_and_capture('reg price weight')

    # Save detailed output to file
    print("\n" + "="*80)
    print("Saving detailed output to file...")
    print("="*80)

    with open('/home/eh2889/projects/xeus-stata/kernel_output_capture.json', 'w') as f:
        json.dump({
            'sysuse_output': outputs1,
            'regression_output': outputs2
        }, f, indent=2)

    print("\nSaved to: /home/eh2889/projects/xeus-stata/kernel_output_capture.json")

    # Show summary
    print("\n" + "="*80)
    print("SUMMARY - Regression Output")
    print("="*80)

    for output in outputs2:
        if output['type'] == 'execute_result':
            print("\nMIME types available:", list(output['data'].keys()))

            if 'text/plain' in output['data']:
                print("\n--- text/plain (raw bytes) ---")
                text = output['data']['text/plain']
                print(repr(text))
                print("\n--- text/plain (rendered) ---")
                print(text)

            if 'text/html' in output['data']:
                print("\n--- text/html (first 500 chars) ---")
                html = output['data']['text/html']
                print(html[:500])

    # Cleanup
    print("\nShutting down kernel...")
    kc.stop_channels()
    km.shutdown_kernel()
    print("Done!")

if __name__ == '__main__':
    capture_kernel_output()
