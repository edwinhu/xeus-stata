# Stata Command Support Matrix for HTML Export

**Purpose**: Comprehensive list of Stata commands and their HTML export capabilities

## Legend

- ✅ **Native HTML**: Stata's `etable`/`collect export` produces proper `<table>` HTML
- ⚠️ **Simple HTML**: Current xeus-stata styled `<pre>` wrapper approach
- ❌ **Text Only**: No table structure, plain text output

---

## Estimation Commands (Regression and Models)

All estimation commands can use `etable, export()` for native HTML tables.

### Linear Models

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `regress` | ✅ Native | `regress y x1 x2` | Linear regression |
| `reg` | ✅ Native | `reg y x1 x2` | Alias for regress |
| `areg` | ✅ Native | `areg y x1, absorb(id)` | Linear with absorbed effects |
| `xtreg` | ✅ Native | `xtreg y x1, fe` | Panel data regression |
| `xtgls` | ✅ Native | `xtgls y x1` | GLS for panel data |
| `mixed` | ✅ Native | `mixed y x1 \|\| id:` | Multilevel mixed-effects |
| `ivregress` | ✅ Native | `ivregress 2sls y (x1=z1) x2` | Instrumental variables |
| `ivreg` | ✅ Native | `ivreg y x1 (x2=z1)` | IV regression (older syntax) |

### Binary Outcome Models

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `logit` | ✅ Native | `logit y x1 x2` | Logistic regression |
| `logistic` | ✅ Native | `logistic y x1 x2` | Logistic (reports odds ratios) |
| `probit` | ✅ Native | `probit y x1 x2` | Probit regression |
| `clogit` | ✅ Native | `clogit y x1, group(id)` | Conditional logit |

### Ordinal Outcome Models

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `ologit` | ✅ Native | `ologit y x1 x2` | Ordered logit |
| `oprobit` | ✅ Native | `oprobit y x1 x2` | Ordered probit |

### Count Data Models

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `poisson` | ✅ Native | `poisson y x1 x2` | Poisson regression |
| `nbreg` | ✅ Native | `nbreg y x1 x2` | Negative binomial |
| `zip` | ✅ Native | `zip y x1, inflate(x2)` | Zero-inflated Poisson |
| `zinb` | ✅ Native | `zinb y x1, inflate(x2)` | Zero-inflated negative binomial |

### Limited Dependent Variable Models

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `tobit` | ✅ Native | `tobit y x1 x2, ll(0)` | Tobit regression |
| `truncreg` | ✅ Native | `truncreg y x1, ll(0)` | Truncated regression |
| `heckman` | ✅ Native | `heckman y x1, select(s=z1)` | Heckman selection |

### Survival Analysis

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `stcox` | ✅ Native | `stcox x1 x2` | Cox proportional hazards |
| `streg` | ✅ Native | `streg x1, dist(exp)` | Parametric survival |

### Other Estimation Commands

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `sem` | ✅ Native | `sem (y <- x1 x2)` | Structural equation modeling |
| `gsem` | ✅ Native | `gsem (y <- x1 x2)` | Generalized SEM |
| `glm` | ✅ Native | `glm y x1, family(poisson)` | Generalized linear model |
| `qreg` | ✅ Native | `qreg y x1 x2` | Quantile regression |

**Total estimation commands with native HTML**: 25+

---

## Descriptive Statistics

### Table Commands (Stata 17+)

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `table` | ✅ Native | `table (var), stat(mean x)` | New table syntax (Stata 17+) |
| `dtable` | ✅ Native | `dtable x1 x2, by(group)` | Descriptive statistics table |
| `etable` | ✅ Native | `etable, estimates(m1 m2)` | Estimation results table |

### Summary Statistics (Traditional)

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `summarize` | ⚠️ Simple | `summarize x1 x2 x3` | Basic summary stats |
| `sum` | ⚠️ Simple | `sum x1 x2` | Alias for summarize |
| `summarize, detail` | ⚠️ Simple | `sum x, detail` | Detailed statistics |
| `tabstat` | ⚠️ Simple | `tabstat x, stats(mean sd)` | Flexible summary stats |
| `codebook` | ⚠️ Simple | `codebook x1 x2` | Variable documentation |

