#pragma once

#include <stdexcept>
#include <string>

class SQLException : public std::runtime_error
{
public:
    explicit SQLException(const std::string& msg);
};
