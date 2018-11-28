//
// Created by kater on 29.09.17.
//

#ifndef ACOSAR_DRIVERMANAGERSLAVE_H
#define ACOSAR_DRIVERMANAGERSLAVE_H

#include "dcp/logic/AbstractDcpManagerSlave.hpp"

#include <thread>

/**
 * DCP mangement of an slave
 * @author Christian Kater <kater@sim.uni-hannover.de>
 */
class DcpManagerSlave : public AbstractDcpManagerSlave {

public:


    /**
     * Instanciate a DCP manager for an slave
     * @param dcpSlaveDescription Slave description of the slave
     * @param driver DCP driver of the slave
     */
    DcpManagerSlave(const SlaveDescription_t &dcpSlaveDescription, DcpDriver driver) : AbstractDcpManagerSlave(
            dcpSlaveDescription) {
        this->driver = driver;
    }

    ~DcpManagerSlave() {
        delete initializing;
        delete configuring;
        delete stopping;
        delete _doStep;
        delete running;
        delete heartbeat;
    }

    /**
     * Stops the slave if possible
     * @return True means slave switched to state STOPPING, false means no action done
     */
    virtual bool stop() override {
        if (!(state == DcpState::ALIVE || state == DcpState::CONFIGURATION || state == DcpState::STOPPING ||
              state == DcpState::STOPPED || state == DcpState::ERROR_HANDLING || state == DcpState::ERROR_RESOLVED)) {
            lastExecution++;
            std::thread *toStop = NULL;

            if (state == DcpState::CONFIGURING) {
                toStop = configuring;
            } else if (state == DcpState::INITIALIZING) {
                toStop = initializing;
            }
            state = DcpState::STOPPING;
            notifyStateChange();
            if (stopping != NULL) {
                stopping->detach();
                delete stopping;
            }
            stopping = new std::thread(&DcpManagerSlave::startStopping, this, toStop);
            return true;
        }
        return false;
    }

    DcpManager getDcpManager() override {
        return {[this](DcpPdu &msg) { receive(msg); },
                [this](const DcpError errorCode) { reportError(errorCode); }};
    }


protected:
    /**
     * Upcounting id for last execution. Is used to determine
     * if between an longer operation, e.g. initializing, an
     * state change has happened and the done calculations are
     * obsolete.
     */
    int lastExecution;
    std::thread *initializing = NULL;
    std::thread *synchronizing = NULL;
    std::thread *configuring = NULL;
    std::thread *preparing = NULL;
    std::thread *stopping = NULL;
    std::thread *_doStep = NULL;
    std::thread *running = NULL;
    std::thread *heartbeat = NULL;

    /* Mutex */
    std::mutex mtxInput;
    std::mutex mtxOutput;
    std::mutex mtxHeartbeat;
    std::mutex mtxParam;
    std::mutex mtxLog;

    /* Time Handling */
    std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> lastStateRequest;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds> nextCommunication;



    /**************************
    *  General
    **************************/

    template<DcpState preconditon, DcpState postcondition>
    inline void routineFinished(const LogTemplate &finished, const LogTemplate &interrupted) {
#ifdef DEBUG
        Log(finished);
#endif
        if (state == preconditon) {
            state = postcondition;
            notifyStateChange();
        } else {
#ifdef DEBUG
            Log(interrupted);
#endif
        }
    }

    /**************************
    *  Prepare
    **************************/

    virtual void prepare() override {
        lastExecution++;
        if (preparing != NULL) {
            preparing->detach();
            delete preparing;

        }
        preparing = new std::thread(&DcpManagerSlave::startPreparing, this);
    }

    void startPreparing() {
#ifdef DEBUG
        Log(PREPARING_STARTED);
#endif
        if (asynchronousCallback[DcpCallbackTypes::PREPARE]) {
            std::thread t(prepareCallback);
            t.detach();
        } else {
            prepareCallback();
            preparingFinished();

        }
    }

