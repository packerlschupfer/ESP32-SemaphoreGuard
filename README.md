# SemaphoreGuard

A RAII (Resource Acquisition Is Initialization) wrapper for FreeRTOS semaphores and recursive mutexes that ensures automatic release when going out of scope.

## Overview

SemaphoreGuard provides two C++ classes designed for ESP32/FreeRTOS environments that provide automatic semaphore management through RAII principles:

- **SemaphoreGuard**: For regular mutexes and binary semaphores
- **RecursiveSemaphoreGuard**: For recursive mutexes that can be acquired multiple times by the same task

Both classes acquire a semaphore in their constructor and automatically release it in their destructor, preventing common semaphore-related bugs like forgetting to release a semaphore.

## Features

- **Automatic Resource Management**: Semaphore is automatically released when the guard goes out of scope
- **Exception Safe**: Even if an exception is thrown, the semaphore will be properly released
- **Timeout Support**: Supports both infinite wait and timeout-based semaphore acquisition
- **Lock Status Checking**: Provides `hasLock()` method to verify if semaphore was successfully acquired
- **Lightweight**: Minimal overhead wrapper around FreeRTOS semaphore primitives
- **Zero-Overhead Logging**: Uses LogInterface for flexible logging without forcing Logger instantiation

## Dependencies

- FreeRTOS (included with ESP-IDF)
- C++11 or later

## Installation

### PlatformIO

Add to your `platformio.ini`:
```ini
lib_deps = 
    SemaphoreGuard
```

### Manual Installation

Copy the `src/SemaphoreGuard.h` and `src/SemaphoreGuard.cpp` files to your project.

## Usage Examples

### Basic Usage with Infinite Wait

```cpp
#include <SemaphoreGuard.h>

SemaphoreHandle_t xMutex = xSemaphoreCreateMutex();

void criticalSection() {
    SemaphoreGuard guard(xMutex);  // Waits indefinitely for semaphore
    
    if (guard.hasLock()) {
        // Critical section - semaphore is held
        // Perform protected operations here
        
    } // Semaphore automatically released when guard goes out of scope
}
```

### Usage with Timeout

```cpp
void criticalSectionWithTimeout() {
    SemaphoreGuard guard(xMutex, pdMS_TO_TICKS(100));  // 100ms timeout
    
    if (guard.hasLock()) {
        // Successfully acquired semaphore
        // Perform protected operations
    } else {
        // Failed to acquire semaphore within timeout
        Serial.println("Could not acquire semaphore!");
    }
    // Semaphore automatically released if it was acquired
}
```

### Protecting Shared Resources

```cpp
class SharedResource {
private:
    SemaphoreHandle_t m_mutex;
    int m_data;
    
public:
    SharedResource() : m_data(0) {
        m_mutex = xSemaphoreCreateMutex();
    }
    
    ~SharedResource() {
        vSemaphoreDelete(m_mutex);
    }
    
    void increment() {
        SemaphoreGuard guard(m_mutex);
        if (guard.hasLock()) {
            m_data++;
        }
    }
    
    int read() {
        SemaphoreGuard guard(m_mutex, pdMS_TO_TICKS(50));
        if (guard.hasLock()) {
            return m_data;
        }
        return -1;  // Indicate error
    }
};
```

### Early Return Safety

```cpp
bool processData() {
    SemaphoreGuard guard(xDataMutex);
    
    if (!guard.hasLock()) {
        return false;  // Could not acquire semaphore
    }
    
    if (dataNotReady()) {
        return false;  // Semaphore automatically released
    }
    
    if (errorCondition()) {
        return false;  // Semaphore automatically released
    }
    
    // Process data
    return true;  // Semaphore automatically released
}
```

### Recursive Mutex Usage

```cpp
SemaphoreHandle_t xRecursiveMutex = xSemaphoreCreateRecursiveMutex();

class Counter {
private:
    SemaphoreHandle_t m_mutex;
    int m_value;
    
public:
    Counter() : m_value(0) {
        m_mutex = xSemaphoreCreateRecursiveMutex();
    }
    
    ~Counter() {
        vSemaphoreDelete(m_mutex);
    }
    
    void increment() {
        RecursiveSemaphoreGuard guard(m_mutex);
        if (guard.hasLock()) {
            m_value++;
            // Can call other methods that also lock
            if (m_value == 10) {
                reset();  // This also acquires the mutex
            }
        }
    }
    
    void reset() {
        RecursiveSemaphoreGuard guard(m_mutex);
        if (guard.hasLock()) {
            m_value = 0;
        }
    }
};
```

### Debug Mode Usage

```cpp
// Enable debug mode in your build
#define SEMAPHORE_GUARD_DEBUG

// Use convenience macros for automatic file/line tracking
void debugExample() {
    SEMAPHORE_GUARD(xMutex);  // Logs acquisition location
    
    // Critical section
    
}  // Logs release location and hold time

// With timeout
void debugTimeoutExample() {
    SEMAPHORE_GUARD_TIMEOUT(xMutex, pdMS_TO_TICKS(100));
    
    if (guard.hasLock()) {
        // Protected code
    }
}
```

### Logging Configuration

This library supports flexible logging configuration with zero overhead in production builds:

#### Using ESP-IDF Logging (Default)
No configuration needed. The library will use ESP-IDF logging:

```cpp
#include <SemaphoreGuard.h>

void example() {
    SemaphoreGuard guard(xMutex);
    // Logs go directly to ESP-IDF logging system
}
```

