* Test script for xeus-stata inline plotting
* This demonstrates various Stata graphics capabilities

* Load example dataset
sysuse auto, clear

* 1. Simple scatter plot
scatter mpg weight

* 2. Scatter with fit line
twoway (scatter mpg weight) (lfit mpg weight)

* 3. Histogram
histogram price

* 4. Box plot
graph box mpg, over(foreign)

* 5. Bar chart
graph bar (mean) price (mean) mpg, over(foreign)

* Summary statistics
summarize price mpg weight
