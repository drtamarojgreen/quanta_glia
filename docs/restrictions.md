# CDD Restrictions: Glia Standalone Application

This document defines the structural, pattern, tool, and architectural restrictions for the Glia standalone C++ application, following Chai Driven Development (CDD) principles.

## 1. Pattern Restrictions
- **FORBID raw pointers:** Use of raw pointers for resource management is strictly prohibited.
- **REQUIRE smart pointers:** All dynamic allocations must be managed by `std::unique_ptr` or `std::shared_ptr`.
- **FORBID empty catch blocks:** All exceptions must be handled or logged.

## 2. Tool Restrictions
- **LIMIT configuration handling:** XML and JSON serialization/deserialization must be performed exclusively using Qt's native libraries (`QDomDocument`, `QJsonDocument`).
- **REQUIRE QProcess for integration:** All interactions with existing Python scripts (`quanta_glia.py`, `pruner.py`) must be handled via `QProcess`.

## 3. Architectural Restrictions
- **REQUIRE separation of concerns:** Configuration data (`GliaConfig`), serialization logic (`GliaConfigManager`), and application control logic must be kept in distinct classes/files.
- **LIMIT system dependencies:** The application should depend only on Qt6 Core and Qt6 XML modules for its core functionality.
- **FORBID TODOs in source:** All deferred work must be tracked in `chai_checkins.md`.
