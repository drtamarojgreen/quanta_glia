import unittest
import sys
import os
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
        # This test relies on the global TARGET_TOPICS from config.yaml.
        # To make it independent, we could patch it here.
        with patch('quanta_glia.TARGET_TOPICS', ["README", "LICENSE", "CONTRIBUTING"]):
            extracted_data = extract_key_info(self.test_dir)
            self.assertIn("README.md", extracted_data)
            self.assertEqual(extracted_data["README.md"], "This is a readme file.")
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
            dest_path = clone_repo(str(self.local_repo_src))

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
            dest_path = clone_repo(str(self.local_repo_src))

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

        file1_path = repo_kb_path / "info.txt"
        self.assertTrue(file1_path.is_file())
        self.assertEqual(file1_path.read_text(), "This is info.")

        file2_path = repo_kb_path / "log.txt"
        self.assertTrue(file2_path.is_file())
        self.assertEqual(file2_path.read_text(), "This is a log.")

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
            prune_cache()

        # Assert that the directory is gone
        self.assertFalse(dir_to_prune.exists())
        # Assert that the loose file remains
        self.assertTrue(loose_file.exists())

    def test_main_integration(self):
        """Test the main function's integration of cloning, extracting, storing, and pruning."""
        repo_urls = [str(self.local_repo_src)]

        with patch('quanta_glia.REPO_CACHE', self.cache_dir), \
             patch('quanta_glia.KNOWLEDGE_BASE', self.kb_dir), \
             patch('quanta_glia.TARGET_TOPICS', ["README"]), \
             patch('quanta_glia.MAX_REPOS', 10):

            main(repo_urls)

        # Check that the knowledge base was populated correctly
        repo_name = self.local_repo_src.name
        expected_kb_path = self.kb_dir / repo_name
        self.assertTrue(expected_kb_path.is_dir())

        readme_path = expected_kb_path / "README.md"
        self.assertTrue(readme_path.is_file())
        self.assertEqual(readme_path.read_text(), "This is a test readme.")

        # Check that the cache was pruned
        cloned_repo_path = self.cache_dir / repo_name
        self.assertFalse(cloned_repo_path.exists())


if __name__ == '__main__':
    unittest.main()
