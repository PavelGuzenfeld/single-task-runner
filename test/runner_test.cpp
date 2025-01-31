#include "single-task-runner/runner.hpp"
#include <cassert>
#include <fmt/core.h>
#include <thread>

void inrement_nubmer_test()
{
    unsigned int number = 0;
    run::SingleTaskRunner runner([&number]()
                                 { ++number; },
                                 [](std::string_view msg)
                                 { fmt::print("Error: {}\n", msg); });
    runner.async_start();
    runner.trigger_once(); // increment number
    runner.trigger_once(); // increment number
    runner.trigger_once(); // increment number

    runner.wait_for_all_tasks(); // ensure run are processed
    assert(number == 3);
    fmt::print("inrement_nubmer_test - OK\n");
}

void test_trigger_exception()
{
    unsigned int exception_counter = 0;
    auto error_msg = "Test exception";
    auto size = std::string_view(error_msg).length();
    run::SingleTaskRunner runner([error_msg]()
                                 { throw std::runtime_error(error_msg); },
                                 [size, &exception_counter](std::string_view msg)
                                 {
                                     assert(msg.length() == size);
                                     (void)msg; // suppress warning in release mode
                                     exception_counter++;
                                 });
    runner.async_start();
    runner.trigger_once();
    runner.trigger_once();
    runner.trigger_once();
    runner.wait_for_all_tasks(); // ensure run are processed
    assert(exception_counter == 3);
    fmt::print("test_trigger_exception - OK\n");
}

void move_constructor_test()
{
    auto counter = 0;
    run::SingleTaskRunner runner([&]()
                                 { ++counter; },
                                 [](std::string_view msg)
                                 {
                                     fmt::print("Error: {}\n", msg);
                                 });
    runner.async_start();
    runner.trigger_once();
    runner.trigger_once();
    runner.wait_for_all_tasks(); // ensure run are processed
    assert(counter == 2 && " move_constractor_test failed");

    run::SingleTaskRunner runner2(std::move(runner));
    runner2.trigger_once();
    runner2.trigger_once();
    runner2.wait_for_all_tasks(); // ensure run are processed
    assert(counter == 4 && " move_constractor_test failed");
    fmt::print("move_constractor_test - OK\n");
}

void move_assignment_test()
{
    auto counter = 0;
    run::SingleTaskRunner runner([&]()
                                 { ++counter; },
                                 [](std::string_view msg)
                                 {
                                     fmt::print("Error: {}\n", msg);
                                 });
    runner.async_start();
    runner.trigger_once();
    runner.trigger_once();
    runner.wait_for_all_tasks(); // ensure run are processed
    assert(counter == 2 && " move_constractor_test failed");

    run::SingleTaskRunner runner2 = std::move(runner);
    runner2.trigger_once();
    runner2.trigger_once();
    runner2.wait_for_all_tasks(); // ensure run are processed
    assert(counter == 4 && " move_constractor_test failed");

    fmt::print("move_operator_test - OK\n");
}

void start_stop_test()
{
    auto counter = 0;

    run::SingleTaskRunner runner([&]()
                                 { ++counter; },
                                 [](std::string_view msg)
                                 {
                                     fmt::print("Error: {}\n", msg);
                                 });

    // Start the runner
    runner.async_start();

    // Trigger a few run
    runner.trigger_once();
    runner.trigger_once();
    runner.wait_for_all_tasks();
    assert(counter == 2 && "start_stop_test failed - run were not processed before stop");

    // Stop the runner
    runner.async_stop();

    // Try triggering run after stopping
    runner.trigger_once();
    runner.trigger_once();
    runner.wait_for_all_tasks();
    assert(counter == 2 && "start_stop_test failed - run were processed after stop");

    // Restart the runner and trigger run again
    runner.async_start();
    runner.trigger_once();
    runner.trigger_once();
    runner.wait_for_all_tasks();
    assert(counter == 4 && "start_stop_test failed - run were not processed after restart");

    // Final stop
    runner.async_stop();
    fmt::print("start_stop_test - OK\n");
}

int main()
{
    inrement_nubmer_test();
    test_trigger_exception();
    move_constructor_test();
    move_assignment_test();
    start_stop_test();
    fmt::print("All tests passed\n");
    return 0;
}