#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <algorithm>
#include "mutex_queue.h"

TEST(MutexQueueTest, MultiProducerMultiConsumer) {
    const int NUM_PRODUCERS = 4;
    const int NUM_CONSUMERS = 4;
    const int ITEMS_PER_PRODUCER = 10000;
    const int TOTAL_ITEMS = NUM_PRODUCERS * ITEMS_PER_PRODUCER;

    MutexQueue<int> q(100);  // capacity 100

    std::vector<int> results;
    std::mutex results_mutex;

    // --- Producers ---
    std::vector<std::thread> producers;
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        producers.emplace_back([&q, i]() {
            for (int j = 0; j < ITEMS_PER_PRODUCER; j++) {
                q.push(i * ITEMS_PER_PRODUCER + j);  // unique value per item
            }
        });
    }

    // --- Consumers ---
    std::vector<std::thread> consumers;
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        consumers.emplace_back([&]() {
            for (int j = 0; j < ITEMS_PER_PRODUCER; j++) {
                int val = q.pop();
                std::lock_guard<std::mutex> lock(results_mutex);
                results.push_back(val);
            }
        });
    }

    // --- Join everyone ---
    for (auto& t : producers) t.join();
    for (auto& t : consumers) t.join();

    // --- Verify every item consumed exactly once ---
    ASSERT_EQ(results.size(), TOTAL_ITEMS);
    std::sort(results.begin(), results.end());
    for (int i = 0; i < TOTAL_ITEMS; i++) {
        EXPECT_EQ(results[i], i);
    }
}
