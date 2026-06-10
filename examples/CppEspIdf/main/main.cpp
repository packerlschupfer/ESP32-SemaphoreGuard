// Native ESP-IDF (non-Arduino) example for ESP32-SemaphoreGuard.
//
// Demonstrates RAII locking of FreeRTOS semaphores/mutexes using
// SemaphoreGuard and RecursiveSemaphoreGuard under pure ESP-IDF.

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <esp_log.h>

#include "SemaphoreGuard.h"
#include "RecursiveSemaphoreGuard.h"

static const char* TAG = "CppEspIdf";

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "ESP32-SemaphoreGuard native ESP-IDF example starting");

    // --- SemaphoreGuard over a binary/mutex semaphore ---
    SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
    configASSERT(mutex != nullptr);

    {
        // RAII scope: take with infinite timeout, auto-release on scope exit.
        SemaphoreGuard guard(mutex);
        if (guard.hasLock()) {
            ESP_LOGI(TAG, "Acquired mutex (valid=%d)", guard.isValid());
        } else {
            ESP_LOGE(TAG, "Failed to acquire mutex");
        }
    }  // guard destructor gives the semaphore back here
    ESP_LOGI(TAG, "Mutex released after SemaphoreGuard scope");

    // RAII scope with a finite timeout.
    {
        SemaphoreGuard guard(mutex, pdMS_TO_TICKS(100));
        ESP_LOGI(TAG, "Timed guard hasLock=%d", guard.hasLock());
    }

    // --- RecursiveSemaphoreGuard over a recursive mutex ---
    SemaphoreHandle_t recursiveMutex = xSemaphoreCreateRecursiveMutex();
    configASSERT(recursiveMutex != nullptr);

    {
        RecursiveSemaphoreGuard outer(recursiveMutex);
        ESP_LOGI(TAG, "Outer recursive lock hasLock=%d", outer.hasLock());
        {
            // Same task can take the recursive mutex again.
            RecursiveSemaphoreGuard inner(recursiveMutex, pdMS_TO_TICKS(100));
            ESP_LOGI(TAG, "Inner recursive lock hasLock=%d", inner.hasLock());
        }  // inner release
    }      // outer release

    ESP_LOGI(TAG, "Recursive mutex released after nested scopes");

    vSemaphoreDelete(mutex);
    vSemaphoreDelete(recursiveMutex);

    ESP_LOGI(TAG, "Example complete");
}
