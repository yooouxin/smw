//
// Created by youxinxin on 2021/12/3.
//
#include "host_id.h"
#include <arpa/inet.h>
#include <cassert>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <net/if.h>
#include <set>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace smw::utils
{

static constexpr std::size_t DEFAULT_BUFF_SIZE = 512;

std::string getMangledIp() noexcept
{
    std::string result;
    std::set<std::string> sorted_ips;
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sock_fd > 0);
    char buf[DEFAULT_BUFF_SIZE];
    struct ifconf if_config
    {
    };
    if_config.ifc_len = DEFAULT_BUFF_SIZE;
    if_config.ifc_ifcu.ifcu_buf = buf;

    int return_code = ::ioctl(sock_fd, SIOCGIFCONF, &if_config);
    assert(return_code == 0);


    /// get all devices ip
    struct ifreq* if_req = (struct ifreq*)if_config.ifc_buf;

    for (auto i = (if_config.ifc_len / sizeof(struct ifreq)); i > 0; i--)
    {
        if (if_req->ifr_flags == AF_INET)
        { // for ipv4

            sorted_ips.emplace(inet_ntoa(((struct sockaddr_in*)&(if_req->ifr_addr))->sin_addr));
            if_req++;
        }
    }

    for (auto& ip : sorted_ips)
    {
        result += ip + "/";
    }
    return result;
}


const std::string& getHostId() noexcept
{
    static std::string result = getMangledIp();
    return result;
}

const std::string& getProcessId() noexcept
{
    static std::string result = std::to_string(::getpid());
    return result;
}
} // namespace smw::utils