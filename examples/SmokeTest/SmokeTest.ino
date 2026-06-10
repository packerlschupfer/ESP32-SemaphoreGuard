// SmokeTest — minimal compile-smoke example for ESP32-SemaphoreGuard.
// Exercises the public API: create a FreeRTOS mutex, then take it via a
// RAII SemaphoreGuard scope and a RecursiveSemaphoreGuard scope.

#include <Arduino.h>
#include "SemaphoreGuard.h"
#include "RecursiveSemaphoreGuard.h"

static SemaphoreHandle_t g_mutex = nullptr;
static SemaphoreHandle_t g_recursiveMutex = nullptr;

void setup() {
  Serial.begin(115200);

  g_mutex = xSemaphoreCreateMutex();
  g_recursiveMutex = xSemaphoreCreateRecursiveMutex();

  // Plain mutex guarded by SemaphoreGuard (RAII scope).
  {
    SemaphoreGuard guard(g_mutex);
    if (guard.hasLock()) {
      Serial.println("Acquired mutex");
    }
    (void)guard.isValid();
    (void)guard.getHandle();
  }  // released here

  // Recursive mutex guarded by RecursiveSemaphoreGuard, with timeout ctor.
  {
    RecursiveSemaphoreGuard guard(g_recursiveMutex, pdMS_TO_TICKS(100));
    if (guard.hasLock()) {
      Serial.println("Acquired recursive mutex");
    }
  }  // released here

  // Exercise the convenience macro too.
  {
    SEMAPHORE_GUARD(g_mutex);
    (void)guard.hasLock();
  }
}

void loop() {
  {
    SemaphoreGuard guard(g_mutex, pdMS_TO_TICKS(10));
    (void)guard.hasLock();
  }
  delay(1000);
}
