#include "PeriodicTimer.h"

// Start the timer in a separate thread
void PeriodicTimer::start() {
    if (running_) return; // Prevent double start
    running_ = true;
    worker_ = std::thread([this]() {
        while (running_) {
            auto next = std::chrono::steady_clock::now() + interval_;
            task_();
            std::this_thread::sleep_until(next);
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
