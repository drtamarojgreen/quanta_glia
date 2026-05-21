#include <cassert>
#include <vector>
#include <string>
#include "analyze_lines.h"

void test_analyze_empty() {
    AnalyzeLines analyzer;
    std::vector<std::string> lines;
    ErrorObject obj = analyzer.analyze(lines);

    assert(obj.warnings == 0);
    assert(obj.typeErrors == 0);
    assert(obj.valueErrors == 0);
    assert(obj.syntaxErrors == 0);
    assert(obj.importErrors == 0);
    assert(obj.otherErrors == 0);
    assert(obj.totalErrors == 0);
    assert(obj.entries.size() == 0);
}

void test_analyze_warning_only() {
    AnalyzeLines analyzer;
    std::vector<std::string> lines = {
        "file1.cpp:10: this is just a warning message"
    };
    ErrorObject obj = analyzer.analyze(lines);

    assert(obj.warnings == 1);
    assert(obj.totalErrors == 0);
    assert(obj.entries.size() == 1);
    assert(obj.entries[0].type == "WARNING");
    assert(obj.entries[0].text == lines[0]);
    assert(obj.entries[0].filename == "file1.cpp");
    assert(obj.entries[0].lineNumber == 10);
}

void test_analyze_type_error() {
    AnalyzeLines analyzer;
    std::vector<std::string> lines = {
        "file2.cpp:20: TypeError: unsupported operand type(s)"
    };
    ErrorObject obj = analyzer.analyze(lines);

    assert(obj.warnings == 0);
    assert(obj.typeErrors == 1);
    assert(obj.totalErrors == 1);
    assert(obj.entries.size() == 1); // only error entry
    assert(obj.entries[0].type == "TYPE");
    assert(obj.entries[0].filename == "file2.cpp");
    assert(obj.entries[0].lineNumber == 20);
}

void test_analyze_value_error() {
    AnalyzeLines analyzer;
    std::vector<std::string> lines = {
        "file3.cpp(30): ValueError: invalid literal for int()"
    };
    ErrorObject obj = analyzer.analyze(lines);

    assert(obj.valueErrors == 1);
    assert(obj.totalErrors == 1);
    assert(obj.entries.size() == 1);
    assert(obj.entries[0].type == "VALUE");
    assert(obj.entries[0].filename == "file3.cpp");
    assert(obj.entries[0].lineNumber == 30);
}

void test_analyze_syntax_error() {
    AnalyzeLines analyzer;
    std::vector<std::string> lines = {
        "file4.py:40: SyntaxError: invalid syntax"
    };
    ErrorObject obj = analyzer.analyze(lines);

    assert(obj.syntaxErrors == 1);
    assert(obj.totalErrors == 1);
    assert(obj.entries.size() == 1);
    assert(obj.entries[0].type == "SYNTAX");
    assert(obj.entries[0].filename == "file4.py");
    assert(obj.entries[0].lineNumber == 40);
}

void test_analyze_import_error() {
    AnalyzeLines analyzer;
    std::vector<std::string> lines = {
        "file5.java:50: ImportError: cannot import name X"
    };
    ErrorObject obj = analyzer.analyze(lines);

    assert(obj.importErrors == 1);
    assert(obj.totalErrors == 1);
    assert(obj.entries.size() == 1);
    assert(obj.entries[0].type == "IMPORT");
    assert(obj.entries[0].filename == "file5.java");
    assert(obj.entries[0].lineNumber == 50);
}

void test_analyze_other_error() {
    AnalyzeLines analyzer;
    std::vector<std::string> lines = {
        "file6.js:60: RuntimeError: something bad happened"
    };
    ErrorObject obj = analyzer.analyze(lines);

    assert(obj.otherErrors == 1);
    assert(obj.totalErrors == 1);
    assert(obj.entries.size() == 1);
    assert(obj.entries[0].type == "ERROR");
    assert(obj.entries[0].filename == "file6.js");
    assert(obj.entries[0].lineNumber == 60);
}

void test_analyze_warning_and_error_same_line() {
    AnalyzeLines analyzer;
    std::vector<std::string> lines = {
        "file7.h:70: warning: this will be deprecated, error: something else"
    };
    ErrorObject obj = analyzer.analyze(lines);

    // one warning, one generic error
    assert(obj.warnings == 1);
    assert(obj.otherErrors == 1);
    assert(obj.totalErrors == 1);
    // two entries: one WARNING, one ERROR
    assert(obj.entries.size() == 2);
    bool hasWarning = false;
    bool hasError = false;
    for (const auto& e : obj.entries) {
        if (e.type == "WARNING") {
            hasWarning = true;
            assert(e.filename == "file7.h");
            assert(e.lineNumber == 70);
        }
        if (e.type == "ERROR") {
            hasError = true;
            assert(e.filename == "file7.h");
            assert(e.lineNumber == 70);
        }
    }
    assert(hasWarning && hasError);
}

void test_analyze_case_insensitive() {
    AnalyzeLines analyzer;
    std::vector<std::string> lines = {
        "file8.go:80: WARNING: something",
        "file8.go:81: error: TYPE mismatch",
        "file8.go:82: VaLuE error occurred"
    };
    ErrorObject obj = analyzer.analyze(lines);

    assert(obj.warnings == 1);
    assert(obj.typeErrors == 1);
    assert(obj.valueErrors == 1);
    assert(obj.totalErrors == 2);

    // Verify filenames and line numbers for the entries
    assert(obj.entries[0].filename == "file8.go");
    assert(obj.entries[0].lineNumber == 80);
    assert(obj.entries[0].type == "WARNING");

    assert(obj.entries[1].filename == "file8.go");
    assert(obj.entries[1].lineNumber == 81);
    assert(obj.entries[1].type == "TYPE");

    assert(obj.entries[2].filename == "file8.go");
    assert(obj.entries[2].lineNumber == 82);
    assert(obj.entries[2].type == "VALUE");
}
