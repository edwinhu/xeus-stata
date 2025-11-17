# xeus-stata Rich Output Implementation Plan

## Goal
Enable rich HTML output for Stata results (tables, graphs) in Jupyter/euporie notebooks using MIME bundles.

## Current State

### What Works
- ✅ Basic text output (plain text)
- ✅ Command execution and output parsing
- ✅ Clean output (markers/prompts removed)
- ✅ Placeholder for graph file detection

### What's Missing
- ❌ Rich HTML table formatting
- ❌ Automatic graph export to PNG
- ❌ MIME bundle construction with multiple formats
- ❌ Graph display in notebook (inline)

## Implementation Plan

### Phase 1: Graph Auto-Export & Display

**Goal**: Automatically export and display graphs inline in notebooks

**Location**: `src/stata_session.cpp`

**Changes**:
1. Wrap user commands to detect graph creation
2. Auto-export graphs to temporary PNG files
3. Return graph file paths in `execution_result`

**Implementation**:
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

**Pros**:
- Simple, automatic
- Works with all Stata graph commands
- No user intervention needed

**Cons**:
- Creates temp files (need cleanup)
- May slow down execution slightly
- Could interfere with user's graph export commands

**Alternative Approach**:
- Use Stata's `graph dir` to list active graphs
- Only export if graphs exist
- More selective, less intrusive

### Phase 2: MIME Bundle Construction

**Goal**: Send multiple output formats (text/plain, text/html, image/png)

**Location**: `src/xinterpreter.cpp`

**Changes**:
1. Modify `execute_request_impl` to build MIME bundles
2. Send plain text + HTML for tables
3. Send image/png for graphs

**Implementation**:
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

**MIME Types to Support**:
- `text/plain` - Always (fallback)
- `text/html` - For tables and rich output
- `image/png` - For graphs
- `text/markdown` - Optional, for simple formatting

### Phase 3: HTML Table Formatting

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

## Open Questions

1. **Temp file cleanup**: When to delete temporary graph files?
   - Option A: Delete immediately after reading
   - Option B: Delete on session shutdown
   - Option C: Use /tmp with auto-cleanup

2. **Graph format**: PNG only or support SVG?
   - SVG is better for notebooks (scalable, text-searchable)
   - PNG is simpler to implement
   - Could support both via MIME bundle

3. **Error handling**: What if graph export fails?
   - Silent failure?
   - Warning message?
   - Continue with text-only output?

4. **Performance**: Impact of wrapping every command?
   - Could detect "graphing commands" first
   - Or just try export and ignore failures
   - Profile to see actual impact

## Success Criteria

### Minimum Viable (MVP)
- ✅ Graphs display inline as PNG
- ✅ Tables have basic formatting (monospace)
- ✅ No crashes or data loss

### Stretch Goals
- ✅ Proper HTML table formatting for common outputs
- ✅ SVG graph support
- ✅ Styled tables with CSS
- ✅ Interactive widgets (future)

## Timeline Estimate

- **Phase 1 (Graphs)**: 2-3 hours
  - Auto-export: 1 hour
  - Base64 encoding: 30 min
  - MIME bundle: 30 min
  - Testing: 1 hour

- **Phase 2-3 (Tables)**: 2-4 hours
  - Simple formatting: 1 hour
  - Detection logic: 1 hour
  - Testing: 1-2 hours

- **Phase 4 (Polish)**: 1-2 hours
  - Cleanup, error handling
  - Documentation

**Total**: 5-9 hours for full implementation
**MVP** (graphs only): 2-3 hours
