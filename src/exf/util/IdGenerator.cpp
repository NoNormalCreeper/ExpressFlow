#include "exf/util/IdGenerator.hpp"

#include <atomic>
#include <string>

namespace exf {

// Creates a process-local parcel identifier.
std::string IdGenerator::nextParcelId()
{
    static std::atomic<unsigned long> counter{0};
    const unsigned long value = ++counter;
    return "P" + std::to_string(value);
}

} // namespace exf
