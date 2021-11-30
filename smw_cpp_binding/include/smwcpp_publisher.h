//
// Created by youxinxin on 2021/11/30.
//

#ifndef SMW_SMWCPP_PUBLISHER_H
#define SMW_SMWCPP_PUBLISHER_H

#include <iostream>
namespace smw::cpp
{
template <typename T>
class Publisher
{
  public:
    Publisher() noexcept;
};

template <typename T>
Publisher<T>::Publisher() noexcept
{
    std::cout << "hello publisher\n";
}
} // namespace smw::cpp


#endif // SMW_SMWCPP_PUBLISHER_H