### Frequency Tables

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `tabulate` | ⚠️ Simple | `tab x1` | One-way frequency table |
| `tab` | ⚠️ Simple | `tab x1` | Alias for tabulate |
| `tabulate x y` | ⚠️ Simple | `tab x y` | Two-way cross-tab |
| `tab1` | ⚠️ Simple | `tab1 x1 x2 x3` | Multiple one-way tables |
| `tab2` | ⚠️ Simple | `tab2 x1 x2 x3` | All two-way combinations |

### Correlation and Association

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `correlate` | ⚠️ Simple | `correlate x1 x2 x3` | Correlation matrix |
| `corr` | ⚠️ Simple | `corr x1 x2` | Alias for correlate |
| `pwcorr` | ⚠️ Simple | `pwcorr x1 x2, sig` | Pairwise correlations |
| `spearman` | ⚠️ Simple | `spearman x1 x2` | Spearman rank correlation |
| `tetrachoric` | ⚠️ Simple | `tetrachoric x1 x2` | Tetrachoric correlation |

---

## Data Display

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `list` | ⚠️ Simple | `list x1 x2 in 1/10` | Display observations |
| `browse` | ❌ Text | `browse` | Interactive browser (GUI only) |
| `edit` | ❌ Text | `edit` | Interactive editor (GUI only) |
| `display` | ❌ Text | `display "Hello"` | Print text |
| `di` | ❌ Text | `di 2+2` | Alias for display |

---

## Diagnostics and Tests

### Post-Estimation Diagnostics

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `predict` | ❌ Text | `predict yhat` | Generate predictions |
| `residuals` | ❌ Text | `predict resid, resid` | Generate residuals |
| `estat` | ⚠️ Simple | `estat ic` | Post-estimation stats |
| `margins` | ⚠️ Simple | `margins, dydx(*)` | Marginal effects |
| `test` | ⚠️ Simple | `test x1 = x2` | Hypothesis tests |

### Statistical Tests

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `ttest` | ⚠️ Simple | `ttest x1 == 0` | t-test |
| `prtest` | ⚠️ Simple | `prtest x1 == 0.5` | Proportion test |
| `anova` | ✅ Native | `anova y x1 x2` | ANOVA (estimation cmd) |
| `ranksum` | ⚠️ Simple | `ranksum x, by(group)` | Wilcoxon rank-sum |
| `signrank` | ⚠️ Simple | `signrank x1 = x2` | Wilcoxon signed-rank |

---

## Data Management

| Command | Support | Example | Notes |
|---------|---------|---------|-------|
| `describe` | ⚠️ Simple | `describe x1 x2` | Variable descriptions |
| `inspect` | ⚠️ Simple | `inspect x1` | Variable inspection |
| `assert` | ❌ Text | `assert x > 0` | Assertion check |
| `count` | ❌ Text | `count if x > 0` | Count observations |
| `duplicates` | ⚠️ Simple | `duplicates report` | Duplicate detection |

---

## Implementation Priority

### High Priority (Implement First)

1. ✅ **Estimation commands** - Most important for research
   - `regress`, `logit`, `probit` (core models)
   - Use `etable, export()` for native HTML

### Medium Priority (Implement Later)

2. ⚠️ **Summary statistics** - Already works with simple HTML
   - `summarize`, `correlate`, `tabulate`
   - Current approach is adequate

3. ✅ **Table commands** (if Stata 17+)
   - `table`, `dtable`
   - Can add native HTML support

### Low Priority (Current Implementation OK)

4. ⚠️ **Diagnostics and tests** - Simple HTML is fine
   - `estat`, `margins`, `test`
   - Less critical for pretty formatting

5. ❌ **Text-only commands** - No formatting needed
   - `display`, `count`, `assert`
   - Plain text is correct output

---

## Implementation Strategy by Command Type

### Type 1: Estimation Commands → Native HTML

**Commands**: `regress`, `logit`, `probit`, `poisson`, `ologit`, etc.

**Detection**:
```cpp
bool is_estimation_command(string cmd) {
    vector<string> est_cmds = {
        "reg", "regress", "logit", "probit", "ologit", "oprobit",
        "poisson", "nbreg", "tobit", "heckman", "areg", "xtreg",
        "ivregress", "mixed", "glm", "qreg", "anova", "stcox"
    };
    return find(est_cmds, cmd) != end;
}
```

