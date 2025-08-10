# QuantaGlia Testing Plan

This document outlines the automated process for setting up the QuantaGlia workspace and running the full test suite.

---

## Automated Testing Suite

The primary method for testing the QuantaGlia project and its integration with the wider workspace is the master test script: `scripts/test_all.sh`.

### Running the Full Test Suite

To run all tests, simply execute the script from the root of the `quanta_glia` repository:

```bash
bash scripts/test_all.sh
```

### What the Script Does

This script provides a fully automated, end-to-end testing process. It will:

1.  **Bootstrap the Workspace:** Automatically run `scripts/bootstrap.sh` to clone or verify all required sibling repositories.
2.  **Run Unit Tests:** Execute the Python unit tests for the `quanta_glia` repository.
3.  **Run Integration Tests:** Perform a series of integration tests, including a self-contained test and a test against a sibling repository (`quanta_ethos`).
4.  **Run Workspace-Wide Tests:** Execute the test suites for all other repositories in the workspace.

The script is designed to exit immediately if any step fails, providing clear feedback on the location of the error. For a complete testing solution, always use this master script.
