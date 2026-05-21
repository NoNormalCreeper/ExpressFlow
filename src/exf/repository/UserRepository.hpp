#pragma once

namespace exf {

/**
 * UserRepository is the stage 1 placeholder for user persistence.
 */
class UserRepository {
public:
    /**
     * Creates a repository placeholder.
     */
    UserRepository();

    /**
     * Returns whether the repository placeholder is usable.
     */
    bool isReady() const;
};

} // namespace exf