    virtual void preparingFinished() override {
        routineFinished<DcpState::PREPARING, DcpState::PREPARED>(PREPARING_FINISHED, PREPARING_INTERRUPTED);
    }

    /**************************
     *  Configure
     **************************/

    virtual void configure() override {
        lastExecution++;
        if (configuring != NULL) {
            configuring->detach();
            delete configuring;

        }
        configuring = new std::thread(&DcpManagerSlave::startConfiguring, this);
    }

    void startConfiguring() {
#ifdef DEBUG
        Log(CONFIGURING_STARTED);
#endif
        if (asynchronousCallback[DcpCallbackTypes::CONFIGURE]) {
            std::thread t(configureCallback);
            t.detach();
        } else {
            configureCallback();
            configuringFinished();

        }
    }

    virtual void configuringFinished() override {
        routineFinished<DcpState::CONFIGURING, DcpState::CONFIGURED>(CONFIGURING_FINISHED, CONFIGURING_INTERRUPTED);
    }

    /**************************
    *  Initialize
    **************************/

    virtual void initialize() override {
        lastExecution++;
        if (initializing != NULL) {
            initializing->detach();
            delete initializing;
        }
        initializing = new std::thread(&DcpManagerSlave::startInitializing, this);
    }

    void startInitializing() {
#ifdef DEBUG
        Log(INITIALIZING_STARTED);
#endif

        if (asynchronousCallback[DcpCallbackTypes::INITIALIZE]) {
            std::thread t(initializeCallback);
            t.detach();
        } else {
            initializeCallback();
            initializingFinished();
        }
    }

    virtual void initializingFinished() override {
        routineFinished<DcpState::INITIALIZING, DcpState::INITIALIZED>(INITIALIZING_FINISHED, INITIALIZING_INTERRUPTED);
    }

    /**************************
     *  Synchronize
     **************************/

    virtual void synchronize() override {
        lastExecution++;
        if (synchronizing != NULL) {
            synchronizing->detach();
            delete synchronizing;
        }
        synchronizing = new std::thread(&DcpManagerSlave::startSynchronizing, this);
    }

    void startSynchronizing() {
#ifdef DEBUG
        Log(SYNCHRONIZING_STARTED);
#endif

        if (asynchronousCallback[DcpCallbackTypes::SYNCHRONIZE]) {
            std::thread t(synchronizeCallback);
            t.detach();
        } else {
            synchronizeCallback();
            synchronizingFinished();
        }
    }

    virtual void synchronizingFinished() override {
#ifdef DEBUG
        Log(SYNCHRONIZING_FINISHED);
#endif
        if (state == DcpState::SYNCHRONIZING) {
            state = DcpState::SYNCHRONIZED;
            realtimeState = DcpState::SYNCHRONIZED;
            notifyStateChange();
        } else {
#ifdef DEBUG
            Log(SYNCHRONIZING_INTERRUPTED);
#endif
        }
    }


    /**************************
    *  Compute
    **************************/

    virtual void doStep(const uint32_t steps) override {
        if (_doStep != NULL) {
            _doStep->detach();
            delete _doStep;
        }
        _doStep = new std::thread(&DcpManagerSlave::startComputing, this,
                                  steps);
    }

    void startComputing(uint32_t steps) {
#ifdef DEBUG
        Log(COMPUTING_STARTED);
#endif

        switch (runLastExitPoint) {
            case DcpState::RUNNING: {
                if (state == DcpState::RUNNING) {
                    if (asynchronousCallback[DcpCallbackTypes::RUNNING_NRT_STEP]) {
                        std::thread t(runningNRTStepCallback, steps);
                        t.detach();
                    } else {
                        runningNRTStepCallback(steps);
                        computingFinished();
                    }
                }
                break;
            }
            case DcpState::SYNCHRONIZING: {
                if (asynchronousCallback[DcpCallbackTypes::SYNCHRONIZING_NRT_STEP]) {
                    std::thread t(synchronizingNRTStepCallback, steps);
                    t.detach();
                } else {
                    synchronizingNRTStepCallback(steps);
                    computingFinished();
                }
                break;
            }
            case DcpState::SYNCHRONIZED: {
                if (asynchronousCallback[DcpCallbackTypes::SYNCHRONIZED_NRT_STEP]) {
                    std::thread t(synchronizedNRTStepCallback, steps);
                    t.detach();
                } else {
                    synchronizedNRTStepCallback(steps);
                    computingFinished();
                }
                break;
            }
            default: {
                //computing only from RUNNING, SYNCHRONIZING, SYNCHRONIZED possible.
                break;
            }
        }
    }

