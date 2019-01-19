//
// Created by Christian Kater on 11.01.19.
//

#ifndef DCPLIB_DCPOPMODE_HPP
#define DCPLIB_DCPOPMODE_HPP

#include <string>
#if defined(DEBUG) || defined(LOGGING)
#include <sstream>
#endif

/**
 * Operations modes of a DCP slave
 */
enum class DcpOpMode : uint8_t {
    /**
     * All deadlines for all outputs must be met. Simulation time is synchronous to absolute time. In case of any
     * deviations, the DCP slave transitions to the error state.
     */
    HRT = 0,
    /**
     * It depends on the application if and how SRT DCP slaves are integrated into scenarios. The DCP slave tries to
     * meet deadlines for all outputs. If deadlines are not met, the DCP slave continues operation. Simulation time
     * should be synchronous to absolute time. It depends on the application, if and when the DCP slave signals an
     * error.
     */
    SRT = 1,
    /**
     * Simulation time is independent from absolute time. It can be faster or slower. Reception of PDU STC_do_step
     * is required.
     */
    NRT = 2,
};

#if defined(DEBUG) || defined(LOGGING)

/**
 * Adds an DcpOpMode to an osstream in a human readable format.
 * @param str
 * @param type
 */
static std::ostream &operator<<(std::ostream &os, DcpOpMode type) {
    switch (type) {
        case DcpOpMode::HRT:
            return os << "HRT";
        case DcpOpMode::SRT:
            return os << "SRT";
        case DcpOpMode::NRT:
            return os << "NRT";
    }
    return os;
}


/**
 * Concatenate an DcpOpMode to an string in a human readable format.
 * @param str
 * @param type
 */
static std::string to_string(DcpOpMode type) {
    std::ostringstream oss;
    oss << type;
    return oss.str();
}
#endif
#endif //DCPLIB_DCPOPMODE_HPP
