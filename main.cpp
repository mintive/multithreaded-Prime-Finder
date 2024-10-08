#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cmath>
#include <chrono>

// Mutex for synchronizing access to the console and shared data
std::mutex mtx;

// Function to check if a number is prime
bool is_prime(int n) {
    if (n < 2) return false;
    for (int i = 2; i <= std::sqrt(n); ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

// Function to find prime numbers in a given range
void find_primes(int start, int end, std::vector<int>& primes) {
    for (int i = start; i <= end; ++i) {
        if (is_prime(i)) {
            std::lock_guard<std::mutex> lock(mtx); // Synchronized access to shared resource
            primes.push_back(i);
            std::cout << "Prime found: " << i << std::endl;
        }
    }
}

int main() {
    int range_start = 2;
    int range_end = 100000;
    int thread_count = std::thread::hardware_concurrency(); // Get number of available threads
    std::vector<int> primes; // Array to store prime numbers

    // Divide the range of numbers among threads
    int range_per_thread = (range_end - range_start + 1) / thread_count;

    // Vector to hold the threads
    std::vector<std::thread> threads;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Launch threads
    for (int i = 0; i < thread_count; ++i) {
        int start = range_start + i * range_per_thread;
        int end = (i == thread_count - 1) ? range_end : start + range_per_thread - 1;
        threads.emplace_back(find_primes, start, end, std::ref(primes));
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;

    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Total primes found: " << primes.size() << std::endl;
    std::cout << "Execution time: " << elapsed.count() << " seconds" << std::endl;

    return 0;
}
