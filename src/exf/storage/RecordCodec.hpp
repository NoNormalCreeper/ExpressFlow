#pragma once

namespace exf {

/**
 * RecordCodec centralizes the text delimiter planned for file records.
 */
class RecordCodec {
public:
    /**
     * Returns the delimiter used by simple text records.
     */
    static char delimiter();
};

} // namespace exf
