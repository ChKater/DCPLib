//
// Created by kater on 18.07.18.
//

#ifndef DCPLIB_LOGMANAGER_H
#define DCPLIB_LOGMANAGER_H

#include <cstdint>
#include <dcp/model/LogTemplate.hpp>
#include <functional>


struct LogManager{
    std::function<void(const LogTemplate&, uint8_t*, size_t)> consume;
    std::function<uint8_t*(size_t)> alloc;
};
#endif //DCPLIB_LOGMANAGER_H
