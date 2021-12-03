//
// Created by youxinxin on 2021/12/3.
//

#ifndef SMW_SERVICE_REGISTRY_H
#define SMW_SERVICE_REGISTRY_H


#include "dds/dds_factory.h"
#include "service_description.h"
#include "service_info.pb.h"
#include <optional>
namespace smw::core
{
struct ServiceStatus
{
    bool offer_by_same_process{false};
    bool offer_by_same_machine{false};
};

class ServiceRegistry
{
  public:
    static ServiceRegistry& getInstance() noexcept;

    ServiceRegistry(const ServiceRegistry&) = delete;
    ServiceRegistry& operator=(const ServiceRegistry&) = delete;

    ServiceRegistry(ServiceRegistry&&) = delete;
    ServiceRegistry& operator=(ServiceRegistry&&) = delete;

    ~ServiceRegistry() noexcept;

    void offerService(const ServiceDescription& service_description) noexcept;
    void stopOfferService(const ServiceDescription& service_description) noexcept;
    std::optional<ServiceStatus> findService(const ServiceDescription& service_description) noexcept;

    using find_service_callback_t = std::function<void(std::optional<ServiceStatus>)>;
    void startFindService(const ServiceDescription& service_description,
                          const find_service_callback_t& callback) noexcept;
    void stopFindService(const ServiceDescription& service_description) noexcept;
    const std::map<ServiceDescription, ServiceStatus>& getRegistry() const noexcept;

    void clearRegistry() noexcept;

  private:
    ServiceRegistry() noexcept;
    std::unique_ptr<DdsWriter<ServiceInfo>> m_service_info_writer;
    std::unique_ptr<DdsReader<ServiceInfo>> m_service_info_reader;

    std::map<ServiceDescription, ServiceStatus> m_registry;
    std::mutex m_registry_mutex;

    std::map<ServiceDescription, find_service_callback_t> m_find_service_callbacks;
    std::mutex m_find_service_callbacks_mutex;

    static constexpr char BUILTIN_SERVICE_INFO_TOPIC[] = "/smw/builtin/service_info";

    void updateRegistryFromServiceInfo(const ServiceInfo& info) noexcept;
    void notifyUserCallback();
};
} // namespace smw::core
#endif // SMW_SERVICE_REGISTRY_H
