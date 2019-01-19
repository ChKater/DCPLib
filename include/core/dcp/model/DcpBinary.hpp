//
// Created by kater on 11.03.18.
//

#ifndef ACOSAR_ACIBINARY_H
#define ACOSAR_ACIBINARY_H

#include <cstdint>

class DcpBinary{
public:
    DcpBinary(uint32_t maxSize){
        payload = new uint8_t[maxSize];
        *((uint16_t*) payload) = 0;
    }

    DcpBinary(uint16_t length, uint8_t* binary, uint32_t maxSize){
        payload = new uint8_t[maxSize];
        setBinary(length, binary);
    }

    ~DcpBinary(){
    }

    const uint8_t* getBinary() const{
        return payload + 2;
    }

    const void setBinary(uint16_t length, uint8_t* binary){
        *((uint16_t*) payload) = length;
        std::copy(binary, binary + length, payload+2);
    }

    const uint16_t getSize() const{
        return *((uint16_t*) payload);
    }

    const uint8_t* getPayload() const {
        return payload;
    }

private:
    uint8_t* payload;

};


#endif //ACOSAR_ACIBINARY_H
