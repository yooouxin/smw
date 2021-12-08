//
// Created by youxinxin on 2021/12/3.
//
#include "service_registry.h"
#include "host_id.h"
#include "serializer/serializer_protobuf.h"
#include "spdlog/spdlog.h"

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
    , m_exit(false)
{
    m_service_discovery_writer =
        DDSFactory::createWriter<proto::ServiceDiscovery, SerializerProtobuf<proto::ServiceDiscovery>>(
            BUILTIN_SERVICE_DISCOVERY_TOPIC);
    assert(m_service_discovery_writer != nullptr);
    m_service_discovery_reader =
        DDSFactory::createReader<proto::ServiceDiscovery, SerializerProtobuf<proto::ServiceDiscovery>>(
            BUILTIN_SERVICE_DISCOVERY_TOPIC);
    assert(m_service_discovery_reader != nullptr);

    auto service_discovery_callback = [this](SamplePtr<const proto::ServiceDiscovery>&& discovery) {
        updateRegistryFromServiceDiscovery(*discovery);
    };
    m_service_discovery_reader->setDataCallback(service_discovery_callback);

    m_message_write_thread = std::thread([this]() {
        while (!m_exit)
        {
            /// handle "always" message
            send_always_message();
            /// handle "some-time" message
            send_and_remove_some_time_message();

            std::this_thread::sleep_for(DISCOVERY_CYCLE_TIME);
        }
    });
}

ServiceRegistry::~ServiceRegistry() noexcept
{
    m_service_discovery_reader->setDataCallback(nullptr);
    m_exit = true;
    if (m_message_write_thread.joinable())
    {
        m_message_write_thread.join(); /// we should wait thread exit? it may spend some time
    }
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

std::uint32_t
ServiceRegistry::startObserveServiceStatus(const ServiceDescription& service_description,
                                           const ServiceRegistry::observe_service_callback_t& callback) noexcept
{
    std::unique_lock<std::mutex> lock(m_observe_service_callbacks_mutex);
    static std::uint32_t unique_id = 0;

    m_observe_service_callbacks[service_description].emplace_back(ObserveCallbackWithId{callback, unique_id});
    if (m_registry.find(service_description) != m_registry.end() && callback)
    {
        callback(m_registry[service_description]);
    }
    return unique_id++;
}

void ServiceRegistry::notifyUserCallback(const ServiceDescription& service_description,
                                         const ServiceStatus& status) noexcept
{
    std::unique_lock<std::mutex> lock(m_observe_service_callbacks_mutex);
    if (m_observe_service_callbacks.find(service_description) != m_observe_service_callbacks.end())
    {
        for (auto& callback : m_observe_service_callbacks[service_description])
        {
            if (callback.callback)
            {
                callback.callback(status);
            }
        }
    }
}

void ServiceRegistry::stopObserveServiceStatus(const ServiceDescription& service_description,
                                               std::uint32_t unique_id) noexcept
{
    std::unique_lock<std::mutex> lock(m_observe_service_callbacks_mutex);
    if (m_observe_service_callbacks.find(service_description) != m_observe_service_callbacks.end())
    {
        auto should_deleted_iter = m_observe_service_callbacks[service_description].begin();
        for (; should_deleted_iter != m_observe_service_callbacks[service_description].end(); should_deleted_iter++)
        {
            if (should_deleted_iter->unique_id == unique_id)
            {
                break;
            }
        }
        if (should_deleted_iter != m_observe_service_callbacks[service_description].end())
        {
            m_observe_service_callbacks[service_description].erase(should_deleted_iter);
        }
    }
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

    /// we write one time here
    assert(m_service_discovery_writer->write(discovery_message));

    /// offer and find need send always,stop offer and stop find need send some time
    if (operation == proto::ServiceDiscovery::OFFER || operation == proto::ServiceDiscovery::FIND)
    {
        /// if there has old message send some time ,remove them
        std::unique_lock<std::mutex> lock_msgs_some_time(m_message_need_send_some_times_mutex);
        if (m_message_need_send_some_times.find(service_description) != m_message_need_send_some_times.end())
        {
            m_message_need_send_some_times.erase(service_description);
        }
        lock_msgs_some_time.unlock();

        std::unique_lock<std::mutex> lock_msgs_always(m_message_need_send_always_mutex);
        m_message_need_send_always[service_description] = discovery_message;
    }
    else
    {
        std::unique_lock<std::mutex> lock_msgs_always(m_message_need_send_always_mutex);
        /// if there has old message send always ,remove them
        if (m_message_need_send_always.find(service_description) != m_message_need_send_always.end())
        {
            m_message_need_send_always.erase(service_description);
        }
        lock_msgs_always.unlock();

        std::unique_lock<std::mutex> lock_msgs_some_time(m_message_need_send_some_times_mutex);
        DiscoveryMessageWithCounter discoveryMessageWithCounter;
        discoveryMessageWithCounter.discovery_message = discovery_message;
        discoveryMessageWithCounter.count = 0;
        m_message_need_send_some_times[service_description] = discoveryMessageWithCounter;
    }
}
void ServiceRegistry::send_and_remove_some_time_message() noexcept
{
    std::unique_lock<std::mutex> lock_msgs_some_time(m_message_need_send_some_times_mutex);

    for (auto iter = m_message_need_send_some_times.begin(); iter != m_message_need_send_some_times.end();)
    {
        assert(m_service_discovery_writer->write(iter->second.discovery_message));
        iter->second.count++;
        if (iter->second.count >= MAX_DISCOVERY_COUNT)
        {
            iter = m_message_need_send_some_times.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}
void ServiceRegistry::send_always_message() noexcept
{
    std::unique_lock<std::mutex> lock_msgs_always(m_message_need_send_always_mutex);
    for (auto& message : m_message_need_send_always)
    {
        assert(m_service_discovery_writer->write(message.second));
    }
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