#include "exf/storage/RecordCodec.hpp"

namespace exf {

// Returns a simple delimiter for future text persistence records.
char RecordCodec::delimiter()
{
    return '|';
}

} // namespace exf
