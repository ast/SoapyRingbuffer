#include <gtest/gtest.h>

#include "RingBuffer.hpp"

#include <complex>
#include <thread>

TEST(ringbuffer, alloc) {
    // Test multiple allocations
    for (int i = 0; i < 100; i++) {

        RingBuffer<uint8_t> rb(1 << 15);

        EXPECT_EQ(rb.capacity(), 1 << 15);
    }
}

TEST(ringbuffer, magic_uint8_t) {
    RingBuffer<uint8_t> rb(1 << 15);
    uint32_t size = 1 << 15;

    EXPECT_EQ(rb.capacity(), size);
    EXPECT_EQ(rb.size(), size * sizeof(uint8_t));

    uint8_t* buffer = rb.write_ptr();

    // Test for magic
    for(uint32_t i = 0; i < rb.capacity(); i++) {
        buffer[i] = i % 255;
        EXPECT_EQ(buffer[i + size], i % 255);
    }
}

TEST(ringbuffer, magic_float) {
    RingBuffer<float> rb(1 << 15);
    uint32_t size = 1 << 15;

    EXPECT_EQ(rb.capacity(), size);
    EXPECT_EQ(rb.size(), rb.capacity() * sizeof(float));

    float* buffer = rb.write_ptr();

    // Test for magic
    for(uint32_t i = 0; i < rb.capacity(); i++) {
        buffer[i] = i;
        EXPECT_EQ(buffer[i + size], i);
    }
}


TEST(ringbuffer, read_at_least) {
    RingBuffer<uint8_t> rb(1 << 15);
    uint32_t size = 1 << 15;
    EXPECT_EQ(rb.capacity(), size);

    using namespace std::chrono_literals;


    auto read = rb.read_at_least(10,
                                 10us,
                                 [](auto begin, auto avail) {
                                     EXPECT_NE(begin, nullptr);
                                     EXPECT_EQ(avail, 10);
                                     return 10;
                                 });

    EXPECT_EQ(read, -1);

    rb.produce(10);

    read = rb.read_at_least(10, 10us,
                            [](const uint8_t *begin, const uint32_t avail) {
                                EXPECT_NE(begin, nullptr);
                                EXPECT_GE(avail, 10);
                                return 10;
                            });

    EXPECT_EQ(read, 10);
}

TEST(ringbuffer, write_at_least) {
    RingBuffer<uint8_t> rb(1 << 16);
    uint32_t size = 1 << 16;
    EXPECT_EQ(rb.capacity(), size);

    using namespace std::chrono_literals;

    auto written = rb.write_at_least(20,
                               10us,
                               [](uint8_t* begin, const uint32_t avail) {
                                      EXPECT_NE(begin, nullptr);
                                      EXPECT_GE(avail, 20);
                                   return 20;
                               });

    EXPECT_EQ(written, 20);
}


TEST(ringbuffer, produce_consume) {

    using namespace std::chrono_literals;

    RingBuffer<float> rb(1 << 10);

    int times = 100000;

    auto producer = [&rb, times]() {
        for (int i = 0; i < times; i++) {
            // std::cout << "task1 says hello" << std::endl;
            auto written = rb.write_at_least(10,
                                             100000us,
                                             [](float* begin, const uint32_t free) {
                                                 float test[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
                                                 memcpy(begin, test, sizeof(test));
                                                 EXPECT_GE(free, 10);
                                                 return 10;
                                             });
            EXPECT_EQ(written, 10);
        }
    };

    auto consumer = [&rb, times]() {
        for (int i = 0; i < times; i++) {
            // std::cout << "task2 says hello" << std::endl;
            auto read = rb.read_at_least(10,
                                         100000us,
                                         [](const float* begin, const uint32_t avail) {
                                             EXPECT_GE(avail, 10);

                                             EXPECT_EQ(begin[0], 1);
                                             EXPECT_EQ(begin[1], 2);
                                             EXPECT_EQ(begin[2], 3);
                                             EXPECT_EQ(begin[3], 4);
                                             EXPECT_EQ(begin[4], 5);
                                             EXPECT_EQ(begin[5], 6);
                                             EXPECT_EQ(begin[6], 7);
                                             EXPECT_EQ(begin[7], 8);
                                             EXPECT_EQ(begin[8], 9);
                                             EXPECT_EQ(begin[9], 10);

                                             return 10;
                                         });
            EXPECT_EQ(read, 10);
        }
    };

    auto t1 = std::thread(producer);
    auto t2 = std::thread(consumer);

    t1.join();
    t2.join();
}


// TODO, can this be simplified?
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
