#include "SQLException.h"

SQLException::SQLException(const std::string& msg) : std::runtime_error(msg)
{
}
