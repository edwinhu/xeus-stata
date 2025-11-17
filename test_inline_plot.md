# Testing Inline Plotting with xeus-stata

## Current Status
The xeus-stata kernel has placeholder code for graph detection, but doesn't yet automatically export/display graphs inline.

## Simple Test Commands

Run these in euporie console one at a time:

### 1. Load data and show summary
```stata
sysuse auto, clear
summarize price mpg weight
```

### 2. Create a simple scatter plot (will open in Stata GUI)
```stata
scatter mpg weight
```

### 3. Export plot manually to test file detection
```stata
scatter mpg weight
graph export "/tmp/test_plot.png", replace
```

### 4. Test with histogram
```stata
histogram price
graph export "/tmp/histogram.png", replace
```

## What Needs to Be Implemented

For automatic inline plotting, we need to modify `stata_session.cpp` to:

1. **Auto-export graphs**: After each command, check if a graph was created
2. **Use temp files**: Export to temporary PNG files
3. **Return file paths**: Include graph file paths in execution_result
4. **Display in Jupyter**: The xinterpreter.cpp already has code to publish graphs

The key is wrapping each user command with:
```stata
user_command_here
if (c(k) > 0) {
    graph export "/tmp/xeus_stata_graph_RANDOM.png", replace
}
```

This would automatically export any generated graph to a temp file that the kernel can detect and display.
