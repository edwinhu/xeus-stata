#!/usr/bin/env python3
"""Comprehensive test for xeus-stata inline graph display."""

import jupyter_client
import time
import base64
import os

def execute_and_check(kc, code, expect_graph=False, test_name="Test"):
    """Execute code and check for graph output."""
    print(f"\n{test_name}")
    print("="*60)

    msg_id = kc.execute(code)

    messages = []
    timeout = 15
    start_time = time.time()
    idle_count = 0

    has_graph = False
    has_text_output = False

    while time.time() - start_time < timeout:
        try:
            msg = kc.get_iopub_msg(timeout=2)
            messages.append(msg)

            msg_type = msg['msg_type']
            content = msg['content']

            if msg_type == 'stream':
                text = content.get('text', '').strip()
                if text:
                    has_text_output = True
                    print(f"Output: {text[:200]}")

            elif msg_type == 'execute_result':
                data = content.get('data', {})
                if 'image/png' in data:
                    has_graph = True
                    img_data = data['image/png']
                    print(f"✓ Graph received: {len(img_data)} bytes (base64)")

                    # Try to decode base64 to verify it's valid
                    try:
                        decoded = base64.b64decode(img_data)
                        # Check PNG magic number
                        if decoded[:8] == b'\x89PNG\r\n\x1a\n':
                            print(f"✓ Valid PNG file ({len(decoded)} bytes)")
                        else:
                            print(f"✗ Not a valid PNG file")
                    except Exception as e:
                        print(f"✗ Base64 decode error: {e}")

            elif msg_type == 'error':
                print(f"✗ Error: {content.get('ename', '')}: {content.get('evalue', '')}")

            elif msg_type == 'status':
                if content.get('execution_state') == 'idle':
                    idle_count += 1
                    if idle_count >= 2:
                        break

        except Exception as e:
            if "Timeout" not in str(e):
                print(f"Error: {e}")
            break

    # Verify expectations
    if expect_graph:
        if has_graph:
            print("✓ Test passed: Graph displayed as expected")
        else:
            print("✗ Test failed: Expected graph but none received")
    else:
        if has_graph:
            print("✗ Test failed: Unexpected graph received")
        else:
            print("✓ Test passed: No graph as expected")

    return has_graph

def main():
    """Run comprehensive tests."""
    print("Starting comprehensive xeus-stata graph display tests...")

    # Create kernel
    km = jupyter_client.KernelManager(kernel_name='xeus-stata')
    km.start_kernel()
    kc = km.client()
    kc.start_channels()
    time.sleep(2)

    try:
        # Test 1: Simple scatter plot
        execute_and_check(
            kc,
            "sysuse auto, clear\nscatter mpg weight",
            expect_graph=True,
            test_name="Test 1: Simple scatter plot"
        )

        # Test 2: Text output only (no graph)
        execute_and_check(
            kc,
            "summarize price mpg weight",
            expect_graph=False,
            test_name="Test 2: Text output (no graph)"
        )

        # Test 3: Histogram
        execute_and_check(
            kc,
            "histogram price, normal",
            expect_graph=True,
            test_name="Test 3: Histogram"
        )

        # Test 4: Box plot
        execute_and_check(
            kc,
            "graph box price, over(foreign)",
            expect_graph=True,
            test_name="Test 4: Box plot"
        )

        # Test 5: Regression without graph
        execute_and_check(
            kc,
            "regress price mpg weight foreign",
            expect_graph=False,
            test_name="Test 5: Regression (no graph)"
        )

        print("\n" + "="*60)
        print("All tests completed!")
        print("="*60)

    finally:
        kc.stop_channels()
        km.shutdown_kernel()

if __name__ == '__main__':
    main()
