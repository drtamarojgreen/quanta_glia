# Design Document: Quanta Lista

## 1. Overview
Quanta Lista is a Qt-based task management application designed for the PrismQuanta ecosystem. It focuses on structural correctness and multi-format configuration management (XML and JSON).

## 2. Architecture
The application follows a modular architecture:
- **Task**: Data model representing a single task.
- **TaskManager**: Handles the collection of tasks and business logic for task manipulation.
- **ConfigManager**: Responsible for serializing and deserializing tasks and configuration to/from XML and JSON.
- **MainWindow**: The graphical user interface, built using Qt Widgets.

## 3. Component Interaction
1. `MainWindow` interacts with `TaskManager` to display and modify tasks.
2. `MainWindow` triggers `ConfigManager` to save or load data.
3. `ConfigManager` reads/writes `Task` objects using `QXmlStream` and `QJson` modules.

## 4. Persistence Strategy
- **XML**: Used for structured, hierarchical configuration data.
- **JSON**: Used for lightweight, interchangeable task data.
- Both formats will be supported for both loading and saving.
