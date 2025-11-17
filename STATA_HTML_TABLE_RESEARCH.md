# Stata HTML Table Export Research Summary

**Date**: 2025-11-17
**Purpose**: Research Stata's native HTML table export capabilities for xeus-stata integration
**Goal**: Determine if Stata's `table`, `etable`, `collect export` commands can provide better HTML tables than current approach

## Executive Summary

### Key Findings

1. **Stata 17+ has native HTML export** via `collect export` and `etable` commands
2. **HTML output is file-based only** - no direct stdout capture option
3. **Works for SOME commands** - estimation results, table, dtable (not summarize, tabulate, list)
4. **Produces proper HTML `<table>` elements** with inline CSS styling
5. **Requires Stata 17+** - not available in Stata 16 or earlier

### Recommendation

**Hybrid Approach**: Use Stata's native HTML export for estimation commands (where it excels), keep current simple formatting for other outputs (where it doesn't apply).

**Implementation Complexity**: Medium - requires command detection, temp file management, and HTML reading.

**Benefits vs. Current Approach**:
- ✅ Proper `<table>` HTML structure (vs. current `<pre>` approach)
- ✅ Better styling and formatting for regression tables
- ✅ Native Stata rendering ensures correctness
- ❌ More complex implementation
- ❌ Only works for subset of commands
- ❌ Requires Stata 17+ (version dependency)

---

## Research Details

### 1. Stata Commands with HTML Export

#### etable (Estimation Tables)

**Purpose**: Format and export regression/estimation results
**Stata Version**: 17+
**HTML Export**: Yes, via `export()` option

**Syntax**:
```stata
regress price mpg weight
estimates store model1

etable, estimates(model1) ///
    mstat(N) mstat(r2) ///
    cstat(_r_b, nformat(%9.3f)) ///
    cstat(_r_se, nformat(%9.3f)) ///
    showstars ///
    export(filename.html, tableonly replace)
```

**HTML Output**:
- Produces HTML `<table>` fragment (with `tableonly` option)
- Includes inline `<style>` block with CSS
- Formatted with proper headers, cells, borders
- Not a complete HTML document (embeddable)

**Works for**: `regress`, `logit`, `probit`, `poisson`, any estimation command that saves results

#### table + collect export (Summary Tables)

**Purpose**: Create custom tables with summary statistics, cross-tabs
**Stata Version**: 17+
**HTML Export**: Yes, via `collect export`

**Syntax**:
```stata
table (var), statistic(mean price mpg) statistic(sd price mpg)
collect style cell result[mean sd], nformat(%9.2f)
collect export filename.html, tableonly replace
```

**HTML Output**:
- Similar to etable (proper `<table>` structure)
- Customizable via `collect style` commands
- Works with `tableonly` option for embedding

**Works for**: Summary statistics, cross-tabulations, custom aggregations

#### dtable (Descriptive Tables)

**Purpose**: Descriptive statistics tables (mean, sd, min, max, etc.)
**Stata Version**: 17+
**HTML Export**: Yes, via `export()` option

**Syntax**:
```stata
dtable price mpg weight, by(foreign) ///
    export(filename.html, tableonly replace)
```

**HTML Output**:
- Proper `<table>` HTML with styling
- Works with `tableonly` for embedding

**Works for**: Descriptive statistics with grouping

### 2. Commands WITHOUT Native HTML Export

The following common Stata commands do NOT support the `collect`/`etable` system:

- ❌ `summarize` - basic summary statistics
- ❌ `tabulate` - frequency tables
- ❌ `list` - data listings
- ❌ `correlate` - correlation matrices
- ❌ `pwcorr` - pairwise correlations
- ❌ `display` - text output

**For these commands**: Current xeus-stata approach (styled `<pre>` wrapper) is appropriate.

### 3. HTML Output Format Analysis

#### With `tableonly` option:

**Example HTML output**:
```html
<style>
table {
  border-collapse: collapse;
  font-family: sans-serif;
  font-size: 12px;
}
th, td {
  border: 1px solid #ddd;
  padding: 8px;
  text-align: right;
}
th {
  background-color: #f2f2f2;
  font-weight: bold;
}
</style>
<table>
  <thead>
    <tr>
      <th></th>
      <th>(1)</th>
      <th>(2)</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>mpg</td>
      <td>-49.512***</td>
      <td>-43.214***</td>
    </tr>
    <!-- ... more rows ... -->
  </tbody>
</table>
```

