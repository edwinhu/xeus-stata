#!/usr/bin/env python3
"""
Test script for Phase 3: HTML table formatting in xeus-stata
Tests various Stata commands that produce table output
"""

import jupyter_client
import time

def test_stata_tables():
    """Test HTML table formatting with various Stata commands"""

    # Connect to kernel
    print("Starting xeus-stata kernel...")
    km, kc = jupyter_client.manager.start_new_kernel(kernel_name='xeus-stata')

    try:
        # Wait for kernel to be ready
        time.sleep(2)

        # Load sample dataset
        print("\n" + "="*80)
        print("Loading sample dataset...")
        print("="*80)
        msg_id = kc.execute("sysuse auto, clear")

        # Wait for completion
        while True:
            msg = kc.get_iopub_msg(timeout=10)
            if msg['msg_type'] == 'status' and msg['content']['execution_state'] == 'idle':
                break

        # Test 1: summarize (basic statistics table)
        print("\n" + "="*80)
        print("TEST 1: summarize - Basic statistics table")
        print("="*80)
        msg_id = kc.execute("summarize price mpg weight")

        test1_has_html = False
        test1_has_plain = False
        test1_output = ""

        while True:
            msg = kc.get_iopub_msg(timeout=10)
            msg_type = msg['msg_type']

            if msg_type == 'execute_result':
                data = msg['content']['data']
                print(f"Execute result received with MIME types: {list(data.keys())}")

                if 'text/html' in data:
                    test1_has_html = True
                    print("✓ HTML output present")
                    print(f"HTML length: {len(data['text/html'])} chars")
                    # Check for styling elements
                    if 'stata-output' in data['text/html']:
                        print("✓ HTML contains stata-output class")
                    if 'background-color' in data['text/html']:
                        print("✓ HTML contains CSS styling")

                if 'text/plain' in data:
                    test1_has_plain = True
                    test1_output = data['text/plain']
                    print("✓ Plain text output present")
                    print(f"Plain text preview:\n{test1_output[:200]}")

            elif msg_type == 'status' and msg['content']['execution_state'] == 'idle':
                break

        # Test 2: regress (regression table)
        print("\n" + "="*80)
        print("TEST 2: regress - Regression output table")
        print("="*80)
        msg_id = kc.execute("regress price mpg weight foreign")

        test2_has_html = False
        test2_has_plain = False
        test2_output = ""

        while True:
            msg = kc.get_iopub_msg(timeout=10)
            msg_type = msg['msg_type']

            if msg_type == 'execute_result':
                data = msg['content']['data']
                print(f"Execute result received with MIME types: {list(data.keys())}")

                if 'text/html' in data:
                    test2_has_html = True
                    print("✓ HTML output present")
                    print(f"HTML length: {len(data['text/html'])} chars")

                if 'text/plain' in data:
                    test2_has_plain = True
                    test2_output = data['text/plain']
                    print("✓ Plain text output present")
                    print(f"Plain text preview:\n{test2_output[:200]}")

            elif msg_type == 'status' and msg['content']['execution_state'] == 'idle':
                break

        # Test 3: tabulate (frequency table)
        print("\n" + "="*80)
        print("TEST 3: tabulate - Frequency table")
        print("="*80)
        msg_id = kc.execute("tabulate foreign")

        test3_has_html = False
        test3_has_plain = False
        test3_output = ""

        while True:
            msg = kc.get_iopub_msg(timeout=10)
            msg_type = msg['msg_type']

            if msg_type == 'execute_result':
                data = msg['content']['data']
                print(f"Execute result received with MIME types: {list(data.keys())}")

                if 'text/html' in data:
                    test3_has_html = True
                    print("✓ HTML output present")
                    print(f"HTML length: {len(data['text/html'])} chars")

                if 'text/plain' in data:
                    test3_has_plain = True
                    test3_output = data['text/plain']
                    print("✓ Plain text output present")
                    print(f"Plain text preview:\n{test3_output[:200]}")

            elif msg_type == 'status' and msg['content']['execution_state'] == 'idle':
                break

        # Test 4: list (data listing - should have table-like structure)
        print("\n" + "="*80)
        print("TEST 4: list - Data listing")
        print("="*80)
        msg_id = kc.execute("list make price mpg in 1/5")

        test4_has_html = False
        test4_has_plain = False
        test4_output = ""

        while True:
            msg = kc.get_iopub_msg(timeout=10)
            msg_type = msg['msg_type']

            if msg_type in ['execute_result', 'stream']:
                if msg_type == 'execute_result':
                    data = msg['content']['data']
                    print(f"Execute result received with MIME types: {list(data.keys())}")

                    if 'text/html' in data:
                        test4_has_html = True
                        print("✓ HTML output present")
                        print(f"HTML length: {len(data['text/html'])} chars")

                    if 'text/plain' in data:
                        test4_has_plain = True
                        test4_output = data['text/plain']
                        print("✓ Plain text output present")
                        print(f"Plain text preview:\n{test4_output[:200]}")
                else:
                    # Stream output
                    print(f"Stream output: {msg['content']['text'][:100]}")

            elif msg_type == 'status' and msg['content']['execution_state'] == 'idle':
                break

        # Test 5: Non-table output (should not be formatted as HTML)
        print("\n" + "="*80)
        print("TEST 5: display - Non-table output (should NOT be HTML)")
        print("="*80)
        msg_id = kc.execute("display \"Hello, world!\"")

        test5_has_html = False
        test5_has_plain = False
        test5_stream = False

        while True:
            msg = kc.get_iopub_msg(timeout=10)
            msg_type = msg['msg_type']

            if msg_type == 'execute_result':
                data = msg['content']['data']
                print(f"Execute result received with MIME types: {list(data.keys())}")

                if 'text/html' in data:
                    test5_has_html = True
                    print("✗ WARNING: HTML output present (should be plain text only)")

                if 'text/plain' in data:
                    test5_has_plain = True
                    print("✓ Plain text output present")

            elif msg_type == 'stream':
                test5_stream = True
                print(f"✓ Stream output: {msg['content']['text']}")

            elif msg_type == 'status' and msg['content']['execution_state'] == 'idle':
                break

        # Print summary
        print("\n" + "="*80)
        print("TEST SUMMARY")
        print("="*80)

        print("\nTest 1 (summarize):")
        print(f"  HTML: {'✓ PASS' if test1_has_html else '✗ FAIL'}")
        print(f"  Plain text: {'✓ PASS' if test1_has_plain else '✗ FAIL'}")

        print("\nTest 2 (regress):")
        print(f"  HTML: {'✓ PASS' if test2_has_html else '✗ FAIL'}")
        print(f"  Plain text: {'✓ PASS' if test2_has_plain else '✗ FAIL'}")

        print("\nTest 3 (tabulate):")
        print(f"  HTML: {'✓ PASS' if test3_has_html else '✗ FAIL'}")
        print(f"  Plain text: {'✓ PASS' if test3_has_plain else '✗ FAIL'}")

        print("\nTest 4 (list):")
        print(f"  Output received: {'✓ PASS' if (test4_has_html or test4_has_plain or test4_output) else '✗ FAIL'}")

        print("\nTest 5 (display - non-table):")
        print(f"  No HTML (correct): {'✓ PASS' if not test5_has_html else '✗ FAIL'}")
        print(f"  Has output: {'✓ PASS' if (test5_stream or test5_has_plain) else '✗ FAIL'}")

        overall_pass = (test1_has_html and test2_has_html and test3_has_html and
                       not test5_has_html)

        print("\n" + "="*80)
        if overall_pass:
            print("OVERALL: ✓ PHASE 3 TESTS PASSED")
        else:
            print("OVERALL: ⚠ SOME TESTS FAILED - Review output above")
        print("="*80)

    finally:
        # Shutdown kernel
        km.shutdown_kernel()
        print("\nKernel shut down.")

if __name__ == "__main__":
    test_stata_tables()
