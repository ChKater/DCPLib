//
// Created by Christian Kater on 14.01.19.
//

#ifndef DCPLIB_DCPLOGMODE_HPP
#define DCPLIB_DCPLOGMODE_HPP

#include <string>
#if defined(DEBUG) || defined(LOGGING)
#include <sstream>
#endif


enum class DcpLogMode : uint8_t {
    /**
     * Logging will not take place over the DCP protocol. The slave may use proprietary or no logging at all.
     */
    NO_LOGGING = 0,
    /**
     * Log messages will be stored until they are requested.
     */
    LOG_ON_REQUEST = 1,
    /**
     * Log messages will be send out immediatly by RSP_log_ack. Log message will not be stored by the slave.
     */
    LOG_ON_NOTIFICATION = 2,
};

#if defined(DEBUG) || defined(LOGGING)
static std::ostream &operator<<(std::ostream &os, DcpLogMode logMode) {
    switch (logMode) {
        case DcpLogMode::NO_LOGGING:
            return os << "NO_LOGGING";
        case DcpLogMode::LOG_ON_REQUEST:
            return os << "LOG_ON_REQUEST";
        case DcpLogMode::LOG_ON_NOTIFICATION:
            return os << "LOG_ON_NOTIFICATION";
        default:
            return os << "UNKNOWN(" << (unsigned((uint8_t) logMode)) << ")";
    }
    return os;
}

static std::string to_string(DcpLogMode logMode) {
    std::ostringstream oss;
    oss << logMode;
    return oss.str();
}
#endif
#endif //DCPLIB_DCPLOGMODE_HPP
