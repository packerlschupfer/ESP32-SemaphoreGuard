/*
 * SemaphoreGuard.h - part of the ESP32-SemaphoreGuard library
 *
 * Copyright (C) 2025-2026 packerlschupfer
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _SEMAPHORE_GUARD_H_
#define _SEMAPHORE_GUARD_H_
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Include the logging configuration
#include "SemaphoreGuardLogging.h"

class SemaphoreGuard {
public:
    // Constructor: Takes the semaphore with an infinite timeout
    explicit SemaphoreGuard(SemaphoreHandle_t handle);

    // Constructor: Takes the semaphore with a provided timeout
    SemaphoreGuard(SemaphoreHandle_t handle, TickType_t timeout);

    // Destructor: Gives the semaphore back
    ~SemaphoreGuard();

    // Delete copy constructor and copy assignment to prevent double-release
    SemaphoreGuard(const SemaphoreGuard&) = delete;
    SemaphoreGuard& operator=(const SemaphoreGuard&) = delete;

    // Delete move constructor and move assignment for safety
    SemaphoreGuard(SemaphoreGuard&&) = delete;
    SemaphoreGuard& operator=(SemaphoreGuard&&) = delete;

#ifdef SEMAPHORE_GUARD_DEBUG
    // Debug constructors with file/line info
    SemaphoreGuard(SemaphoreHandle_t handle, const char* file, int line);
    SemaphoreGuard(SemaphoreHandle_t handle, TickType_t timeout, const char* file, int line);
#endif

    // Check if the semaphore was successfully acquired
    [[nodiscard]] bool hasLock() const noexcept;

    // Get the semaphore handle (for advanced use cases)
    [[nodiscard]] SemaphoreHandle_t getHandle() const noexcept { return m_handle; }

    // Check if this guard is valid (has non-null handle)
    [[nodiscard]] bool isValid() const noexcept { return m_handle != nullptr; }

private:
    SemaphoreHandle_t m_handle;
    bool m_taken;  // Indicates whether the semaphore was successfully taken

#ifdef SEMAPHORE_GUARD_DEBUG
    const char* m_file;
    int m_line;
    TickType_t m_acquireTime;
#endif
};

// Macro for debug support
#ifdef SEMAPHORE_GUARD_DEBUG
    #define SEMAPHORE_GUARD(handle) SemaphoreGuard guard(handle, __FILE__, __LINE__)
    #define SEMAPHORE_GUARD_TIMEOUT(handle, timeout) SemaphoreGuard guard(handle, timeout, __FILE__, __LINE__)
#else
    #define SEMAPHORE_GUARD(handle) SemaphoreGuard guard(handle)
    #define SEMAPHORE_GUARD_TIMEOUT(handle, timeout) SemaphoreGuard guard(handle, timeout)
#endif

#endif  // _SEMAPHORE_GUARD_H_
