#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <functional>
#include <utility>

class PeriodicTimer {
public:
    PeriodicTimer(std::chrono::milliseconds interval, std::function<void()> task)
        : interval_(interval), task_(std::move(task)), running_(false) {
    }
    // Start the timer in a separate thread
    void start();
    // Stop the timer
    void stop();
    void restart(std::chrono::milliseconds newInterval);
    void setTask(std::function<void()> task) { task_ = std::move(task); }

    ~PeriodicTimer() {
        stop();
    }

private:
    std::chrono::milliseconds interval_;
    std::function<void()> task_;
    std::atomic<bool> running_;
    std::thread worker_;
};