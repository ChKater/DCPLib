//
// Created by kater on 24.08.2017.
//

#ifndef LIBACOSAR_ACICONSTANTS_H
#define LIBACOSAR_ACICONSTANTS_H

#include "dcp/model/DcpTypes.hpp"

#include <cstring>
#include <stdexcept>
#include <typeinfo>
#include <cxxabi.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

/*******************************************
 * DcpState
 *******************************************/
enum class DcpState : uint8_t {
    ALIVE = 0x00,
    CONFIGURATION = 0x01,
    PREPARING = 0x02,
    PREPARED = 0x03,
    CONFIGURING = 0x04,
    CONFIGURED = 0x05,
    INITIALIZING = 0x06,
    INITIALIZED = 0x07,
    SENDING_I = 0x08,
    SYNCHRONIZING = 0x09,
    SYNCHRONIZED = 0x0A,
    RUNNING = 0x0B,
    COMPUTING = 0x0C,
    COMPUTED = 0x0D,
    SENDING_D = 0x0E,
    STOPPING = 0x0F,
    STOPPED = 0x10,
    ERROR_HANDLING = 0x11,
    ERROR_RESOLVED = 0x12,

};

/**
 * Adds an DcpState to an osstream in a human readable format.
 * @param str
 * @param type
 */
static std::ostream &operator<<(std::ostream &os, DcpState type) {
    switch (type) {
        case DcpState::ALIVE:
            return os << "ALIVE";
        case DcpState::CONFIGURATION:
            return os << "CONFIGURATION";
        case DcpState::PREPARING:
            return os << "PREPARING";
        case DcpState::PREPARED:
            return os << "PREPARED";
        case DcpState::CONFIGURING:
            return os << "CONFIGURING";
        case DcpState::CONFIGURED:
            return os << "CONFIGURED";
        case DcpState::INITIALIZING:
            return os << "INITIALIZING";
        case DcpState::INITIALIZED:
            return os << "INITIALIZED";
        case DcpState::SENDING_I:
            return os << "SENDING_I I";
        case DcpState::SYNCHRONIZING:
            return os << "SYNCHRONIZING";
        case DcpState::SYNCHRONIZED:
            return os << "SYNCHRONIZED";
        case DcpState::RUNNING:
            return os << "RUNNING";
        case DcpState::COMPUTING:
            return os << "COMPUTING";
        case DcpState::COMPUTED:
            return os << "COMPUTED";
        case DcpState::SENDING_D:
            return os << "SENDING_D D";
        case DcpState::STOPPING:
            return os << "STOPPING";
        case DcpState::STOPPED:
            return os << "STOPPED";
        case DcpState::ERROR_HANDLING:
            return os << "ERROR_HANDLING";
        case DcpState::ERROR_RESOLVED:
            return os << "ERROR_HANDLING";
    }
    return os;
}

/**
 * Concatenate an DcpState to an string in a human readable format.
 * @param str
 * @param type
 */
static std::string to_string(DcpState type) {
    std::ostringstream oss;
    oss << type;
    return oss.str();
}

/*******************************************
 * DcpPduType
 *******************************************/
enum class DcpPduType : uint8_t {
    STC_register = 0x01,
    STC_deregister = 0x02,
    STC_prepare = 0x03,
    STC_configure = 0x04,
    STC_initialize = 0x05,
    STC_run = 0x06,
    STC_do_step = 0x07,
    STC_send_outputs = 0x08,
    STC_stop = 0x09,
    STC_reset = 0x0A,

    CFG_set_time_res = 0x20,
    CFG_set_steps = 0x21,
    CFG_config_input = 0x22,
    CFG_config_output = 0x23,
    CFG_config_clear = 0x24,
    CFG_set_target_network_information = 0x25,
    CFG_set_source_network_information = 0x26,
    CFG_set_parameter = 0x27,
    CFG_config_tunable_parameter = 0x28,
    CFG_set_param_network_information = 0x29,
    CFG_set_logging = 0x2A,
    CFG_set_scope = 0x2B,

