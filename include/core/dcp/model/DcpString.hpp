//
// Created by kater on 11.03.18.
//

#ifndef ACOSAR_ACISTRING_H
#define ACOSAR_ACISTRING_H

#include <cstdint>
#include <string>
#include <math.h>


class DcpString {
public:

    DcpString(uint32_t maxSize){
        payload = new char[maxSize + 4];
        *((uint32_t*) payload) = 0;
        managed = true;
    }

    DcpString(std::string& str, uint32_t maxSize){
        payload = new char[maxSize + 4];
        setString(str);
        managed = true;
    }

    DcpString(char* payload){
        this->payload = payload;
        managed = false;
    }

    ~DcpString(){
        if(managed){
            delete[] payload;
        }
    }

    const char* getChar() const{
        return payload + 4;
    }

    const void setString(std::string& str){
        assert(str.size() < pow(2, 32));
        *((uint32_t*) payload) = str.size();
        strcpy(payload + 4, str.c_str());
    }

    const std::string getString() const {
        return std::string(payload + 4, getSize());
    }

    const uint32_t getSize() const{
        return *((uint16_t*) payload);
    }

    const char* getPayload() const {
        return payload;
    }

private:
    char* payload;
    bool managed;
};




#endif //ACOSAR_ACISTRING_H
