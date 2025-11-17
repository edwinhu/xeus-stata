#!/usr/bin/env python3
"""
Display a sample HTML output to verify formatting
"""

import jupyter_client
import time

# Connect to kernel
km, kc = jupyter_client.manager.start_new_kernel(kernel_name='xeus-stata')

try:
    time.sleep(2)

    # Load data
    kc.execute("sysuse auto, clear")
    while True:
        msg = kc.get_iopub_msg(timeout=10)
        if msg['msg_type'] == 'status' and msg['content']['execution_state'] == 'idle':
            break

    # Run summarize
    msg_id = kc.execute("summarize price mpg weight")
    found_output = False

    while True:
        msg = kc.get_iopub_msg(timeout=10)
        msg_type = msg['msg_type']

        if msg_type == 'execute_result':
            data = msg['content']['data']

            print("="*80)
            print("HTML OUTPUT:")
            print("="*80)
            print(data['text/html'])
            print("\n" + "="*80)
            print("PLAIN TEXT OUTPUT:")
            print("="*80)
            print(data['text/plain'])
            found_output = True

        elif msg_type == 'status' and msg['content']['execution_state'] == 'idle':
            if found_output:
                break
            # Continue waiting if we haven't seen output yet

finally:
    km.shutdown_kernel()
