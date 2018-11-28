//
// Created by kater on 25.07.18.
//

#ifndef DCPLIB_FILTEREDOSTREAMLOG_H
#define DCPLIB_FILTEREDOSTREAMLOG_H

#include "OstreamLog.hpp"

class FilteredOstreamLog : public OstreamLog {
public:
    FilteredOstreamLog(std::ostream &_stream, DcpLogLevel level, logCategory_t category) :
            OstreamLog(_stream), level(level), category(category) {}

    ~FilteredOstreamLog() override {}

    void logOstream(const LogEntry &log) override {
        if(log.getCategory() == category && log.getLevel() == level){
            OstreamLog::logOstream(log);
        }
    }

private:
    DcpLogLevel level;
    logCategory_t category;
};
#endif //DCPLIB_FILTEREDOSTREAMLOG_H
