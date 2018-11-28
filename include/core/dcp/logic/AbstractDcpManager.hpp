/*
 * AbstractAciDriverManager.h
 *
 *  Created on: 19.02.2017
 *      Author: kater
 */

#ifndef ACI_DRIVER_ABSTRACTACIDRIVERMANAGER_H_
#define ACI_DRIVER_ABSTRACTACIDRIVERMANAGER_H_

#include <cstdint>
#include <iostream>
#include <map>

#include "dcp/logic/DcpManager.hpp"
#include "dcp/driver/DcpDriver.hpp"
#include "dcp/logic/Logable.hpp"


/**
 * General interface for the AciLogic.
 *
 * @author Christian Kater
 */
class AbstractDcpManager : public Logable {
public:

    ~AbstractDcpManager() {}

    void DAT_input_output(const uint16_t dataId, uint8_t *payload,
                          const uint16_t payloadSize) {
        DcpPduDatInputOutput data = {getNextDataSeqNum(dataId), dataId, payload, payloadSize};
        driver.send(data);
    }

    void start() {

        driver.setDcpManager(getDcpManager());
        driver.setLogManager(logManager);
        driver.startReceiving();
    }

    virtual void receive(DcpPdu &msg) = 0;

    void addLogListener(std::function<void(const LogEntry &)> fct) {
        logListeners.push_back(std::move(fct));
    }

    void setGenerateLogString(bool generateLogString) {
        this->generateLogString = generateLogString;
    }

    virtual void reportError(const DcpError errorCode) = 0;

    virtual DcpManager getDcpManager() = 0;

protected:
    /**
     * DCP Driver instance
     */
    DcpDriver driver;

    /**
     * last seq. id which was send out
     */
    std::map<uint8_t, uint16_t> segNumsOut;
    /**
     * last seq. id which was received
     */
    std::map<uint8_t, uint16_t> segNumsIn;

    /**
     * last seq. id which was send out
     */
    std::map<uint16_t, uint16_t> dataSegNumsOut;
    /**
     * last seq. id which was received
     */
    std::map<uint16_t, uint16_t> dataSegNumsIn;

/**
	 * last seq. id which was send out
	 */
    std::map<uint16_t, uint16_t> parameterSegNumsOut;
    /**
     * last seq. id which was received
     */
    std::map<uint16_t, uint16_t> parameterSegNumsIn;


    std::vector<std::function<void(const LogEntry &)>> logListeners;
    bool generateLogString;

    uint8_t dcpId;
    uint8_t masterId;


protected:
    AbstractDcpManager() {
        setLogManager({[this](const LogTemplate &logTemplate, uint8_t *payload, size_t size) {
            consume(logTemplate, payload, size);
        }, [this](size_t size) { return alloc(size); }});
    }

    /**
     * Returns the next sequence number for an given acuId
     * @param acuId acuId for which the seq. id. will be returned
     */
    uint16_t getNextSeqNum(const uint8_t acuId) {
        int nextSeq = segNumsOut[acuId];
        segNumsOut[acuId] += 1;
        return nextSeq;
    }

    uint16_t getNextDataSeqNum(const uint16_t data_id) {
        int nextSeq = dataSegNumsOut[data_id];
        dataSegNumsOut[data_id] += 1;
        return nextSeq;
    }

    uint16_t checkSeqId(const uint8_t acuId, const uint16_t seqId) {
        uint16_t old = segNumsIn[acuId];
        if (seqId > old) {
            segNumsIn[acuId] = seqId + 1;
        }
        return seqId - old;
    }


    uint16_t checkSeqIdInOut(const uint16_t dataId, const uint16_t seqId) {
        uint16_t old = dataSegNumsIn[dataId];
        if (seqId > old) {
            dataSegNumsIn[dataId] = seqId + 1;
        }
        return seqId - old;
    }

    uint16_t checkSeqIdParam(const uint16_t parameterId,
                             const uint16_t seqId) {
        uint16_t old = parameterSegNumsIn[parameterId];
        if (seqId > old) {
            parameterSegNumsIn[parameterId] = seqId + 1;
        }
        return seqId - old;
    }

    uint16_t getNextParameterSeqNum(const uint16_t parameterId) {
        int nextSeq = parameterSegNumsOut[parameterId];
        parameterSegNumsOut[parameterId] += 1;
        return nextSeq;
    }


    virtual void consume(const LogTemplate &logTemplate, uint8_t *payload, size_t size) {
        LogEntry logEntry(logTemplate, payload, size);
        if (generateLogString) {
            logEntry.applyPayloadToString();
        }
        for (const std::function<void(const LogEntry &)> &logListener: logListeners) {
            logListener(logEntry);
        }
        delete[] payload;

    }

    virtual uint8_t *alloc(size_t size) {
        return new uint8_t[size];
    }
};

#endif /* ACI_DRIVER_ABSTRACTACIDRIVERMANAGER_H_ */
