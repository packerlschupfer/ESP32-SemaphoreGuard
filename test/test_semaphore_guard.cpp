/**
 * @file test_semaphore_guard.cpp
 * @brief Unit tests for SemaphoreGuard
 */

#ifdef UNIT_TEST

#include <Arduino.h>
#include <unity.h>
#include <SemaphoreGuard.h>

static SemaphoreHandle_t binarySem = nullptr;
static SemaphoreHandle_t countingSem = nullptr;

void setUp() {
    if (binarySem == nullptr) {
        binarySem = xSemaphoreCreateBinary();
        xSemaphoreGive(binarySem);  // Start with semaphore available
    }
    if (countingSem == nullptr) {
        countingSem = xSemaphoreCreateCounting(3, 3);  // Max 3, start with 3
    }
}

void tearDown() {
    // Ensure semaphores are in known state
    xSemaphoreGive(binarySem);
}

void test_semaphore_guard_acquires_binary() {
    {
        SemaphoreGuard guard(binarySem);
        TEST_ASSERT_TRUE(guard.hasLock());
    }
}

void test_semaphore_guard_releases_on_destruction() {
    {
        SemaphoreGuard guard(binarySem);
        TEST_ASSERT_TRUE(guard.hasLock());
    }
    // After destruction, semaphore should be available
    BaseType_t taken = xSemaphoreTake(binarySem, 0);
    TEST_ASSERT_EQUAL(pdTRUE, taken);
    xSemaphoreGive(binarySem);
}

void test_semaphore_guard_timeout() {
    // Take semaphore manually first
    xSemaphoreTake(binarySem, portMAX_DELAY);

    // Try to acquire with short timeout - should fail
    SemaphoreGuard guard(binarySem, pdMS_TO_TICKS(10));
    TEST_ASSERT_FALSE(guard.hasLock());

    // Release manual lock
    xSemaphoreGive(binarySem);
}

void test_semaphore_guard_null_handle() {
    SemaphoreGuard guard(nullptr);
    TEST_ASSERT_FALSE(guard.hasLock());
    TEST_ASSERT_FALSE(guard.isValid());
}

void test_semaphore_guard_is_valid() {
    SemaphoreGuard validGuard(binarySem);
    SemaphoreGuard invalidGuard(nullptr);

    TEST_ASSERT_TRUE(validGuard.isValid());
    TEST_ASSERT_FALSE(invalidGuard.isValid());
}

void test_semaphore_guard_get_handle() {
    SemaphoreGuard guard(binarySem);

    TEST_ASSERT_EQUAL(binarySem, guard.getHandle());
}

void test_semaphore_guard_counting_semaphore() {
    // Counting semaphore allows multiple acquisitions
    {
        SemaphoreGuard guard1(countingSem, pdMS_TO_TICKS(100));
        TEST_ASSERT_TRUE(guard1.hasLock());

        SemaphoreGuard guard2(countingSem, pdMS_TO_TICKS(100));
        TEST_ASSERT_TRUE(guard2.hasLock());

        SemaphoreGuard guard3(countingSem, pdMS_TO_TICKS(100));
        TEST_ASSERT_TRUE(guard3.hasLock());

        // Fourth should fail (max 3)
        SemaphoreGuard guard4(countingSem, pdMS_TO_TICKS(10));
        TEST_ASSERT_FALSE(guard4.hasLock());
    }

    // After all guards destroyed, all 3 should be available again
    UBaseType_t count = uxSemaphoreGetCount(countingSem);
    TEST_ASSERT_EQUAL(3, count);
}

void test_semaphore_guard_infinite_wait() {
    // This test just verifies infinite wait works when semaphore is available
    {
        SemaphoreGuard guard(binarySem);  // portMAX_DELAY by default
        TEST_ASSERT_TRUE(guard.hasLock());
    }
}

void test_semaphore_guard_noexcept() {
    // These should all compile - verifying noexcept doesn't break anything
    SemaphoreGuard guard(binarySem);

    [[maybe_unused]] bool lock = guard.hasLock();
    [[maybe_unused]] bool valid = guard.isValid();
    [[maybe_unused]] SemaphoreHandle_t handle = guard.getHandle();

    TEST_PASS();
}

void test_semaphore_guard_signaling_pattern() {
    // Create a fresh binary semaphore starting empty (signaling use case)
    SemaphoreHandle_t signal = xSemaphoreCreateBinary();

    // Should not be able to take it (no signal yet)
    SemaphoreGuard guard1(signal, pdMS_TO_TICKS(10));
    TEST_ASSERT_FALSE(guard1.hasLock());

    // Signal by giving
    xSemaphoreGive(signal);

    // Now should be able to take
    SemaphoreGuard guard2(signal, pdMS_TO_TICKS(10));
    TEST_ASSERT_TRUE(guard2.hasLock());

    vSemaphoreDelete(signal);
}

// Test runner
void runSemaphoreGuardTests() {
    UNITY_BEGIN();

    RUN_TEST(test_semaphore_guard_acquires_binary);
    RUN_TEST(test_semaphore_guard_releases_on_destruction);
    RUN_TEST(test_semaphore_guard_timeout);
    RUN_TEST(test_semaphore_guard_null_handle);
    RUN_TEST(test_semaphore_guard_is_valid);
    RUN_TEST(test_semaphore_guard_get_handle);
    RUN_TEST(test_semaphore_guard_counting_semaphore);
    RUN_TEST(test_semaphore_guard_infinite_wait);
    RUN_TEST(test_semaphore_guard_noexcept);
    RUN_TEST(test_semaphore_guard_signaling_pattern);

    UNITY_END();
}

void setup() {
    delay(2000);
    Serial.begin(115200);
    Serial.println("\n=== SemaphoreGuard Unit Tests ===\n");
    runSemaphoreGuardTests();
}

void loop() {}

#endif // UNIT_TEST
