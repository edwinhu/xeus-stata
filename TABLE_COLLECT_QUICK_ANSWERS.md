# Quick Answers: Stata Table Collection System

**Last Updated**: 2025-11-17

---

## Question 1: Does `table` work for estimation results AND summary statistics?

### Summary Statistics: ✅ YES

```stata
table (var), statistic(mean price mpg) statistic(sd price mpg)
collect export "summary.html", tableonly replace
```

**Works perfectly** - this is what `table` is designed for.

### Estimation Results: ⚠️ USE `etable` INSTEAD

```stata
* ❌ DON'T: Awkward with table
collect clear
regress price mpg weight
collect get
table  // Requires manual layout, awkward

* ✅ DO: Use etable
regress price mpg weight
etable, export("results.html", tableonly)  // Clean, automatic
```

**Technically possible** with `table` but `etable` is purpose-built for regressions.

### Bottom Line

| Task | Use This | Not This |
|------|----------|----------|
| Regression results | `etable` | `table` |
| Summary statistics | `table` | `etable` |
| Cross-tabulations | `table` | `etable` |
| Multiple models | `etable` | `table` |

---

## Question 2: Can we capture HTML to stdout (no files)?

### Answer: ❌ ABSOLUTELY NOT

**Every method requires a file**:
- `etable, export(filename.html)` → File only
- `collect export filename.html` → File only
- `dtable, export(filename.html)` → File only

**No alternatives exist**:
- ❌ No stdout option
- ❌ No macro capture
- ❌ No variable storage
- ❌ No memory buffer
- ❌ No return value
- ❌ No Mata workaround

**Must use temp file workflow**:

```cpp
// 1. Create temp file
char temp[] = "/tmp/xeus_stata_XXXXXX.html";
mkstemp(temp);

// 2. Export to file
stata("etable, export('" + temp + "', tableonly)");

// 3. Read file
string html = read_file(temp);

// 4. Delete file
unlink(temp);

// 5. Use HTML
publish_mime_bundle({"text/html": html});
```

**No exceptions, no workarounds, no shortcuts.**

---

## Question 3: What can `table` command do?

### ✅ CAN DO (Works Well)

```stata
* Summary statistics
table (var), statistic(mean x) statistic(sd x)

* Cross-tabulations
table foreign rep78, statistic(mean price)

* Multi-way tables
table (foreign) (rep78), statistic(count price)

* Custom aggregations
table (var), statistic(median x) statistic(p25 x) statistic(p75 x)

* Export to HTML
collect export "output.html", tableonly replace
```

### ⚠️ CAN DO (But Awkward)

```stata
* Regression results (use etable instead)
collect clear
regress price mpg weight
collect get
collect layout (colname) (result)
table
```

### ❌ CANNOT DO

```stata
* Traditional summarize output
summarize price mpg weight  // Different format, cannot replicate

* Traditional tabulate (Stata <19)
tabulate foreign  // Use table with statistic() instead

* Data listings
list price mpg in 1/10  // table is not for listing data

* Correlation matrices
correlate price mpg weight  // Not in collect system

* Export to stdout
* (see Question 2)
```

---

## Question 4: Full scope comparison

### `table` vs `etable` vs `dtable`

| Feature | `table` | `etable` | `dtable` |
|---------|---------|----------|----------|
| **Summary stats** | ✅ Best | ❌ No | ⚠️ Limited |
| **Regression** | ⚠️ Awkward | ✅ Best | ❌ No |
| **Cross-tabs** | ✅ Best | ❌ No | ❌ No |
| **Descriptive stats by group** | ✅ Good | ❌ No | ✅ Best |
| **Multiple models** | ⚠️ Manual | ✅ Auto | ❌ No |
| **HTML export** | ✅ Yes | ✅ Yes | ✅ Yes |
| **Custom layouts** | ✅ Powerful | ⚠️ Limited | ⚠️ Limited |

### When to Use Each

```stata
* Use etable for: REGRESSIONS
regress price mpg weight
etable, export("reg.html", tableonly)

* Use table for: SUMMARIES & CROSS-TABS
table (var), statistic(mean price mpg)
collect export "summary.html", tableonly

* Use dtable for: DESCRIPTIVE STATS BY GROUP
dtable price mpg weight, by(foreign) export("desc.html", tableonly)
```

