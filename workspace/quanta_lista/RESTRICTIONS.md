# SDD Restrictions: Quanta Lista

## 1. Structural Guardrails
- **FORBID**: Empty catch blocks. All exceptions must be handled or logged.
- **FORBID**: Meaningless assertions (e.g., `assert(true)`).
- **REQUIRE**: Use of RAII for all resource management.
- **REQUIRE**: Explicit error handling for all File I/O operations.
- **LIMIT**: No manual memory management (prefer `std::unique_ptr` or Qt's parent-child system).

## 2. Persistence Restrictions
- **REQUIRE**: `ConfigManager` must use `QXmlStreamWriter` and `QXmlStreamReader` for XML.
- **REQUIRE**: `ConfigManager` must use `QJsonDocument` for JSON.
- **FORBID**: Direct string manipulation for creating XML or JSON payloads.

## 3. UI Restrictions
- **REQUIRE**: Separation of UI logic from business logic. `MainWindow` should not contain persistence or complex task management logic.
