#ifndef _RECURSIVE_SEMAPHORE_GUARD_H_
#define _RECURSIVE_SEMAPHORE_GUARD_H_
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Include the logging configuration
#include "SemaphoreGuardLogging.h"

class RecursiveSemaphoreGuard {
public:
    // Constructor: Takes the recursive mutex with an infinite timeout
    explicit RecursiveSemaphoreGuard(SemaphoreHandle_t handle);

    // Constructor: Takes the recursive mutex with a provided timeout
    RecursiveSemaphoreGuard(SemaphoreHandle_t handle, TickType_t timeout);

    // Destructor: Gives the recursive mutex back
    ~RecursiveSemaphoreGuard();

    // Delete copy constructor and copy assignment to prevent double-release
    RecursiveSemaphoreGuard(const RecursiveSemaphoreGuard&) = delete;
    RecursiveSemaphoreGuard& operator=(const RecursiveSemaphoreGuard&) = delete;

    // Delete move constructor and move assignment for safety
    RecursiveSemaphoreGuard(RecursiveSemaphoreGuard&&) = delete;
    RecursiveSemaphoreGuard& operator=(RecursiveSemaphoreGuard&&) = delete;

#ifdef SEMAPHORE_GUARD_DEBUG
    // Debug constructors with file/line info
    RecursiveSemaphoreGuard(SemaphoreHandle_t handle, const char* file, int line);
    RecursiveSemaphoreGuard(SemaphoreHandle_t handle, TickType_t timeout, const char* file, int line);
#endif

    // Check if the recursive mutex was successfully acquired
    bool hasLock() const;

    // Get the semaphore handle (for advanced use cases)
    SemaphoreHandle_t getHandle() const { return m_handle; }

    // Check if this guard is valid (has non-null handle)
    bool isValid() const { return m_handle != nullptr; }

private:
    SemaphoreHandle_t m_handle;
    bool m_taken;  // Indicates whether the recursive mutex was successfully taken

#ifdef SEMAPHORE_GUARD_DEBUG
    const char* m_file;
    int m_line;
    TickType_t m_acquireTime;
#endif
};

// Macro for debug support
#ifdef SEMAPHORE_GUARD_DEBUG
    #define RECURSIVE_SEMAPHORE_GUARD(handle) RecursiveSemaphoreGuard guard(handle, __FILE__, __LINE__)
    #define RECURSIVE_SEMAPHORE_GUARD_TIMEOUT(handle, timeout) RecursiveSemaphoreGuard guard(handle, timeout, __FILE__, __LINE__)
#else
    #define RECURSIVE_SEMAPHORE_GUARD(handle) RecursiveSemaphoreGuard guard(handle)
    #define RECURSIVE_SEMAPHORE_GUARD_TIMEOUT(handle, timeout) RecursiveSemaphoreGuard guard(handle, timeout)
#endif

#endif  // _RECURSIVE_SEMAPHORE_GUARD_H_