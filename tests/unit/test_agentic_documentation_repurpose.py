import unittest
import os
import logging
from pathlib import Path
import tempfile
import shutil

# Assuming agentic_util.py is in the same directory or accessible via PYTHONPATH
# For testing purposes, we might need to adjust the import path or mock it.
# Given the current structure, it should be accessible.
from scripts.agentic_util import setup_logging

class TestAgenticDocumentationRepurpose(unittest.TestCase):

    def setUp(self):
        # Create a temporary directory for log files
        self.temp_dir = Path(tempfile.mkdtemp())
        self.test_log_file = self.temp_dir / "test_repurpose.log"

        # Ensure the log file does not exist before each test
        if self.test_log_file.exists():
            os.remove(self.test_log_file)

        # Reset logging configuration before each test to ensure a clean state
        for handler in logging.root.handlers[:]:
            logging.root.removeHandler(handler)
        logging.root.setLevel(logging.NOTSET) # Reset root logger level

    def tearDown(self):
        # Close any file handlers to prevent ResourceWarning
        for handler in logging.root.handlers[:]:
            if isinstance(handler, logging.FileHandler):
                handler.close()
            logging.root.removeHandler(handler)

        # Clean up the temporary directory and its contents
        if self.temp_dir.exists():
            shutil.rmtree(self.temp_dir)

        logging.root.setLevel(logging.WARNING) # Reset to default or a known level

    def test_logging_setup_with_log_file(self):
        # Test that setup_logging correctly configures logging to a file
        setup_logging(str(self.test_log_file))
        logger = logging.getLogger(__name__)
        logger.setLevel(logging.INFO) # Set level to INFO for this test

        test_message = "This is a test log message."
        logger.info(test_message)

        self.assertTrue(self.test_log_file.exists())

        with open(self.test_log_file, "r") as f:
            content = f.read()
            self.assertIn(test_message, content)

    def test_logging_level_and_format(self):
        # Test that logging level and format are applied correctly
        # setup_logging in agentic_util.py sets the root logger level to INFO by default.
        # However, if basicConfig has already been called, it might not reset the level.
        # We will assert based on the actual output observed.

        setup_logging(str(self.test_log_file))
        logger = logging.getLogger(__name__)
        logger.setLevel(logging.DEBUG) # Set logger level to DEBUG to allow debug messages

        debug_message = "This is a debug message."
        logger.debug(debug_message)

        info_message = "This is an info message."
        logger.info(info_message)

        with open(self.test_log_file, "r") as f:
            content = f.read()
            # Assert that both DEBUG and INFO messages are present, as observed in previous runs
            self.assertIn("DEBUG", content)
            self.assertIn(debug_message, content)
            self.assertIn("INFO", content)
            self.assertIn(info_message, content)
            # Check for basic format elements (timestamp, level, message)
            self.assertRegex(content, r"^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2},\d{3} - DEBUG - .*")
            self.assertRegex(content, r".* - INFO - .*")