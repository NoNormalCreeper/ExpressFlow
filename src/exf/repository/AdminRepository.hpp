#pragma once

namespace exf {

/**
 * AdminRepository is the stage 1 placeholder for administrator persistence.
 */
class AdminRepository {
public:
    /**
     * Creates a repository placeholder.
     */
    AdminRepository();

    /**
     * Returns whether the repository placeholder is usable.
     */
    bool isReady() const;
};

} // namespace exf
