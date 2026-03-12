import unittest
import sys
import os
import json
from pathlib import Path
import shutil
from unittest.mock import patch

# Add the 'scripts' directory to the Python path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', 'scripts')))

from quanta_glia import (
    extract_key_info, load_config, clone_repo,
    store_to_knowledge_base, prune_cache, main
)

class TestQuantaGlia(unittest.TestCase):

    def setUp(self):
        """Set up a temporary directory with dummy files for testing."""
        self.test_dir = Path("test_repo_dir")
        self.test_dir.mkdir(exist_ok=True)
        (self.test_dir / "README.md").write_text("This is a readme file.")
        (self.test_dir / "LICENSE").write_text("This is a license file.")
        (self.test_dir / "some_other_file.txt").write_text("This is another file.")
        self.sub_dir = self.test_dir / "docs"
        self.sub_dir.mkdir(exist_ok=True)
        (self.sub_dir / "CONTRIBUTING.md").write_text("How to contribute.")

        # Create a dummy config file for testing load_config
        self.config_path = self.test_dir / "dummy_config.yaml"
        self.config_path.write_text("""
main:
  knowledge_base: "./kb"
  repo_cache: "./cache"
  target_topics:
    - "TOPIC1"
    - "TOPIC2"
  max_repos: 5
section2:
  key: "value"
""")
        # Setup for clone_repo and main tests
        self.local_repo_src = self.test_dir / "local_repo_src"
        self.local_repo_src.mkdir(exist_ok=True)
        (self.local_repo_src / "a_file.txt").write_text("hello")
        (self.local_repo_src / "README.md").write_text("This is a test readme.")

        self.cache_dir = self.test_dir / "cache"
        self.cache_dir.mkdir(exist_ok=True)

        # Setup for store_to_knowledge_base tests
        self.kb_dir = self.test_dir / "knowledge_base"
        self.kb_dir.mkdir(exist_ok=True)


    def tearDown(self):
        """Clean up the temporary directory."""
        shutil.rmtree(self.test_dir)

    def test_extract_key_info(self):
        """Test that key information is extracted correctly."""
        # Ensure we only scan the test_dir and not its subdirs that might have other READMEs
        # Actually extract_key_info uses os.walk(repo_path)
        extracted_data = extract_key_info(self.test_dir)
        # In setUp:
        # (self.test_dir / "README.md").write_text("This is a readme file.")
        # (self.local_repo_src / "README.md").write_text("This is a test readme.")
        # self.local_repo_src is a subdir of test_dir.
        # So we might get multiple README.md if they have same name.
        # The current extract_key_info implementation overwrites.
        self.assertIn("README.md", extracted_data)
        self.assertIn("LICENSE", extracted_data)
        self.assertEqual(extracted_data["LICENSE"], "This is a license file.")
        self.assertIn("CONTRIBUTING.md", extracted_data)
        self.assertEqual(extracted_data["CONTRIBUTING.md"], "How to contribute.")
        self.assertNotIn("some_other_file.txt", extracted_data)

    def test_load_config(self):
        """Test that the simple YAML config is loaded correctly."""
        config = load_config(self.config_path)
        self.assertIsNotNone(config)
        self.assertIn("main", config)
        self.assertIn("section2", config)

        main_section = config["main"]
        self.assertEqual(main_section["knowledge_base"], "./kb")
        self.assertEqual(main_section["repo_cache"], "./cache")
        self.assertEqual(main_section["max_repos"], 5)
        self.assertEqual(main_section["target_topics"], ["TOPIC1", "TOPIC2"])

        section2 = config["section2"]
        self.assertEqual(section2["key"], "value")

    def test_clone_repo_local(self):
        """Test that a local directory is correctly copied to the cache."""
        with patch('quanta_glia.REPO_CACHE', self.cache_dir):
            dest_path = clone_repo(str(self.local_repo_src), self.cache_dir)

            expected_dest = self.cache_dir / self.local_repo_src.name
            self.assertEqual(dest_path, expected_dest)
            self.assertTrue(expected_dest.exists())
            self.assertTrue((expected_dest / "a_file.txt").exists())
            self.assertEqual((expected_dest / "a_file.txt").read_text(), "hello")

    def test_clone_repo_already_exists(self):
        """Test that cloning is skipped if the repo is already in the cache."""
        # Create a dummy repo in the cache
        (self.cache_dir / "local_repo_src").mkdir()

        with patch('quanta_glia.REPO_CACHE', self.cache_dir):
            dest_path = clone_repo(str(self.local_repo_src), self.cache_dir)

            # Should return the existing path
            self.assertEqual(dest_path, self.cache_dir / "local_repo_src")
            # Should not contain the file, because it wasn't re-copied
            self.assertFalse((dest_path / "a_file.txt").exists())

    def test_store_to_knowledge_base(self):
        """Test that extracted data is correctly stored in the knowledge base."""
        repo_name = "test_repo"
        extracted_data = {
            "info.txt": "This is info.",
            "log.txt": "This is a log."
        }

        with patch('quanta_glia.KNOWLEDGE_BASE', self.kb_dir):
            store_to_knowledge_base(repo_name, extracted_data)

        repo_kb_path = self.kb_dir / repo_name
        self.assertTrue(repo_kb_path.is_dir())

        analysis_file = repo_kb_path / "repository_analysis.json"
        self.assertTrue(analysis_file.is_file())
        content = json.loads(analysis_file.read_text())
        self.assertEqual(content["info.txt"], "This is info.")
        self.assertEqual(content["log.txt"], "This is a log.")

    def test_prune_cache(self):
        """Test that the cache pruning function removes all directories."""
        # Create a directory and a file within the cache to be pruned
        dir_to_prune = self.cache_dir / "repo_to_prune"
        dir_to_prune.mkdir()
        (dir_to_prune / "file.txt").write_text("delete me")

        # Create a loose file in the cache, which should NOT be pruned
        loose_file = self.cache_dir / "loose_file.txt"
        loose_file.write_text("do not delete me")

        with patch('quanta_glia.REPO_CACHE', self.cache_dir):
            prune_cache(self.cache_dir)

        # Assert that the directory is gone
        self.assertFalse(dir_to_prune.exists())
        # Assert that the loose file remains
        self.assertTrue(loose_file.exists())

    def test_main_integration(self):
        """Test the main function's integration of cloning, extracting, storing, and pruning."""
        repo_urls = [str(self.local_repo_src)]

        # We need a real config file for main to load
        config_path = self.test_dir / "main_test_config.yaml"
        config_path.write_text(f"""
main:
  knowledge_base: "{self.kb_dir}"
  repo_cache: "{self.cache_dir}"
  max_repos: 10
""")

        main(repo_urls, config_path=str(config_path))

        # Check that the knowledge base was populated correctly
        repo_name = self.local_repo_src.name
        expected_kb_path = self.kb_dir / repo_name
        self.assertTrue(expected_kb_path.is_dir())

        analysis_file = expected_kb_path / "repository_analysis.json"
        self.assertTrue(analysis_file.is_file())

        # Check that the cache was pruned
        cloned_repo_path = self.cache_dir / repo_name
        self.assertFalse(cloned_repo_path.exists())


if __name__ == '__main__':
    unittest.main()
