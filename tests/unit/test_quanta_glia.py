import unittest
import sys
import os
from pathlib import Path
import shutil

# Add the 'scripts' directory to the Python path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', 'scripts')))

from quanta_glia import extract_key_info

class TestQuantaGlia(unittest.TestCase):

    def setUp(self):
        """Set up a temporary directory with dummy files for testing."""
        self.test_dir = Path("test_repo_dir")
        self.test_dir.mkdir()
        (self.test_dir / "README.md").write_text("This is a readme file.")
        (self.test_dir / "LICENSE").write_text("This is a license file.")
        (self.test_dir / "some_other_file.txt").write_text("This is another file.")
        self.sub_dir = self.test_dir / "docs"
        self.sub_dir.mkdir()
        (self.sub_dir / "CONTRIBUTING.md").write_text("How to contribute.")

    def tearDown(self):
        """Clean up the temporary directory."""
        shutil.rmtree(self.test_dir)

    def test_extract_key_info(self):
        """Test that key information is extracted correctly."""
        extracted_data = extract_key_info(self.test_dir)
        self.assertIn("README.md", extracted_data)
        self.assertEqual(extracted_data["README.md"], "This is a readme file.")
        self.assertIn("LICENSE", extracted_data)
        self.assertEqual(extracted_data["LICENSE"], "This is a license file.")
        self.assertIn("CONTRIBUTING.md", extracted_data)
        self.assertEqual(extracted_data["CONTRIBUTING.md"], "How to contribute.")
        self.assertNotIn("some_other_file.txt", extracted_data)

if __name__ == '__main__':
    unittest.main()
