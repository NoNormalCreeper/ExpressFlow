#pragma once

namespace exf {

/**
 * AuthService will coordinate registration, login, and password changes.
 */
class AuthService {
public:
    /**
     * Creates a service placeholder.
     */
    AuthService();

    /**
     * Returns whether the service placeholder is usable.
     */
    bool isReady() const;
};

} // namespace exf
