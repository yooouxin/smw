//
// Created by youxinxin on 2021/12/4.
//
#include "hello_wrold.pb.h"
#include "runtime.h"
#include <csignal>
#include <fastrtps/log/Log.h>

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
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    RuntimeOption option{"smw_example_pub"};
    auto& runtime = Runtime::initRuntime(option);
    ServiceDescription serviceDescription;
    serviceDescription.service_id = TEST_SERVICE;
    serviceDescription.instance_id = TEST_INSTANCE;

    auto service_skeleton = runtime.getInstance().createServiceSkeleton(serviceDescription);

    std::unique_ptr<Publisher<HelloWorld>> publisher{nullptr};
    publisher = std::move(
        service_skeleton->createPublisher<HelloWorld>(TEST_EVENT).orElse([](auto& error) { return error; }).getValue());

    assert(publisher);

    HelloWorld message;
    int32_t index = 0;
    while (!exit_request)
    {
        message.set_index(index++);
        message.set_message("Hello");
        publisher->publish(message).orElse(
            [](auto& error) { std::cout << static_cast<std::int32_t>(error) << std::endl; });
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }


    return 0;
}