    INF_state = 0x80,
    INF_error = 0x81,
    INF_log = 0x82,

    RSP_ack = 0xB0,
    RSP_nack = 0xB1,
    RSP_state_ack = 0xB2,
    RSP_error_ack = 0xB3,
    RSP_log_ack = 0xB4,

    NTF_state_changed = 0xE0,
    NTF_log = 0xE1,

    DAT_input_output = 0xF0,
    DAT_parameter = 0xF1,


};


/**
 * Adds an DcpPduType to an osstream in a human readable format.
 * @param str
 * @param type
 */
static std::ostream &operator<<(std::ostream &os, DcpPduType type) {
    switch (type) {
        case DcpPduType::STC_configure:
            return os << "STC_configure";
        case DcpPduType::STC_prepare:
            return os << "STC_prepare";
        case DcpPduType::STC_initialize:
            return os << "STC_initialize";
        case DcpPduType::STC_run:
            return os << "STC_run";
        case DcpPduType::STC_do_step:
            return os << "STC_do_step";
        case DcpPduType::STC_send_outputs:
            return os << "STC_send_outputs";
        case DcpPduType::STC_stop:
            return os << "STC_stop";
        case DcpPduType::STC_reset:
            return os << "STC_reset";
        case DcpPduType::INF_state:
            return os << "INF_state";
        case DcpPduType::INF_error:
            return os << "INF_error";
        case DcpPduType::INF_log:
            return os << "INF_log";
        case DcpPduType::NTF_state_changed:
            return os << "NTF_state_changed";
        case DcpPduType::NTF_log:
            return os << "NTF_log";
        case DcpPduType::CFG_set_time_res:
            return os << "CFG_set_time_res";
        case DcpPduType::CFG_set_steps:
            return os << "CFG_set_steps";
        case DcpPduType::STC_register:
            return os << "STC_register";
        case DcpPduType::STC_deregister:
            return os << "STC_deregister";
        case DcpPduType::CFG_config_input:
            return os << "CFG_config_input";
        case DcpPduType::CFG_config_output:
            return os << "CFG_config_output";
        case DcpPduType::CFG_set_parameter:
            return os << "CFG_set_parameter";
        case DcpPduType::CFG_config_tunable_parameter:
            return os << "CFG_config_tunable_parameter";
        case DcpPduType::CFG_set_param_network_information:
            return os << "CFG_set_param_network_information";
        case DcpPduType::CFG_config_clear:
            return os << "CFG_config_clear";
        case DcpPduType::CFG_set_logging:
            return os << "CFG_set_logging";
        case DcpPduType::DAT_input_output:
            return os << "DAT_input_output";
        case DcpPduType::DAT_parameter:
            return os << "DAT_parameter";
        case DcpPduType::RSP_ack:
            return os << "RSP_ack";
        case DcpPduType::RSP_nack:
            return os << "RSP_nack";
        case DcpPduType::RSP_state_ack:
            return os << "RSP_state_ack";
        case DcpPduType::RSP_error_ack:
            return os << "RSP_error_ack";
        case DcpPduType::RSP_log_ack:
            return os << "RSP_log_ack";
        case DcpPduType::CFG_set_target_network_information:
            return os << "CFG_set_target_network_information";
        case DcpPduType::CFG_set_source_network_information:
            return os << "CFG_set_source_network_information";
        case DcpPduType::CFG_set_scope:
            return os << "CFG_set_scope";
    }
    return os;
}

/**
 * Concatenate an DcpPduType to an string in a human readable format.
 * @param str
 * @param type
 */
static std::string to_string(DcpPduType type) {
    std::ostringstream oss;
    oss << type;
    return oss.str();
}


/*******************************************
 * DcpOpMode
 *******************************************/
enum class DcpOpMode : uint8_t {
    RT = 0,
    SRT = 1,
    NRT = 2,
};

/**
 * Adds an DcpOpMode to an osstream in a human readable format.
 * @param str
 * @param type
 */
