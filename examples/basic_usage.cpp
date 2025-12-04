#include <Arduino.h>
#include "SemaphoreGuard.h"
#include "RecursiveSemaphoreGuard.h"

// Global semaphores
SemaphoreHandle_t xSerialMutex = nullptr;
SemaphoreHandle_t xDataMutex = nullptr;
SemaphoreHandle_t xRecursiveMutex = nullptr;

// Shared data
volatile int sharedCounter = 0;

// Task that increments counter
void incrementTask(void* parameter) {
    while (true) {
        // Use SemaphoreGuard for automatic mutex handling
        {
            SemaphoreGuard guard(xDataMutex);
            if (guard.hasLock()) {
                sharedCounter++;
                
                // Safe serial output
                {
                    SemaphoreGuard serialGuard(xSerialMutex);
                    if (serialGuard.hasLock()) {
                        Serial.printf("Task %s incremented counter to %d\n", 
                                    pcTaskGetName(nullptr), sharedCounter);
                    }
                }
            }
        } // Mutex automatically released here
        
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Example of recursive mutex usage
class SafeCounter {
private:
    int value;
    SemaphoreHandle_t mutex;
    
public:
    SafeCounter() : value(0) {
        mutex = xSemaphoreCreateRecursiveMutex();
    }
    
    ~SafeCounter() {
        if (mutex) {
            vSemaphoreDelete(mutex);
        }
    }
    
    void increment() {
        RecursiveSemaphoreGuard guard(mutex);
        if (guard.hasLock()) {
            value++;
            // Can safely call other methods that also lock
            if (value >= 10) {
                reset(); // This will acquire the mutex again
            }
        }
    }
    
    void reset() {
        RecursiveSemaphoreGuard guard(mutex);
        if (guard.hasLock()) {
            value = 0;
            log("Counter reset");
        }
    }
    
    int getValue() {
        RecursiveSemaphoreGuard guard(mutex);
        if (guard.hasLock()) {
            return value;
        }
        return -1;
    }
    
private:
    void log(const char* message) {
        RecursiveSemaphoreGuard guard(mutex);
        if (guard.hasLock()) {
            SemaphoreGuard serialGuard(xSerialMutex);
            if (serialGuard.hasLock()) {
                Serial.printf("[SafeCounter] %s (value=%d)\n", message, value);
            }
        }
    }
};

SafeCounter safeCounter;

void recursiveTask(void* parameter) {
    while (true) {
        safeCounter.increment();
        
        // Try to read with timeout
        {
            SemaphoreGuard guard(xSerialMutex, pdMS_TO_TICKS(100));
            if (guard.hasLock()) {
                Serial.printf("Safe counter value: %d\n", safeCounter.getValue());
            } else {
                // Could not acquire serial mutex within timeout
                // This is fine, we'll try again next iteration
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("SemaphoreGuard Example Starting...");
    
    // Create mutexes
    xSerialMutex = xSemaphoreCreateMutex();
    xDataMutex = xSemaphoreCreateMutex();
    xRecursiveMutex = xSemaphoreCreateRecursiveMutex();
    
    if (!xSerialMutex || !xDataMutex || !xRecursiveMutex) {
        Serial.println("Failed to create semaphores!");
        return;
    }
    
    // Create tasks
    xTaskCreate(incrementTask, "Task1", 4096, nullptr, 1, nullptr);
    xTaskCreate(incrementTask, "Task2", 4096, nullptr, 1, nullptr);
    xTaskCreate(recursiveTask, "RecursiveTask", 4096, nullptr, 2, nullptr);
    
    Serial.println("Tasks created successfully");
}

void loop() {
    // Main loop can also safely access shared data
    static uint32_t lastReport = 0;
    
    if (millis() - lastReport >= 5000) {
        lastReport = millis();
        
        // Read shared counter safely
        SemaphoreGuard guard(xDataMutex, pdMS_TO_TICKS(50));
        if (guard.hasLock()) {
            SemaphoreGuard serialGuard(xSerialMutex);
            if (serialGuard.hasLock()) {
                Serial.printf("\n=== Status Report ===\n");
                Serial.printf("Shared counter: %d\n", sharedCounter);
                Serial.printf("Safe counter: %d\n", safeCounter.getValue());
                Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
                Serial.printf("===================\n\n");
            }
        }
    }
    
    delay(100);
}