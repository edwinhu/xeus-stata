# Stata HTML Table Export - Quick Reference

**For xeus-stata development**

## TL;DR

- ✅ Stata 17+ has native HTML export: `etable, export(file.html, tableonly)`
- ❌ NO stdout option - must use temp files
- ✅ Works for: `regress`, `logit`, estimation commands
- ❌ Does NOT work for: `summarize`, `tabulate`, `list`
- 📋 Recommendation: Hybrid approach (native for regression, simple for rest)

---

## Syntax Examples

### Regression Results (etable)

```stata
regress price mpg weight
etable, export("table.html", tableonly replace)
```

### Multiple Models

```stata
regress price mpg weight
estimates store m1
regress price mpg weight foreign
estimates store m2
etable, estimates(m1 m2) export("table.html", tableonly)
```

### Summary Statistics (table + collect)

```stata
table (var), statistic(mean price mpg) statistic(sd price mpg)
collect export "table.html", tableonly replace
```

### Descriptive Stats (dtable)

```stata
dtable price mpg weight, by(foreign) export("table.html", tableonly)
```

---

## HTML Output Format

### With `tableonly` option:

```html
<style>
table { border-collapse: collapse; }
th, td { border: 1px solid #ddd; padding: 8px; }
</style>
<table>
  <thead><tr><th></th><th>Model 1</th></tr></thead>
  <tbody><tr><td>mpg</td><td>-49.512***</td></tr></tbody>
</table>
```

- Embeddable HTML fragment
- Inline CSS styling
- Proper `<table>` structure
- No `<!DOCTYPE>`, `<html>`, `<body>` tags

---

## Commands That Work vs. Don't Work

### ✅ Works (Stata 17+)

| Command | Export Method | Example |
|---------|---------------|---------|
| `regress` | `etable, export()` | Regression results |
| `logit` | `etable, export()` | Logistic regression |
| `probit` | `etable, export()` | Probit regression |
| `table` | `collect export` | Summary tables |
| `dtable` | `export()` option | Descriptive stats |

### ❌ Doesn't Work

| Command | Why | Alternative |
|---------|-----|-------------|
| `summarize` | Not in collect system | Current `<pre>` approach |
| `tabulate` | Not in collect system | Current `<pre>` approach |
| `list` | Not in collect system | Current `<pre>` approach |
| `correlate` | Not in collect system | Current `<pre>` approach |

---

## Implementation Pseudocode

```cpp
// Detect estimation command
if (code matches "regress|logit|probit|...") {
    // Execute user command
    run_stata_code(code);

    // Export to temp HTML file
    string temp = "/tmp/xeus_stata_table_XXXXX.html";
    run_stata_code("etable, export(\"" + temp + "\", tableonly)");

    // Read HTML from file
    string html = read_file(temp);

    // Clean up
    unlink(temp);

    // Send as MIME bundle
    publish_mime_bundle({
        "text/plain": text_output,
        "text/html": html
    });
}
else if (is_stata_table(output)) {
    // Use current simple HTML approach
    publish_mime_bundle({
        "text/plain": output,
        "text/html": format_as_html_table(output)
    });
}
else {
    // Regular text output
    publish_stream("stdout", output);
}
```

---

## Key Design Decisions

### 1. Temp File Workflow

**Problem**: Stata only exports to files (no stdout)

**Solution**:
1. Generate unique temp file: `/tmp/xeus_stata_table_XXXXX.html`
2. Run: `etable, export(tempfile, tableonly)`
3. Read file contents to string
4. Delete temp file immediately
5. Send HTML in MIME bundle

### 2. Command Detection

**Estimation commands to detect**:
- `regress`, `reg`
- `logit`, `logistic`
- `probit`
- `ologit`, `oprobit`
- `poisson`, `nbreg`
- `xtreg`, `areg`, `mixed`
- `ivregress`, `ivreg`

**Detection method**: Regex matching at start of command (after whitespace/comments)

### 3. Version Compatibility

**Stata 17+**: Use native HTML export
**Stata 16 and earlier**: Fall back to simple `<pre>` HTML

**Version detection**:
```stata
display c(stata_version)
```

### 4. Error Handling

**Scenarios**:
- `etable` fails (no estimation results)
- Temp file not created
- File read error
- HTML parsing issues

**Strategy**: Silent fallback to simple HTML (no user-facing errors)

---

## Comparison Matrix

| Feature | Current (`<pre>`) | Native (`<table>`) |
|---------|-------------------|-------------------|
| **Semantic HTML** | ❌ | ✅ |
| **Works for all commands** | ✅ | ❌ |
| **Stata version** | Any | 17+ |
| **Implementation complexity** | Simple | Medium |
| **Professional styling** | Basic | Advanced |
| **Sortable/interactive** | ❌ | Potentially |
| **File I/O required** | ❌ | ✅ |

---

## Testing Checklist

- [ ] Test `regress` command → native HTML
- [ ] Test `summarize` command → simple HTML
- [ ] Test multiple models with `estimates store`
- [ ] Test explicit `etable` command
- [ ] Test Stata 16 (if available) → fallback
- [ ] Test error cases (no estimation results)
- [ ] Verify temp file cleanup
- [ ] Check HTML rendering in Jupyter
- [ ] Performance test (latency with temp files)

---

## References

- Research doc: `/home/eh2889/projects/xeus-stata/STATA_HTML_TABLE_RESEARCH.md`
- Test script: `/home/eh2889/projects/xeus-stata/test_html_table_export.do`
- Stata docs: https://www.stata.com/stata17/tables-of-estimation-results/
