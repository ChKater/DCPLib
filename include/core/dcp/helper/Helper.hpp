//
// Created by kater on 06.09.17.
//

#ifndef PROJECT_HELPER_H_H
#define PROJECT_HELPER_H_H

#include "dcp/model/DcpConstants.hpp"
#include "dcp/model/LogEntry.hpp"

#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <iostream>
#include <iomanip>




static inline   int char2int(char input) {
    if(input >= '0' && input <= '9')
        return input - '0';
    if(input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if(input >= 'a' && input <= 'f')
        return input - 'a' + 10;
    throw std::invalid_argument("Invalid input string");
}

static inline uint128_t convertToUUID(std::string uuidStr){
    uuidStr.erase(std::remove(uuidStr.begin(), uuidStr.end(), '-'), uuidStr.end());

    uint128_t uuid;
    for(int i = 0; i < 32; i = i + 2){
        char first = uuidStr.at(i);
        char second = uuidStr.at(i+1);
        uint8_t b = char2int(first) * 16 + char2int(second);
        uuid.data[i / 2] = b;
    }
    return uuid;
}



static inline std::string convertToUUIDStr(uint128_t uuid){
    std::stringstream sstream;
    for(int i = 0; i < 16; i++){
        sstream << std::hex << std::setw(2) << std::setfill('0')
                << unsigned(uuid.data[i]);
        if(i == 3 || i == 5 || i == 7 || i == 9){
            sstream << "-";
        }
    }
    return sstream.str();
}

static std::string to_string(std::chrono::system_clock::time_point tp){
    // convert to std::time_t in order to convert to std::tm (broken time)
    auto timer = std::chrono::system_clock::to_time_t(tp);

    // convert to broken time
    std::tm bt = *std::localtime(&timer);

    std::ostringstream oss;
    oss << std::put_time(&bt, "%F"); // %Y-%m-%d
    oss << " ";
    oss << std::put_time(&bt, "%T"); // HH:MM:SS
    return oss.str();
}

static inline std::string convertUnixTimestamp(int64_t unixTimestamp){
    return to_string(std::chrono::system_clock::time_point(std::chrono::seconds(unixTimestamp)));

}
static inline std::string currentTime(){
    return to_string(std::chrono::system_clock::now());
}

namespace DcpLogHelper {


    template<typename T>
    inline size_t calcsize(const T val) {
        return sizeof(T);
    };

    template<>
    inline size_t calcsize(const std::string val) {
        return val.length() + 2;
    };

    template<>
    inline size_t calcsize(const char *const val) {
        return calcsize(std::string(val));
    };


    inline size_t size() {
        return 0;
    };

    template<typename T, typename ... Args>
    inline size_t size(const T val, const Args... args) {
        return calcsize<T>(val) + size(args...);
    };



    template<typename T>
    inline size_t applyField(uint8_t *payload, const T val) {
        *((T *) payload) = val;
        return sizeof(T);
    };

    template<>
    inline size_t applyField(uint8_t *payload, const std::string val) {
        *((uint16_t *) payload) = val.length();
        std::memcpy(payload + 2, val.c_str(), val.length());
        return val.length() + 2;
    };

    template<>
    inline size_t applyField(uint8_t *payload, const char *const val) {
        return applyField(payload, std::string(val));
    };

    inline void applyFields(uint8_t *payload) {
        //exit recursion
    };

    template<typename T>
    inline void applyFields(uint8_t *payload, const T val) {
        applyField<T>(payload, val);
    };

    template<typename T, typename ... Args>
    inline void applyFields(uint8_t *payload, const T val, const Args... args) {
        applyFields(payload + applyField<T>(payload, val), args...);
    };

    template<typename T>
    inline void _checkDataTypes(const LogTemplate &logTemplate, const size_t index, const T val) {
        if(index >= logTemplate.dataTypes.size()){
            throw std::invalid_argument("To many arguments given for template id " + std::to_string(logTemplate.id));
        }
        try {
            if (logTemplate.dataTypes.at(index) != getFixedSizeDcpDataType<T>()) {
                throw std::invalid_argument("Wrong data type in for template_id " + std::to_string(logTemplate.id) + " (index = " +
                                       std::to_string(index) + " ). Is " + type_name<T>()+ ". Is expected to be " +
                                       to_string(logTemplate.dataTypes.at(index)) + " on " +
                                       std::to_string(index) + ". index.");
            }
        } catch(std::exception e){
            std::cout << e.what() << std::endl;
            throw std::invalid_argument("Error for template_id " + std::to_string(logTemplate.id) + "index=" + std::to_string(index) + " type=" + type_name<T>());
        }
    }

    template<>
    inline void _checkDataTypes(const LogTemplate &logTemplate, const size_t index, const std::string val) {
        if(index >= logTemplate.dataTypes.size()){
            throw std::invalid_argument("To many arguments given for template id " + std::to_string(logTemplate.id));
        }
        try {
            if (logTemplate.dataTypes.at(index) != DcpDataType::string) {
                throw std::invalid_argument("Wrong data type in for template_id " + std::to_string(logTemplate.id) + " (index = " +
                                       std::to_string(index) + " ). Is string. Is expected to be " +
                                       to_string(logTemplate.dataTypes.at(index)) + " on " +
                                       std::to_string(index) + ". index.");
            }
        } catch(std::exception e){
            throw std::invalid_argument("Error for template_id " + std::to_string(logTemplate.id));
        }

    }

    template<>
    inline void
    _checkDataTypes(const LogTemplate &logTemplate, const size_t index, const char *const val) {
        _checkDataTypes(logTemplate, index, std::string(val));
    }

    inline void checkDataTypes(const LogTemplate &logTemplate, const size_t index) {
        //exit recursion
    }

    template<typename T, typename ... Args>
    inline void checkDataTypes(const LogTemplate &logTemplate, const size_t index, const T val, const Args... args) {
        _checkDataTypes<T>(logTemplate, index, val);
        checkDataTypes(logTemplate, index + 1, args...);
    }


}
#endif //PROJECT_HELPER_H_H
