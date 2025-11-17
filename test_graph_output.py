#!/usr/bin/env python3
"""Test script for xeus-stata inline graph display."""

import jupyter_client
import time
import json

def test_graph_display():
    """Test that graphs are displayed inline with base64 encoding."""

    # Create a kernel manager
    km = jupyter_client.KernelManager(kernel_name='xeus-stata')
    km.start_kernel()

    # Create a client
    kc = km.client()
    kc.start_channels()

    # Wait for kernel to be ready
    time.sleep(2)

    try:
        # Test 1: Load data and create a scatter plot
        print("Test 1: Creating scatter plot...")
        code = """sysuse auto, clear
scatter mpg weight"""

        msg_id = kc.execute(code)

        # Collect messages
        messages = []
        timeout = 15  # seconds
        start_time = time.time()
        idle_count = 0

        while time.time() - start_time < timeout:
            try:
                msg = kc.get_iopub_msg(timeout=2)
                messages.append(msg)

                msg_type = msg['msg_type']
                content = msg['content']

                print(f"  [{msg_type}]", end="")

                if msg_type == 'stream':
                    text = content.get('text', '')
                    print(f" {text.strip()}")

                elif msg_type == 'execute_result':
                    print()
                    data = content.get('data', {})
                    for mime_type, value in data.items():
                        if mime_type == 'image/png':
                            # Check if it's base64 encoded
                            print(f"    {mime_type}: {len(value)} characters (base64)")
                            # Verify it looks like base64
                            if len(value) > 100 and all(c in 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=' for c in value[:100]):
                                print(f"    ✓ Image data appears to be valid base64")
                            else:
                                print(f"    ✗ Image data does not look like base64")
                        else:
                            print(f"    {mime_type}: {value[:100]}...")

                    metadata = content.get('metadata', {})
                    if metadata:
                        print(f"    Metadata: {json.dumps(metadata, indent=6)}")

                elif msg_type == 'error':
                    print()
                    print(f"    ename: {content.get('ename', '')}")
                    print(f"    evalue: {content.get('evalue', '')}")
                    for line in content.get('traceback', []):
                        print(f"    {line}")

                elif msg_type == 'status':
                    state = content.get('execution_state', '')
                    print(f" {state}")
                    if state == 'idle':
                        idle_count += 1
                        if idle_count >= 2:  # Wait for 2 idle messages
                            break
                else:
                    print()

            except Exception as e:
                if "Timeout" not in str(e):
                    print(f"  Error receiving message: {e}")
                else:
                    print(f"  Timeout waiting for messages")
                break

        # Summary
        print("\n" + "="*60)
        print("Summary:")
        execute_result_msgs = [m for m in messages if m['msg_type'] == 'execute_result']

        if execute_result_msgs:
            print(f"✓ Found {len(execute_result_msgs)} execute_result message(s)")
            for i, msg in enumerate(execute_result_msgs):
                data = msg['content'].get('data', {})
                if 'image/png' in data:
                    print(f"  ✓ Message {i+1} contains image/png data")
                else:
                    print(f"  ✗ Message {i+1} missing image/png data. Keys: {list(data.keys())}")
        else:
            print("✗ No execute_result messages found")
            print(f"  Message types received: {[m['msg_type'] for m in messages]}")

        print("="*60)

    finally:
        # Clean up
        kc.stop_channels()
        km.shutdown_kernel()

if __name__ == '__main__':
    test_graph_display()
