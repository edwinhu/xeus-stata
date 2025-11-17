# xeus-stata Rich Output Implementation Plan

## Goal
Enable rich HTML output for Stata results (tables, graphs) in Jupyter/euporie notebooks using MIME bundles.

## Current State (Updated: 2025-11-17)

### ✅ Completed Features
- ✅ Basic text output (plain text)
- ✅ Command execution and output parsing
- ✅ Clean output (markers/prompts removed)
- ✅ **Automatic graph export to PNG**
- ✅ **MIME bundle construction with multiple formats**
- ✅ **Graph display in notebook (inline)**
- ✅ **Base64 encoding for binary image data**
- ✅ **Temp file management and cleanup**

### ⏳ Remaining Work
- ⏳ Rich HTML table formatting (Phase 3)

## Implementation Plan

### Phase 1: Graph Auto-Export & Display ✅ COMPLETED

**Status**: ✅ Fully implemented and tested (2025-11-17)

**Goal**: Automatically export and display graphs inline in notebooks

**Location**: `src/stata_session.cpp`

**Implementation Details**:
```cpp
execution_result execute(const std::string& code) {
    // Generate temp file path
    std::string temp_graph = generate_temp_filename(".png");

    // Wrap user code with graph export
    std::string wrapped_code = code + "\n";
    wrapped_code += "capture noisily {\n";
    wrapped_code += "  if (c(k) > 0) {\n";  // Check if graph exists
    wrapped_code += "    graph export \"" + temp_graph + "\", replace\n";
    wrapped_code += "  }\n";
    wrapped_code += "}\n";
    wrapped_code += "display \"__MARKER__" + marker + "__\"";

    // Execute and check if temp file was created
    // If yes, add to result.graph_files
}
```

**Actual Implementation Used**:
- Uses `graph describe Graph` to detect if a graph exists (more reliable than `c(k)`)
- Generates unique temp files with `/tmp/xeus_stata_graph_XXXXXX.png`
- Deletes old temp files before execution to prevent reuse
- Uses `quietly capture` to avoid showing wrapper commands
- Drops all graphs after export with `graph drop _all`
- Verifies file exists and has content before adding to results
- Returns graph file paths in `execution_result.graph_files`

**Key Design Decision**: Chosen approach wraps every command but uses smart detection to only export when graphs actually exist, avoiding false positives.

### Phase 2: MIME Bundle Construction ✅ COMPLETED

**Status**: ✅ Fully implemented and tested (2025-11-17)

**Goal**: Send multiple output formats (text/plain, text/html, image/png)

**Location**: `src/xinterpreter.cpp`

**Implementation Details**:
```cpp
// For text output with potential tables
nl::json display_data;
display_data["text/plain"] = exec_result.output;

// If output looks like a table, add HTML version
if (is_stata_table(exec_result.output)) {
    display_data["text/html"] = format_as_html_table(exec_result.output);
}

publish_execution_result(execution_counter,
                        std::move(display_data),
                        nl::json::object());

// For graphs
for (const auto& graph_file : exec_result.graph_files) {
    nl::json graph_bundle;
    graph_bundle["image/png"] = read_file_as_base64(graph_file);

    nl::json metadata;
    metadata["image/png"]["width"] = 600;
    metadata["image/png"]["height"] = 400;

    publish_execution_result(execution_counter,
                            std::move(graph_bundle),
                            std::move(metadata));
}
```

**Actual Implementation**:
- Reads PNG files as binary data using `std::ifstream` with `std::ios::binary`
- Base64-encodes image data using custom `base64_encode()` function
- Creates MIME bundle with `image/png` MIME type
- Adds metadata with suggested dimensions (600x400)
- Uses `publish_execution_result()` to send to Jupyter frontend
- Cleans up temp files immediately after reading with `unlink()`

**MIME Types Implemented**:
- ✅ `text/plain` - Always sent (fallback)
- ✅ `image/png` - For graphs (base64-encoded)
- ⏳ `text/html` - For tables (Phase 3)

**Files Added**:
- `include/xeus-stata/base64.hpp` - Base64 encoding header
- `src/base64.cpp` - Base64 encoding implementation

### Phase 3: HTML Table Formatting ⏳ PENDING

**Goal**: Convert Stata text tables to formatted HTML

**Location**: `src/stata_parser.cpp` or new `src/stata_formatter.cpp`

**Challenges**:
- Stata tables are ASCII art (aligned columns)
- Need to parse column boundaries
- Detect table headers vs data rows
- Handle different table types (summarize, regress, tabulate, etc.)

**Detection Strategy**:
```cpp
bool is_stata_table(const std::string& output) {
    // Look for common table patterns:
    // - Lines with dashes (-----)
    // - Aligned columns (multiple spaces)
    // - Header words (Variable, Obs, Mean, Std. Dev., etc.)
    // - Regression output (Coef., Std. Err., t, P>|t|, etc.)
}
```

**Parsing Strategy**:
1. **Simple approach**: Wrap in `<pre>` tags for monospace formatting
   - Pros: Easy, preserves alignment
   - Cons: Not truly "rich", just formatted text

