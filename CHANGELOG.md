# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] - 2025-12-04

### Added
- Initial public release
- SemaphoreGuard class for FreeRTOS counting and binary semaphores
- Automatic semaphore take on construction, give on destruction
- Timeout support for semaphore acquisition (default: infinite wait)
- hasLock() method to verify successful acquisition
- isValid() method to check for valid semaphore handle
- Null handle protection in all operations
- ISR context detection and protection
- Non-copyable, non-movable design for safe ownership
- Optional debug logging with SEMAPHORE_GUARD_DEBUG macro
- File/line tracking in debug mode
- Convenience macros: SEMAPHORE_GUARD() and SEMAPHORE_GUARD_TIMEOUT()

### Notes
- Production-tested in FreeRTOS multi-task environment
- Used for resource counting and task synchronization
- Previous internal versions (v1.x) not publicly released
- Reset to v0.1.0 for clean public release start
