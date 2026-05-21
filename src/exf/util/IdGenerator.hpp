#pragma once

#include <string>

namespace exf {

/**
 * IdGenerator creates simple identifiers for future parcel records.
 */
class IdGenerator {
public:
    /**
     * Returns a new parcel identifier.
     */
    static std::string nextParcelId();
};

} // namespace exf