#### Using Custom Logger
Define `USE_CUSTOM_LOGGER` in your build flags:

```ini
build_flags = -DUSE_CUSTOM_LOGGER
lib_deps = 
    Logger  ; Must include Logger library when using custom logger
    SemaphoreGuard
```

Your application must include LogInterfaceImpl.h:
```cpp
#include <Logger.h>
#include <LogInterfaceImpl.h>
#include <SemaphoreGuard.h>

void setup() {
    Logger::getInstance().init(1024);
    // SemaphoreGuard will now log through custom Logger
}
```

#### Debug Logging
To enable debug/verbose logging for this library:

```ini
build_flags = -DSEMAPHORE_GUARD_DEBUG
```

**Production vs Debug Behavior:**
- **Without `SEMAPHORE_GUARD_DEBUG`**: Only Error, Warning, and Info logs are shown. Debug and Verbose logs are completely removed at compile time (zero overhead).
- **With `SEMAPHORE_GUARD_DEBUG`**: All log levels are enabled, including Debug and Verbose logs for detailed troubleshooting.

#### Complete Example
```ini
[env:production]
; Production build - minimal logging
platform = espressif32
framework = arduino

[env:debug]
; Debug build with custom logger
platform = espressif32
framework = arduino
build_flags = 
    -DUSE_CUSTOM_LOGGER      ; Use custom logger
    -DSEMAPHORE_GUARD_DEBUG  ; Enable debug logging for this library
lib_deps = 
    Logger
    SemaphoreGuard
```

This library is C++11 compatible and does not require C++17 features.

## API Reference

### SemaphoreGuard

#### Constructors

#### `explicit SemaphoreGuard(SemaphoreHandle_t handle)`
Constructs a guard that attempts to take the semaphore with infinite timeout (`portMAX_DELAY`).

**Parameters:**
- `handle`: The FreeRTOS semaphore handle to acquire

#### `SemaphoreGuard(SemaphoreHandle_t handle, TickType_t timeout)`
Constructs a guard that attempts to take the semaphore with a specified timeout.

**Parameters:**
- `handle`: The FreeRTOS semaphore handle to acquire
- `timeout`: Maximum time to wait in ticks (use `pdMS_TO_TICKS()` to convert from milliseconds)

### Methods

#### `bool hasLock() const`
Returns whether the semaphore was successfully acquired.

**Returns:**
- `true` if the semaphore is held by this guard
- `false` if the semaphore could not be acquired (e.g., timeout expired)

#### `bool isValid() const`
Returns whether the guard has a valid (non-null) semaphore handle.

**Returns:**
- `true` if the handle is valid
- `false` if the handle is null

#### `SemaphoreHandle_t getHandle() const`
Returns the underlying semaphore handle for advanced use cases.

#### Destructor

##### `~SemaphoreGuard()`
Automatically releases the semaphore if it was successfully acquired. Safe to call even if the semaphore was not acquired.

### RecursiveSemaphoreGuard

Has the same API as SemaphoreGuard but uses `xSemaphoreTakeRecursive()` and `xSemaphoreGiveRecursive()` for recursive mutex support. Use this class when working with mutexes created with `xSemaphoreCreateRecursiveMutex()`.

## Design Patterns

This library implements the RAII (Resource Acquisition Is Initialization) pattern, which ties resource management to object lifetime. This pattern is particularly useful in embedded systems where proper resource management is critical.

## Implemented Improvements

### Critical Priority (✅ Completed)
1. **Null Handle Protection** - Added null checks in all constructors and destructor
   - Prevents crashes when passing invalid handles
   
2. **Copy/Move Semantics** - Explicitly deleted copy and move constructors/operators
   - Prevents accidental double-release of semaphores

### High Priority (✅ Completed)
3. **ISR Safety Check** - Added runtime checks using `xPortInIsrContext()`
   - Prevents illegal usage in ISR context with error logging
   
4. **Debug Support** - Added comprehensive debug logging with `SEMAPHORE_GUARD_DEBUG`
   - Tracks acquisition location, release location, and hold time
   - Provides convenience macros for automatic file/line tracking

5. **Recursive Mutex Support** - Added dedicated `RecursiveSemaphoreGuard` class
   - Proper support for recursive mutexes with same safety features

### Suggested Future Improvements

#### Medium Priority
1. **Lock Upgrade/Downgrade** - Add methods to temporarily release and reacquire
   - Useful for reducing lock contention in complex scenarios
   
2. **Try-Lock Support** - Add non-blocking acquisition methods
   - `bool tryAcquire()` for immediate return if lock unavailable

#### Low Priority
3. **Statistics Tracking** - Extended metrics beyond debug mode
   - Average hold times, contention counts, peak usage
   
4. **Template Version** - Generic template for different synchronization primitives
   - Could support counting semaphores, event groups
   
5. **Scoped Lock Guards** - Nested scope support
   - Allow temporary unlock/relock within a scope

## Thread Safety

SemaphoreGuard itself is thread-safe as it only operates on its own member variables and FreeRTOS thread-safe primitives. However, the protected resources must still be accessed only while holding the semaphore.

## Performance Considerations

- Minimal overhead: Only stores handle and boolean flag
- No dynamic memory allocation
- Inline-friendly methods for optimized builds
- Stack-based RAII ensures deterministic cleanup

## License

[Specify your license here]

## Contributing

[Add contribution guidelines if applicable]