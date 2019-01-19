//
// Created by Christian Kater on 11.01.19.
//

#ifndef DCPLIB_DCPDATATYPE_HPP
#define DCPLIB_DCPDATATYPE_HPP

#include <string>
#if defined(DEBUG) || defined(LOGGING)
#include <sstream>
#endif

/**
 * Data types a DCP slaves variable
 */
enum class DcpDataType : uint8_t {
    /**
     * unsigned integer, 8 bit long, little endian byte order
     */
    uint8 = 0,
    /**
     * unsigned integer, 16 bit long, little endian byte order
     */
    uint16 = 1,
    /**
     * unsigned integer, 32 bit long, little endian byte order
     */
    uint32 = 2,
    /**
     * unsigned integer, 64 bit long, little endian byte order
     */
    uint64 = 3,
    /**
     * Signed integers in two's complement representation, 8 bit long, little endian byte order
     */
    int8 = 4,
    /**
     * Signed integers in two's complement representation, 16 bit long, little endian byte order
     */
    int16 = 5,
    /**
     * Signed integers in two's complement representation, 32 bit long, little endian byte order
     */
    int32 = 6,
    /**
     * Signed integers in two's complement representation, 64 bit long, little endian byte order
     */
    int64 = 7,
    /**
     * 32 bit floating point in binary32 format, as described in IEEE 754-2008, little endian byte order
     */
    float32 = 8,
    /**
     * 64 bit floating point in binary64 format, as described in IEEE 754-2008, little endian byte order
     */
    float64 = 9,
    /**
     * the string data type is encoded in the same way as the binary data type. Strings are of variable length and are
     * not terminated in any way. The specified character encoding for strings is UTF-8.
     */
    string = 10,
    /**
     * The binary representation consists of an unsigned integer (uint32) that specifies the length in bytes of the
     * actual data, followed by the binary data itself. The data is transmitted as given without changing the order of
     * its bits.     */
    binary = 11,

    //internal usage for log entries
    state = 12,
    //toDo other types
};

#if defined(DEBUG) || defined(LOGGING)
/**
 * Adds an DcpDataType to an osstream in a human readable format.
 * @param str
 * @param type
 */
static std::ostream &operator<<(std::ostream &os, DcpDataType type) {
    switch (type) {
        case DcpDataType::uint8:
            return os << "uint8";
        case DcpDataType::uint16:
            return os << "uint16";
        case DcpDataType::uint32:
            return os << "uint32";
        case DcpDataType::uint64:
            return os << "uint64";
        case DcpDataType::int8:
            return os << "int8";
        case DcpDataType::int16:
            return os << "int16";
        case DcpDataType::int32:
            return os << "int32";
        case DcpDataType::int64:
            return os << "int64";
        case DcpDataType::float32:
            return os << "float32";
        case DcpDataType::float64:
            return os << "float64";
        case DcpDataType::string:
            return os << "string";
        case DcpDataType::binary:
            return os << "binary";
        case DcpDataType::state:
            return os << "uint8";

    }
    return os;
}

/**
 * Transform an DcpDataType to an string in a human readable format.
 * @param str
 * @param type
 */
static std::string to_string(DcpDataType type) {
    std::ostringstream oss;
    oss << type;
    return oss.str();
}

#endif
#endif //DCPLIB_DCPDATATYPE_HPP