**Workflow**:
1. Execute user command
2. Check if estimation successful (`e(cmd)` not empty)
3. Run `etable, export(tempfile.html, tableonly)`
4. Read HTML, send in MIME bundle
5. Delete temp file

### Type 2: Table Commands → Native HTML (Stata 17+)

**Commands**: `table`, `dtable`

**Detection**:
```cpp
bool is_table_command(string cmd) {
    return cmd == "table" || cmd == "dtable";
}
```

**Workflow**:
- For `table`: Use `collect export`
- For `dtable`: Use `export()` option directly

### Type 3: Traditional Stats → Simple HTML

**Commands**: `summarize`, `tabulate`, `correlate`, `list`

**Detection**: Use existing `is_stata_table()` function

**Workflow**: Current implementation (styled `<pre>` wrapper)

### Type 4: Text Output → No Formatting

**Commands**: `display`, `count`, `assert`

**Workflow**: Plain text stream output

---

## Version-Specific Features

### Stata 17+ Only

- `table` (new syntax with collect)
- `dtable`
- `etable`
- `collect export`

**Fallback for Stata 16**: Use simple HTML for all commands

### All Stata Versions

- Estimation commands (detection still works)
- Simple HTML formatting (current approach)

---

## Examples by Use Case

### Use Case 1: Regression Results

**User code**:
```stata
regress price mpg weight foreign
```

**xeus-stata processing**:
1. Detect "regress" → estimation command
2. Execute regression
3. Export HTML: `etable, export(temp.html, tableonly)`
4. Read and send HTML `<table>`
5. Result: Professional formatted regression table

### Use Case 2: Summary Statistics

**User code**:
```stata
summarize price mpg weight
```

**xeus-stata processing**:
1. Not estimation command
2. Execute summarize
3. Check output with `is_stata_table()` → true
4. Apply `format_as_html_table()` → styled `<pre>`
5. Result: Formatted text table (current approach)

### Use Case 3: Multiple Models Comparison

**User code**:
```stata
regress price mpg weight
estimates store m1
regress price mpg weight foreign
estimates store m2
etable, estimates(m1 m2)
```

**xeus-stata processing**:
1. First regress → export HTML (or skip if auto-detect disabled)
2. `estimates store` → no output
3. Second regress → export HTML (or skip)
4. Detect explicit `etable` command
5. Export HTML table comparing models
6. Result: Side-by-side model comparison

### Use Case 4: Descriptive Table (Stata 17+)

**User code**:
```stata
dtable price mpg weight, by(foreign)
```

**xeus-stata processing**:
1. Detect `dtable` command
2. Execute command
3. Export HTML: `dtable ..., export(temp.html, tableonly)`
4. Read and send HTML
5. Result: Formatted descriptive statistics table

---

## Summary Statistics

- **Total commands analyzed**: 50+
- **Native HTML support**: ~30 commands (Stata 17+)
  - 25+ estimation commands
  - 3 table commands (table, dtable, etable)
- **Simple HTML support**: ~15 commands (all versions)
  - Summary stats (summarize, tabstat)
  - Frequency tables (tabulate)
  - Correlations (correlate, pwcorr)
- **Text-only**: ~10 commands
  - Display, count, assert, etc.

---

## Recommended Implementation Phases

### Phase 1: Core Estimation Commands ✅ HIGH PRIORITY
- Detect: `regress`, `logit`, `probit`, `poisson`
- Export via `etable`
- Test with Stata 17+
- Add version detection and fallback

### Phase 2: Extended Estimation Commands
- Add: `ologit`, `nbreg`, `tobit`, `xtreg`, `mixed`
- Same `etable` workflow
- Test edge cases (multiple models, options)

### Phase 3: Table Commands (Stata 17+ only)
- Add: `table`, `dtable` detection
- Export via `collect export` or `export()` option
- Test with various table layouts

### Phase 4: Optimization
- Performance tuning (temp file I/O)
- Better error handling
- User configuration options (default etable options)

---

**Document created**: 2025-11-17
**Status**: Reference complete, ready for implementation
