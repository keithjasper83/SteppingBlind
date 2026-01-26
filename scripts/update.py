#!/usr/bin/env python3
import subprocess
import sys
import logging
import os

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

REPO_DIR = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SERVICE_NAME = "blind-controller.service"

def run_command(command, cwd=None, capture_output=True):
    """Run a shell command and return the result."""
    try:
        result = subprocess.run(
            command,
            cwd=cwd,
            check=True,
            text=True,
            stdout=subprocess.PIPE if capture_output else None,
            stderr=subprocess.PIPE if capture_output else None
        )
        return result.stdout.strip() if capture_output else ""
    except subprocess.CalledProcessError as e:
        logger.error(f"Command failed: {' '.join(command)}")
        if capture_output:
            logger.error(f"Stdout: {e.stdout}")
            logger.error(f"Stderr: {e.stderr}")
        raise

def check_for_updates():
    """Check if there are updates available in the git repo."""
    logger.info("Checking for updates...")
    run_command(["git", "fetch", "origin"], cwd=REPO_DIR)

    local_hash = run_command(["git", "rev-parse", "HEAD"], cwd=REPO_DIR)
    remote_hash = run_command(["git", "rev-parse", "origin/main"], cwd=REPO_DIR)

    if local_hash != remote_hash:
        logger.info(f"Update available. Local: {local_hash[:7]}, Remote: {remote_hash[:7]}")
        return True

    logger.info("No updates available.")
    return False

def apply_update():
    """Download, test, and apply the update."""
    logger.info("Starting update process...")

    # Save current branch
    current_branch = run_command(["git", "rev-parse", "--abbrev-ref", "HEAD"], cwd=REPO_DIR)

    try:
        # Checkout remote HEAD to test
        logger.info("Checking out remote version for testing...")
        run_command(["git", "checkout", "origin/main"], cwd=REPO_DIR)

        # Install dependencies (in case they changed)
        logger.info("Installing dependencies...")
        run_command([sys.executable, "-m", "pip", "install", "-r", "requirements.txt"], cwd=REPO_DIR)

        # Run tests
        logger.info("Running tests...")
        run_command([sys.executable, "-m", "pytest"], cwd=REPO_DIR)

        logger.info("Tests passed. Applying update...")

        # Checkout back to main and merge
        run_command(["git", "checkout", current_branch], cwd=REPO_DIR)
        run_command(["git", "merge", "origin/main"], cwd=REPO_DIR)

        logger.info("Update applied successfully.")
        return True

    except Exception as e:
        logger.error(f"Update failed during testing or application: {e}")
        logger.info("Reverting to original state...")
        run_command(["git", "checkout", current_branch], cwd=REPO_DIR)
        return False

def restart_service():
    """Restart the systemd service."""
    logger.info(f"Restarting {SERVICE_NAME}...")
    try:
        # Check if running as root or has sudo
        run_command(["sudo", "systemctl", "restart", SERVICE_NAME])
        logger.info("Service restarted.")
    except Exception as e:
        logger.error(f"Failed to restart service: {e}")

def main():
    try:
        if check_for_updates():
            if apply_update():
                restart_service()
            else:
                logger.warning("Update failed tests. Update aborted.")
                sys.exit(1)
        else:
            logger.info("System is up to date.")
    except Exception as e:
        logger.critical(f"An unexpected error occurred: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
