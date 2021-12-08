//
// Created by youxinxin on 2021/12/8.
//


#include "runtime.h"
#include "serializer_hello_world.h"
constexpr static uint32_t TEST_SERVICE = 10;
constexpr static uint32_t TEST_INSTANCE = 1;
constexpr static uint32_t TEST_EVENT = 2;

static bool exit_request = false;

using namespace smw::core;
using smw::examples::zero_copy::HelloWorld;
using smw::examples::zero_copy::SerializerHelloworld;
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

    RuntimeOption option{"smw_example_hello_world_idl_pub"};
    auto& runtime = Runtime::initRuntime(option);
    ServiceDescription serviceDescription;
    serviceDescription.service_id = TEST_SERVICE;
    serviceDescription.instance_id = TEST_INSTANCE;

    auto service_skeleton = runtime.getInstance().createServiceSkeleton(serviceDescription);

    auto publisher = std::move(service_skeleton->createPublisher<HelloWorld, SerializerHelloworld>(TEST_EVENT)
                                   .orElse([](auto& error) { return error; })
                                   .getValue());

    assert(publisher);

    HelloWorld message;
    int32_t index = 0;
    while (!exit_request)
    {
        message.index = index++;
        message.message = {'H', 'e', 'l', 'l', 'o'};
        publisher->publish(message).orElse(
            [](auto& error) { std::cout << static_cast<std::int32_t>(error) << std::endl; });
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }


    return 0;
}