---

## Comprehensive Scope

### What Works with `collect export`

| Command | Collect Support | HTML Export | Notes |
|---------|-----------------|-------------|-------|
| `regress` | ✅ via `etable` | ✅ Yes | Use etable, not table |
| `logit` | ✅ via `etable` | ✅ Yes | All estimation commands |
| `table` | ✅ Native | ✅ Yes | Summary stats, cross-tabs |
| `dtable` | ✅ Native | ✅ Yes | Descriptive stats |
| `summarize` | ❌ No | ❌ No | Use table instead |
| `tabulate` | ⚠️ Stata 19+ | ⚠️ Stata 19+ | Use table instead |
| `list` | ❌ No | ❌ No | No collect support |
| `correlate` | ❌ No | ❌ No | No collect support |

### Stata Version Requirements

| Feature | Version | Notes |
|---------|---------|-------|
| `table` command | 17+ | Redesigned in Stata 17 |
| `etable` command | 17+ | New in Stata 17 |
| `dtable` command | 17+ | New in Stata 17 |
| `collect` system | 17+ | Entire system new in 17 |
| `tabulate, collect` | 19+ | Enhancement in Stata 19 |
| `anova` + collect | 19+ | Enhancement in Stata 19 |

---

## Implementation Examples

### Example 1: Summary Statistics

```stata
sysuse auto, clear

* Create summary table
table (var), statistic(mean price mpg weight) ///
              statistic(sd price mpg weight) ///
              statistic(min price mpg weight) ///
              statistic(max price mpg weight)

* Style and export
collect style cell result, nformat(%9.2f)
collect export "summary.html", tableonly replace
```

### Example 2: Regression Results

```stata
sysuse auto, clear

* Run regressions
quietly regress price mpg weight
estimates store m1
quietly regress price mpg weight foreign
estimates store m2

* Create table and export
etable, estimates(m1 m2) ///
    mstat(N) mstat(r2) ///
    cstat(_r_b) cstat(_r_se) ///
    showstars ///
    export("regression.html", tableonly replace)
```

### Example 3: Cross-Tabulation

```stata
sysuse auto, clear

* Create cross-tab
table foreign rep78, statistic(mean price) ///
                     statistic(count price) ///
                     statistic(sd price)

* Style and export
collect style cell result, nformat(%9.1f)
collect style cell result[count], nformat(%9.0f)
collect export "crosstab.html", tableonly replace
```

---

## HTML Output Format

### With `tableonly` option

```html
<style>
table {
  border-collapse: collapse;
  font-family: sans-serif;
}
th, td {
  border: 1px solid #ddd;
  padding: 8px;
}
</style>
<table>
  <thead>...</thead>
  <tbody>...</tbody>
</table>
```

**Characteristics**:
- ✅ Embeddable HTML fragment
- ✅ No `<!DOCTYPE>`, `<html>`, `<body>` tags
- ✅ Inline CSS (no external dependencies)
- ✅ Proper semantic `<table>` structure

### Without `tableonly` option

Produces complete HTML document (not suitable for notebook embedding).

---

## Key Takeaways

1. **`table` and `etable` are complementary** - use each for its purpose
2. **No stdout capture** - temp files are the ONLY option
3. **Stata 17+ required** - all table/collect features are new
4. **`etable` for regressions** - easier and better than table
5. **`table` for summaries** - what it's designed for
6. **File I/O is mandatory** - no way to avoid temp files

---

## For xeus-stata Implementation

### Current Status (Phase 3)

✅ Simple HTML with styled `<pre>` wrapper
- Works for ALL commands
- No version dependency
- No file I/O
- Good user experience

### Future Enhancement (Phase 4 - Optional)

⏳ Native HTML via `etable`
- Only for estimation commands
- Requires temp file workflow
- Stata 17+ only
- Better formatting, more complexity

### Recommendation

**Stick with Phase 3** unless users specifically request better regression tables.

Current approach is:
- ✅ Universal (all commands)
- ✅ Simple (low complexity)
- ✅ Fast (no file I/O)
- ✅ Compatible (all Stata versions)

---

**Document Type**: Quick Reference
**Full Analysis**: See `TABLE_VS_ETABLE_INVESTIGATION.md`
**Test Script**: See `test_table_scope.do`
