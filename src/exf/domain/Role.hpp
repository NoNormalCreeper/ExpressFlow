#pragma once

namespace exf {

/**
 * Role lists the login identities required by stage 1.
 */
enum class Role {
    /** Normal platform user. */
    User,

    /** Logistics company administrator. */
    Admin
};

} // namespace exf
