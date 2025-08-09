# Quanta Series Workspace Testing Plan

This document outlines the necessary steps to manually set up the Quanta series workspace and perform a basic integration test to ensure core functionality is working correctly.

**Note:** The automated scripts `scripts/bootstrap.sh` and `scripts/test_all.sh` are currently empty. The following manual steps are required until those scripts are implemented.

---

## 1. Workspace Setup

The `README.md` describes an automated setup process that is not yet functional. To set up the workspace, you must manually clone the required repositories.

First, create a parent directory for your workspace:

```bash
mkdir quanta-workspace
cd quanta-workspace
```

Now, clone the `quanta_glia` repository, which is the primary entry point:

```bash
git clone https://github.com/drtamarojgreen/quanta_glia.git
```

Next, clone all other necessary sibling repositories into the same workspace directory:

```bash
git clone https://github.com/drtamarojgreen/prismquanta.git
git clone https://github.com/drtamarojgreen/quanta_dorsa.git
git clone https://github.com/drtamarojgreen/quanta_ethos.git
git clone https://github.com/drtamarojgreen/quanta_porto.git
git clone https://github.com/drtamarojgreen/quanta_sensa.git
git clone https://github.com/drtamarojgreen/quanta_synapse.git
```

After these steps, your directory structure should look like this:

```text
quanta-workspace/
├── prismquanta/
├── quanta_dorsa/
├── quanta_ethos/
├── quanta_glia/
├── quanta_porto/
├── quanta_sensa/
└── quanta_synapse/
```

---

## 2. Manual Integration Test (Smoke Test)

With the workspace set up, you can perform a simple integration test to verify that `quanta_glia` can clone another repository and extract its knowledge.

This test will involve running the `quanta_glia.py` script to process the `quanta_ethos` repository.

### Step 2.1: Configure the Test Target

Navigate to the `quanta_glia` directory:

```bash
cd quanta_glia
```

Open the core script `scripts/quanta_glia.py` in a text editor. Locate the `if __name__ == "__main__":` block at the bottom of the file.

Modify the `example_repos` list to contain the URL for the local `quanta_ethos` repository. Since you cloned it into a sibling directory, you can use a relative path.

Change this:
```python
if __name__ == "__main__":
    example_repos = [
        "https://github.com/example/project-one.git",
        "https://github.com/example/project-two.git"
    ]
    main(example_repos)
```

To this:
```python
if __name__ == "__main__":
    # Point to the local sibling repository for the test
    example_repos = [
        "../quanta_ethos"
    ]
    main(example_repos)
```

### Step 2.2: Run the Script

Execute the script from the `quanta_glia` directory:

```bash
python3 scripts/quanta_glia.py
```

The script will:
1. "Clone" the `quanta_ethos` repository into a temporary `repo_cache` directory.
2. Scan the repository for key files (like READMEs, etc.).
3. Store the contents of those files in the `knowledge_base/quanta_ethos/` directory.
4. Clean up the `repo_cache`.

### Step 2.3: Verify the Outcome

Check if the test was successful by listing the contents of the `knowledge_base` directory:

```bash
ls -l knowledge_base/
```

You should see a directory named `quanta_ethos`. Now, check the contents of that directory:

```bash
ls -l knowledge_base/quanta_ethos/
```

A successful test will show that at least one file was extracted from the `quanta_ethos` repository. For example, you might see `quanta_ethos.py` listed. The presence of these files confirms that the `quanta_glia` script can correctly integrate with another project in the workspace.
