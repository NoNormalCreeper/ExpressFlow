#pragma once

namespace exf {

/**
 * UserService will hold user profile and balance operations.
 */
class UserService {
public:
    /**
     * Creates a service placeholder.
     */
    UserService();

    /**
     * Returns whether the service placeholder is usable.
     */
    bool isReady() const;
};

} // namespace exf
