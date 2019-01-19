//
// Created by kater on 26.06.18.
//

#ifndef DCPLIB_DCPMANAGERCALLBACKS_H
#define DCPLIB_DCPMANAGERCALLBACKS_H

#include <dcp/model/pdu/DcpPdu.hpp>
#include <dcp/model/constant/DcpError.hpp>

struct DcpManager{
    std::function<void(DcpPdu&)> receive;
    std::function<void(const DcpError)> reportError;
};

#endif //DCPLIB_DCPMANAGERCALLBACKS_H
