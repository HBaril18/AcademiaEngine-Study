#include "PeriodicTimer.h"

// Start the timer in a separate thread
void PeriodicTimer::start() {
    if (running_) return; // Prevent double start
    running_ = true;
    worker_ = std::thread([this]() {
        // Wait the first interval before invoking the task
        auto next = std::chrono::steady_clock::now() + interval_;
        while (running_) {
            std::this_thread::sleep_until(next);
            if (!running_) break;
            task_();
            next += interval_;
        }
    });
}

// Stop the timer
void PeriodicTimer::stop() {
    running_ = false;
    if (worker_.joinable()) {
        worker_.join();
    }
}