**Characteristics**:
- Proper semantic HTML structure
- Inline CSS (no external dependencies)
- Professional styling
- Embeddable in larger documents
- No `<!DOCTYPE>`, `<html>`, `<head>`, `<body>` tags (that's what `tableonly` does)

#### Without `tableonly` option:

Produces complete HTML document with full structure (not suitable for notebook embedding).

### 4. Key Limitations

#### File-Based Only

**Critical limitation**: Stata's HTML export ONLY writes to files. There is no way to:
- Export to stdout
- Capture HTML in a Stata macro
- Return HTML directly

**Implication**: Must use temp files:
1. Generate unique temp filename
2. Execute: `etable, export(tempfile.html, tableonly)`
3. Read HTML from file
4. Delete temp file

#### Version Requirement

**Stata 17+ only**: The `collect`, `etable`, `dtable` commands were introduced in Stata 17 (2021).

**For Stata 16 and earlier**: Would need fallback to current approach or third-party commands (`estout`, `outreg2`, etc.)

#### Selective Applicability

Works well for:
- Regression results (via `etable`)
- Summary tables (via `table` + `collect`)
- Descriptive stats (via `dtable`)

Does NOT work for:
- Basic `summarize` output
- `tabulate` frequency tables
- `list` data listings
- Most other commands

---

## Implementation Plan for xeus-stata

### Proposed Approach: Dual-Path Strategy

#### Path 1: Native HTML Export (for supported commands)

**When to use**:
- After estimation commands (`regress`, `logit`, etc.)
- After `table` commands with collect
- After `dtable` commands

**Implementation**:

```cpp
// In stata_session.cpp, detect estimation commands
execution_result execute(const std::string& code) {
    // Detect if code contains estimation command
    bool is_estimation = detect_estimation_command(code);

    // Execute user code
    std::string output = run_stata_code(code);

    // If estimation command, generate HTML table
    if (is_estimation && estimation_successful(output)) {
        std::string html_table = export_estimation_html();
        result.html_table = html_table;
    }

    return result;
}

std::string export_estimation_html() {
    // Generate temp file
    std::string tempfile = generate_temp_filename(".html");

    // Run etable export
    std::string export_cmd = "quietly etable, ";
    export_cmd += "mstat(N) mstat(r2) ";
    export_cmd += "cstat(_r_b) cstat(_r_se) ";
    export_cmd += "showstars ";
    export_cmd += "export(\"" + tempfile + "\", tableonly replace)";

    run_stata_code(export_cmd);

    // Read HTML from file
    std::string html = read_file_contents(tempfile);

    // Delete temp file
    unlink(tempfile.c_str());

    return html;
}
```

**Command Detection**:
```cpp
bool detect_estimation_command(const std::string& code) {
    // List of estimation commands
    std::vector<std::string> est_commands = {
        "regress", "reg", "logit", "probit", "ologit", "oprobit",
        "poisson", "nbreg", "tobit", "heckman", "areg", "xtreg",
        "mixed", "melogit", "sem", "gsem", "ivregress"
    };

    // Check if code starts with estimation command
    for (const auto& cmd : est_commands) {
        std::regex pattern("^\\s*" + cmd + "\\s+");
        if (std::regex_search(code, pattern)) {
            return true;
        }
    }

    return false;
}
```

#### Path 2: Current Simple HTML (for other commands)

**When to use**:
- `summarize`, `tabulate`, `list`, `correlate`, etc.
- Any command not detected as estimation/table/dtable

**Implementation**:
- Keep existing `is_stata_table()` and `format_as_html_table()` functions
- Continue using styled `<pre>` wrapper approach
- No changes needed

### MIME Bundle Strategy

```cpp
// In xinterpreter.cpp
if (!exec_result.html_table.empty()) {
    // Native HTML table from etable/collect export
    nl::json display_data;
    display_data["text/plain"] = exec_result.output;
    display_data["text/html"] = exec_result.html_table;  // Proper <table> HTML
    publish_execution_result(execution_counter,
                            std::move(display_data),
                            nl::json::object());
} else if (is_stata_table(exec_result.output)) {
    // Fallback: styled <pre> wrapper
    nl::json display_data;
    display_data["text/plain"] = exec_result.output;
    display_data["text/html"] = format_as_html_table(exec_result.output);
    publish_execution_result(execution_counter,
                            std::move(display_data),
                            nl::json::object());
} else {
    // Regular text output
    publish_stream("stdout", exec_result.output);
}
```

### Version Detection

```cpp
int get_stata_version() {
    std::string result = run_stata_code("display c(stata_version)");
    return std::stoi(result);
}

bool supports_native_html() {
    static int version = get_stata_version();
    return version >= 17;
}
```

### Error Handling

```cpp
std::string export_estimation_html() {
    try {
        // Generate and export HTML
        // ...

        // Verify file exists and has content
        if (!file_exists(tempfile) || file_is_empty(tempfile)) {
            return "";  // Fall back to simple HTML
        }

        return html;
    } catch (...) {
        return "";  // Silent failure, use fallback
    }
}
```

---

## Testing Strategy

### Test Cases

#### Test 1: Estimation command with native HTML
```stata
sysuse auto, clear
regress price mpg weight foreign
```

**Expected**:
- Detects as estimation command
- Runs `etable, export(..., tableonly)`
- Returns proper `<table>` HTML
- Displays formatted table in notebook

#### Test 2: Non-estimation command with simple HTML
```stata
summarize price mpg weight
```

**Expected**:
- Not detected as estimation
- Uses current `format_as_html_table()`
- Returns styled `<pre>` HTML
- Displays in notebook (same as current)

#### Test 3: Multiple estimations
```stata
quietly regress price mpg weight
estimates store m1
quietly regress price mpg weight foreign
estimates store m2
etable, estimates(m1 m2)
```

**Expected**:
- Should work (etable with multiple models)
- Need to handle `estimates store` commands

#### Test 4: Stata 16 or earlier (if applicable)
```stata
regress price mpg weight
```

**Expected**:
- Detects Stata version < 17
- Skips native HTML export
- Falls back to simple HTML approach
- Works without errors

#### Test 5: Error in etable export
```stata
regress price mpg weight
* Force error by deleting estimation results
estimates clear
* Try to export anyway
```

**Expected**:
- `etable` fails gracefully
- Returns empty string
- Falls back to text output
- No crash

### Test Script

See `/home/eh2889/projects/xeus-stata/test_html_table_export.do` for comprehensive tests.

---

## Comparison: Native HTML vs. Current Approach

### Current Approach (Styled `<pre>`)

**Advantages**:
- ✅ Works for ALL commands
- ✅ Simple implementation
- ✅ No version dependency
- ✅ No temp file management
- ✅ Preserves exact Stata formatting

**Disadvantages**:
- ❌ Not semantic HTML (`<pre>` not `<table>`)
- ❌ Cannot sort/filter/interact with table
- ❌ Fixed monospace font
- ❌ Limited styling options

### Native HTML (etable/collect export)

**Advantages**:
- ✅ Proper semantic HTML (`<table>` structure)
- ✅ Better styling (Stata's official CSS)
- ✅ Potential for interactivity (sorting, filtering)
- ✅ Familiar to Stata users (matches Stata GUI output)
- ✅ Professional appearance

**Disadvantages**:
- ❌ Only works for subset of commands
- ❌ Requires Stata 17+
- ❌ More complex implementation
- ❌ Temp file overhead
- ❌ Potential for edge cases/errors

### Recommendation: Hybrid Approach

Use both:
1. **Native HTML for estimation/table/dtable** (where it works well)
2. **Simple HTML for everything else** (where native doesn't apply)

**Benefits**:
- Best of both worlds
- Gradual enhancement (works in Stata 16, better in Stata 17+)
- Maintains backward compatibility
- Professional output for regressions (most important use case)

---

## Implementation Checklist

### Phase 1: Command Detection
- [ ] Implement `detect_estimation_command()` function
- [ ] Add estimation command list (regress, logit, etc.)
- [ ] Test detection with various command formats
- [ ] Handle edge cases (comments, line breaks)

### Phase 2: HTML Export
- [ ] Implement `export_estimation_html()` function
- [ ] Add temp file generation and cleanup
- [ ] Test etable export with various options
- [ ] Handle errors gracefully (fallback to simple HTML)

### Phase 3: Version Detection
- [ ] Implement `get_stata_version()` function
- [ ] Add version check before native HTML export
- [ ] Test with Stata 17+ and Stata 16 (if available)
- [ ] Ensure graceful degradation

### Phase 4: Integration
- [ ] Modify `xinterpreter.cpp` to use native HTML when available
- [ ] Update MIME bundle construction
- [ ] Ensure both text/plain and text/html present
- [ ] Maintain backward compatibility

### Phase 5: Testing
- [ ] Run test script (`test_html_table_export.do`)
- [ ] Test with euporie/Jupyter notebooks
- [ ] Verify HTML rendering in browsers
- [ ] Test edge cases and error handling
- [ ] Performance testing (temp file overhead)

### Phase 6: Documentation
- [ ] Update README with Stata 17+ features
- [ ] Document version-specific behavior
- [ ] Add examples of native HTML output
- [ ] Note limitations and fallback behavior

---

## Alternative Approaches Considered

### Alternative 1: Parse Stata output and build `<table>` manually

**Approach**: Parse text output, detect columns/rows, construct HTML `<table>` in C++

**Rejected because**:
- Complex parsing logic (brittle, error-prone)
- Difficult to handle all table formats
- Reinventing Stata's wheel
- Native export is more reliable

### Alternative 2: Use third-party commands (estout, outreg2)

**Approach**: Require users to install `estout` or `outreg2` packages

**Rejected because**:
- Requires external dependencies
- Not part of base Stata
- Inconsistent across installations
- Native commands are better

### Alternative 3: Only use native HTML (no fallback)

**Approach**: Only provide HTML tables for commands that support etable/collect

**Rejected because**:
- Poor user experience for non-estimation commands
- Regression from current functionality
- Confusing (tables work sometimes, not others)
- Hybrid approach is better

---

## Open Questions

### Q1: Performance impact of temp file I/O?

**Question**: Does writing to temp file and reading back add noticeable latency?

**Investigation needed**:
- Benchmark temp file creation/read/delete
- Compare with current simple HTML approach
- Test with large tables (many coefficients)

**Hypothesis**: Minimal impact (< 10ms per execution)

### Q2: Which etable options to use by default?

**Question**: What default options for `etable` export?

**Current thinking**:
```stata
etable, mstat(N) mstat(r2) cstat(_r_b) cstat(_r_se) showstars
```

**Options to consider**:
- `showstars` - show significance stars (* ** ***)
- `showstarsnote` - include stars legend
- `mstat(N)` - show number of observations
- `mstat(r2)` - show R-squared
- Number formatting (`nformat()`)

**User customization**: Could add config option later

### Q3: How to handle `estimates store` workflows?

**Question**: Users often store multiple estimates and compare with `etable`

**Example**:
```stata
regress price mpg weight
estimates store m1
regress price mpg weight foreign
estimates store m2
etable, estimates(m1 m2)
```

**Challenge**: How to detect this pattern and export HTML?

**Possible solutions**:
1. Detect `etable` command directly (not just estimation commands)
2. Always try etable export after ANY command (check if e() results exist)
3. Let user explicitly call `etable` (no auto-export)

**Recommendation**: Detect both estimation commands AND explicit `etable` commands

### Q4: What about `table` and `dtable` commands?

**Question**: Should we auto-export HTML for `table` and `dtable` too?

**Current plan**: Start with `etable` only (estimation results are most important)

**Future enhancement**: Add `table` and `dtable` detection/export later

---

## References

### Stata Documentation
- [Tables of Estimation Results (Stata 17)](https://www.stata.com/stata17/tables-of-estimation-results/)
- [Customizable Tables in Stata 17](https://www.stata.com/stata17/tables/)
- [etable manual](https://www.stata.com/manuals/retable.pdf)
- [collect export manual](https://www.stata.com/manuals/tablescollectexport.pdf)

### Stata Blog Posts
- [Customizable tables in Stata 17, part 2: The new collect command](https://blog.stata.com/2021/06/07/customizable-tables-in-stata-17-part-2-the-new-collect-command/)
- [New FAQs about customizable tables](https://blog.stata.com/2024/03/13/new-faqs-about-customizable-tables-are-here/)

### Stata FAQs
- [Which versions of Stata have the table, dtable, etable, and collect commands?](https://www.stata.com/support/faqs/reporting/stata-versions-table-commands/)
- [What file formats are supported when exporting tables from Stata?](https://www.stata.com/support/faqs/reporting/export-customizable-table-formats/)
- [How can I export multiple tables to one file?](https://www.stata.com/support/faqs/reporting/export-multiple-tables/)

---

## Conclusion

### Summary

Stata 17+ provides native HTML table export via `etable` and `collect export` commands. This produces proper `<table>` HTML with professional styling, but:

1. **Only works for subset of commands** (estimation results, table, dtable)
2. **Requires file-based workflow** (write to temp file, read, delete)
3. **Version dependency** (Stata 17+)

### Recommendation

**Implement hybrid approach**:
- Use native HTML export for estimation commands (where it excels)
- Keep current simple HTML for other commands (where native doesn't apply)
- Detect Stata version and gracefully degrade if < 17

**Rationale**:
- Best user experience (proper tables for regressions)
- Maintains backward compatibility
- Gradual enhancement strategy
- Leverages Stata's native capabilities

### Next Steps

1. ✅ Research completed (this document)
2. ⏳ Create implementation plan (see checklist above)
3. ⏳ Implement Phase 1 (command detection)
4. ⏳ Test with Stata 17+ environment
5. ⏳ Iterate based on testing results

---

**Document created**: 2025-11-17
**Last updated**: 2025-11-17
**Status**: Research complete, implementation pending
