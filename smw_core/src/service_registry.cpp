//
// Created by youxinxin on 2021/12/3.
//
#include "service_registry.h"
#include <fmt/format.h>
#include <unistd.h>


namespace smw::core
{

void ServiceRegistry::offerService(const ServiceDescription& service_description) noexcept
{
    ServiceInfo info;
    info.set_service_id(service_description.service_id);
    info.set_instance_id(service_description.instance_id);
    info.set_host_id(getHostId());
    info.set_process_id(getProcessId());
    info.set_is_available(true);

    assert(m_service_info_writer->write(info));
}

void ServiceRegistry::stopOfferService(const ServiceDescription& service_description) noexcept
{
    ServiceInfo info;
    info.set_service_id(service_description.service_id);
    info.set_instance_id(service_description.instance_id);
    info.set_host_id(getHostId());
    info.set_process_id(getProcessId());
    info.set_is_available(false);

    assert(m_service_info_writer->write(info));
}

std::optional<ServiceStatus> ServiceRegistry::findService(const ServiceDescription& service_description) noexcept
{
    std::optional<ServiceStatus> result = std::nullopt;
    std::unique_lock<std::mutex> lock(m_registry_mutex);
    /// just lookup our local registry
    if (m_registry.find(service_description) != m_registry.end())
    {
        result = m_registry[service_description];
    }
    return result;
}

ServiceRegistry& ServiceRegistry::getInstance() noexcept
{
    static ServiceRegistry instance;
    return instance;
}

ServiceRegistry::ServiceRegistry() noexcept
    : m_service_info_writer(nullptr)
    , m_service_info_reader(nullptr)
{
    m_service_info_writer = DDSFactory::createWriter<ServiceInfo>(BUILTIN_SERVICE_INFO_TOPIC);
    assert(m_service_info_writer != nullptr);
    m_service_info_reader = DDSFactory::createReader<ServiceInfo>(BUILTIN_SERVICE_INFO_TOPIC);
    assert(m_service_info_reader != nullptr);

    auto service_info_callback = [this](const ServiceInfo& info) { updateRegistryFromServiceInfo(info); };
    m_service_info_reader->setDataCallback(service_info_callback);
}

ServiceRegistry::~ServiceRegistry() noexcept
{
    m_service_info_reader->setDataCallback(nullptr);
    clearRegistry();
}
const std::map<ServiceDescription, ServiceStatus>& ServiceRegistry::getRegistry() const noexcept
{
    return m_registry;
}

void ServiceRegistry::clearRegistry() noexcept
{
    std::unique_lock<std::mutex> lock(m_registry_mutex);
    m_registry.clear();
}

void ServiceRegistry::updateRegistryFromServiceInfo(const ServiceInfo& info) noexcept
{
    ServiceDescription service_desc{info.service_id(), info.instance_id()};
    std::unique_lock<std::mutex> lock(m_registry_mutex);
    if (info.is_available())
    {
        ServiceStatus status;

        if (info.host_id() == getHostId())
        {
            status.offer_by_same_machine = true;
            if (info.process_id() == getProcessId())
            {
                status.offer_by_same_process = true;
            }
        }

        m_registry[service_desc] = status;
    }
    else
    {
        m_registry.erase(service_desc);
    }
}

std::string ServiceRegistry::getHostId() noexcept
{
    /// TODO get actual host id
    return "ttt";
}

std::string ServiceRegistry::getProcessId() noexcept
{
    return std::to_string(::getpid());
}


} // namespace smw::core