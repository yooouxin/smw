//
// Created by youxinxin on 2021/12/4.
//

#include "runtime.h"
#include "serializer_hello_world.h"
#include <csignal>

using namespace smw::core;
using smw::examples::zero_copy::HelloWorld;
using smw::examples::zero_copy::SerializerHelloworld;
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
    static_assert(std::is_trivial_v<smw::examples::zero_copy::HelloWorld>);
    eprosima::fastdds::dds::Log::SetVerbosity(eprosima::fastdds::dds::Log::Kind::Error);
    spdlog::set_level(spdlog::level::level_enum::err);
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    RuntimeOption option{"smw_example_hello_world_idl_sub"};
    auto& runtime = Runtime::initRuntime(option);
    ServiceDescription serviceDescription;
    serviceDescription.service_id = TEST_SERVICE;
    serviceDescription.instance_id = TEST_INSTANCE;


    auto service_proxy = runtime.getInstance().createServiceProxy(serviceDescription);

    auto subscriber = std::move(service_proxy->createSubscriber<HelloWorld, SerializerHelloworld>(TEST_EVENT)
                                    .orElse([](auto& error) { return error; })
                                    .getValue());

    assert(subscriber);

    /// print debug string received
    auto callback = [](SamplePtr<const HelloWorld> sample_ptr) {
        std::cout << sample_ptr->index << std::endl;
        /// convert std::array to std::string
        std::string string_message{sample_ptr->message.begin(), sample_ptr->message.end()};
        std::cout << string_message << std::endl;
    };
    subscriber->registerReceiveHandler(callback);

    while (!exit_request)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
    return 0;
}