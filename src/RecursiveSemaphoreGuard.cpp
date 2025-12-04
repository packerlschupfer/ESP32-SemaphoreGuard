#include "RecursiveSemaphoreGuard.h"

RecursiveSemaphoreGuard::RecursiveSemaphoreGuard(SemaphoreHandle_t handle) 
    : m_handle(handle), m_taken(false) {
    // Check for null handle
    if (m_handle == nullptr) {
        RSEMG_LOG_E("Null recursive mutex handle provided");
        return;
    }
    
    // Check if we're in ISR context (FreeRTOS restriction)
    if (xPortInIsrContext()) {
        RSEMG_LOG_E("Cannot use RecursiveSemaphoreGuard in ISR context");
        return;
    }
    
    m_taken = (xSemaphoreTakeRecursive(m_handle, portMAX_DELAY) == pdTRUE);
}

RecursiveSemaphoreGuard::RecursiveSemaphoreGuard(SemaphoreHandle_t handle, TickType_t timeout) 
    : m_handle(handle), m_taken(false) {
    // Check for null handle
    if (m_handle == nullptr) {
        RSEMG_LOG_E("Null recursive mutex handle provided");
        return;
    }
    
    // Check if we're in ISR context (FreeRTOS restriction)
    if (xPortInIsrContext()) {
        RSEMG_LOG_E("Cannot use RecursiveSemaphoreGuard in ISR context");
        return;
    }
    
    m_taken = (xSemaphoreTakeRecursive(m_handle, timeout) == pdTRUE);
}

RecursiveSemaphoreGuard::~RecursiveSemaphoreGuard() {
    if (m_taken && m_handle != nullptr) {
#ifdef SEMAPHORE_GUARD_DEBUG
        TickType_t holdTime = xTaskGetTickCount() - m_acquireTime;
        RSEMG_LOG_D("Releasing recursive mutex at %s:%d (held for %lu ticks)", 
                 m_file, m_line, (unsigned long)holdTime);
#endif
        xSemaphoreGiveRecursive(m_handle);
    }
}

bool RecursiveSemaphoreGuard::hasLock() const {
    return m_taken;
}

#ifdef SEMAPHORE_GUARD_DEBUG
RecursiveSemaphoreGuard::RecursiveSemaphoreGuard(SemaphoreHandle_t handle, const char* file, int line)
    : m_handle(handle), m_taken(false), m_file(file), m_line(line) {
    // Check for null handle
    if (m_handle == nullptr) {
        RSEMG_LOG_E("Null recursive mutex handle provided at %s:%d", m_file, m_line);
        return;
    }
    
    // Check if we're in ISR context (FreeRTOS restriction)
    if (xPortInIsrContext()) {
        RSEMG_LOG_E("Cannot use RecursiveSemaphoreGuard in ISR context at %s:%d", m_file, m_line);
        return;
    }
    
    RSEMG_LOG_D("Attempting to acquire recursive mutex at %s:%d", m_file, m_line);
    m_acquireTime = xTaskGetTickCount();
    m_taken = (xSemaphoreTakeRecursive(m_handle, portMAX_DELAY) == pdTRUE);
    
    if (m_taken) {
        RSEMG_LOG_D("Acquired recursive mutex at %s:%d", m_file, m_line);
    }
}

RecursiveSemaphoreGuard::RecursiveSemaphoreGuard(SemaphoreHandle_t handle, TickType_t timeout, const char* file, int line)
    : m_handle(handle), m_taken(false), m_file(file), m_line(line) {
    // Check for null handle
    if (m_handle == nullptr) {
        RSEMG_LOG_E("Null recursive mutex handle provided at %s:%d", m_file, m_line);
        return;
    }
    
    // Check if we're in ISR context (FreeRTOS restriction)
    if (xPortInIsrContext()) {
        RSEMG_LOG_E("Cannot use RecursiveSemaphoreGuard in ISR context at %s:%d", m_file, m_line);
        return;
    }
    
    RSEMG_LOG_D("Attempting to acquire recursive mutex with timeout %lu at %s:%d", 
             (unsigned long)timeout, m_file, m_line);
    m_acquireTime = xTaskGetTickCount();
    m_taken = (xSemaphoreTakeRecursive(m_handle, timeout) == pdTRUE);
    
    if (m_taken) {
        RSEMG_LOG_D("Acquired recursive mutex at %s:%d", m_file, m_line);
    } else {
        RSEMG_LOG_W("Failed to acquire recursive mutex within timeout at %s:%d", m_file, m_line);
    }
}
#endif