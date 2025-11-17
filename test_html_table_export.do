* Test Script: HTML Table Export Using Stata's Collect/Etable Commands
* Purpose: Demonstrate methods for generating HTML tables from Stata commands
* Date: 2025-11-17
*
* This script explores two approaches:
* 1. Using etable with export() option for regression results
* 2. Using collect export for custom tables and statistics
*
* NOTE: These commands work in Stata 17+

* Load sample data
sysuse auto, clear

* ==============================================================================
* APPROACH 1: Using etable for regression results
* ==============================================================================

display "=== APPROACH 1: ETABLE FOR REGRESSION RESULTS ==="

* Run multiple regressions
quietly regress price mpg weight
estimates store model1

quietly regress price mpg weight foreign
estimates store model2

quietly regress price mpg weight foreign length
estimates store model3

* Method 1a: Export etable to HTML file with tableonly option
* This creates HTML table fragment (not full document)
display "Test 1a: etable export to temp file (tableonly)"
tempfile html_table1
etable, estimates(model1 model2 model3) ///
    mstat(N) mstat(r2) ///
    cstat(_r_b, nformat(%9.3f)) ///
    cstat(_r_se, nformat(%9.3f)) ///
    showstars showstarsnote ///
    title("Table 1: Regression Results") ///
    export("`html_table1'.html", tableonly replace)

* Display the HTML output
display "HTML output saved to: `html_table1'.html"
type "`html_table1'.html"

* Method 1b: Export full HTML document (without tableonly)
display _newline "Test 1b: etable export to full HTML document"
tempfile html_table2
etable, estimates(model1 model2) ///
    mstat(N) mstat(r2_a) ///
    cstat(_r_b) cstat(_r_se) ///
    showstars ///
    export("`html_table2'.html", replace)

display "Full HTML document saved to: `html_table2'.html"
* Note: Full document includes <!DOCTYPE html>, <head>, <body> tags

* ==============================================================================
* APPROACH 2: Using table + collect export for summary statistics
* ==============================================================================

display _newline "=== APPROACH 2: TABLE + COLLECT EXPORT ==="

* Method 2a: Summary statistics table
display "Test 2a: Summary statistics via table command"
table (var), statistic(mean price mpg weight) ///
              statistic(sd price mpg weight) ///
              statistic(min price mpg weight) ///
              statistic(max price mpg weight)

* Customize and export
collect style cell result[mean sd min max], nformat(%9.2f)
tempfile html_table3
collect export "`html_table3'.html", tableonly replace

display "Summary statistics HTML saved to: `html_table3'.html"
type "`html_table3'.html"

* Method 2b: Cross-tabulation with statistics
display _newline "Test 2b: Cross-tab via table command"
table (foreign), statistic(mean price) ///
                 statistic(mean mpg) ///
                 statistic(mean weight) ///
                 statistic(count price)

collect style cell result, nformat(%9.1f)
collect style cell result[count], nformat(%9.0f)
tempfile html_table4
collect export "`html_table4'.html", tableonly replace

display "Cross-tab HTML saved to: `html_table4'.html"
type "`html_table4'.html"

* ==============================================================================
* APPROACH 3: Using dtable for descriptive statistics
* ==============================================================================

display _newline "=== APPROACH 3: DTABLE FOR DESCRIPTIVE STATISTICS ==="

* Method 3a: Basic descriptive statistics table
display "Test 3a: dtable for descriptive stats"
dtable price mpg weight, by(foreign)

tempfile html_table5
dtable price mpg weight, by(foreign) ///
    export("`html_table5'.html", tableonly replace)

display "Descriptive stats HTML saved to: `html_table5'.html"
type "`html_table5'.html"

* ==============================================================================
* QUESTIONS TO ANSWER
* ==============================================================================

* Q1: Can we capture HTML output without writing to file?
* A1: NO - collect export and etable export() only write to files
*      We need to:
*      1. Write to temp file
*      2. Read file contents
*      3. Send as MIME bundle
*      4. Delete temp file

* Q2: What HTML does tableonly produce?
* A2: tableonly produces HTML <table> fragment with inline CSS
*     - NOT a complete HTML document (no <!DOCTYPE>, <html>, <head>, <body>)
*     - Contains <style> block with table CSS
*     - Contains <table> element with formatted data
*     - Can be embedded in larger HTML document or Jupyter output

* Q3: Which commands work with this approach?
* A3:
*     - etable: For regression/estimation results (regress, logit, etc.)
*     - table + collect: For summary stats, cross-tabs, custom tables
*     - dtable: For descriptive statistics tables
*     - NOT for: summarize, tabulate, list (these don't use collect system)

* Q4: Does this require Stata 17+?
* A4: YES - collect/etable/dtable introduced in Stata 17
*      For Stata 16 and earlier, would need different approach

* ==============================================================================
* SUMMARY AND RECOMMENDATIONS
* ==============================================================================

display _newline "=== SUMMARY ==="
display "1. HTML export requires writing to file (no stdout option)"
display "2. tableonly option produces embeddable HTML fragments"
display "3. Works for: etable (regression), table (summaries), dtable (descriptives)"
display "4. Does NOT work for: summarize, tabulate, list, correlate"
display "5. Requires Stata 17+"
display ""
display "For xeus-stata implementation:"
display "  - Detect estimation commands (regress, logit, etc.)"
display "  - After estimation, use: etable, export(tempfile.html, tableonly)"
display "  - Read HTML from tempfile"
display "  - Send as text/html MIME type"
display "  - Delete tempfile"
display "  - Keep existing approach for other commands (summarize, etc.)"
