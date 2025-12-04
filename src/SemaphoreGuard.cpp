#include "SemaphoreGuard.h"

SemaphoreGuard::SemaphoreGuard(SemaphoreHandle_t handle) 
    : m_handle(handle), m_taken(false) {
    // Check for null handle
    if (m_handle == nullptr) {
        SEMG_LOG_E("Null semaphore handle provided");
        return;
    }
    
    // Check if we're in ISR context (FreeRTOS restriction)
    if (xPortInIsrContext()) {
        SEMG_LOG_E("Cannot use SemaphoreGuard in ISR context");
        return;
    }
    
    m_taken = (xSemaphoreTake(m_handle, portMAX_DELAY) == pdTRUE);
}

SemaphoreGuard::SemaphoreGuard(SemaphoreHandle_t handle, TickType_t timeout) 
    : m_handle(handle), m_taken(false) {
    // Check for null handle
    if (m_handle == nullptr) {
        SEMG_LOG_E("Null semaphore handle provided");
        return;
    }
    
    // Check if we're in ISR context (FreeRTOS restriction)
    if (xPortInIsrContext()) {
        SEMG_LOG_E("Cannot use SemaphoreGuard in ISR context");
        return;
    }
    
    m_taken = (xSemaphoreTake(m_handle, timeout) == pdTRUE);
}

SemaphoreGuard::~SemaphoreGuard() {
    if (m_taken && m_handle != nullptr) {
#ifdef SEMAPHORE_GUARD_DEBUG
        TickType_t holdTime = xTaskGetTickCount() - m_acquireTime;
        SEMG_LOG_D("Releasing semaphore at %s:%d (held for %lu ticks)", 
                 m_file, m_line, (unsigned long)holdTime);
#endif
        xSemaphoreGive(m_handle);
    }
}

bool SemaphoreGuard::hasLock() const noexcept {
    return m_taken;
}

#ifdef SEMAPHORE_GUARD_DEBUG
SemaphoreGuard::SemaphoreGuard(SemaphoreHandle_t handle, const char* file, int line)
    : m_handle(handle), m_taken(false), m_file(file), m_line(line) {
    // Check for null handle
    if (m_handle == nullptr) {
        SEMG_LOG_E("Null semaphore handle provided at %s:%d", m_file, m_line);
        return;
    }
    
    // Check if we're in ISR context (FreeRTOS restriction)
    if (xPortInIsrContext()) {
        SEMG_LOG_E("Cannot use SemaphoreGuard in ISR context at %s:%d", m_file, m_line);
        return;
    }
    
    SEMG_LOG_D("Attempting to acquire semaphore at %s:%d", m_file, m_line);
    m_acquireTime = xTaskGetTickCount();
    m_taken = (xSemaphoreTake(m_handle, portMAX_DELAY) == pdTRUE);
    
    if (m_taken) {
        SEMG_LOG_D("Acquired semaphore at %s:%d", m_file, m_line);
    }
}

SemaphoreGuard::SemaphoreGuard(SemaphoreHandle_t handle, TickType_t timeout, const char* file, int line)
    : m_handle(handle), m_taken(false), m_file(file), m_line(line) {
    // Check for null handle
    if (m_handle == nullptr) {
        SEMG_LOG_E("Null semaphore handle provided at %s:%d", m_file, m_line);
        return;
    }
    
    // Check if we're in ISR context (FreeRTOS restriction)
    if (xPortInIsrContext()) {
        SEMG_LOG_E("Cannot use SemaphoreGuard in ISR context at %s:%d", m_file, m_line);
        return;
    }
    
    SEMG_LOG_D("Attempting to acquire semaphore with timeout %lu at %s:%d", 
             (unsigned long)timeout, m_file, m_line);
    m_acquireTime = xTaskGetTickCount();
    m_taken = (xSemaphoreTake(m_handle, timeout) == pdTRUE);
    
    if (m_taken) {
        SEMG_LOG_D("Acquired semaphore at %s:%d", m_file, m_line);
    } else {
        SEMG_LOG_W("Failed to acquire semaphore within timeout at %s:%d", m_file, m_line);
    }
}
#endif
