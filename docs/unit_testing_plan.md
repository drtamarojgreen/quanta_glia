# Unit Testing Plan

This document outlines the official strategies and conventions for developing, implementing, and running unit tests within this repository. Adhering to these guidelines ensures that our tests are consistent, maintainable, and effective at catching regressions.

---

## 1. Testing Framework

All unit tests should be written using Python's built-in `unittest` framework. It is the standard for this project, as it requires no external dependencies and is already in use in the existing test suite (see `tests/unit/test_quanta_glia.py`).

---

## 2. Test Structure and Location

To maintain a clear and organized test suite, all unit tests must follow these structural conventions:

-   **Directory**: All unit test files must be placed in the `tests/unit/` directory.
-   **File Naming**: Test files should be named using the pattern `test_<module_name>.py`, where `<module_name>` corresponds to the Python module being tested.
-   **Test Class Naming**: Within a test file, the test class should be named using the pattern `Test<ClassName>`, where `<ClassName>` is the name of the class being tested. For module-level functions, the class name can be a descriptive name like `Test<ModuleName>`.
-   **Test Method Naming**: Each test case should be a method within the test class, and its name must start with `test_`. The rest of the name should clearly describe what the test case is verifying (e.g., `test_extract_key_info_with_valid_data`).

---

## 3. What to Test

Unit tests should focus on testing the smallest pieces of logic in isolation (the "units"). A good unit test is focused and does not depend on external systems or the state of other tests.

-   **Public Interfaces**: Prioritize testing the public functions and methods of your modules and classes.
-   **Business Logic**: Ensure all critical business logic is thoroughly tested.
-   **Positive and Negative Cases**: For each function, include tests that verify it works correctly with valid inputs (positive cases) and handles invalid or unexpected inputs gracefully (negative cases).
-   **Edge Cases**: Test for edge cases, such as empty lists, zero values, `None` inputs, or other boundary conditions that might cause unexpected behavior.

---

## 4. Writing Tests

To ensure tests are clean, readable, and maintainable, follow these best practices:

-   **Arrange-Act-Assert (AAA)**: Structure your test methods following the AAA pattern:
    1.  **Arrange**: Set up the test conditions. This includes creating objects, preparing mock data, or setting up any required state.
    2.  **Act**: Execute the function or method being tested.
    3.  **Assert**: Verify that the outcome of the action is what you expected. Use the various `self.assert...` methods provided by `unittest.TestCase`.

-   **Test Fixtures**: Use the `setUp()` and `tearDown()` methods to manage test fixtures.
    -   `setUp()`: Use this method to create any objects or resources that are needed by multiple test cases in a class (e.g., creating a temporary directory or a database connection). It is run before each test method.
    -   `tearDown()`: Use this method to clean up any resources created in `setUp()`. It is run after each test method, regardless of whether the test passed or failed.

-   **Independence**: Each test case must be completely independent. The success or failure of one test should never affect another. Avoid creating dependencies between tests.

---

## 5. Mocking and Patching

When a unit of work has dependencies on other parts of the system (e.g., network requests, database calls, or other classes), those dependencies should be replaced with "mocks" during testing. Mocking allows you to isolate the code you are testing from its dependencies.

For this project, we will use the `unittest.mock` library, which is part of the standard Python library.

### Key Use Cases:

-   **`mock.Mock` / `mock.MagicMock`**: Create mock objects that can be configured to return specific values, track calls, and assert how they were used.
-   **`@patch` decorator**: Use this to temporarily replace objects in a specific scope for the duration of a test.

**Example:**
```python
from unittest.mock import patch

@patch('your_module.some_function_to_mock')
def test_something(self, mock_some_function):
    # Arrange: Configure the mock to return a specific value
    mock_some_function.return_value = "mocked_value"

    # Act: Call the code that uses the mocked function
    result = your_code_that_calls_some_function()

    # Assert: Check that the code behaved as expected
    self.assertEqual(result, "expected_output")
    mock_some_function.assert_called_once()
```

---

## 6. Test Coverage

Test coverage is a metric that measures the percentage of your codebase that is executed by your tests. While 100% coverage is not always the goal, aiming for a high percentage helps ensure that most of the critical code paths are being tested.

It is possible to measure test coverage using external tools, which can help identify areas of the code that are not well-tested.

---

## 7. Running Tests

The entire unit test suite can be run from the root of the repository.

### Running All Unit Tests

To discover and run all unit tests in the `tests/unit/` directory, use the following command:

```bash
python3 -m unittest discover tests/unit/
```

### Test Automation Script

The `scripts/test_all.sh` script is the designated place for running all automated checks for this repository. It is recommended that the command to run unit tests be added to this script to ensure they are executed as part of the standard testing process.

Example addition to `scripts/test_all.sh`:
```bash
echo "Running unit tests..."
python3 -m unittest discover tests/unit/
```
