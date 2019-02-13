//
// Created by kater on 11.03.18.
//

#ifndef ACOSAR_ACIBINARY_H
#define ACOSAR_ACIBINARY_H

#include <cstdint>
#include <algorithm>

class DcpBinary{
public:
    DcpBinary(uint32_t maxSize){
        payload = new uint8_t[maxSize + 4];
        *((uint16_t*) payload) = 0;
        managed = true;
    }

    DcpBinary(uint32_t length, uint8_t* binary, uint32_t maxSize){
        payload = new uint8_t[maxSize];
        setBinary(length, binary);
        managed = true;
    }

    DcpBinary(uint8_t* payload){
        this->payload = payload;
        managed = false;
    }

    ~DcpBinary(){
        if(managed){
            delete[] payload;
        }
    }

    const uint8_t* getBinary() const{
        return payload + 4;
    }

    const void setBinary(uint32_t length, uint8_t* binary){
        *((uint32_t*) payload) = length;
        std::copy(binary, binary + length, payload+4);
    }

    const uint32_t getSize() const{
        return *((uint16_t*) payload);
    }

    const uint8_t* getPayload() const {
        return payload;
    }

private:
    uint8_t* payload;
    bool managed;
};


#endif //ACOSAR_ACIBINARY_H
