#include "exf/domain/ParcelStatus.hpp"
#include "exf/domain/Role.hpp"
#include "exf/util/StringUtil.hpp"

#include <cassert>
#include <string>

int main()
{
    const exf::Role role = exf::Role::User;
    const exf::ParcelStatus status = exf::ParcelStatus::WaitingForSign;

    assert(role == exf::Role::User);
    assert(status == exf::ParcelStatus::WaitingForSign);
    assert(exf::StringUtil::trim("  ExpressFlow \n") == "ExpressFlow");
    assert(exf::StringUtil::trim("\t") == "");

    return 0;
}