    virtual void computingFinished() override {
        routineFinished<DcpState::COMPUTING, DcpState::COMPUTED>(COMPUTING_FINISHED, COMPUTING_INTERRUPTED);
    }

    /**************************
    *  Realtime
    **************************/

    virtual void run(const int64_t startTime) override {
        if (running != NULL) {
            running->detach();
            delete running;
        }
        running = new std::thread(&DcpManagerSlave::startRealtime,
                                  this, startTime);
    }

    void startRealtime(int64_t startTime) {
        using namespace std::chrono;
        if (startTime == 0) {
            nextCommunication = time_point_cast<microseconds>(system_clock::now());
        } else {
            nextCommunication = time_point<system_clock, microseconds>(
                    seconds(startTime));
            std::this_thread::sleep_until(nextCommunication);
        }

        if(state == DcpState::SYNCHRONIZING){
            realtimeState = state;
            startRealtimeStep();
            synchronize();
        } else if(state == DcpState::RUNNING){
            realtimeState = state;
        }
    }

    void startRealtimeStep() {
        using namespace std::chrono;

        uint32_t steps = 1;

        switch (realtimeState) {
            case DcpState::RUNNING: {
                if (state == DcpState::RUNNING) {
                    mtxInput.lock();
                    mtxOutput.lock();

                    if (asynchronousCallback[DcpCallbackTypes::RUNNING_STEP]) {
                        std::thread t(runningStepCallback, steps);
                        t.detach();
                    } else {
                        runningStepCallback(steps);
                        std::thread t(&DcpManagerSlave::realtimeStepFinished, this);
                        t.detach();
                    }
                }
                break;
            }
            case DcpState::SYNCHRONIZING: {
                mtxInput.lock();
                mtxOutput.lock();

                if (asynchronousCallback[DcpCallbackTypes::SYNCHRONIZING_STEP]) {
                    std::thread t(synchronizingStepCallback, steps);
                    t.detach();
                } else {
                    synchronizingStepCallback(steps);
                    std::thread t(&DcpManagerSlave::realtimeStepFinished, this);
                    t.detach();
                }
                break;
            }
            case DcpState::SYNCHRONIZED: {
                mtxInput.lock();
                mtxOutput.lock();

                if (asynchronousCallback[DcpCallbackTypes::SYNCHRONIZED_STEP]) {
                    std::thread t(synchronizedStepCallback, steps);
                    t.detach();
                } else {
                    synchronizedStepCallback(steps);
                    std::thread t(&DcpManagerSlave::realtimeStepFinished, this);
                    t.detach();
                }
                break;
            }
            default: {
                //realtime routine is only in RUNNING, SYNCHRONIZING, SYNCHRONIZED active.
                break;
            }
        }
    }

    virtual void realtimeStepFinished() override {
        mtxInput.unlock();
        uint32_t steps = 1;

        for (std::tuple<std::vector<uint16_t>, uint32_t, uint32_t> el : outputCounter) {
            std::get<1>(el) -= steps;
            if (std::get<1>(el) == 0) {
                std::get<1>(el) = 0 + std::get<2>(el);
                sendOutputs(std::get<0>(el));
            }
        }
        mtxOutput.unlock();

        nextCommunication += std::chrono::microseconds((int64_t) ((((double) numerator) / ((double) denominator)
                                                                   * ((double) steps)) * 1000000.0));
        //steps = newSteps;
        std::this_thread::sleep_until(nextCommunication);
        startRealtimeStep();
    }


