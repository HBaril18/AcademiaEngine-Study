#include "PeriodicTimer.h"
#include <random>
#include <chrono>

// Start the timer in a separate thread
void PeriodicTimer::start() {
    if (running_) return;

    running_ = true;
    worker_ = std::thread([this]() {

        static thread_local std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<int> distrib(3, 10);

        while (running_) {

            int delay = distrib(gen);

            auto end = std::chrono::steady_clock::now() + std::chrono::seconds(delay);

            while (running_ && std::chrono::steady_clock::now() < end) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }


            if (!running_) break;

            task_();
        }
        });
}

// Stop the timer
void PeriodicTimer::stop() {
    running_ = false;

    if (worker_.joinable()) {
        // Évite le self-join
        if (std::this_thread::get_id() == worker_.get_id()) {
            worker_.detach();
        }
        else {
            worker_.join();
        }
    }
}

void PeriodicTimer::restart(std::chrono::milliseconds newInterval) {
    stop();
    interval_ = newInterval;
    start();
}
