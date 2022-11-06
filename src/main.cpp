#include <iostream>
#include <chrono>
#include <cassert>
#include <thread>
#include <future>

#include "RingBuffer.hpp"

int main() {
    std::cout << "Hello, World!" << std::endl;

   using namespace std::chrono_literals;

    RingBuffer<float> rb(1 << 16);

    uint32_t times = 10'000'000;

    auto producer = [&rb](uint32_t times, std::promise<size_t>&& p) {

        size_t total_written = 0;

        for (uint32_t i = 0; i < times; i++) {
            // std::cout << "task1 says hello" << std::endl;
            auto written = rb.write_at_least(10,
                                             100000us,
                                             [](float* begin, [[maybe_unused]] const uint32_t free) {
                                                 assert(free >= 10);

                                                 float test[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
                                                 memcpy(begin, test, sizeof(test));

                                                 return 10;
                                             });
            assert(written == 10);
            total_written += written;
        }

        p.set_value(total_written);
    };

    auto consumer = [&rb](uint32_t times, std::promise<size_t>&& p) {

        size_t total_read = 0;

        for (uint32_t i = 0; i < times; i++) {
            // std::cout << "task2 says hello" << std::endl;
            auto read = rb.read_at_least(10,
                                         100000us,
                                         [](const float* begin, const uint32_t avail) {
                                             assert(avail >= 10);

                                             assert(begin[0] == 1);
                                             assert(begin[1] == 2);
                                             // assert(begin[2] == 3);
                                             // assert(begin[3] == 4);
                                             // assert(begin[4] == 5);
                                             assert(begin[5] == 6);
                                             // assert(begin[6] == 7);
                                             // assert(begin[7] == 8);
                                             // assert(begin[8] == 9);
                                             assert(begin[9] == 10);

                                             return 10;
                                         });
            assert(read == 10);
            total_read += read;
        }

        p.set_value(total_read);
    };

    std::promise<size_t> p1;
    std::future<size_t> f1 = p1.get_future();
    auto t1 = std::thread(producer, times, std::move(p1));

    std::promise<size_t> p2;
    std::future<size_t> f2 = p2.get_future();
    auto t2 = std::thread(consumer, times, std::move(p2));

    t1.join();
    t2.join();

    auto total_written = f1.get();
    auto total_read = f2.get();

    std::cout << "producer wrote " << total_written << " elements" << std::endl;
    std::cout << "consumer read " << total_read << " elements" << std::endl;

    assert(total_written == total_read);

    return 0;
}