    virtual void sendOutputs(std::vector<dataId_t> dataIdsToSend) override {
        for (dataId_t dataId  : dataIdsToSend) {
            if (outputAssignment.find(dataId) == outputAssignment.end()) {
                continue;
            }
            DcpPduDatInputOutput *pdu = outputBuffer[dataId];
            size_t offset = 0;

            std::map<uint16_t, uint64_t> vrsToSend = outputAssignment[dataId];
            for (uint16_t i = 0; i < vrsToSend.size(); i++) {
                uint64_t vr = vrsToSend[i];

                offset += values[vr]->serialize(pdu->getPayload(), offset);

            }
            pdu->getPduSeqId() = getNextDataSeqNum(pdu->getDataId());
            pdu->setPduSize(offset + 5);

            driver.send(*pdu);
        }
    }

    virtual void updateLastStateRequest() override {
        mtxHeartbeat.lock();
        lastStateRequest = std::chrono::time_point_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now());
        mtxHeartbeat.unlock();
    }

    /**************************
    *  Stop
    **************************/

    //In stopping there can not be another legal message. So no executionNr is needed
    void startStopping(std::thread *toStop) {
#ifdef DEBUG
        Log(STOPPING_STARTED);
#endif
        if (asynchronousCallback[DcpCallbackTypes::STOP]) {
            std::thread t(stopCallback, toStop);
            t.detach();
        } else {
            stopCallback(toStop);
            stoppingFinished();
        }
    }

    virtual void stoppingFinished() override {
#ifdef DEBUG
        Log(STOPPING_FINISHED);
#endif
        state = DcpState::STOPPED;
        notifyStateChange();
    }

    /**************************
    *  Heartbeat
    **************************/

    void startHeartbeat() override {
        if (slaveDescription.CapabilityFlags.canMonitorHeartbeat) {
            if (slaveDescription.Heartbeat == nullptr) {
#ifdef DEBUG
                Log(HEARTBEAT_IGNORED);
#endif
                return;
            }
            if (heartbeat != NULL) {
                heartbeat->detach();
                delete heartbeat;
            }
            heartbeat = new std::thread(&DcpManagerSlave::heartBeat, this);
        }
    }

    void heartBeat() {
        using namespace std::chrono;
#ifdef DEBUG
        Log(HEARTBEAT_STARTED);
#endif
        MaximumPeriodicInterval_t &interval = slaveDescription.Heartbeat->MaximumPeriodicInterval;
        uint32_t numerator = interval.numerator;
        uint32_t denominator = interval.denominator;
        time_point<system_clock, microseconds> now;
        mtxHeartbeat.lock();
        lastStateRequest = time_point_cast<microseconds>(system_clock::now());
        mtxHeartbeat.unlock();
        while (!(state == DcpState::ALIVE || state == DcpState::ERROR_HANDLING || state == DcpState::ERROR_RESOLVED)) {
            now = time_point_cast<microseconds>(system_clock::now());

            mtxHeartbeat.lock();
            auto between = duration_cast<microseconds>(now - lastStateRequest).count();
            mtxHeartbeat.unlock();
            if (between * denominator >= numerator * 1000000) {
#ifdef DEBUG
                Log(HEARTBEAT_MISSED, to_string(now), to_string(lastStateRequest));
#endif
                errorCode = DcpError::PROTOCOL_ERROR_HEARTBEAT_MISSED;
                gotoErrorHandling();
                gotoErrorResolved();
            } else {
                mtxHeartbeat.lock();
                time_point<system_clock, microseconds> nextCheck = lastStateRequest + microseconds(
                        (int64_t) (1000000 * ((double) numerator) / ((double) denominator)));
                mtxHeartbeat.unlock();
                std::this_thread::sleep_until(nextCheck);
            }
        }
#ifdef DEBUG
        Log(HEARTBEAT_STOPPED);
#endif
    }


};


#endif //ACOSAR_DRIVERMANAGERSLAVE_H
