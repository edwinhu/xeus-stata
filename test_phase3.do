* Phase 3 HTML Table Formatting Test Script
* Tests various Stata commands that produce table output
* Run this in euporie or Jupyter notebook to see HTML formatting

* Load sample data
sysuse auto, clear

* Test 1: Basic statistics table
display "TEST 1: SUMMARIZE"
summarize price mpg weight

* Test 2: Regression table
display "TEST 2: REGRESSION"
regress price mpg weight foreign

* Test 3: Frequency table
display "TEST 3: TABULATE"
tabulate foreign

* Test 4: Two-way tabulation
display "TEST 4: TWO-WAY TABULATE"
tabulate foreign rep78

* Test 5: Data listing
display "TEST 5: LIST"
list make price mpg in 1/5

* Test 6: Correlation matrix
display "TEST 6: CORRELATE"
correlate price mpg weight length

* Test 7: Non-table output (should NOT be HTML formatted)
display "TEST 7: SIMPLE TEXT (should not be formatted as table)"
display "This is just plain text output"
display "It should appear as regular stream output"

* Test 8: Summary statistics with detail
display "TEST 8: SUMMARIZE WITH DETAIL"
summarize price, detail

* All tests complete
display "All Phase 3 tests completed!"
