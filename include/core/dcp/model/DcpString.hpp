//
// Created by kater on 11.03.18.
//

#ifndef ACOSAR_ACISTRING_H
#define ACOSAR_ACISTRING_H

#include <cstdint>
#include <string>
#include <iostream>
#include <cstring>
#include <memory>

class DcpString {
public:

    DcpString(uint32_t maxSize){
        payload = new char[maxSize];
        *((uint16_t*) payload) = 0;
    }

    DcpString(std::string& str, uint32_t maxSize){
        payload = new char[maxSize + 4];
        setString(str);
    }

    ~DcpString(){}

    const char* getChar() const{
        return payload + 2;
    }

    const void setString(std::string& str){
        //todo check if length > max(uint16)
        *((uint16_t*) payload) = str.size();
        strcpy(payload + 2, str.c_str());
    }

    const std::string getString() const {
        return std::string(payload + 2, getSize());
    }

    const uint16_t getSize() const{
        return *((uint16_t*) payload);
    }

    const char* getPayload() const {
        return payload;
    }

private:
    char* payload;
};




#endif //ACOSAR_ACISTRING_H
