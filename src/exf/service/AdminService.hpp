#pragma once

namespace exf {

/**
 * AdminService will hold administrator business operations for stage 1.
 */
class AdminService {
public:
    /**
     * Creates a service placeholder.
     */
    AdminService();

    /**
     * Returns whether the service placeholder is usable.
     */
    bool isReady() const;
};

} // namespace exf
