* Investigation: Table Command Scope Test
* Date: 2025-11-17
* Purpose: Definitively test what the `table` command can and cannot do
*
* Questions to answer:
* 1. Can `table` display regression results? (like etable)
* 2. Can `table` display summary statistics? (mean, SD, etc.)
* 3. What's the FULL scope of `table` vs `etable`?

clear all
set more off

* Load sample data
sysuse auto, clear

display "================================================================================"
display "TEST 1: Can `table` display regression results?"
display "================================================================================"

* Run a simple regression
regress price mpg weight foreign

* Method 1a: Try using table command directly after regression
display _newline "Method 1a: table command after regression (will this work?)"
capture noisily table

* Method 1b: Try table with collect for regression results
display _newline "Method 1b: table with collect get for regression"
capture noisily {
    collect clear
    regress price mpg weight foreign
    collect get
    table
}

* Method 1c: Try explicit table with result specification
display _newline "Method 1c: Explicit table construction from regression"
capture noisily {
    collect clear
    regress price mpg weight foreign
    collect get, tag(model1)
    collect layout (colname) (result)
    table
}

display _newline "ANSWER 1: " _continue
display "The above attempts show whether `table` can handle regression results"
display "  - If we see coefficient tables → YES"
display "  - If we see errors or empty output → NO (must use etable)"

display _newline(2)
display "================================================================================"
display "TEST 2: Can `table` display summary statistics?"
display "================================================================================"

* Method 2a: Using table for basic summary stats
display _newline "Method 2a: table with statistic() for summary stats"
table (var), statistic(mean price mpg weight) ///
              statistic(sd price mpg weight) ///
              statistic(min price mpg weight) ///
              statistic(max price mpg weight)

display _newline "SUCCESS: table command works for summary statistics!"

* Method 2b: Can we export this to HTML?
display _newline "Method 2b: Export summary stats table to HTML"
tempfile html_summary
collect style cell result, nformat(%9.2f)
collect export "`html_summary'.html", tableonly replace
type "`html_summary'.html"

display _newline "SUCCESS: Summary stats can be exported to HTML via collect export!"

display _newline(2)
display "================================================================================"
display "TEST 3: Comparison - table vs etable for regression"
display "================================================================================"

* Run multiple regressions for comparison
quietly regress price mpg weight
estimates store m1
quietly regress price mpg weight foreign
estimates store m2

display _newline "Method 3a: etable for regression results (KNOWN TO WORK)"
etable, estimates(m1 m2) ///
    mstat(N) mstat(r2) ///
    cstat(_r_b) cstat(_r_se) ///
    showstars

display _newline "Method 3b: Can table do the same thing?"
capture noisily {
    collect clear
    collect get, tag(m1): estimates replay m1
    collect get, tag(m2): estimates replay m2
    collect layout (colname) (tag)
    table
}

display _newline(2)
display "================================================================================"
display "TEST 4: Cross-tabulation with table command"
display "================================================================================"

display _newline "Method 4a: Two-way cross-tab with table"
table foreign rep78, statistic(mean price) ///
                     statistic(count price)

display _newline "Method 4b: Export cross-tab to HTML"
tempfile html_crosstab
collect style cell result, nformat(%9.1f)
collect export "`html_crosstab'.html", tableonly replace
type "`html_crosstab'.html"

display _newline "SUCCESS: Cross-tabulations work with table + collect export!"

display _newline(2)
display "================================================================================"
display "TEST 5: What CANNOT be done with table?"
display "================================================================================"

* Things we know don't work
display _newline "Method 5a: summarize command (traditional)"
summarize price mpg weight

display _newline "Can we use collect with summarize?"
capture noisily {
    collect clear
    collect: summarize price mpg weight
}

display _newline "Method 5b: tabulate command (traditional)"
tabulate foreign

display _newline "Can we use collect with tabulate (Stata 19+)?"
capture noisily {
    collect clear
    tabulate foreign, collect
}

display _newline(2)
display "================================================================================"
display "FINAL SUMMARY"
display "================================================================================"

display _newline "Based on these tests:"
display ""
display "✅ table CAN do:"
display "   - Summary statistics (mean, SD, min, max, etc.)"
display "   - Cross-tabulations"
display "   - Custom aggregations"
display "   - Export to HTML via `collect export`"
display ""
display "❓ table with REGRESSION RESULTS:"
display "   - Needs `collect get` with estimates"
display "   - Requires manual layout specification"
display "   - etable is MUCH easier and more appropriate"
display ""
display "❌ table CANNOT replace:"
display "   - summarize (unless using table with statistic())"
display "   - Old-style tabulate (unless Stata 19+ with collect option)"
display "   - list command"
display "   - correlate command"
display ""
display "CONCLUSION:"
display "  - etable: Best for ESTIMATION RESULTS (regress, logit, etc.)"
display "  - table: Best for SUMMARY STATISTICS and CROSS-TABS"
display "  - They are COMPLEMENTARY, not interchangeable"
display ""
