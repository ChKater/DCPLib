//
// Created by kater on 26.06.18.
//

#ifndef DCPLIB_LOGABLE_H
#define DCPLIB_LOGABLE_H

#include "dcp/helper/Helper.hpp"
#include "dcp/model/LogEntry.hpp"
#include "dcp/log/LogManager.hpp"


class Logable {
public:
    void setLogManager(const LogManager &logManager) {
        Logable::logManager = logManager;
    }

private:

protected:
    LogManager logManager;

public:
    template<typename ... Args>
    inline void Log(const LogTemplate &logTemplate, const Args... args) {
        using namespace std::chrono;

        DcpLogHelper::checkDataTypes(logTemplate, 0, args...);
        size_t size = DcpLogHelper::size(args...);
        const auto logTime = time_point_cast<microseconds>(system_clock::now());

        uint8_t* payload = logManager.alloc(size + 9);
        *((int64_t *) payload) = (int64_t) duration_cast<seconds>(logTime.time_since_epoch()).count();
        *((uint8_t *) payload + 8) = logTemplate.id;


        DcpLogHelper::applyFields(payload + 9, args...);
        logManager.consume(logTemplate, payload, size + 9);
    }
};
#endif //DCPLIB_LOGABLE_H
