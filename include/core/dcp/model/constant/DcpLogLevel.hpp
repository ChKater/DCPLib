//
// Created by Christian Kater on 14.01.19.
//

#ifndef DCPLIB_DCPLOGLEVEL_HPP
#define DCPLIB_DCPLOGLEVEL_HPP

#include <string>
#if defined(DEBUG) || defined(LOGGING)
#include <sstream>
#endif

enum class DcpLogLevel : uint8_t {
    /**
     * The simulation cannot be continued. The DCP slave will transition to the error superstate.
     */
    LVL_FATAL = 0,
    /**
     * The current action cannot be continued.
     */
    LVL_ERROR = 1,
    /**
     * The current action can be continued, but simulation results could be affected.
     */
    LVL_WARNING = 2,
    /**
     * This log level reflects the status of a DCP slave.
     */
    LVL_INFORMATION = 3,
    /**
     * This log level is intended for debug information.
     */
    LVL_DEBUG = 4,
};

#if defined(DEBUG) || defined(LOGGING)
static std::ostream &operator<<(std::ostream &os, DcpLogLevel level) {
    switch (level) {
        case DcpLogLevel::LVL_FATAL:
            return os << "FATAL";
        case DcpLogLevel::LVL_ERROR:
            return os << "ERROR";
        case DcpLogLevel::LVL_WARNING:
            return os << "WARNING";
        case DcpLogLevel::LVL_INFORMATION:
            return os << "INFO";
        case DcpLogLevel::LVL_DEBUG:
            return os << "DEBUG";
    }
    return os;
}

static std::string to_string(DcpLogLevel level) {
    std::ostringstream oss;
    oss << level;
    return oss.str();
}
#endif
#endif //DCPLIB_DCPLOGLEVEL_HPP
