#ifndef SEMAPHORE_GUARD_LOGGING_H
#define SEMAPHORE_GUARD_LOGGING_H

// Log tags for different components
#define SEMG_LOG_TAG "SemaphoreGuard"
#define RSEMG_LOG_TAG "RecursiveSemaphoreGuard"

// Define log levels based on debug flag
#ifdef SEMAPHORE_GUARD_DEBUG
    // Debug mode: Show all levels
    #define SEMG_LOG_LEVEL_E ESP_LOG_ERROR
    #define SEMG_LOG_LEVEL_W ESP_LOG_WARN
    #define SEMG_LOG_LEVEL_I ESP_LOG_INFO
    #define SEMG_LOG_LEVEL_D ESP_LOG_DEBUG
    #define SEMG_LOG_LEVEL_V ESP_LOG_VERBOSE
#else
    // Release mode: Only Error, Warn, Info
    #define SEMG_LOG_LEVEL_E ESP_LOG_ERROR
    #define SEMG_LOG_LEVEL_W ESP_LOG_WARN
    #define SEMG_LOG_LEVEL_I ESP_LOG_INFO
    #define SEMG_LOG_LEVEL_D ESP_LOG_NONE  // Suppress
    #define SEMG_LOG_LEVEL_V ESP_LOG_NONE  // Suppress
#endif

// Route to custom logger or ESP-IDF
#ifdef USE_CUSTOM_LOGGER
    #include <LogInterface.h>
    // SemaphoreGuard logging macros
    #define SEMG_LOG_E(...) LOG_WRITE(SEMG_LOG_LEVEL_E, SEMG_LOG_TAG, __VA_ARGS__)
    #define SEMG_LOG_W(...) LOG_WRITE(SEMG_LOG_LEVEL_W, SEMG_LOG_TAG, __VA_ARGS__)
    #define SEMG_LOG_I(...) LOG_WRITE(SEMG_LOG_LEVEL_I, SEMG_LOG_TAG, __VA_ARGS__)
    #define SEMG_LOG_D(...) LOG_WRITE(SEMG_LOG_LEVEL_D, SEMG_LOG_TAG, __VA_ARGS__)
    #define SEMG_LOG_V(...) LOG_WRITE(SEMG_LOG_LEVEL_V, SEMG_LOG_TAG, __VA_ARGS__)
    
    // RecursiveSemaphoreGuard logging macros
    #define RSEMG_LOG_E(...) LOG_WRITE(SEMG_LOG_LEVEL_E, RSEMG_LOG_TAG, __VA_ARGS__)
    #define RSEMG_LOG_W(...) LOG_WRITE(SEMG_LOG_LEVEL_W, RSEMG_LOG_TAG, __VA_ARGS__)
    #define RSEMG_LOG_I(...) LOG_WRITE(SEMG_LOG_LEVEL_I, RSEMG_LOG_TAG, __VA_ARGS__)
    #define RSEMG_LOG_D(...) LOG_WRITE(SEMG_LOG_LEVEL_D, RSEMG_LOG_TAG, __VA_ARGS__)
    #define RSEMG_LOG_V(...) LOG_WRITE(SEMG_LOG_LEVEL_V, RSEMG_LOG_TAG, __VA_ARGS__)
#else
    // ESP-IDF logging with compile-time suppression
    #include <esp_log.h>
    
    // SemaphoreGuard logging macros
    #define SEMG_LOG_E(...) ESP_LOGE(SEMG_LOG_TAG, __VA_ARGS__)
    #define SEMG_LOG_W(...) ESP_LOGW(SEMG_LOG_TAG, __VA_ARGS__)
    #define SEMG_LOG_I(...) ESP_LOGI(SEMG_LOG_TAG, __VA_ARGS__)
    #ifdef SEMAPHORE_GUARD_DEBUG
        #define SEMG_LOG_D(...) ESP_LOGD(SEMG_LOG_TAG, __VA_ARGS__)
        #define SEMG_LOG_V(...) ESP_LOGV(SEMG_LOG_TAG, __VA_ARGS__)
    #else
        #define SEMG_LOG_D(...) ((void)0)
        #define SEMG_LOG_V(...) ((void)0)
    #endif
    
    // RecursiveSemaphoreGuard logging macros
    #define RSEMG_LOG_E(...) ESP_LOGE(RSEMG_LOG_TAG, __VA_ARGS__)
    #define RSEMG_LOG_W(...) ESP_LOGW(RSEMG_LOG_TAG, __VA_ARGS__)
    #define RSEMG_LOG_I(...) ESP_LOGI(RSEMG_LOG_TAG, __VA_ARGS__)
    #ifdef SEMAPHORE_GUARD_DEBUG
        #define RSEMG_LOG_D(...) ESP_LOGD(RSEMG_LOG_TAG, __VA_ARGS__)
        #define RSEMG_LOG_V(...) ESP_LOGV(RSEMG_LOG_TAG, __VA_ARGS__)
    #else
        #define RSEMG_LOG_D(...) ((void)0)
        #define RSEMG_LOG_V(...) ((void)0)
    #endif
#endif

// Legacy debug macro for backward compatibility
#ifdef SEMAPHORE_GUARD_DEBUG
    #define SEMG_LOG_DEBUG(...) SEMG_LOG_D(__VA_ARGS__)
    #define RSEMG_LOG_DEBUG(...) RSEMG_LOG_D(__VA_ARGS__)
#else
    #define SEMG_LOG_DEBUG(...) ((void)0)
    #define RSEMG_LOG_DEBUG(...) ((void)0)
#endif

#endif // SEMAPHORE_GUARD_LOGGING_H