//
// Created by youxinxin on 2021/12/3.
//
#include "service_registry.h"
#include "test.h"
using namespace smw::core;
TEST(service_registry, offer)
{
    ServiceRegistry::getInstance().clearRegistry();
    ServiceDescription serviceDescription;
    serviceDescription.service_id = 10;
    serviceDescription.instance_id = 1;
    ServiceRegistry::getInstance().offerService(serviceDescription);
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistry().size(), 1);

    ServiceRegistry::getInstance().stopOfferService(serviceDescription);
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistry().size(), 0);

    /// test update
    ServiceRegistry::getInstance().offerService(serviceDescription);
    ServiceRegistry::getInstance().offerService(serviceDescription);
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistry().size(), 1);

    ServiceRegistry::getInstance().stopOfferService(serviceDescription);
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistry().size(), 0);

    /// test multi-service
    ServiceDescription serviceDescription2{1, 2};
    ServiceRegistry::getInstance().offerService(serviceDescription);
    ServiceRegistry::getInstance().offerService(serviceDescription2);
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistry().size(), 2);
}


TEST(service_registry, find)
{
    ServiceRegistry::getInstance().clearRegistry();
    ServiceDescription serviceDescription{1, 2};
    ServiceRegistry::getInstance().offerService(serviceDescription);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(ServiceRegistry::getInstance().findService(serviceDescription).has_value());

    ServiceRegistry::getInstance().stopOfferService(serviceDescription);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_FALSE(ServiceRegistry::getInstance().findService(serviceDescription).has_value());
    EXPECT_EQ(ServiceRegistry::getInstance().getRegistry().size(), 0);
}


TEST(service_registry, location)
{
    ServiceRegistry::getInstance().clearRegistry();
    ServiceDescription serviceDescription{1, 2};
    ServiceRegistry::getInstance().offerService(serviceDescription);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(ServiceRegistry::getInstance().findService(serviceDescription).value().offer_by_same_process);
}