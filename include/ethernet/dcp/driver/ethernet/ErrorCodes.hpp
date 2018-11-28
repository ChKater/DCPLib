//
// Created by Christian Kater on 27.11.18.
//

#ifndef DCPLIB_ERRORCODES_H
#define DCPLIB_ERRORCODES_H

#include <dcp/model/DcpConstants.hpp>
#include <dcp/log/LogTemplate.hpp>

static const LogTemplate NEW_SOCKET = LogTemplate(logId++, LogCategory::DCP_LIB_ETHERNET,
                                           DcpLogLevel::LVL_DEBUG, "%string socket opened on %string",
                                           {DcpDataType::string, DcpDataType::string});
static const LogTemplate SOCKET_CLOSED = LogTemplate(logId++, LogCategory::DCP_LIB_ETHERNET,
                                              DcpLogLevel::LVL_DEBUG, "%string socket closed on %string.",
                                              {DcpDataType::string, DcpDataType::string});

static const LogTemplate NEW_TCP_CONNECTION_IN = LogTemplate(logId++, LogCategory::DCP_LIB_ETHERNET,
                                                 DcpLogLevel::LVL_DEBUG, "TCP connection established from %string.",
                                                 {DcpDataType::string});

static const LogTemplate NEW_TCP_CONNECTION_OUT = LogTemplate(logId++, LogCategory::DCP_LIB_ETHERNET,
                                                          DcpLogLevel::LVL_DEBUG, "TCP connection established to %string.",
                                                          {DcpDataType::string});

static const LogTemplate TCP_CONNECTION_CLOSED = LogTemplate(logId++, LogCategory::DCP_LIB_ETHERNET,
                                                      DcpLogLevel::LVL_DEBUG, "TCP connection with %string closed.",
                                                      {DcpDataType::string});

static const LogTemplate NEW_MASTER_ENDPOINT = LogTemplate(logId++, LogCategory::DCP_LIB_ETHERNET,
                                                    DcpLogLevel::LVL_DEBUG,
                                                    "%string endpoint for the master is now: %string.",
                                                    {DcpDataType::string, DcpDataType::string});
static const LogTemplate PDU_RECEIVED = LogTemplate(logId++, LogCategory::DCP_LIB_ETHERNET,
                                             DcpLogLevel::LVL_DEBUG, "Pdu was received with content=%string",
                                             {DcpDataType::string});
static const LogTemplate PDU_SEND = LogTemplate(logId++, LogCategory::DCP_LIB_ETHERNET,
                                         DcpLogLevel::LVL_DEBUG, "Pdu was send with content=%string",
                                         {DcpDataType::string});

static const LogTemplate NETWORK_PROBLEM = LogTemplate(logId++, LogCategory::DCP_LIB_ETHERNET,
                                                         DcpLogLevel::LVL_ERROR,
                                                         "Network problem in %string driver. Error Message: %string",
                                                         {DcpDataType::string, DcpDataType::string});


#endif //DCPLIB_ERRORCODES_H