2. **Full parsing**: Convert to proper HTML `<table>`
   - Pros: True HTML tables, can style/sort
   - Cons: Complex, fragile parsing

3. **Hybrid**: Detect specific table types, format known patterns
   - Pros: Best results for common cases
   - Cons: May miss edge cases

**Recommended**: Start with approach #1 (pre-formatted), add #3 for common tables later

### Phase 4: Base64 Image Encoding

**Goal**: Embed PNG graphs as base64 in MIME bundles

**Location**: `src/xinterpreter.cpp`

**Implementation**:
```cpp
std::string read_file_as_base64(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());

    // Use base64 encoding library or implement simple encoder
    return base64_encode(buffer.data(), buffer.size());
}
```

**Note**: May need to add base64 encoding dependency or implement simple encoder

## Testing Strategy

### Test 1: Graph Display
```stata
sysuse auto, clear
scatter mpg weight
```
**Expected**: PNG image appears inline in notebook

### Test 2: Table Output
```stata
summarize price mpg weight
```
**Expected**: Formatted table (at minimum, monospace `<pre>`)

### Test 3: Regression Output
```stata
regress price mpg weight foreign
```
**Expected**: Formatted regression table

### Test 4: Multiple Outputs
```stata
summarize price mpg
scatter mpg weight
```
**Expected**: Both table and graph displayed inline

## Dependencies

**May Need**:
- Base64 encoding library (or implement simple version)
- Temp file management (mkstemp or similar)
- File I/O for reading binary PNG data

**Already Have**:
- xeus display API
- JSON library (nlohmann)
- String manipulation (C++ std)

## Implementation Order

1. **Start with graphs** (Phase 1 + Phase 4)
   - Auto-export to temp PNG
   - Read as base64
   - Send in MIME bundle
   - **Why first**: Most visible impact, clearer success criteria

2. **Add simple table formatting** (Phase 3, approach #1)
   - Wrap tables in `<pre>` tags
   - Send as text/html
   - **Why second**: Easy win, immediate improvement

3. **Refine table parsing** (Phase 3, approach #3)
   - Detect specific table types
   - Convert to proper HTML tables
   - Add CSS styling
   - **Why last**: Nice-to-have, can iterate

4. **Optimize and polish** (Phase 2 refinements)
   - Clean up temp files
   - Handle errors gracefully
   - Add metadata (image dimensions, etc.)

## Resolved Design Decisions

1. **Temp file cleanup**: ✅ **Resolved** - Delete immediately after reading
   - Chosen: Option A (immediate cleanup)
   - Rationale: Prevents disk space issues, simpler lifecycle management
   - Implementation: `unlink()` called right after base64 encoding

2. **Graph format**: ✅ **Resolved** - PNG only for now
   - Chosen: PNG only (base64-encoded)
   - Rationale: Simpler to implement, works everywhere
   - Future: SVG support can be added later via same MIME bundle mechanism

3. **Error handling**: ✅ **Resolved** - Silent failure with `capture`
   - Chosen: Silent failure, continue with text-only output
   - Rationale: Non-intrusive, doesn't break workflow
   - Implementation: Uses `quietly capture` in Stata wrapper

4. **Performance**: ✅ **Resolved** - Wrap every command with smart detection
   - Chosen: Wrap all commands, use `graph describe` to check existence
   - Rationale: Simple, reliable, minimal overhead for non-graph commands
   - Testing: No noticeable performance impact

## Open Questions (Remaining)

## Success Criteria

### ✅ Minimum Viable (MVP) - ACHIEVED
- ✅ Graphs display inline as PNG
- ⏳ Tables have basic formatting (monospace) - Phase 3
- ✅ No crashes or data loss

### Stretch Goals
- ⏳ Proper HTML table formatting for common outputs - Phase 3
- ⏳ SVG graph support - Future enhancement
- ⏳ Styled tables with CSS - Phase 3
- ⏳ Interactive widgets - Future enhancement

## Actual Timeline

- **Phase 1 (Graphs)**: ✅ **Completed** (2025-11-17)
  - Auto-export implementation
  - Base64 encoding implementation
  - MIME bundle creation
  - Comprehensive testing
  - **Result**: Fully functional inline graph display

- **Phase 2 (MIME Bundles)**: ✅ **Completed** (2025-11-17)
  - Integrated with Phase 1
  - Binary file reading
  - Base64 encoding
  - Metadata handling
  - Temp file cleanup

- **Phase 3 (Tables)**: ⏳ **Pending**
  - Simple formatting: TBD
  - Detection logic: TBD
  - Testing: TBD

## Testing Results

### ✅ Completed Tests
1. **Scatter plots** - Working (22KB PNG, properly encoded)
2. **Histograms** - Working (16KB PNG)
3. **Box plots** - Working (11KB PNG)
4. **Text-only commands** - No false graph detections
5. **Sequential executions** - No cross-contamination
6. **Error handling** - Silent failures, no crashes

### Test Scripts
- `test_graph_output.py` - Basic graph test
- `test_comprehensive.py` - Full test suite
