//
// Created by youxinxin on 2021/12/3.
//

#ifndef SMW_SERVICE_REGISTRY_H
#define SMW_SERVICE_REGISTRY_H


#include "dds/dds_factory.h"
#include "service_description.h"
#include "service_discovery.pb.h"
#include "service_status.h"

namespace smw::core
{


class ServiceRegistry
{
  public:
    using service_registry_content_t = std::map<ServiceDescription, ServiceStatus>;

    static ServiceRegistry& getInstance() noexcept;

    ServiceRegistry(const ServiceRegistry&) = delete;
    ServiceRegistry& operator=(const ServiceRegistry&) = delete;

    ServiceRegistry(ServiceRegistry&&) = delete;
    ServiceRegistry& operator=(ServiceRegistry&&) = delete;

    ~ServiceRegistry() noexcept;


    void requestDiscoveryOperation(proto::ServiceDiscovery::ServiceDiscoveryOperation operation,
                                   const ServiceDescription& service_description) noexcept;

    const ServiceStatus& queryServiceStatus(const ServiceDescription& service_description) noexcept;

    using observe_service_callback_t = std::function<void(const ServiceStatus&)>;

    std::uint32_t startObserveServiceStatus(const ServiceDescription& service_description,
                                            const observe_service_callback_t& callback) noexcept;

    void stopObserveServiceStatus(const ServiceDescription& service_description, std::uint32_t unique_id) noexcept;

    void clearRegistry() noexcept;

    const service_registry_content_t& getRegistryContent() const noexcept;

  private:
    ServiceRegistry() noexcept;
    std::unique_ptr<DdsWriter<proto::ServiceDiscovery>> m_service_discovery_writer;
    std::unique_ptr<DdsReader<proto::ServiceDiscovery>> m_service_discovery_reader;

    service_registry_content_t m_registry;
    std::mutex m_registry_mutex;

    std::thread m_message_write_thread;
    bool m_exit;

    struct ObserveCallbackWithId
    {
        observe_service_callback_t callback;
        std::uint32_t unique_id;
    };
    std::map<ServiceDescription, std::vector<ObserveCallbackWithId>> m_observe_service_callbacks;
    std::mutex m_observe_service_callbacks_mutex;

    std::map<ServiceDescription, proto::ServiceDiscovery> m_message_need_send_always;
    std::mutex m_message_need_send_always_mutex;

    struct DiscoveryMessageWithCounter
    {
        proto::ServiceDiscovery discovery_message;
        std::uint16_t count{0};
    };
    std::map<ServiceDescription, DiscoveryMessageWithCounter> m_message_need_send_some_times;
    std::mutex m_message_need_send_some_times_mutex;

    static constexpr char BUILTIN_SERVICE_DISCOVERY_TOPIC[] = "/smw/builtin/service_discovery";
    static constexpr std::uint16_t MAX_DISCOVERY_COUNT = 5;
    static constexpr std::chrono::duration<std::uint64_t, std::milli> DISCOVERY_CYCLE_TIME = std::chrono::seconds(1);

    void updateRegistryFromServiceDiscovery(const proto::ServiceDiscovery& discovery) noexcept;
    void notifyUserCallback(const ServiceDescription& service_description, const ServiceStatus& status) noexcept;

    static void fillServiceInfoMessage(const ServiceDescription& description, proto::ServiceInfo* info) noexcept;

    void send_always_message() noexcept;
    void send_and_remove_some_time_message() noexcept;
};
} // namespace smw::core
#endif // SMW_SERVICE_REGISTRY_H
