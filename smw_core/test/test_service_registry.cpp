//
// Created by youxinxin on 2021/12/3.
//
#include "service_registry.h"
#include "test.h"
using namespace smw::core;
using namespace std::chrono_literals;
static constexpr std::chrono::duration<std::int64_t, std::milli> WAIT_TIME_MS = 10ms;

TEST(service_registry, offer)
{
    ServiceRegistry::getInstance().clearRegistry();
    ServiceDescription serviceDescription;
    serviceDescription.service_id = 10;
    serviceDescription.instance_id = 1;


    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::OFFER, serviceDescription);

    std::this_thread::sleep_for(WAIT_TIME_MS);
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistryContent().size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localProviders.size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localConsumers.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteConsumers.size(), 0);


    /// test update,content should no change even offer again
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::OFFER, serviceDescription);
    std::this_thread::sleep_for(WAIT_TIME_MS);
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistryContent().size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localProviders.size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localConsumers.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteConsumers.size(), 0);

    /// test stop offer,content should contain this service,but no one provide it
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::STOP_OFFER, serviceDescription);
    std::this_thread::sleep_for(WAIT_TIME_MS);
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistryContent().size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localConsumers.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteConsumers.size(), 0);

    /// test multi-service
    ServiceDescription serviceDescription2{1, 2};
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::OFFER, serviceDescription);
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::OFFER, serviceDescription2);
    std::this_thread::sleep_for(WAIT_TIME_MS);
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistryContent().size(), 2);

    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localProviders.size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localConsumers.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteConsumers.size(), 0);

    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription2).localProviders.size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription2).remoteProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription2).localConsumers.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription2).remoteConsumers.size(), 0);
}


TEST(service_registry, find)
{
    ServiceRegistry::getInstance().clearRegistry();
    ServiceDescription serviceDescription;
    serviceDescription.service_id = 10;
    serviceDescription.instance_id = 1;


    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::FIND, serviceDescription);

    std::this_thread::sleep_for(WAIT_TIME_MS);
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistryContent().size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localConsumers.size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteConsumers.size(), 0);


    /// test update,content should no change even find again
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::FIND, serviceDescription);
    std::this_thread::sleep_for(WAIT_TIME_MS);
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistryContent().size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localConsumers.size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteConsumers.size(), 0);

    /// test stop offer,content should contain this service,but no one find it
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::STOP_FIND, serviceDescription);
    std::this_thread::sleep_for(WAIT_TIME_MS);
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistryContent().size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localConsumers.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteConsumers.size(), 0);

    /// test multi-service
    ServiceDescription serviceDescription2{1, 2};
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::FIND, serviceDescription);
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::FIND, serviceDescription2);
    std::this_thread::sleep_for(WAIT_TIME_MS);
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistryContent().size(), 2);

    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).localConsumers.size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription).remoteConsumers.size(), 0);

    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription2).localProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription2).remoteProviders.size(), 0);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription2).localConsumers.size(), 1);
    EXPECT_EQ(ServiceRegistry::getInstance().queryServiceStatus(serviceDescription2).remoteConsumers.size(), 0);
}


TEST(service_registry, find_callback)
{
    ServiceRegistry::getInstance().clearRegistry();
    ServiceDescription serviceDescription{1, 2};
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::OFFER, serviceDescription);

    bool service_found = false;
    ServiceRegistry::getInstance().startObserveServiceStatus(serviceDescription,
                                                             [&service_found](const ServiceStatus& status) {
                                                                 if (status.localProviders.size() > 0)
                                                                 {
                                                                     service_found = true;
                                                                 }
                                                                 else
                                                                 {
                                                                     service_found = false;
                                                                 }
                                                             });
    std::this_thread::sleep_for(WAIT_TIME_MS);

    EXPECT_TRUE(service_found);

    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::STOP_OFFER, serviceDescription);
    std::this_thread::sleep_for(WAIT_TIME_MS);
    EXPECT_FALSE(service_found);

    ServiceRegistry::getInstance().stopObserveServiceStatus(serviceDescription);
    ServiceRegistry::getInstance().requestDiscoveryOperation(proto::ServiceDiscovery::OFFER, serviceDescription);
    std::this_thread::sleep_for(WAIT_TIME_MS);
    EXPECT_FALSE(service_found);
}
