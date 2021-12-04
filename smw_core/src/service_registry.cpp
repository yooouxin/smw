//
// Created by youxinxin on 2021/12/3.
//
#include "service_registry.h"
#include "host_id.h"
#include <fmt/format.h>

namespace smw::core
{


const ServiceStatus& ServiceRegistry::queryServiceStatus(const ServiceDescription& service_description) noexcept
{
    /// just lookup our local registry
    std::unique_lock<std::mutex> lock(m_registry_mutex);

    return m_registry[service_description];
}

ServiceRegistry& ServiceRegistry::getInstance() noexcept
{
    static ServiceRegistry instance;
    return instance;
}

ServiceRegistry::ServiceRegistry() noexcept
    : m_service_discovery_writer(nullptr)
    , m_service_discovery_reader(nullptr)
{
    m_service_discovery_writer = DDSFactory::createWriter<proto::ServiceDiscovery>(BUILTIN_SERVICE_DISCOVERY_TOPIC);
    assert(m_service_discovery_writer != nullptr);
    m_service_discovery_reader = DDSFactory::createReader<proto::ServiceDiscovery>(BUILTIN_SERVICE_DISCOVERY_TOPIC);
    assert(m_service_discovery_reader != nullptr);

    auto service_discovery_callback = [this](const proto::ServiceDiscovery& discovery) {
        updateRegistryFromServiceDiscovery(discovery);
    };
    m_service_discovery_reader->setDataCallback(service_discovery_callback);
}

ServiceRegistry::~ServiceRegistry() noexcept
{
    m_service_discovery_reader->setDataCallback(nullptr);
}


void ServiceRegistry::updateRegistryFromServiceDiscovery(const proto::ServiceDiscovery& discovery) noexcept
{
    ServiceDescription service_desc{discovery.service_info().service_id(), discovery.service_info().instance_id()};
    ServiceEntityInfo entity_info{discovery.service_info().host_id(), discovery.service_info().process_id()};
    std::unique_lock<std::mutex> lock(m_registry_mutex);

    ServiceStatus& service_status = m_registry[service_desc];

    switch (discovery.operation())
    {
    case proto::ServiceDiscovery::OFFER:
    {
        if (discovery.service_info().host_id() == utils::getHostId())
        {
            service_status.localProviders.emplace(entity_info);
        }
        else
        {
            service_status.remoteProviders.emplace(entity_info);
        }
    }
    break;
    case proto::ServiceDiscovery::STOP_OFFER:
    {
        if (discovery.service_info().host_id() == utils::getHostId())
        {
            service_status.localProviders.erase(entity_info);
        }
        else
        {
            service_status.remoteProviders.erase(entity_info);
        }
    }
    break;
    case proto::ServiceDiscovery::FIND:
    {
        if (discovery.service_info().host_id() == utils::getHostId())
        {
            service_status.localConsumers.emplace(entity_info);
        }
        else
        {
            service_status.remoteConsumers.emplace(entity_info);
        }
    }
    break;
    case proto::ServiceDiscovery::STOP_FIND:
    {
        if (discovery.service_info().host_id() == utils::getHostId())
        {
            service_status.localConsumers.erase(entity_info);
        }
        else
        {
            service_status.remoteConsumers.erase(entity_info);
        }
    }
    break;
    default:
        break;
    }

    notifyUserCallback(service_desc, service_status);
}

void ServiceRegistry::startObserveServiceStatus(const ServiceDescription& service_description,
                                                const ServiceRegistry::observe_service_callback_t& callback) noexcept
{
    std::unique_lock<std::mutex> lock(m_observe_service_callbacks_mutex);
    m_observe_service_callbacks[service_description] = callback;
    if (m_registry.find(service_description) != m_registry.end() && callback)
    {
        callback(m_registry[service_description]);
    }
}

void ServiceRegistry::notifyUserCallback(const ServiceDescription& service_description,
                                         const ServiceStatus& status) noexcept
{
    std::unique_lock<std::mutex> lock(m_observe_service_callbacks_mutex);
    if (m_observe_service_callbacks.find(service_description) != m_observe_service_callbacks.end())
    {
        m_observe_service_callbacks[service_description](status);
    }
}

void ServiceRegistry::stopObserveServiceStatus(const ServiceDescription& service_description) noexcept
{
    std::unique_lock<std::mutex> lock(m_observe_service_callbacks_mutex);
    m_observe_service_callbacks.erase(service_description);
}

void ServiceRegistry::fillServiceInfoMessage(const ServiceDescription& description, proto::ServiceInfo* info) noexcept
{
    info->set_service_id(description.service_id);
    info->set_instance_id(description.instance_id);
    info->set_host_id(utils::getHostId());
    info->set_process_id(utils::getProcessId());
}

void ServiceRegistry::requestDiscoveryOperation(proto::ServiceDiscovery::ServiceDiscoveryOperation operation,
                                                const ServiceDescription& service_description) noexcept
{
    proto::ServiceDiscovery discovery_message;
    discovery_message.set_operation(operation);
    fillServiceInfoMessage(service_description, discovery_message.mutable_service_info());

    assert(m_service_discovery_writer->write(discovery_message));
}

void ServiceRegistry::clearRegistry() noexcept
{
    std::unique_lock<std::mutex> lock(m_registry_mutex);
    m_registry.clear();
}

const ServiceRegistry::service_registry_content_t& ServiceRegistry::getRegistryContent() const noexcept
{
    return m_registry;
}


} // namespace smw::core