static std::ostream &operator<<(std::ostream &os, DcpOpMode type) {
    switch (type) {
        case DcpOpMode::RT:
            return os << "RT";
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

/**
* Transform an string in a human readable format to an DcpOpMode.
* @param str
* @param type
*/
static DcpOpMode from_string_DcpOpMode(std::string str) {
    if (str == "RT") {
        return DcpOpMode::RT;
    } else if (str == "SRT") {
        return DcpOpMode::SRT;
    } else if (str == "NRT") {
        return DcpOpMode::NRT;
    }
    return DcpOpMode::RT;
}

/*******************************************
 * DcpDataType
 *******************************************/
enum class DcpDataType : uint8_t {
    uint8 = 0,
    uint16 = 1,
    uint32 = 2,
    uint64 = 3,
    int8 = 4,
    int16 = 5,
    int32 = 6,
    int64 = 7,
    float32 = 8,
    float64 = 9,
    string = 10,
    binary = 11,

    //internal
    state = 12,
   //toDo other types
};

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

/**
* Transform an string in a human readable format to an DcpDataType.
* @param str
* @param type
*/
static DcpDataType from_string_DcpDataType(std::string str) {
    if (str == "uint8") {
        return DcpDataType::uint8;
    } else if (str == "uint16") {
        return DcpDataType::uint16;
    } else if (str == "uint32") {
        return DcpDataType::uint32;
    } else if (str == "uint64") {
        return DcpDataType::uint64;
    } else if (str == "int8") {
        return DcpDataType::int8;
    } else if (str == "int16") {
        return DcpDataType::int16;
    } else if (str == "int32") {
        return DcpDataType::int32;
    } else if (str == "int64") {
        return DcpDataType::int64;
    } else if (str == "float32") {
        return DcpDataType::float32;
    } else if (str == "float64") {
        return DcpDataType::float64;
    } else if (str == "string") {
        return DcpDataType::string;
    } else if (str == "binary") {
        return DcpDataType::binary;
    }
    return DcpDataType::uint8;
}


/*******************************************
 * DcpError
 *******************************************/
enum class DcpError : uint16_t {
    NONE = 0x00,
    PROTOCOL_ERROR_GENERIC = 0x1001,
    PROTOCOL_ERROR_HEARTBEAT_MISSED = 0x1002,
    PROTOCOL_ERROR_PDU_NOT_ALLOWED_IN_THIS_STATE = 0x1003,
    PROTOCOL_ERROR_PROPERTY_VIOLATED = 0x1004,
    PROTOCOL_ERROR_STATE_TRANSITION_IN_PROGRESS = 0x1005,

    INVALID_LENGTH = 0x2001,
    INVALID_LOG_CATEGORY = 0x2002,
    INVALID_LOG_LEVEL = 0x2003,
    INVALID_LOG_MODE = 0x2004,
    INVALID_MAJOR_VERSION = 0x2005,
    INVALID_MINOR_VERSION = 0x2006,
    INVALID_NETWORK_INFORMATION = 0x2007,
    INVALID_OP_MODE = 0x2008,
    INVALID_PAYLOAD = 0x2009,
    INVALID_SCOPE = 0x200A,
    INVALID_SOURCE_DATA_TYPE = 0x200B,
    INVALID_START_TIME = 0x200C,
    INVALID_STATE_ID = 0x200D,
    INVALID_STEPS = 0x200E,
    INVALID_TIME_RESOLUTION = 0x200F,
    INVALID_TRANSPORT_PROTOCOL = 0x2010,
    INVALID_UUID = 0x2011,
    INVALID_VALUE_REFERENCE = 0x2012,


    INCOMPLETE_CONFIG_GAP_INPUT_POS = 0x3001,
    INCOMPLETE_CONFIG_GAP_OUTPUT_POS = 0x3002,
    INCOMPLETE_CONFIG_GAP_TUNABLE_POS = 0x3003,
    INCOMPLETE_CONFIG_NW_INFO_INPUT = 0x3004,
    INCOMPLETE_CONFIG_NW_INFO_OUTPUT = 0x3005,
    INCOMPLETE_CONFIG_NW_INFO_TUNABLE = 0x3006,
    INCOMPLETE_CONFIG_SCOPE = 0x3007,
    INCOMPLETE_CONFIG_STEPS = 0x3008,
    INCOMPLETE_CONFIG_TIME_RESOLUTION = 0x3009,
    INCOMPLETE_CONFIGURATION = 0x300A,

    NOT_SUPPORTED_LOG_ON_NOTIFICATION = 0x4001,
    NOT_SUPPORTED_LOG_ON_REQUEST = 0x4002,
    NOT_SUPPORTED_VARIABLE_STEPS = 0x4003,
    NOT_SUPPORTED_TRANSPORT_PROTOCOL = 04004,
    NOT_SUPPORTED_PDU = 0x4005,
    NOT_SUPPORTED_PDU_SIZE = 0x4006,

};

/**
 * Adds an DcpError to an osstream in a human readable format.
 * @param str
 * @param type
 */
static std::ostream &operator<<(std::ostream &os, DcpError type) {
    switch (type) {
        case DcpError::NONE:
            return os << "NONE";
        case DcpError::PROTOCOL_ERROR_GENERIC:
            return os << "PROTOCOL_ERROR_GENERIC";
        case DcpError::PROTOCOL_ERROR_HEARTBEAT_MISSED:
            return os << "PROTOCOL_ERROR_HEARTBEAT_MISSED";
        case DcpError::INCOMPLETE_CONFIG_GAP_INPUT_POS:
            return os << "INCOMPLETE_CONFIG_GAP_INPUT_POS";
        case DcpError::INCOMPLETE_CONFIG_GAP_OUTPUT_POS:
            return os << "INCOMPLETE_CONFIG_GAP_OUTPUT_POS";
        case DcpError::INCOMPLETE_CONFIG_GAP_TUNABLE_POS:
            return os << "INCOMPLETE_CONFIG_GAP_TUNABLE_POS";
        case DcpError::INCOMPLETE_CONFIG_NW_INFO_OUTPUT:
            return os << "INCOMPLETE_CONFIG_NW_INFO_OUTPUT";
        case DcpError::INCOMPLETE_CONFIG_NW_INFO_INPUT:
            return os << "INCOMPLETE_CONFIG_NW_INFO_INPUT";
        case DcpError::INCOMPLETE_CONFIG_NW_INFO_TUNABLE:
            return os << "INCOMPLETE_CONFIG_NW_INFO_TUNABLE";
        case DcpError::INCOMPLETE_CONFIG_SCOPE:
            return os << "INCOMPLETE_CONFIG_SCOPE";
        case DcpError::INCOMPLETE_CONFIG_STEPS:
            return os << "INCOMPLETE_CONFIG_STEPS";
        case DcpError::INCOMPLETE_CONFIG_TIME_RESOLUTION:
            return os << "INCOMPLETE_CONFIG_TIME_RESOLUTION";
        case DcpError::INCOMPLETE_CONFIGURATION:
            return os << "INCOMPLETE_CONFIGURATION";
        case DcpError::INVALID_LENGTH:
            return os << "INVALID_LENGTH";
        case DcpError::INVALID_LOG_CATEGORY:
            return os << "INVALID_LOG_CATEGORY";
        case DcpError::INVALID_LOG_LEVEL:
            return os << "INVALID_LOG_LEVEL";
        case DcpError::INVALID_LOG_MODE:
            return os << "INVALID_LOG_MODE";
        case DcpError::INVALID_MAJOR_VERSION:
            return os << "INVALID_MAJOR_VERSION";
        case DcpError::INVALID_MINOR_VERSION:
            return os << "INVALID_MINOR_VERSION";
        case DcpError::INVALID_NETWORK_INFORMATION:
            return os << "INVALID_NETWORK_INFORMATION";
        case DcpError::INVALID_OP_MODE:
            return os << "INVALID_OP_MODE";
        case DcpError::INVALID_SCOPE:
            return os << "INVALID_SCOPE";
        case DcpError::INVALID_START_TIME:
            return os << "INVALID_START_TIME";
        case DcpError::INVALID_STATE_ID:
            return os << "INVALID_STATE_ID";
        case DcpError::INVALID_STEPS:
            return os << "INVALID_STEPS";
        case DcpError::INVALID_TIME_RESOLUTION:
            return os << "INVALID_TIME_RESOLUTION";
        case DcpError::INVALID_TRANSPORT_PROTOCOL:
            return os << "INVALID_TRANSPORT_PROTOCOL";
        case DcpError::INVALID_UUID:
            return os << "INVALID_UUID";
        case DcpError::INVALID_VALUE_REFERENCE:
            return os << "INVALID_VALUE_REFERENCE";
        case DcpError::NOT_SUPPORTED_LOG_ON_NOTIFICATION:
            return os << "NOT_SUPPORTED_LOG_ON_NOTIFICATION";
        case DcpError::NOT_SUPPORTED_LOG_ON_REQUEST:
            return os << "NOT_SUPPORTED_LOG_ON_REQUEST";
        case DcpError::NOT_SUPPORTED_VARIABLE_STEPS:
            return os << "NOT_SUPPORTED_VARIABLE_STEPS";
        case DcpError::NOT_SUPPORTED_PDU_SIZE:
            return os << "NOT_SUPPORTED_PDU_SIZE";
        case DcpError::PROTOCOL_ERROR_PDU_NOT_ALLOWED_IN_THIS_STATE:
            return os << "PROTOCOL_ERROR_PDU_NOT_ALLOWED_IN_THIS_STATE";
        case DcpError::PROTOCOL_ERROR_PROPERTY_VIOLATED:
            return os << "PROTOCOL_ERROR_PROPERTY_VIOLATED";
        case DcpError::PROTOCOL_ERROR_STATE_TRANSITION_IN_PROGRESS:
            return os << "PROTOCOL_ERROR_STATE_TRANSITION_IN_PROGRESS";
        case DcpError::NOT_SUPPORTED_PDU:
            return os << "NOT_SUPPORTED_PDU";
        case DcpError::INVALID_PAYLOAD:
            return os << "INVALID_PAYLOAD";
        case DcpError::INVALID_SOURCE_DATA_TYPE:
            return os << "INVALID_SOURCE_DATA_TYPE";
        case DcpError::NOT_SUPPORTED_TRANSPORT_PROTOCOL:
            return os << "NOT_SUPPORTED_TRANSPORT_PROTOCOL";
    }
    return os;
}


/**
 * Concatenate an DcpError to an string in a human readable format.
 * @param str
 * @param type
 */
static std::string to_string(DcpError type) {
    std::ostringstream oss;
    oss << type;
    return oss.str();
}


/*******************************************
 * DcpTransportProtocol
 *******************************************/
enum class DcpTransportProtocol : uint8_t {
    UDP_IPv4 = 0,
    BLUETOOTH = 1,
    CAN = 2,
    USB = 3,
    TCP_IPv4 = 4,

};

/**
 * Adds an DcpTransportProtocol to an osstream in a human readable format.
 * @param str
 * @param type
 */
static std::ostream &operator<<(std::ostream &os, DcpTransportProtocol type) {
    switch (type) {
        case DcpTransportProtocol::UDP_IPv4:
            return os << "UDP_IPv4";
        case DcpTransportProtocol::CAN:
            return os << "CAN";
        case DcpTransportProtocol::BLUETOOTH:
            return os << "BLUETOOTH";
        case DcpTransportProtocol::USB:
            return os << "USB";
        case DcpTransportProtocol::TCP_IPv4:
            return os << "TCP_IPv4";
    }
    return os;
}


/**
 * Concatenate an DcpTransportProtocol to an string in a human readable format.
 * @param str
 * @param type
 */
static std::string to_string(DcpTransportProtocol type) {
    std::ostringstream oss;
    oss << type;
    return oss.str();
}


/*******************************************
 * DcpCausality
 *******************************************/
enum class DcpCausality : uint8_t {
    input = 0,
    output = 1,
    local = 2,
    parameter = 3,
};

static inline uint8_t getCausalityInt(DcpCausality causailty) {
    return static_cast<uint8_t>(causailty);
}

/**
 * Adds an DcpCausality to an osstream in a human readable format.
 * @param str
 * @param type
 */
static std::ostream &operator<<(std::ostream &os, DcpCausality type) {
    switch (type) {
        case DcpCausality::input:
            return os << "input";
        case DcpCausality::output:
            return os << "output";
        case DcpCausality::local:
            return os << "local";
        case DcpCausality::parameter:
            return os << "parameter";
    }
    return os;
}


/**
 * Concatenate an DcpCausality to an string in a human readable format.
 * @param str
 * @param type
 */
static std::string to_string(DcpCausality type) {
    std::ostringstream oss;
    oss << type;
    return oss.str();
}


/*******************************************
 * DcpVariability
 *******************************************/
enum class DcpVariability : uint8_t {
    fixed = 0,
    tunable = 1,
    discrete = 2,
    continuous = 3,
};

/**
 * Adds an DcpVariability to an osstream in a human readable format.
 * @param str
 * @param type
 */
static std::ostream &operator<<(std::ostream &os, DcpVariability type) {
    switch (type) {
        case DcpVariability::fixed:
            return os << "fixed";
        case DcpVariability::tunable:
            return os << "tunable";
        case DcpVariability::discrete:
            return os << "discrete";
        case DcpVariability::continuous:
            return os << "continuous";
    }
    return os;
}


/**
 * Concatenate an DcpVariability to an string in a human readable format.
 * @param str
 * @param type
 */
static std::string to_string(DcpVariability type) {
    std::ostringstream oss;
    oss << type;
    return oss.str();
}

enum class DcpCallbackTypes {
    PREPARE, CONFIGURE, SYNCHRONIZING_NRT_STEP, SYNCHRONIZED_NRT_STEP, RUNNING_NRT_STEP, STOP, TIME_RES, STEPS, OPERATION_INFORMATION, CONFIGURATION_CLEARED,
    RUNTIME, CONTROL_MISSED, IN_OUT_MISSED, PARAM_MISSED, STATE_CHANGED, ERROR_LI, INITIALIZE,
    ACK, NACK, STATE_ACK, ERROR_ACK, PDU_MISSED, DATA, RSP_log_ack, NTF_LOG, SYNCHRONIZING_STEP, SYNCHRONIZED_STEP, RUNNING_STEP, SYNCHRONIZE,
};

enum FunctionType {
    SYNC, ASYNC,
};

static inline uint8_t getDcpDataTypeSize(DcpDataType type) {
    switch (type) {
        case DcpDataType::uint8:
            return 1;
        case DcpDataType::uint16:
            return 2;
        case DcpDataType::uint32:
            return 4;
        case DcpDataType::uint64:
            return 8;
        case DcpDataType::int8:
            return 1;
        case DcpDataType::int16:
            return 2;
        case DcpDataType::int32:
            return 4;
        case DcpDataType::int64:
            return 8;
        case DcpDataType::float32:
            return 4;
        case DcpDataType::float64:
            return 8;
        default:
            return 0;
    }
    return 0;
}



enum class DcpScope : uint8_t {
    CONFIGURED_INITIALIZING_INITIALIZED_RUNNING = 0x00,
    CONFIGURED_INITIALIZING_INITIALIZED = 0x01,
    RUNNING = 0x02,
};

static std::ostream &operator<<(std::ostream &os, DcpScope scope) {
    switch (scope) {
        case DcpScope::CONFIGURED_INITIALIZING_INITIALIZED_RUNNING:
            return os << "CONFIGURED/INITIALIZING/INITIALIZED/RUNNING";
        case DcpScope::CONFIGURED_INITIALIZING_INITIALIZED:
            return os << "CONFIGURED/INITIALIZING/INITIALIZED ";
        case DcpScope::RUNNING:
            return os << "RUNNING";
    }
    return os;
}

static std::string to_string(DcpScope scope) {
    std::ostringstream oss;
    oss << scope;
    return oss.str();
}

enum class DcpLogLevel : uint8_t {
    LVL_FATAL = 0,
    LVL_ERROR = 1,
    LVL_WARNING = 2,
    LVL_INFORMATION = 3,
    LVL_DEBUG = 4,
};

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

enum class DcpLogMode : uint8_t {
    LOG_ON_REQUEST = 0,
    LOG_ON_NOTIFICATION = 1,
};

static std::ostream &operator<<(std::ostream &os, DcpLogMode logMode) {
    switch (logMode) {
        case DcpLogMode::LOG_ON_REQUEST:
            return os << "LOG_ON_REQUEST";
        case DcpLogMode::LOG_ON_NOTIFICATION:
            return os << "LOG_ON_NOTIFICATION";
    }
    return os;
}

static std::string to_string(DcpLogMode logMode) {
    std::ostringstream oss;
    oss << logMode;
    return oss.str();
}

template<typename T>
std::string type_name()
{
    int status;
    std::string tname = typeid(T).name();
    char *demangled_name = abi::__cxa_demangle(tname.c_str(), NULL, NULL, &status);
    if(status == 0) {
        tname = demangled_name;
        std::free(demangled_name);
    }
    return tname;
}

template<typename T>
static DcpDataType getFixedSizeDcpDataType() {
    if (std::is_same<uint8_t, T>::value) {
        return DcpDataType::uint8;
    } else if (std::is_same<uint16_t, T>::value) {
        return DcpDataType::uint16;
    } else if (std::is_same<uint32_t, T>::value) {
        return DcpDataType::uint32;
    } else if (std::is_same<uint64_t, T>::value) {
        return DcpDataType::uint64;
    } else if (std::is_same<int8_t, T>::value) {
        return DcpDataType::int8;
    } else if (std::is_same<int16_t, T>::value) {
        return DcpDataType::int16;
    } else if (std::is_same<int32_t, T>::value) {
        return DcpDataType::int32;
    } else if (std::is_same<int64_t, T>::value) {
        return DcpDataType::int64;
    } else if (std::is_same<float32_t, T>::value) {
        return DcpDataType::float32;
    } else if (std::is_same<float64_t, T>::value) {
        return DcpDataType::float64;
    } else if (std::is_same<DcpLogLevel , T>::value) {
        return DcpDataType::uint8;
    } else if (std::is_same<DcpPduType , T>::value) {
        return DcpDataType::uint8;
    } else if (std::is_same<DcpState , T>::value) {
        return DcpDataType::state;
    }else if (std::is_same<DcpOpMode , T>::value) {
        return DcpDataType::uint8;
    }else if (std::is_same<DcpDataType , T>::value) {
        return DcpDataType::uint8;
    }else if (std::is_same<DcpError , T>::value) {
        return DcpDataType::uint16;
    }else if (std::is_same<DcpCausality , T>::value) {
        return DcpDataType::uint8;
    }else if (std::is_same<DcpScope, T>::value) {
        return DcpDataType::uint8;
    }else if (std::is_same<DcpTransportProtocol , T>::value) {
        return DcpDataType::uint8;
    }else if (std::is_same<DcpVariability , T>::value) {
        return DcpDataType::uint8;
    }else if (std::is_same<DcpLogMode, T>::value) {
        return DcpDataType::uint8;
    }
    throw std::invalid_argument(type_name<T>() + " is not an fixed size DcpDataType");
    return DcpDataType::uint8;
}




#endif //LIBACOSAR_ACICONSTANTS_H
