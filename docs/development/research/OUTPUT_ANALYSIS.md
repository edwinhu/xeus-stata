# xeus-stata Kernel Output Analysis

## Summary

When executing `reg price weight` through the xeus-stata kernel, the output is returned via the Jupyter messaging protocol with **two MIME types**: `text/plain` and `text/html`.

### Key Finding: Left-Padding Stripped

**The kernel output has left-padding/indentation removed** compared to raw Stata output.

## Output Comparison

### Raw Stata Output (from .log file)
```
      Source |       SS           df       MS      Number of obs   =        74
-------------+----------------------------------   F(1, 72)        =     29.42
       Model |   184233937         1   184233937   Prob > F        =    0.0000
    Residual |   450831459        72  6261548.04   R-squared       =    0.2901
-------------+----------------------------------   Adj R-squared   =    0.2802
       Total |   635065396        73  8699525.97   Root MSE        =    2502.3

------------------------------------------------------------------------------
       price | Coefficient  Std. err.      t    P>|t|     [95% conf. interval]
-------------+----------------------------------------------------------------
      weight |   2.044063   .3768341     5.42   0.000     1.292857    2.795268
       _cons |  -6.707353    1174.43    -0.01   0.995     -2347.89    2334.475
------------------------------------------------------------------------------
```

### xeus-stata Kernel Output (text/plain)
```
Source |       SS           df       MS      Number of obs   =        74
-------------+----------------------------------   F(1, 72)        =     29.42
Model |   184233937         1   184233937   Prob > F        =    0.0000
Residual |   450831459        72  6261548.04   R-squared       =    0.2901
-------------+----------------------------------   Adj R-squared   =    0.2802
Total |   635065396        73  8699525.97   Root MSE        =    2502.3
------------------------------------------------------------------------------
price | Coefficient  Std. err.      t    P>|t|     [95% conf. interval]
-------------+----------------------------------------------------------------
weight |   2.044063   .3768341     5.42   0.000     1.292857    2.795268
_cons |  -6.707353    1174.43    -0.01   0.995     -2347.89    2334.475
------------------------------------------------------------------------------
```

## Differences

### 1. Left-Padding Removed

| Line | Stata Leading Spaces | Kernel Leading Spaces | Difference |
|------|---------------------|-----------------------|------------|
| `Source` | 6 spaces | 0 spaces | -6 |
| `Model` | 7 spaces | 0 spaces | -7 |
| `Residual` | 4 spaces | 0 spaces | -4 |
| `Total` | 7 spaces | 0 spaces | -7 |
| `price` | 7 spaces | 0 spaces | -7 |
| `weight` | 6 spaces | 0 spaces | -6 |
| `_cons` | 7 spaces | 0 spaces | -7 |

### 2. Empty Line Missing

Raw Stata output has an empty line between the ANOVA table and the coefficients table (line 7 in Stata output). The kernel output removes this empty line.

- **Stata**: 13 lines (includes empty line)
- **Kernel**: 12 lines (no empty line)

### 3. Content Otherwise Identical

All numerical values, column headers, and table structure are identical. Only the left-padding/indentation is different.

## MIME Type Details

### text/plain
- Raw text output with no formatting
- Left-padding stripped from Stata output
- No ANSI color codes or escape sequences
- Uses standard space characters (ord=32) for spacing
- Newlines are `\n` (ord=10)

### text/html
- Wrapped in styled `<pre>` tag with custom CSS
- Same content as text/plain (left-padding stripped)
- HTML entities: `>` encoded as `&gt;`
- CSS styling:
  - Monospace font (Cascadia Code, Source Code Pro, etc.)
  - Light gray background (#f8f8f8)
  - Border and padding for visual separation
  - Font size: 12px
  - Line height: 1.4

## Character Analysis

First line breakdown (kernel output):
```
Position  Character  ASCII
0         'S'        83
1         'o'        111
2         'u'        117
3         'r'        114
4         'c'        99
5         'e'        101
6         ' '        32   (space)
7         '|'        124  (pipe)
8-14      ' '        32   (7 spaces)
15        'S'        83
16        'S'        83
```

Compare with Stata (first line has 6 leading spaces):
```
Position  Character  ASCII
0-5       ' '        32   (6 spaces) ← REMOVED IN KERNEL OUTPUT
6         'S'        83
7         'o'        111
...
```

## Jupyter Message Structure

### Execute Result Message
```json
{
  "type": "execute_result",
  "data": {
    "text/html": "<style>...</style><pre class=\"stata-output\">...</pre>",
    "text/plain": "Source |       SS  ..."
  }
}
```

### Stream Message (for sysuse)
```json
{
  "type": "stream",
  "name": "stdout",
  "text": "(1978 automobile data)"
}
```

## Implementation Notes

The xeus-stata kernel:
1. Captures Stata's output
2. Strips leading whitespace from each line (left-trim)
3. Preserves all column alignment and internal spacing
4. Removes blank lines
5. Generates both plain text and HTML representations
6. Returns via Jupyter `execute_result` message

### Source Code Location

This formatting is implemented in `/home/eh2889/projects/xeus-stata/src/stata_parser.cpp`:

**Function**: `parse_execution_output()` (lines 185-275)

**Relevant code** (lines 248-272):
```cpp
// Remove empty lines and trim
std::stringstream ss(result.output);
std::stringstream output_ss;
std::string line;
bool first_line = true;

while (std::getline(ss, line))
{
    // Trim line
    line.erase(0, line.find_first_not_of(" \t\r"));  // ← LEFT TRIM
    line.erase(line.find_last_not_of(" \t\r") + 1);  // ← RIGHT TRIM

    // Skip empty lines
    if (!line.empty())
    {
        if (!first_line)
        {
            output_ss << "\n";
        }
        output_ss << line;
        first_line = false;
    }
}

result.output = output_ss.str();
```

**Line 257** is where left-padding is stripped: `line.erase(0, line.find_first_not_of(" \t\r"));`

This removes all leading spaces, tabs, and carriage returns from each line of Stata output.
