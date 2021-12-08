//
// Created by youxinxin on 2021/12/4.
//
#include "hello_wrold.pb.h"
#include "runtime.h"
#include <csignal>

using namespace smw::core;
using namespace smw::example::proto;

constexpr static uint32_t TEST_SERVICE = 10;
constexpr static uint32_t TEST_INSTANCE = 1;
constexpr static uint32_t TEST_EVENT = 2;

static bool exit_request = false;


void signalHandler(int signum)
{
    (void)signum;
    exit_request = true;
}

int main()
{
    eprosima::fastdds::dds::Log::SetVerbosity(eprosima::fastdds::dds::Log::Kind::Error);
    spdlog::set_level(spdlog::level::level_enum::err);
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    RuntimeOption option{"smw_example_hello_world_sub"};
    auto& runtime = Runtime::initRuntime(option);
    ServiceDescription serviceDescription;
    serviceDescription.service_id = TEST_SERVICE;
    serviceDescription.instance_id = TEST_INSTANCE;


    auto service_proxy = runtime.getInstance().createServiceProxy(serviceDescription);

    std::unique_ptr<Subscriber<HelloWorld>> subscriber{nullptr};
    subscriber = std::move(
        service_proxy->createSubscriber<HelloWorld>(TEST_EVENT).orElse([](auto& error) { return error; }).getValue());

    assert(subscriber);

    /// print debug string received
    subscriber->registerReceiveHandler([](auto sample_ptr) { std::cout << sample_ptr->DebugString() << std::endl; });

    while (!exit_request)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
    return 0;
}