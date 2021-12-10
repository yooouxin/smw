//
// Created by youxinxin on 2021/11/30.
//

#ifndef SMW_PUBLISHER_H
#define SMW_PUBLISHER_H

#include "dds/dds_factory.h"
#include "iceoryx/iceoryx_writer.h"
#include "result.h"
#include "sample_ptr.h"
#include "serializer/serializer_protobuf.h"
#include "service_registry.h"
#include "spdlog/spdlog.h"

namespace smw::core
{
using namespace types;

enum class PublisherError
{
    NO_IMPLEMENT = -1,
    LOAN_FAILED = -2,
    DDS_WRITE_FAILED = -3,
    IPC_WRITE_FAILED = -4
};

/// @brief A general publisher class
/// @tparam T Data type publisher want transmit
/// @tparam Serializer serializer of data type,must have some functions
template <typename T, typename Serializer = SerializerProtobuf<T>>
class Publisher
{
  public:
    /// @brief service style publisher,construct a string topic name
    /// @param service_id service id of publisher
    /// @param instance_id instance id of publisher
    /// @param event_id event id of publisher
    Publisher(const ServiceDescription& service_description, std::uint32_t event_id) noexcept
        : m_topic_name(
            fmt::format("/{}/{}/{}", service_description.service_id, service_description.instance_id, event_id))
        , m_dds_writer(DDSFactory::createWriter<T, Serializer>(m_topic_name))
        , m_service_description(service_description)
        , m_iox_writer(std::make_unique<IceoryxWriter<T, Serializer>>(m_service_description, event_id))
    {
    }

    Publisher(const Publisher&) = delete;
    Publisher& operator=(const Publisher&) = delete;


    Publisher(Publisher&&) = delete;
    Publisher& operator=(Publisher&&) = delete;

    ~Publisher() noexcept = default;

    /// @brief return topic name of publisher
    /// @return topic name
    std::string getTopicName() const noexcept
    {
        return m_topic_name;
    }

    /// @brief loan a sample
    /// @return result of loaned sample
    Result<SamplePtr<T>, PublisherError> loanSample() noexcept
    {
        SamplePtr<T> result{nullptr};
        /// TODO magic loan from iceoryx,maybe loan from shared memory,depends on data type
        result = m_iox_writer->loan();
        return Ok(SamplePtr<T>(std::move(result)));
    }

    /// @brief publish a loaned sample,used for zero-copy
    /// @param sample sample loaned before
    /// @return publish result
    Result<PublisherError> publish(SamplePtr<T>&& sample) noexcept
    {
        Result<PublisherError> result{Ok()};
        /// query if exist cross-host subscriber
        if (ServiceRegistry::getInstance().queryServiceStatus(m_service_description).hasRemoteConsumer())
        {
            if (!m_dds_writer->write(*sample))
            {
                result = Err(PublisherError::DDS_WRITE_FAILED);
            }
        }

        if (ServiceRegistry::getInstance().queryServiceStatus(m_service_description).hasLocalConsumer())
        {
            if (!m_iox_writer->write(std::move(sample)))
            {
                result = Err(PublisherError::IPC_WRITE_FAILED);
            }
        }
        else
        {
            // Avoid delete by d
            if (!sample.isHeapAllocated())
            {
                sample.release();
            }
        }

        return result;
    }

    /// @brief publish a value
    /// @param value const reference of a data
    /// @return publish result
    Result<PublisherError> publish(const T& value) noexcept
    {
        Result<PublisherError> result{Ok()};
        /// query if exist cross-host subscriber
        if (ServiceRegistry::getInstance().queryServiceStatus(m_service_description).hasRemoteConsumer())
        {
            if (!m_dds_writer->write(value))
            {
                result = Err(PublisherError::DDS_WRITE_FAILED);
            }
        }

        if (ServiceRegistry::getInstance().queryServiceStatus(m_service_description).hasLocalConsumer())
        {
            SamplePtr<T> sample = m_iox_writer->loan();
            if (!sample)
            {
                result = Err(PublisherError::LOAN_FAILED);
            }
            else
            {
                /// copy here
                *sample = value;
            }

            if (!result.hasError() && !m_iox_writer->write(std::move(sample)))
            {
                result = Err(PublisherError::IPC_WRITE_FAILED);
            }
        }
        return result;
    }

  private:
    std::string m_topic_name;
    ServiceDescription m_service_description;
    std::unique_ptr<TransportWriter<T>> m_dds_writer;
    std::unique_ptr<TransportWriter<T>> m_iox_writer;
};

} // namespace smw::core


#endif // SMW_PUBLISHER_H
