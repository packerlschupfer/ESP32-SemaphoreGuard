# SemaphoreGuard - CLAUDE.md

## Overview
RAII semaphore guard for FreeRTOS counting and binary semaphores. Similar to MutexGuard but for semaphore-specific use cases like resource counting and signaling.

## Key Features
- RAII pattern for automatic semaphore management
- Timeout support
- Non-copyable, non-movable
- Debug mode with file/line tracking

## Usage
```cpp
SemaphoreHandle_t sem = xSemaphoreCreateBinary();

void waitForSignal() {
    SemaphoreGuard guard(sem, pdMS_TO_TICKS(1000));
    if (guard.hasLock()) {
        // Semaphore was taken
    }
}
```

## Debug Mode
```cpp
// With debug enabled, tracks acquisition location
#define SEMAPHORE_GUARD_DEBUG

SEMAPHORE_GUARD(sem);  // Expands to include __FILE__, __LINE__
SEMAPHORE_GUARD_TIMEOUT(sem, pdMS_TO_TICKS(100));
```

## Difference from MutexGuard
| Feature | MutexGuard | SemaphoreGuard |
|---------|------------|----------------|
| Primary use | Mutual exclusion | Resource counting/signaling |
| Default timeout | 100ms | Infinite |
| Debug macros | No | Yes |

## Thread Safety
- Safe for use across multiple tasks
- Proper give/take semantics maintained

## Build Configuration
```ini
build_flags =
    -DSEMAPHORE_GUARD_DEBUG  ; Enable debug with file/line tracking
```
