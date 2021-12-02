//
// Created by youxinxin on 2021/11/30.
//

#ifndef SMW_PUBLISHER_H
#define SMW_PUBLISHER_H

#include "fmt/core.h"
#include "result.h"
#include "sample.h"

namespace smw::core
{
using namespace types;

enum class PublisherError
{
    NO_IMPLEMENT,
    LOAN_FAILED,
    WRITE_FAILED
};
/// @brief A general publisher class
/// @tparam T Data type publisher want transmit
template <typename T>
class Publisher
{
  public:
    /// @brief topic style publisher
    /// @param topic_name topic name of publisher
    Publisher(const std::string& topic_name) noexcept
        : m_topic_name(topic_name)
    {
    }

    /// @brief service style publisher,construct a string topic name
    /// @param service_id service id of publisher
    /// @param instance_id instance id of publisher
    /// @param event_id event id of publisher
    Publisher(std::uint16_t service_id, std::uint16_t instance_id, std::uint16_t event_id) noexcept
        : Publisher(fmt::format("/{}/{}/{}", service_id, instance_id, event_id))
    {
    }

    /// @brief return topic name of publisher
    /// @return topic name
    std::string getTopicName() const noexcept
    {
        return m_topic_name;
    }

    /// @brief loan a sample
    /// @return result of loaned sample
    Result<Sample<T>, PublisherError> loanSample() noexcept
    {
        return Err(PublisherError::NO_IMPLEMENT);
    }

    /// @brief publish a loaned sample,used for zero-copy
    /// @param sample sample loaned before
    /// @return publish result
    Result<PublisherError> publish(Sample<T>&& sample) noexcept
    {
        return Err(PublisherError::NO_IMPLEMENT);
    }

    /// @brief publish a value
    /// @param value const reference of a data
    /// @return publish result
    Result<PublisherError> publish(const T& value) noexcept
    {
        return Err(PublisherError::NO_IMPLEMENT);
    }

  private:
    std::string m_topic_name;
};

} // namespace smw::core


#endif // SMW_PUBLISHER_H
