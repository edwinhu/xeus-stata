#ifndef XEUS_STATA_BASE64_HPP
#define XEUS_STATA_BASE64_HPP

#include <string>
#include <vector>

namespace xeus_stata
{
    // Base64 encode binary data
    std::string base64_encode(const unsigned char* data, size_t length);

    // Base64 encode from string
    std::string base64_encode(const std::string& data);

} // namespace xeus_stata

#endif // XEUS_STATA_BASE64_HPP
