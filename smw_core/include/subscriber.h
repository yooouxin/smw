//
// Created by youxinxin on 2021/12/2.
//

#ifndef SMW_SUBSCRIBER_H
#define SMW_SUBSCRIBER_H

#include "fmt/core.h"
#include "result.h"
#include "sample.h"
#include <vector>

namespace smw::core
{
using namespace types;

enum class SubscriberError
{
    NO_IMPLEMENT,
    READ_ERROR
};

template <typename T>
class Subscriber
{
  public:
    Subscriber(const std::string& topic_name, std::size_t queue_size = DEFAULT_QUEUE_SIZE) noexcept
        : m_topic_name(topic_name)
        , m_queue_size(queue_size)
    {
    }

    Subscriber(std::uint16_t service_id,
               std::uint16_t instance_id,
               std::uint16_t event_id,
               std::size_t queue_size = DEFAULT_QUEUE_SIZE) noexcept
        : m_topic_name(fmt::format("/{}/{}/{}", service_id, instance_id, event_id))
        , m_queue_size(queue_size)
    {
    }

    /// @brief return topic name of publisher
    /// @return topic name
    std::string getTopicName() const noexcept
    {
        return m_topic_name;
    }

    void registerReceiveHandler() noexcept
    {
    }

    Result<std::vector<Sample<T>>, SubscriberError>
    getNewSamples(std::size_t max_sample = std::numeric_limits<size_t>::max()) noexcept
    {
        return core::Err(SubscriberError::NO_IMPLEMENT);
    }

  private:
    std::string m_topic_name;
    std::size_t m_queue_size;
    static constexpr std::size_t DEFAULT_QUEUE_SIZE = 10;
};
} // namespace smw::core
#endif // SMW_SUBSCRIBER_H
