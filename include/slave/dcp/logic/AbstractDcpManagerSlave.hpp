/*
 * AciDriverManager.h
 *
 *  Created on: 17.02.2017
 *      Author: kater
 */

#ifndef ACI_LOGIC_DRIVERMANAGERSLAVE_H_
#define ACI_LOGIC_DRIVERMANAGERSLAVE_H_


#include <chrono>
#include <map>
#include <utility>
#include <vector>
#include <set>
#include <mutex>
#include <thread>
#include <algorithm>
#include <condition_variable>
#include <stdexcept>
#include <iterator>
#include <stdio.h>
#include <string.h>

#include "dcp/model/DcpTypes.hpp"
#include "dcp/model/DcpPdu.hpp"
#include "dcp/model/DcpConstants.hpp"
#include "dcp/model/MultiDimValue.hpp"

#include "dcp/helper/Helper.hpp"
#include "dcp/helper/DcpSlaveDescriptionHelper.hpp"

#include "dcp/logic/AbstractDcpManager.hpp"
#include "dcp/xml/DcpSlaveDescriptionElements.hpp"

#ifdef ERROR
/* windows has already an error define */
#undef ERROR_LI
#endif


#define ALLOWED_INPUT_OUTPUT(input, b0, b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11) \
case DcpDataType::input: \
    switch(output){ \
    case DcpDataType::uint8: \
        return b0;\
    case DcpDataType::uint16:\
        return b1;\
    case DcpDataType::uint32:\
        return b2;\
    case DcpDataType::uint64:\
        return b3;\
    case DcpDataType::int8:\
        return b4;\
    case DcpDataType::int16:\
        return b5;\
    case DcpDataType::int32:\
        return b6;\
    case DcpDataType::int64:\
        return b7;\
    case DcpDataType::float32:\
        return b8;\
    case DcpDataType::float64:\
        return b9;\
    case DcpDataType::string:\
        return b10;\
    case DcpDataType::binary:\
        return b11;\
    }

struct Payload {
    uint8_t *payload;
    size_t size;
};


/**
 * Aci Logic for an slave
 *
 * @author Christian Kater
 */
class AbstractDcpManagerSlave : public AbstractDcpManager {
public:


    ~AbstractDcpManagerSlave() {
        for (auto const &entry : values) {
            delete entry.second;
        }
        for (auto const &entry : updatedStructure) {
            delete entry.second;
        }
    }

    void receive(DcpPdu &msg) override {

        if (!checkForError(msg)) {
            return;
        }


        switch (msg.getTypeId()) {
            case DcpPduType::STC_prepare: {
                state = DcpState::PREPARING;
                driver.prepare();
                prepare();
                break;
            }
            case DcpPduType::STC_configure: {
                state = DcpState::CONFIGURING;
                notifyStateChange();
                for (auto const &ent : outputAssignment) {
                    uint16_t dataId = ent.first;
#ifdef DEBUG
                    Log(DATA_BUFFER_CREATED, ent.first, bufferSize);
#endif
                    DcpPduDatInputOutput *pdu = new DcpPduDatInputOutput(0, ent.first, bufferSize);
                    outputBuffer[ent.first] = pdu;


                }

                std::map<uint32_t, std::vector<uint16_t>> stepsToDataId;
                for (dataId_t dataId : runningScope) {
                    stepsToDataId[steps[dataId]].push_back(dataId);
                }

                outputCounter.clear();
                for (auto &entry : stepsToDataId) {
                    outputCounter.push_back(std::make_tuple(entry.second, entry.first, entry.first));
                }
                driver.configure();
                configure();
                break;
            }
            case DcpPduType::STC_initialize: {
                state = DcpState::INITIALIZING;
                notifyStateChange();
                initialize();
                break;
            }
            case DcpPduType::STC_run: {
                if (state == DcpState::CONFIGURED) {
                    state = DcpState::SYNCHRONIZING;
                } else if (state == DcpState::SYNCHRONIZED) {
                    state = DcpState::RUNNING;
                }
                notifyStateChange();
                DcpPduRun &_run = static_cast<DcpPduRun &>(msg);
                if (opMode != DcpOpMode::NRT) {
                    setRuntime(_run.getStartTime());
                    run(_run.getStartTime());
                }
                break;
            }
            case DcpPduType::STC_do_step: {
                runLastExitPoint = state;
                state = DcpState::COMPUTING;
                notifyStateChange();
                DcpPduDoStep &doStepPdu = static_cast<DcpPduDoStep &>(msg);
                doStep(doStepPdu.getSteps());
                break;
            }
            case DcpPduType::STC_stop: {
                driver.stop();
                stop();
                break;
            }
            case DcpPduType::STC_reset: {
                errorCode = DcpError::NONE;
                state = DcpState::CONFIGURATION;
                notifyStateChange();
                startHeartbeat();
                break;
            }
            case DcpPduType::INF_state: {
                updateLastStateRequest();
                DcpPduBasic &basic = static_cast<DcpPduBasic &>(msg);

                DcpPduStateAck stateAck = {state == DcpState::ALIVE ? basic.getReceiver() : dcpId, basic.getPduSeqId(),
                                           state};
                driver.send(stateAck);
                break;
            }
            case DcpPduType::INF_error: {
                DcpPduBasic &basic = static_cast<DcpPduBasic &>(msg);

                DcpPduNack errorAck = {DcpPduType::RSP_error_ack,
                                       state == DcpState::ALIVE ? basic.getReceiver() : dcpId,
                                       basic.getPduSeqId(), errorCode};
                driver.send(errorAck);
                break;
            }
            case DcpPduType::CFG_set_time_res: {
                DcpPduSetTimeRes timeRes = static_cast<DcpPduSetTimeRes &>(msg);
                setTimeRes(timeRes.getNumerator(), timeRes.getDenominator());
                break;
            }
            case DcpPduType::CFG_set_steps: {
                DcpPduSetSteps &stepsMSG = static_cast<DcpPduSetSteps &>(msg);
                setSteps(stepsMSG.getDataId(), stepsMSG.getSteps());
                break;
            }
            case DcpPduType::STC_register: {
                DcpPduRegister registerPdu = static_cast<DcpPduRegister &>(msg);
                setOperationInformation(registerPdu.getReceiver(), registerPdu.getOpMode());
                seqAtRegister = registerPdu.getPduSeqId();
                segNumsIn[masterId] = seqAtRegister;
#ifdef DEBUG
                Log(NEXT_SEQUENCE_ID_FROM_MASTER, (uint16_t) (segNumsIn[masterId] + 1));
#endif
                driver.registerSuccessfull();
                state = DcpState::CONFIGURATION;
                notifyStateChange();
                startHeartbeat();
                break;
            }
            case DcpPduType::STC_deregister: {
                opMode = DcpOpMode::RT;
                clearConfig();
                state = DcpState::ALIVE;
                notifyStateChange();
                dcpId = 0;
                driver.disconnect();
                break;
            }
            case DcpPduType::CFG_config_input: {
                DcpPduConfigInput &inputConfig = static_cast<DcpPduConfigInput &>(msg);
                configuredInPos[inputConfig.getDataId()].push_back(inputConfig.getPos());
                inputAssignment[inputConfig.getDataId()][inputConfig.getPos()] = std::make_pair(
                        inputConfig.getTargetVr(),
                        inputConfig.getSourceDataType());

#ifdef DEBUG
                Log(NEW_INPUT_CONFIG, inputConfig.getTargetVr(), inputConfig.getSourceDataType(),
                    inputConfig.getDataId());
#endif
                break;
            }

            case DcpPduType::CFG_config_output: {
                DcpPduConfigOutput &outputConfig = static_cast<DcpPduConfigOutput &>(msg);
                if (outputAssignment.find(outputConfig.getDataId()) == outputAssignment.end()) {
                    outputAssignment.insert(
                            std::make_pair(outputConfig.getDataId(),
                                           std::map<uint16_t, uint64_t>()));
                }
                configuredOutPos[outputConfig.getDataId()].push_back(outputConfig.getPos());


                outputAssignment[outputConfig.getDataId()][outputConfig.getPos()] = outputConfig.getSourceVr();


#ifdef DEBUG
                Log(NEW_OUTPUT_CONFIG, outputConfig.getSourceVr(), outputConfig.getDataId());
#endif

                break;

            }
            case DcpPduType::CFG_config_clear: {
                clearConfig();
                segNumsIn[masterId] = seqAtRegister + 1;
#ifdef DEBUG
                Log(NEXT_SEQUENCE_ID_FROM_MASTER, segNumsIn[masterId]);
#endif
                break;
            }
            case DcpPduType::STC_send_outputs: {
                if (state == DcpState::INITIALIZED) {
                    state = DcpState::SENDING_I;
                    notifyStateChange();
                    sendOutputs(initializationScope);
                    state = DcpState::CONFIGURED;
                } else if (state == DcpState::COMPUTED) {
                    state = DcpState::SENDING_D;
                    notifyStateChange();
                    sendOutputs(runningScope);
                    state = runLastExitPoint;
                }
                notifyStateChange();
                break;
            }
            case DcpPduType::DAT_input_output: {
                DcpPduDatInputOutput &data = static_cast<DcpPduDatInputOutput &>(msg);
                int offset = 0;
                std::map<uint16_t, std::pair<uint64_t, DcpDataType>> vrsToReceive =
                        inputAssignment[data.getDataId()];
                for (uint16_t i = 0; i < vrsToReceive.size(); i++) {
                    std::pair<uint64_t, DcpDataType> p = vrsToReceive[i];
                    uint64_t valueReference = p.first;
                    DcpDataType sourceDataType = p.second;

                    offset += values[valueReference]->update(data.getPayload(), offset, sourceDataType);
#ifdef DEBUG
                    Log(ASSIGNED_INPUT, valueReference, sourceDataType,
                        slavedescription::getDataType(slaveDescription, valueReference));
#endif
                }
                break;
            }
            case DcpPduType::DAT_parameter: {
                DcpPduDatParameter &param = static_cast<DcpPduDatParameter &>(msg);

                int offset = 0;
                std::map<uint16_t, std::pair<uint64_t, DcpDataType>> vrsToReceive =
                        paramAssignment[param.getParamId()];
                for (uint16_t i = 0; i < vrsToReceive.size(); i++) {
                    std::pair<uint64_t, DcpDataType> p = vrsToReceive[i];
                    uint64_t valueReference = p.first;
                    DcpDataType sourceDataType = p.second;

                    if (slavedescription::structuralParameterExists(slaveDescription, valueReference)) {
                        offset += values[valueReference]->update(param.getConfiguration(), offset, sourceDataType);

                        size_t value;
                        switch (slavedescription::getDataType(slaveDescription, valueReference)) {
                            case DcpDataType::uint8:
                                value = *values[valueReference]->getValue<int8_t *>();
                                break;
                            case DcpDataType::uint16:
                                value = *values[valueReference]->getValue<int16_t *>();
                                break;
                            case DcpDataType::uint32:
                                value = *values[valueReference]->getValue<int32_t *>();
                                break;
                            case DcpDataType::uint64:
                                value = *values[valueReference]->getValue<int64_t *>();
                                break;
                            default:
                                //only uint datatypes are allowed for structual parameters
                                break;
                        }
                        updateStructualDependencies(valueReference, value);
                    } else {
                        checkForUpdatedStructure(valueReference);
                        offset += values[valueReference]->update(param.getConfiguration(), offset, sourceDataType);
                    }
                }
                break;
            }
            case DcpPduType::CFG_set_parameter: {
                DcpPduSetParameter &parameter = static_cast<DcpPduSetParameter &>(msg);
                uint64_t &valueReference = parameter.getParameterVr();
                if (slavedescription::structuralParameterExists(slaveDescription, valueReference)) {
                    values[valueReference]->update(parameter.getConfiguration(), 0,
                                                   slavedescription::getDataType(slaveDescription, valueReference));

                    size_t value;
                    switch (slavedescription::getDataType(slaveDescription, valueReference)) {
                        case DcpDataType::uint8:
                            value = *values[valueReference]->getValue<int8_t *>();
                            break;
                        case DcpDataType::uint16:
                            value = *values[valueReference]->getValue<int16_t *>();
                            break;
                        case DcpDataType::uint32:
                            value = *values[valueReference]->getValue<int32_t *>();
                            break;
                        case DcpDataType::uint64:
                            value = *values[valueReference]->getValue<int64_t *>();
                            break;
                        default:
                            //only uint datatypes are allowed for structual parameters
                            break;
                    }
                    updateStructualDependencies(valueReference, value);
                } else {
                    checkForUpdatedStructure(parameter.getParameterVr());
                    values[valueReference]->update(parameter.getConfiguration(), 0,
                                                   slavedescription::getDataType(slaveDescription, valueReference));
                }
                break;
            }
            case DcpPduType::CFG_config_tunable_parameter: {
                DcpPduConfigTunableParameter &paramConfig = static_cast<DcpPduConfigTunableParameter &>(msg);
                configuredParamPos[paramConfig.getParamId()].push_back(paramConfig.getPos());
                paramAssignment[paramConfig.getParamId()][paramConfig.getPos()] = std::make_pair(
                        paramConfig.getParameterVr(),
                        paramConfig.getSourceDataType());
#ifdef DEBUG
                Log(NEW_TUNABLE_CONFIG, paramConfig.getParameterVr(), paramConfig.getSourceDataType(),
                    paramConfig.getParamId());
#endif
                break;

            }
            case DcpPduType::CFG_set_scope: {
                DcpPduSetScope &setScope = static_cast<DcpPduSetScope &>(msg);
                const DcpScope scope = setScope.getScope();
                uint16_t dataId = setScope.getDataId();

                runningScope.erase(std::remove(runningScope.begin(), runningScope.end(), dataId), runningScope.end());
                initializationScope.erase(std::remove(initializationScope.begin(), initializationScope.end(), dataId),
                                          initializationScope.end());


                if (scope == DcpScope::RUNNING || scope == DcpScope::CONFIGURED_INITIALIZING_INITIALIZED_RUNNING) {
                    runningScope.push_back(dataId);
                }
                if (scope == DcpScope::CONFIGURED_INITIALIZING_INITIALIZED ||
                    scope == DcpScope::CONFIGURED_INITIALIZING_INITIALIZED_RUNNING) {
                    initializationScope.push_back(dataId);
                }

                break;

            }
            case DcpPduType::CFG_set_source_network_information: {
                DcpPduSetNetworkInformation &netInf = static_cast<DcpPduSetNetworkInformation &>(msg);
                sourceNetworkConfigured.insert(netInf.getDataId());
                driver.setSourceNetworkInformation(netInf.getDataId(), netInf.getNetworkInformation());
                break;
            }
            case DcpPduType::CFG_set_target_network_information: {
                DcpPduSetNetworkInformation &netInf = static_cast<DcpPduSetNetworkInformation &>(msg);
                targetNetworkConfigured.insert(netInf.getDataId());
                driver.setTargetNetworkInformation(netInf.getDataId(), netInf.getNetworkInformation());
                break;
            }
            case DcpPduType::CFG_set_param_network_information: {
                DcpPduSetParamNetworkInformation &netInf = static_cast<DcpPduSetParamNetworkInformation &>(msg);
                paramNetworkConfigured.insert(netInf.getParamId());
                driver.setParamNetworkInformation(netInf.getParamId(), netInf.getNetworkInformation());
                break;
            }
            case DcpPduType::INF_log: {
                DcpPduInfLog &log = static_cast<DcpPduInfLog &>(msg);
                size_t currentSize = 0;
                uint8_t numLogs = 0;
                DcpPduLogAck logAck = {dcpId, log.getPduSeqId(), logRspBuffer, bufferSize};
                std::vector<Payload> &logs = logBuffer[log.getLogCategory()];
                while (numLogs < log.getLogMaxNum() && logs.size() > 0 &&
                       (logs.front().size + currentSize) < bufferSize) {
                    const Payload &current = logs.front();
                    memcpy(logAck.getPayload() + currentSize, current.payload, current.size);
                    delete[] current.payload;
                    logs.erase(logs.begin());
                    currentSize += current.size;
                    numLogs++;
                }
                logAck.setPduSize(currentSize);
                driver.send(logAck);
                break;
            }
            case DcpPduType::CFG_set_logging: {
                DcpPduSetLogging &logging = static_cast<DcpPduSetLogging &>(msg);
                uint8_t categoryStart = 1;
                uint8_t categoryEnd = 255;
                std::map<logCategory_t, std::map<DcpLogLevel, bool>> &map = logOnRequest;
                std::map<logCategory_t, std::map<DcpLogLevel, bool>> &notMap = logOnNotification;
                if (logging.getLogCategory() != 0) {
                    categoryStart = logging.getLogCategory();
                    categoryEnd = logging.getLogCategory();
                }
                if (logging.getLogMode() == DcpLogMode::LOG_ON_NOTIFICATION) {
                    map = logOnNotification;
                    notMap = logOnRequest;
                }
                for (int i = categoryStart; i <= categoryEnd; i++) {
                    map[i][logging.getLogLevel()] = true;
                    notMap[i][logging.getLogLevel()] = false;
                }
                break;
            }
        }
    }

    virtual void reportError(const DcpError errorCode) override {
        if (asynchronousCallback[DcpCallbackTypes::ERROR_LI]) {
            std::thread t(errorListener, errorCode);
            t.detach();
        } else {
            errorListener(errorCode);
        }
    }

    /**
     * Stops the slave if possible
     * @return True means slave switched to state STOPPING, false means no action done
     */
    virtual bool stop() = 0;

    /**
     * Go to state ERRORHANDLING immediatly.
     * The master will be informed by NTF_state_changed.
     */
    void gotoErrorHandling() {
        state = DcpState::ERROR_HANDLING;
        notifyStateChange();
    }

    /**
     * Set the error code
     * @param errorCode New error code
     */
    void setError(DcpError errorCode) {
        this->errorCode = errorCode;
    }

    /**
     * Go to state ERRORRESOLVED, if current state is ERRORHANDLING.
     * Indicate that the error is resolved.
     * The master will be informed by NTF_state_changed.
     */
    void gotoErrorResolved() {
        if (state == DcpState::ERROR_HANDLING) {
            state = DcpState::ERROR_RESOLVED;
            notifyStateChange();
        }
    }

    /**
     * Get a pointer to a input
     * @tparam T Data type of the input. Has to be a pointer. E. g. uint16_t* for uint16.
     * @param vr Value reference of the input
     * @return Pointer to value of the corresponding input.
     */
    template<typename T>
    T getInput(uint64_t vr) {
        //toDo check if T belongs to vr
        return values[vr]->getValue<T>();
    }

    /**
     * Get a pointer to an output
     * @tparam T Data type of the output. Has to be a pointer. E. g. uint16_t* for uint16.
     * @param vr Value reference of the output
     * @return Pointer to value of the corresponding output.
     */
    template<typename T>
    T getOutput(uint64_t vr) {
        //toDo check if T belongs to vr
        return values[vr]->getValue<T>();
    }

    /**
     * Get a pointer to a parameter
     * @tparam T Data type of the parameter. Has to be a pointer. E. g. uint16_t* for uint16.
     * @param vr Value reference of the parameter
     * @return Pointer to value of the corresponding parameter.
     */
    template<typename T>
    T getParameter(uint64_t vr) {
        //toDo check if T belongs to vr
        return values[vr]->getValue<T>();
    }


    /**
     * Set the callback for action followed by a STC_prepare PDU
     * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param prepareCallback function which will be called after the event occurs
     *
     * @post IF ftype == ASYNC: prepareFinished needs to be called after finishing preparing action
     */
    template<FunctionType ftype>
    void setPrepareCallback(std::function<void()> prepareCallback) {
        this->prepareCallback = std::move(prepareCallback);
        asynchronousCallback[DcpCallbackTypes::PREPARE] = ftype == ASYNC;
    }

    /**
	 * Set the callback for action followed by a STC_configure PDU
	 * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
	 * @param prepareCallback function which will be called after the event occurs
	 *
	 * @post IF ftype == ASYNC: configureFinished needs to be called after finishing configuring action
	 */
    template<FunctionType ftype>
    void setConfigureCallback(std::function<void()> configureCallback) {
        this->configureCallback = std::move(configureCallback);
        asynchronousCallback[DcpCallbackTypes::CONFIGURE] = ftype == ASYNC;
    }

    /**
    * Set the callback for a step in state SYNCHRONIZING and operation mode NRT
    * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param prepareCallback function which will be called after the event occurs
    *
    * @post IF ftype == ASYNC: simulationStepFinished needs to be called after finishing configuring action
    */
    template<FunctionType ftype>
    void setSynchronizingNRTStepCallback(std::function<void(uint64_t steps)> synchronizingNRTStepCallback) {
        this->synchronizingNRTStepCallback = std::move(synchronizingNRTStepCallback);
        asynchronousCallback[DcpCallbackTypes::SYNCHRONIZING_NRT_STEP] = ftype == ASYNC;
    }

    /**
   * Set the callback for a step in state SYNCHRONIZED and operation mode NRT
   * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
   * @param prepareCallback function which will be called after the event occurs
   *
   * @post IF ftype == ASYNC: simulationStepFinished needs to be called after finishing configuring action
   */
    template<FunctionType ftype>
    void setSynchronizedNRTStepCallback(std::function<void(uint64_t steps)> synchronizedNRTStepCallback) {
        this->synchronizedNRTStepCallback = std::move(synchronizedNRTStepCallback);
        asynchronousCallback[DcpCallbackTypes::SYNCHRONIZED_NRT_STEP] = ftype == ASYNC;
    }

    /**
    * Set the callback for a step in state RUNNING and operation mode NRT
    * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param prepareCallback function which will be called after the event occurs
    *
    * @post IF ftype == ASYNC: simulationStepFinished needs to be called after finishing configuring action
    */
    template<FunctionType ftype>
    void setRunningNRTStepCallback(std::function<void(uint64_t steps)> runningNRTStepCallback) {
        this->runningNRTStepCallback = std::move(runningNRTStepCallback);
        asynchronousCallback[DcpCallbackTypes::RUNNING_NRT_STEP] = ftype == ASYNC;
    }

    /**
    * Set the callback for a realtime step in state SYNCHRONIZING and operation mode SRT or HRT
    * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param prepareCallback function which will be called after the event occurs
    *
    * @post IF ftype == ASYNC: simulationStepFinished needs to be called after finishing configuring action
    */
    template<FunctionType ftype>
    void setSynchronizingStepCallback(std::function<void(uint64_t steps)> synchronizingStepCallback) {
        this->synchronizingStepCallback = std::move(synchronizingStepCallback);
        asynchronousCallback[DcpCallbackTypes::SYNCHRONIZING_STEP] = ftype == ASYNC;
    }


    /**
    * Set the callback for a realtime step in state SYNCHRONIZED and operation mode SRT or HRT
    * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param prepareCallback function which will be called after the event occurs
    *
    * @post IF ftype == ASYNC: simulationStepFinished needs to be called after finishing configuring action
    */
    template<FunctionType ftype>
    void setSynchronizedStepCallback(std::function<void(uint64_t steps)> synchronizedStepCallback) {
        this->synchronizedStepCallback = std::move(synchronizedStepCallback);
        asynchronousCallback[DcpCallbackTypes::SYNCHRONIZED_STEP] = ftype == ASYNC;
    }

    /**
    * Set the callback for a realtime step in state RUNNING and operation mode SRT or HRT
    * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param prepareCallback function which will be called after the event occurs
    *
    * @post IF ftype == ASYNC: simulationStepFinished needs to be called after finishing configuring action
    */
    template<FunctionType ftype>
    void setRunningStepCallback(std::function<void(uint64_t steps)> runningStepCallback) {
        this->runningStepCallback = std::move(runningStepCallback);
        asynchronousCallback[DcpCallbackTypes::RUNNING_STEP] = ftype == ASYNC;
    }


    /**
     * Set the callback for action followed by a STC_stop PDU
     * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param prepareCallback function which will be called after the event occurs
     *
     * @post IF ftype == ASYNC: stopFinished needs to be called after finishing preparing action
     */
    template<FunctionType ftype>
    void setStopCallback(std::function<void(std::thread *runningRoutine)> stopCallback) {
        this->stopCallback = std::move(stopCallback);
        asynchronousCallback[DcpCallbackTypes::STOP] = ftype == ASYNC;
    }


    /**
     * Set the callback for action followed by a STC_initialize PDU
     * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param prepareCallback function which will be called after the event occurs
     *
     * @post IF ftype == ASYNC: initializeFinished needs to be called after finishing preparing action
     */
    template<FunctionType ftype>
    void setInitializeCallback(std::function<void()> initializeCallback) {
        this->initializeCallback = std::move(initializeCallback);
        asynchronousCallback[DcpCallbackTypes::INITIALIZE] = ftype == ASYNC;
    }

    /**
     * Set the callback for action followed by a STC_synchronize PDU
     * @tparam ftype ftype SYNC means calling the given function is blocking, ASYNC means non blocking
     * @param prepareCallback function which will be called after the event occurs
     *
     * @post IF ftype == ASYNC: synchronizeFinished needs to be called after finishing preparing action
     */
    template<FunctionType ftype>
    void setSynchronizeCallback(std::function<void()> synchronizeCallback) {
        this->synchronizeCallback = std::move(synchronizeCallback);
        asynchronousCallback[DcpCallbackTypes::SYNCHRONIZE] = ftype == ASYNC;
    }

    /**
     * A simulation step is finished.
     *
     * @return True if calling is allowed and action was performed, otherwise False
     *
     * @pre A ASYNC callback for set{Synchronizing, Synchronized, Running}{"", NRT}Callback was called
     *
     */
    bool simulationStepFinished() {
        if (!asynchronousCallback[DcpCallbackTypes::SYNCHRONIZING_STEP] ||
            !asynchronousCallback[DcpCallbackTypes::SYNCHRONIZED_STEP] ||
            !asynchronousCallback[DcpCallbackTypes::RUNNING_STEP] ||
            !asynchronousCallback[DcpCallbackTypes::SYNCHRONIZING_NRT_STEP] ||
            !asynchronousCallback[DcpCallbackTypes::SYNCHRONIZED_NRT_STEP] ||
            !asynchronousCallback[DcpCallbackTypes::RUNNING_NRT_STEP]) {
            return false;
        }

        if (opMode == DcpOpMode::NRT) {
            computingFinished();
        } else {
            realtimeStepFinished();
        }
        return true;
    }

    /**
     * Preparing action is finished.
     * @return True if calling is allowed and action was performed, otherwise False
     *
     * @pre A ASYNC callback for setPrepareCallback was called
     */
    bool prepareFinished() {
        if (!asynchronousCallback[DcpCallbackTypes::PREPARE]) {
            return false;
        }
        preparingFinished();
        return true;
    }

    /**
    * Configure action is finished.
    * @return True if calling is allowed and action was performed, otherwise False
    *
    * @pre A ASYNC callback for setConfigureCallback was called
    */
    bool configureFinished() {
        if (!asynchronousCallback[DcpCallbackTypes::PREPARE]) {
            return false;
        }
        configuringFinished();
        return true;
    }

    /**
   * Stop action is finished.
   * @return True if calling is allowed and action was performed, otherwise False
   *
   * @pre A ASYNC callback for setStopCallback was called
   */
    bool stopFinished() {
        if (!asynchronousCallback[DcpCallbackTypes::STOP]) {
            return false;
        }
        stoppingFinished();
        return true;
    }

    /**
    * Initialize action is finished.
    * @return True if calling is allowed and action was performed, otherwise False
    *
    * @pre A ASYNC callback for setInitializeCallback was called
    */
    bool initializeFinished() {
        if (!asynchronousCallback[DcpCallbackTypes::INITIALIZE]) {
            return false;
        }
        initializingFinished();
        return true;
    }

    /**
    * Synchronize action is finished.
    * @return True if calling is allowed and action was performed, otherwise False
    *
    * @pre A ASYNC callback for setSynchronizeCallback was called
    */
    bool synchronizeFinished() {
        if (!asynchronousCallback[DcpCallbackTypes::SYNCHRONIZE]) {
            return false;
        }
        synchronizingFinished();
        return true;
    }

    /**
      * Set the listener for CFG_time_res PDUs
      * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
      * @param errorAckReceivedListener function which will be called after the event occurs
      */
    template<FunctionType ftype>
    void setTimeResListener(const std::function<void(uint32_t, uint32_t)> timeResListener) {
        this->timeResListener = std::move(timeResListener);
        asynchronousCallback[DcpCallbackTypes::TIME_RES] = ftype == ASYNC;
        if (timeResolutionSet) {
            setTimeRes(numerator, denominator);
        }
    }

    /**
      * Set the listener for CFG_steps PDUs
      * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
      * @param errorAckReceivedListener function which will be called after the event occurs
      */
    template<FunctionType ftype>
    void setStepsListener(const std::function<void(uint16_t, uint32_t)> stepsListener) {
        this->stepsListener = std::move(stepsListener);
        asynchronousCallback[DcpCallbackTypes::STEPS] = ftype == ASYNC;
    }

    /**
    * Set the listener for CFG_time_res PDUs
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setOperationInformationListener(const std::function<void(uint8_t, DcpOpMode)> operationInformationListener) {
        this->operationInformationListener = std::move(operationInformationListener);
        asynchronousCallback[DcpCallbackTypes::OPERATION_INFORMATION] = ftype == ASYNC;

    }

    /**
    * Set the listener for CFG_clear PDUs
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setConfigurationClearedListener(const std::function<void()> configurationClearedListener) {
        this->configurationClearedListener = std::move(configurationClearedListener);
        asynchronousCallback[DcpCallbackTypes::CONFIGURATION_CLEARED] = ftype == ASYNC;
    }

    /**
    * Set the listener for any error that may occur
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setErrorListener(const std::function<void(DcpError)> errorListener) {
        this->errorListener = errorListener;
        asynchronousCallback[DcpCallbackTypes::ERROR_LI] = ftype == ASYNC;
    }

    /**
    * Set the listener for missing control PDUs
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setMissingControlPduListener(const std::function<void()> missingControlPduListener) {
        this->missingControlPduListener = std::move(missingControlPduListener);
        asynchronousCallback[DcpCallbackTypes::CONTROL_MISSED] = ftype == ASYNC;
    }

    /**
    * Set the listener for missing DAT_input_output PDUs
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setMissingInputOutputPduListener(const std::function<void(uint16_t)> missingInputOutputPduListener) {
        this->missingInputOutputPduListener = std::move(missingInputOutputPduListener);
        asynchronousCallback[DcpCallbackTypes::IN_OUT_MISSED] = ftype == ASYNC;
    }

    /**
    * Set the listener for missing DAT_parameter PDUs
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setMissingParameterPduListener(const std::function<void(uint16_t)> missingParameterPduListener) {
        this->missingParameterPduListener = std::move(missingParameterPduListener);
        asynchronousCallback[DcpCallbackTypes::PARAM_MISSED] = ftype == ASYNC;
    }

    /**
    * Set the listener for STC_run PDUs
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setRuntimeListener(const std::function<void(int64_t)> runtimeListener) {
        this->runtimeListener = std::move(runtimeListener);
        asynchronousCallback[DcpCallbackTypes::RUNTIME] = ftype == ASYNC;
    }

    /**
    * Set the listener for state changes which was made by the dcp slave manager
    * @tparam ftype SYNC means calling the given function is blocking, ASYNC means non blocking
    * @param errorAckReceivedListener function which will be called after the event occurs
    */
    template<FunctionType ftype>
    void setStateChangedListener(const std::function<void(DcpState)> stateChangedListener) {
        this->stateChangedListener = std::move(stateChangedListener);
        asynchronousCallback[DcpCallbackTypes::STATE_CHANGED] = ftype == ASYNC;
    }

protected:

    const SlaveDescription_t slaveDescription;

    std::map<DcpState, std::map<DcpPduType, bool>> stateChangePossible;

    uint16_t seqAtRegister;

    DcpState state;
    DcpState realtimeState;
    DcpState runLastExitPoint;
    DcpError errorCode;

    // Acu-D constants
    DcpOpMode opMode;

    uint32_t numerator;
    uint32_t denominator;
    bool timeResolutionSet = false;
    bool timeResolutionFix = false;

    uint32_t fixNrtStep = 0;

    uint32_t bufferSize = 900;


    /*Logging*/
    std::map<logCategory_t, std::map<DcpLogLevel, bool>> logOnNotification;
    std::map<logCategory_t, std::map<DcpLogLevel, bool>> logOnRequest;

    std::map<logCategory_t, std::vector<Payload>> logBuffer;

    uint8_t *logRspBuffer = new uint8_t[bufferSize];

    /*Data Handling*/
    std::map<valueReference_t, MultiDimValue *> values;


    std::set<dataId_t> sourceNetworkConfigured;
    std::set<dataId_t> targetNetworkConfigured;
    std::set<paramId_t> paramNetworkConfigured;

    //Outputs
    std::map<dataId_t, DcpPduDatInputOutput *> outputBuffer;
    std::map<dataId_t, std::map<pos_t, valueReference_t>> outputAssignment;
    std::map<dataId_t, std::vector<pos_t>> configuredOutPos;

    std::vector<dataId_t> runningScope;
    std::vector<dataId_t> initializationScope;


    std::map<uint16_t, uint32_t> steps;
    //<List of data_ids, logBufferCurrent step counter, set steps>
    std::vector<std::tuple<std::vector<dataId_t>, steps_t, steps_t>> outputCounter;

    //Inputs
    std::map<dataId_t, std::map<pos_t, std::pair<valueReference_t, DcpDataType>>> inputAssignment;
    std::map<dataId_t, std::vector<pos_t>> configuredInPos;

    //Parameter
    std::map<paramId_t, std::vector<pos_t>> configuredParamPos;
    std::map<paramId_t, std::map<pos_t, std::pair<valueReference_t, DcpDataType>>> paramAssignment;

    //which sttructual parameter (valueReference) change which inputs/outputs/parameters (valueReference)
    std::map<valueReference_t, std::vector<std::pair<valueReference_t, size_t>>> structualDependencies;
    std::map<valueReference_t, MultiDimValue *> updatedStructure;


    /* Callbacks */
    std::map<DcpCallbackTypes, bool> asynchronousCallback;
    std::function<void()> configureCallback = []() {};
    std::function<void()> prepareCallback = []() {};
    std::function<void()> initializeCallback = []() {};
    std::function<void()> synchronizeCallback = []() {};
    std::function<void(uint64_t steps)> synchronizingStepCallback = [](uint64_t steps) {};
    std::function<void(uint64_t steps)> synchronizedStepCallback = [](uint64_t steps) {};
    std::function<void(uint64_t steps)> runningStepCallback = [](uint64_t steps) {};
    std::function<void(uint64_t steps)> synchronizingNRTStepCallback = [](uint64_t steps) {};
    std::function<void(uint64_t steps)> synchronizedNRTStepCallback = [](uint64_t steps) {};
    std::function<void(uint64_t steps)> runningNRTStepCallback = [](uint64_t steps) {};

    std::function<void(std::thread *runningRoutine)> stopCallback = [](std::thread *runningRoutine) {};

    std::function<void(uint32_t nominator,
                       uint32_t denominator)> timeResListener = [](uint32_t nominator,
                                                                   uint32_t denominator) {};

    std::function<void(uint16_t dataId, uint32_t steps)> stepsListener = [](uint16_t dataId, uint32_t steps) {};
    std::function<void(uint8_t acuId, DcpOpMode opMode)> operationInformationListener = [](uint8_t acuId,
                                                                                           DcpOpMode opMode) {};
    std::function<void(DcpError error)> errorListener = [](DcpError error) {};
    std::function<void()> configurationClearedListener = []() {};
    std::function<void()> missingControlPduListener = []() {};
    std::function<void(uint16_t dataId)> missingInputOutputPduListener = [](uint16_t dataId) {};
    std::function<void(uint16_t dataId)> missingParameterPduListener = [](uint16_t paramId) {};
    std::function<void(int64_t unixTimeStamp)> runtimeListener = [](int64_t unixTimeStamp) {};
    std::function<void(DcpState state)> stateChangedListener = [](DcpState state) {};

    uint8_t id = 162;
    const LogTemplate HEARTBEAT_IGNORED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_INFORMATION,
                                                      "In ADU-D Heartbeat is not defined, but canMonitorHeartBeat. Heartbeat will not be monitored.",
                                                      {});
    const LogTemplate HEARTBEAT_STARTED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_INFORMATION,
                                                      "Monitoring Heartbeat started.", {});
    const LogTemplate HEARTBEAT_STOPPED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_INFORMATION,
                                                      "Monitoring Heartbeat stopped.", {});
    const LogTemplate HEARTBEAT_MISSED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_FATAL,
                                                     "Heartbeat missed. Checked Time: %string. Last state request: %string.",
                                                     {DcpDataType::string, DcpDataType::string});
    const LogTemplate COMPUTING_STARTED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                      "Computing routine started.", {});
    const LogTemplate COMPUTING_FINISHED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                       "Computing routine finished.", {});
    const LogTemplate COMPUTING_INTERRUPTED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                          "Computing routine was interrupted. State was not changed.",
                                                          {});
    const LogTemplate STOPPING_STARTED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                     "Stopping routine started.", {});
    const LogTemplate STOPPING_FINISHED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                      "Stopping routine finished.", {});
    const LogTemplate CONFIGURING_STARTED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                        "Configuring routine started.", {});
    const LogTemplate CONFIGURING_FINISHED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                         "Configuring routine finished.", {});
    const LogTemplate CONFIGURING_INTERRUPTED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                            "Configuring routine was interrupted. State was not changed.",
                                                            {});
    const LogTemplate PREPARING_STARTED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                      "Preparing routine started.", {});
    const LogTemplate PREPARING_FINISHED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                       "Preparing routine finished.", {});
    const LogTemplate PREPARING_INTERRUPTED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                          "Preparing routine was interrupted. State was not changed.",
                                                          {});
    const LogTemplate INITIALIZING_STARTED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                         "Initializing routine started.", {});
    const LogTemplate INITIALIZING_FINISHED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                          "Initializing routine finished.", {});
    const LogTemplate INITIALIZING_INTERRUPTED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                             "Initializing routine was interrupted. State was not changed.",
                                                             {});
    const LogTemplate SYNCHRONIZING_STARTED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                          "Synchronizing routine started.", {});
    const LogTemplate SYNCHRONIZING_FINISHED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                           "Synchronizing routine finished.", {});
    const LogTemplate SYNCHRONIZING_INTERRUPTED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                              "Synchronizing routine was interrupted. State was not changed.",
                                                              {});
    const LogTemplate STATE_CHANGED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                  "DCP state has changed to %uint8", {DcpDataType::state});
    const LogTemplate DATA_BUFFER_CREATED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                        "Buffer for data id %uint16 with buffer size %uint32 created.",
                                                        {DcpDataType::uint16, DcpDataType::uint32});
    const LogTemplate NEXT_SEQUENCE_ID_FROM_MASTER = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                 DcpLogLevel::LVL_DEBUG,
                                                                 "Expected next pdu_seq_id from the master to be %uint16",
                                                                 {DcpDataType::uint16});
    const LogTemplate NEW_INPUT_CONFIG = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                     "Added input configuration for value reference %uint64 with source datatype %uint8 to data_id %uint16",
                                                     {DcpDataType::uint64, DcpDataType::uint8, DcpDataType::uint16});
    const LogTemplate NEW_OUTPUT_CONFIG = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                      "Added output configuration for value reference %uint64 to data_id %uint16",
                                                      {DcpDataType::uint64, DcpDataType::uint16});
    const LogTemplate NEW_TUNABLE_CONFIG = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                       "Added tunable parameter configuration for value reference %uint64 with source datatype %uint8 to data_id %uint16",
                                                       {DcpDataType::uint64, DcpDataType::uint8, DcpDataType::uint16});
    const LogTemplate STEP_SIZE_NOT_SET = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                      "Step size was not set for data id %uin16.",
                                                      {DcpDataType::uint16});
    const LogTemplate ASSIGNED_INPUT = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                   "Assigned input value for value reference %uint64 (%uint8 -> %uint8):",
                                                   {DcpDataType::uint64, DcpDataType::uint8,
                                                    DcpDataType::uint8});
    const LogTemplate NOT_SUPPORTED_RSP_ACK = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                          "It is not supported to receive RSP_ack as slave.", {});
    const LogTemplate NOT_SUPPORTED_RSP_NACK = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                           "It is not supported to receive RSP_nack as slave.", {});
    const LogTemplate NOT_SUPPORTED_RSP_STATE_ACK = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                DcpLogLevel::LVL_ERROR,
                                                                "It is not supported to receive RSP_state_ack as slave.",
                                                                {});
    const LogTemplate NOT_SUPPORTED_RSP_ERROR_ACK = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                DcpLogLevel::LVL_ERROR,
                                                                "It is not supported to receive RSP_error_ack as slave.",
                                                                {});

    const LogTemplate NOT_SUPPORTED_LOG_ON_REQUEST = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                 DcpLogLevel::LVL_DEBUG,
                                                                 "Log on request is not supported. ", {});
    const LogTemplate NOT_SUPPORTED_LOG_ON_NOTIFICATION = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                      DcpLogLevel::LVL_DEBUG,
                                                                      "Log on notification is not supported. ", {});


    const LogTemplate INVALID_TYPE_ID = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                    "A PDU with invalid type id (%uint8) received. PDU will be dropped.",
                                                    {DcpDataType::uint8});
    const LogTemplate INVALID_RECEIVER = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                     "A PDU with invalid receiver (%uint8) received. PDU will be dropped.",
                                                     {DcpDataType::uint8});
    const LogTemplate UNKNOWN_DATA_ID = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                    "A PDU with unknown data_id (%uint16) received. PDU will be dropped.",
                                                    {DcpDataType::uint16});
    const LogTemplate UNKNOWN_PARAM_ID = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                     "A PDU with unknown param id (%uint16) received. PDU will be dropped.",
                                                     {DcpDataType::uint16});
    const LogTemplate CTRL_PDU_MISSED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                    "A CTRL PDU was missed.", {});
    const LogTemplate IN_OUT_PDU_MISSED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                      "A Dat_input_output PDU was missed.", {});
    const LogTemplate PARAM_PDU_MISSED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                     "A Dat_parameter PDU was missed.", {});
    const LogTemplate OLD_CTRL_PDU_RECEIVED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                          "A old CTRL PDU was received. PDU will be dropped.", {});
    const LogTemplate OLD_IN_OUT_PDU_RECEIVED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                            "A old Dat_input_output PDU was received. PDU will be dropped.",
                                                            {});
    const LogTemplate OLD_PARAM_PDU_RECEIVED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                           "An old Dat_parameter PDU was received. PDU will be dropped.",
                                                           {});
    const LogTemplate INVALID_LENGTH = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                   "A PDU with invalid length received. %uint16 (received) != %uint16 (expected).",
                                                   {DcpDataType::uint16, DcpDataType::uint16});
    const LogTemplate ONLY_NRT = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                             "The received PDU is only allowed in NRT. Current op mode is %uint8.",
                                             {DcpDataType::uint8});
    const LogTemplate MSG_NOT_ALLOWED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                    "It is not allowed to receive %uint8 in state %uint8.",
                                                    {DcpDataType::uint8, DcpDataType::uint8});
    const LogTemplate INVALID_UUID = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                 "UUID does not match %string (slave) != %string (received).",
                                                 {DcpDataType::string, DcpDataType::string});
    const LogTemplate INVALID_OP_MODE = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                    "Operation Mode %uint8 is not supported.", {DcpDataType::uint8});
    const LogTemplate INVALID_MAJOR_VERSION = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                          "The requested major version (%uint8) is not supported by this slave (DCP %uint8.%uint8)",
                                                          {DcpDataType::uint8, DcpDataType::uint8, DcpDataType::uint8});
    const LogTemplate INVALID_MINOR_VERSION = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                          "The requested minor version (%uint8) is not supported by this slave (DCP %uint8.%uint8)",
                                                          {DcpDataType::uint8, DcpDataType::uint8, DcpDataType::uint8});
    const LogTemplate INCOMPLETE_CONFIGURATION_GAP_INPUT_POS = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                           DcpLogLevel::LVL_ERROR,
                                                                           "State change to Configuring is not possible. CFG_config_input with position %string was not received for data id %uint16, but max. pos was %uint16.",
                                                                           {DcpDataType::string, DcpDataType::uint16,
                                                                            DcpDataType::uint16});
    const LogTemplate INCOMPLETE_CONFIGURATION_GAP_OUTPUT_POS = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                            DcpLogLevel::LVL_ERROR,
                                                                            "State change to Configuring is not possible. CFG_config_output with position %string was not received for data id %uint16, but max. pos was %uint16.",
                                                                            {DcpDataType::string, DcpDataType::uint16,
                                                                             DcpDataType::uint16});
    const LogTemplate INCOMPLETE_CONFIGURATION_GAP_PARAM_POS = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                           DcpLogLevel::LVL_ERROR,
                                                                           "State change to Configuring is not possible. CFG_config_tunable_parameter with position %string was not received for data id %uint16, but max. pos was %uint16.",
                                                                           {DcpDataType::string, DcpDataType::uint16,
                                                                            DcpDataType::uint16});
    const LogTemplate INCOMPLETE_CONFIGURATION_STEPS = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                   DcpLogLevel::LVL_ERROR,
                                                                   "State change to Configuring is not possible. Steps was not set for data id %uint16.",
                                                                   {DcpDataType::uint16});
    const LogTemplate INCOMPLETE_CONFIGURATION_TIME_RESOLUTION = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                             DcpLogLevel::LVL_ERROR,
                                                                             "State change to Configuring is not possible. Time resolution was not set.",
                                                                             {});
    const LogTemplate INCOMPLETE_CONFIG_NW_INFO_INPUT = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                    DcpLogLevel::LVL_ERROR,
                                                                    "State change to Configuring is not possible. CFG_source_network_information was not set for data id %uint16.",
                                                                    {DcpDataType::uint16});
    const LogTemplate INCOMPLETE_CONFIG_NW_INFO_OUTPUT = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                     DcpLogLevel::LVL_ERROR,
                                                                     "State change to Configuring is not possible. CFG_target_network_information was not set for data id %uint16.",
                                                                     {DcpDataType::uint16});
    const LogTemplate INCOMPLETE_CONFIG_NW_INFO_TUNABLE = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                      DcpLogLevel::LVL_ERROR,
                                                                      "State change to Configuring is not possible. CFG_pram_network_information was not set for data id %uint16.",
                                                                      {DcpDataType::uint16});
    const LogTemplate INCOMPLETE_CONFIG_SCOPE = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                            "State change to Configuring is not possible. CFG_scope was not set for data id %uint16.",
                                                            {DcpDataType::uint16});

    const LogTemplate DATA_NOT_ALLOWED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                     "It is not allowed to receive Data PDUs in state %uint8. PDU will be dropped.",
                                                     {DcpDataType::uint8});

    const LogTemplate START_TIME = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                               "Simulation starts at %string.", {DcpDataType::string});
    const LogTemplate INVALID_START_TIME = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                       "Start time (%string) is before current time (%string)",
                                                       {DcpDataType::string, DcpDataType::string});

    const LogTemplate INVALID_STEPS = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                  "Step %uint32 is not supported. It is expected to be one of %string.",
                                                  {DcpDataType::uint32, DcpDataType::string});
    const LogTemplate NOT_SUPPORTED_VARIABLE_STEPS = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                 DcpLogLevel::LVL_ERROR,
                                                                 "Variable steps are not supported. Current steps is %uint32. Last was %uint32.",
                                                                 {DcpDataType::uint32, DcpDataType::uint32});
    const LogTemplate INVALID_LOG_CATEGORY = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                         "Log category %uint8 is not known by the slave.",
                                                         {DcpDataType::uint8});
    const LogTemplate INVALID_LOG_LEVEL = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                      "%uint8 is not a valid log level.", {DcpDataType::uint8});
    const LogTemplate INVALID_LOG_MODE = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                     "%uint8 is not a valid log mode.", {DcpDataType::uint8});

    const LogTemplate INVALID_SCOPE = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                  "%uint8 is not a valid scope.", {DcpDataType::uint8});


    const LogTemplate FIX_TIME_RESOLUTION = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                        "Setting time resolution not possible, it is fixed.", {});
    const LogTemplate INVALID_TIME_RESOLUTION = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                            "Time resolution %uint32/%uint32 is not supported. It is expected to be %string.",
                                                            {DcpDataType::uint32, DcpDataType::uint32,
                                                             DcpDataType::string});
    const LogTemplate INVALID_STEPS_OUTPUT = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                         "Step %uint32 is not supported by output with vr %uint64_t. It is expected to be one of %string.",
                                                         {DcpDataType::uint32, DcpDataType::uint64,
                                                          DcpDataType::string});
    const LogTemplate INVALID_VALUE_REFERENCE_INPUT = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                  DcpLogLevel::LVL_ERROR,
                                                                  "Value reference %uint64 is not part of the ACU or not a input.",
                                                                  {DcpDataType::uint64});
    const LogTemplate INVALID_VALUE_REFERENCE_OUTPUT = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                   DcpLogLevel::LVL_ERROR,
                                                                   "Value reference %uint64 is not part of the ACU or not a output.",
                                                                   {DcpDataType::uint64});
    const LogTemplate INVALID_VALUE_REFERENCE_PARAMETER = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE,
                                                                      DcpLogLevel::LVL_ERROR,
                                                                      "Value reference %uint64 is not part of the ACU or not a parameter.",
                                                                      {DcpDataType::uint64});
    const LogTemplate INVALID_SOURCE_DATA_TYPE = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                             "A PDU with invalid source datatype received. %uint8 (recieved) is not compatible to %uint8 (slave).",
                                                             {DcpDataType::uint8, DcpDataType::uint8});
    const LogTemplate INVALID_PORT = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                 "Port %uint16 is not supported. It is expected to be %string.",
                                                 {DcpDataType::uint16, DcpDataType::string});
    const LogTemplate INVALID_TRANSPORT_PROTOCOL = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                               "No %uint8 interface STC_configure..",
                                                               {DcpDataType::uint8});
    const LogTemplate CONFIGURATION_CLEARED = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                          "Configuration cleared.", {});
    const LogTemplate TIME_RES_SET = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                 "Time resolution was set to %uint32 / %uint32 s.",
                                                 {DcpDataType::uint32, DcpDataType::uint32});
    const LogTemplate STEP_SET = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                             "Steps for data_id %uint16 is set to %uint32.",
                                             {DcpDataType::uint16, DcpDataType::uint32});
    const LogTemplate DCP_ID_SET = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                               "DCP id is set to %uint8.", {DcpDataType::uint8});
    const LogTemplate OP_MODE_SET = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_DEBUG,
                                                "Operation mode is set to %uint8.", {DcpDataType::uint8});
    const LogTemplate INVALID_STATE_ID = LogTemplate(id++, LogCategory::DCP_LIB_SLAVE, DcpLogLevel::LVL_ERROR,
                                                     "State id (%uint8) in received state change PDU do not match current state (%uint8).",
                                                     {DcpDataType::uint8, DcpDataType::uint8});


    AbstractDcpManagerSlave(const SlaveDescription_t _slaveDescription) : slaveDescription(_slaveDescription) {
        this->errorCode = DcpError::NONE;
        this->state = DcpState::ALIVE;
        this->masterId = 0;
        this->opMode = DcpOpMode::RT;

        this->seqAtRegister = 0;
        this->numerator = 0;
        this->denominator = 0;

        //define possible state changes
        stateChangePossible[DcpState::ALIVE][DcpPduType::STC_register] = true;
        stateChangePossible[DcpState::ALIVE][DcpPduType::INF_state] = true;

        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::STC_deregister] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::STC_prepare] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_set_steps] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_set_time_res] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_config_input] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_config_output] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_config_clear] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_set_target_network_information] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_set_source_network_information] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_config_tunable_parameter] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_set_parameter] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_set_param_network_information] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_set_logging] = true;
        stateChangePossible[DcpState::CONFIGURATION][DcpPduType::CFG_set_scope] = true;

        stateChangePossible[DcpState::PREPARING][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::PREPARING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::PREPARING][DcpPduType::INF_log] = true;

        stateChangePossible[DcpState::PREPARED][DcpPduType::STC_configure] = true;
        stateChangePossible[DcpState::PREPARED][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::PREPARED][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::PREPARED][DcpPduType::INF_log] = true;

        stateChangePossible[DcpState::CONFIGURING][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::CONFIGURING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::CONFIGURING][DcpPduType::INF_log] = true;

        stateChangePossible[DcpState::CONFIGURED][DcpPduType::STC_initialize] = true;
        stateChangePossible[DcpState::CONFIGURED][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::CONFIGURED][DcpPduType::STC_run] = true;
        stateChangePossible[DcpState::CONFIGURED][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::CONFIGURED][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::CONFIGURED][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::CONFIGURED][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::INITIALIZING][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::INITIALIZING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::INITIALIZING][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::INITIALIZING][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::INITIALIZING][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::INITIALIZED][DcpPduType::STC_send_outputs] = true;
        stateChangePossible[DcpState::INITIALIZED][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::INITIALIZED][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::INITIALIZED][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::INITIALIZED][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::INITIALIZED][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::SENDING_I][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::SENDING_I][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::SENDING_I][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::SENDING_I][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::SENDING_I][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::SYNCHRONIZING][DcpPduType::STC_do_step] = true;
        stateChangePossible[DcpState::SYNCHRONIZING][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::SYNCHRONIZING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::SYNCHRONIZING][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::SYNCHRONIZING][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::SYNCHRONIZING][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::STC_run] = true;
        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::STC_do_step] = true;
        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::SYNCHRONIZED][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::RUNNING][DcpPduType::STC_do_step] = true;
        stateChangePossible[DcpState::RUNNING][DcpPduType::STC_stop] = true;
        stateChangePossible[DcpState::RUNNING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::RUNNING][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::RUNNING][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::RUNNING][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::COMPUTING][DcpPduType::STC_send_outputs] = true;
        stateChangePossible[DcpState::COMPUTING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::COMPUTING][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::COMPUTING][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::COMPUTING][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::SENDING_D][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::SENDING_D][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::SENDING_D][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::SENDING_D][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::STOPPING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::STOPPING][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::STOPPING][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::STOPPING][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::STOPPED][DcpPduType::STC_deregister] = true;
        stateChangePossible[DcpState::STOPPED][DcpPduType::STC_reset] = true;
        stateChangePossible[DcpState::STOPPED][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::STOPPED][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::STOPPED][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::STOPPED][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::ERROR_HANDLING][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::ERROR_HANDLING][DcpPduType::INF_error] = true;
        stateChangePossible[DcpState::ERROR_HANDLING][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::ERROR_HANDLING][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::ERROR_HANDLING][DcpPduType::DAT_parameter] = true;

        stateChangePossible[DcpState::ERROR_RESOLVED][DcpPduType::STC_reset] = true;
        stateChangePossible[DcpState::ERROR_RESOLVED][DcpPduType::INF_state] = true;
        stateChangePossible[DcpState::ERROR_RESOLVED][DcpPduType::INF_error] = true;
        stateChangePossible[DcpState::ERROR_RESOLVED][DcpPduType::INF_log] = true;
        stateChangePossible[DcpState::ERROR_RESOLVED][DcpPduType::DAT_input_output] = true;
        stateChangePossible[DcpState::ERROR_RESOLVED][DcpPduType::DAT_parameter] = true;

        for (auto const &timeResolution : slaveDescription.TimeRes.resolutions) {
            if (timeResolution.fixed) {
                numerator = timeResolution.numerator;
                denominator = timeResolution.denominator;
                this->timeResolutionSet = true;
                //this->timeResListener(timeResolution.numerator(), timeResolution.denominator());
                this->timeResolutionFix = true;
                break;
            } else if (timeResolution.recommended) {
                numerator = timeResolution.numerator;
                denominator = timeResolution.denominator;
                this->timeResolutionSet = true;
                //this->timeResListener(timeResolution.numerator(), timeResolution.denominator());
                break;
            }
        }

        for (auto const &var: slaveDescription.Variables) {
            if (var.StructuralParameter.get() != nullptr) {
                const valueReference_t valueReference = var.valueReference;
                const DcpDataType dataType = slavedescription::getDataType(slaveDescription, valueReference);
                size_t baseSize = 0;
                switch (dataType) {
                    case DcpDataType::binary:
                    case DcpDataType::string:
                        baseSize = 1024;
                        break;
                    default:
                        baseSize = getDcpDataTypeSize(dataType);
                        break;
                }
                std::vector<size_t> dimensions;
                dimensions.push_back(1);
                values[valueReference] = new MultiDimValue(dataType, baseSize, dimensions);
                switch (dataType) {
                    case DcpDataType::uint8: {
                        if (var.StructuralParameter.get()->Uint8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint16: {
                        if (var.StructuralParameter.get()->Uint16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint32: {
                        if (var.StructuralParameter.get()->Uint32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint64: {
                        if (var.StructuralParameter.get()->Uint64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    default:
                        //only uint datatypes are allowed for structual parameters
                        break;
                }
            }
        }

        for (auto const &var: slaveDescription.Variables) {
            const valueReference_t &valueReference = var.valueReference;
            const DcpDataType dataType = slavedescription::getDataType(slaveDescription, valueReference);
            size_t baseSize = 0;
            switch (dataType) {
                case DcpDataType::binary:
                    if (var.Input.get()->Binary.get()->maxSize != nullptr) {
                        baseSize = *var.Input.get()->Binary.get()->maxSize;
                    } else {
                        baseSize = 4294967295 + 4;
                    }

                case DcpDataType::string:
                    if (var.Input.get()->String.get()->maxSize != nullptr) {
                        baseSize = *var.Input.get()->String.get()->maxSize;
                    } else {
                        baseSize = 4294967295 + 4;
                    }
                    break;
                default:
                    baseSize = getDcpDataTypeSize(dataType);
                    break;
            }

            if (var.Input != nullptr) {
                std::vector<size_t> dimensions;
                size_t i = 0;
                if (var.Input->dimensions.size() > 0) {
                    for (auto const &dim: var.Input->dimensions) {
                        size_t dimVal;
                        if (dim.type == DimensionType::CONSTANT) {
                            dimVal = dim.value;
                        } else if (dim.type == DimensionType::LINKED_VR) {
                            size_t linkedVr = dim.value;
                            MultiDimValue *mdv = values[linkedVr];
                            DcpDataType dataType = mdv->getDataType();
                            switch (dataType) {
                                case DcpDataType::uint8:
                                    dimVal = *mdv->getValue<uint8_t *>();
                                    break;
                                case DcpDataType::uint16:
                                    dimVal = *mdv->getValue<uint16_t *>();
                                    break;
                                case DcpDataType::uint32:
                                    dimVal = *mdv->getValue<uint32_t *>();
                                    break;
                                case DcpDataType::uint64:
                                    dimVal = *mdv->getValue<uint64_t *>();
                                    break;
                                default:
                                    //only uint datatypes are allowed for dimensions
                                    continue;
                                    break;
                            }
                            structualDependencies[linkedVr].push_back(
                                    std::pair<uint64_t, size_t>((uint64_t) valueReference, i));
                        }
                        dimensions.push_back(dimVal);
                        i++;
                    }
                } else {
                    dimensions.push_back(1);
                }

                values[valueReference] = new MultiDimValue(dataType, baseSize, dimensions);

                switch (dataType) {
                    case DcpDataType::int8: {
                        if (var.Input.get()->Int8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Int8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int16: {
                        if (var.Input.get()->Int16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Int16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int32: {
                        if (var.Input.get()->Int32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Int32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int64: {
                        if (var.Input.get()->Int64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Int64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint8: {
                        if (var.Input.get()->Uint8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Uint8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint16: {
                        if (var.Input.get()->Uint16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Uint16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint32: {
                        if (var.Input.get()->Uint32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Uint32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint64: {
                        if (var.Input.get()->Uint64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Uint64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::float32: {
                        if (var.Input.get()->Float32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Float32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::float64: {
                        if (var.Input.get()->Float64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Input.get()->Float64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::string: {
                        if (var.Input.get()->String.get()->start.get() != nullptr) {
                            std::shared_ptr<std::string> startValue = var.Input.get()->String.get()->start;
                            DcpString startString(baseSize - 4);
                            values[valueReference]->update((uint8_t *) startString.getChar(), 0, DcpDataType::string);
                        }
                        break;
                    }
                    case DcpDataType::binary: {
                        if (var.Input.get()->Binary.get()->start.get() != nullptr) {
                            std::shared_ptr<BinaryStartValue> startValue = var.Input.get()->Binary.get()->start;
                            DcpBinary startBinary(startValue->length, startValue->value, baseSize - 4);
                            values[valueReference]->update(startBinary.getBinary(), 0, DcpDataType::binary);
                        }
                        break;
                    }
                }
            }

            if (var.Output != nullptr) {
                std::vector<size_t> dimensions;
                size_t i = 0;
                if (var.Output->dimensions.size() > 0) {
                    for (auto const &dim: var.Output->dimensions) {
                        size_t dimVal;
                        if (dim.type == DimensionType::CONSTANT) {
                            dimVal = dim.value;
                        } else if (dim.type == DimensionType::LINKED_VR) {
                            size_t linkedVr = dim.value;
                            MultiDimValue *mdv = values[linkedVr];
                            DcpDataType dataType = mdv->getDataType();
                            switch (dataType) {
                                case DcpDataType::uint8:
                                    dimVal = *mdv->getValue<uint8_t *>();
                                    break;
                                case DcpDataType::uint16:
                                    dimVal = *mdv->getValue<uint16_t *>();
                                    break;
                                case DcpDataType::uint32:
                                    dimVal = *mdv->getValue<uint32_t *>();
                                    break;
                                case DcpDataType::uint64:
                                    dimVal = *mdv->getValue<uint64_t *>();
                                    break;
                                default:
                                    //only uint datatypes are allowed for dimensions
                                    continue;
                                    break;
                            }
                            structualDependencies[linkedVr].push_back(
                                    std::pair<uint64_t, size_t>((uint64_t) valueReference, i));
                        }
                        dimensions.push_back(dimVal);
                        i++;
                    }
                } else {
                    dimensions.push_back(1);
                }
                values[valueReference] = new MultiDimValue(dataType, baseSize, dimensions);
                switch (dataType) {
                    case DcpDataType::int8: {
                        if (var.Output.get()->Int8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Int8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int16: {
                        if (var.Output.get()->Int16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Int16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int32: {
                        if (var.Output.get()->Int32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Int32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int64: {
                        if (var.Output.get()->Int64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Int64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint8: {
                        if (var.Output.get()->Uint8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }

                        }
                        break;
                    }
                    case DcpDataType::uint16: {
                        if (var.Output.get()->Uint16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint32: {
                        if (var.Output.get()->Uint32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint64: {
                        if (var.Output.get()->Uint64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::float32: {
                        if (var.Output.get()->Float32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Float32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::float64: {
                        if (var.Output.get()->Float64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Float64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::string: {
                        if (var.Output.get()->String.get()->start.get() != nullptr) {
                            std::shared_ptr<std::string> startValue = var.Output.get()->String.get()->start;
                            DcpString startString(baseSize - 4);
                            values[valueReference]->update((uint8_t *) startString.getChar(), 0, DcpDataType::string);
                        }
                        break;
                    }
                    case DcpDataType::binary: {
                        if (var.Output.get()->Binary.get()->start.get() != nullptr) {
                            std::shared_ptr<BinaryStartValue> startValue = var.Output.get()->Binary.get()->start;
                            DcpBinary startBinary(startValue->length, startValue->value, baseSize - 4);
                            values[valueReference]->update(startBinary.getBinary(), 0, DcpDataType::binary);
                        }
                        break;
                    }
                }
            }

            if (var.Parameter != nullptr) {
                std::vector<size_t> dimensions;
                size_t i = 0;
                if (var.Parameter->dimensions.size() > 0) {
                    for (auto const &dim: var.Parameter->dimensions) {
                        size_t dimVal;
                        if (dim.type == DimensionType::CONSTANT) {
                            dimVal = dim.value;
                        } else if (dim.type == DimensionType::LINKED_VR) {
                            size_t linkedVr = dim.value;
                            MultiDimValue *mdv = values[linkedVr];
                            DcpDataType dataType = mdv->getDataType();
                            switch (dataType) {
                                case DcpDataType::uint8:
                                    dimVal = *mdv->getValue<uint8_t *>();
                                    break;
                                case DcpDataType::uint16:
                                    dimVal = *mdv->getValue<uint16_t *>();
                                    break;
                                case DcpDataType::uint32:
                                    dimVal = *mdv->getValue<uint32_t *>();
                                    break;
                                case DcpDataType::uint64:
                                    dimVal = *mdv->getValue<uint64_t *>();
                                    break;
                                default:
                                    //only uint datatypes are allowed for dimensions
                                    continue;
                                    break;
                            }
                            structualDependencies[linkedVr].push_back(
                                    std::pair<uint64_t, size_t>((uint64_t) valueReference, i));
                        }
                        dimensions.push_back(dimVal);
                        i++;
                    }
                } else {
                    dimensions.push_back(1);
                }
                values[valueReference] = new MultiDimValue(dataType, baseSize, dimensions);
                switch (dataType) {
                    case DcpDataType::int8: {
                        if (var.Parameter.get()->Int8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Int8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int16: {
                        if (var.Parameter.get()->Int16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Int16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int32: {
                        if (var.Parameter.get()->Int32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Int32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::int64: {
                        if (var.Parameter.get()->Int64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Int64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint8: {
                        if (var.Parameter.get()->Uint8.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint8.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint16: {
                        if (var.Parameter.get()->Uint16.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint16.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint32: {
                        if (var.Parameter.get()->Uint32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::uint64: {
                        if (var.Parameter.get()->Uint64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Uint64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::float32: {
                        if (var.Parameter.get()->Float32.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Float32.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::float64: {
                        if (var.Parameter.get()->Float64.get()->start.get() != nullptr) {
                            auto &startValues = *var.Output.get()->Float64.get()->start;
                            for (int i = 0; i < startValues.size(); i++) {
                                values[valueReference]->updateValue(i, startValues[i]);
                            }
                        }
                        break;
                    }
                    case DcpDataType::string: {
                        if (var.Parameter.get()->String.get()->start.get() != nullptr) {
                            std::shared_ptr<std::string> startValue = var.Parameter.get()->String.get()->start;
                            DcpString startString(baseSize - 4);
                            values[valueReference]->update((uint8_t *) startString.getChar(), 0, DcpDataType::string);
                        }
                        break;
                    }
                    case DcpDataType::binary: {
                        if (var.Parameter.get()->Binary.get()->start.get() != nullptr) {
                            std::shared_ptr<BinaryStartValue> startValue = var.Parameter.get()->Binary.get()->start;
                            DcpBinary startBinary(startValue->length, startValue->value, baseSize - 4);
                            values[valueReference]->update(startBinary.getBinary(), 0, DcpDataType::binary);
                        }
                        break;
                    }
                }
            }

        }
    }

    void notifyStateChange() {
        DcpPduNtfStateChanged notification = {dcpId, state};
        driver.send(notification);
#ifdef DEBUG
        Log(STATE_CHANGED, state);
#endif
        if (asynchronousCallback[DcpCallbackTypes::STATE_CHANGED]) {
            std::thread t(stateChangedListener, state);
            t.detach();
        } else {
            stateChangedListener(state);
        }
    }

    void setTimeRes(uint32_t numerator,
                    uint32_t denominator) {
        this->numerator = numerator;
        this->denominator = denominator;
#ifdef DEBUG
        Log(TIME_RES_SET, numerator, denominator);
#endif
        timeResolutionSet = true;

        if (asynchronousCallback[DcpCallbackTypes::TIME_RES]) {
            std::thread t(timeResListener, numerator, denominator);
            t.detach();
        } else {
            timeResListener(numerator, denominator);
        }
    }

    void setSteps(uint16_t dataId, uint32_t steps) {
        this->steps[dataId] = steps;
#ifdef DEBUG
        Log(STEP_SET, dataId, steps);
#endif
        if (asynchronousCallback[DcpCallbackTypes::STEPS]) {
            std::thread t(stepsListener, dataId, steps);
            t.detach();
        } else {
            stepsListener(dataId, steps);
        }
    }

    void setOperationInformation(uint8_t dcpId, DcpOpMode opMode) {
        this->dcpId = dcpId;

        Log(DCP_ID_SET, dcpId);
        this->opMode = opMode;
        Log(OP_MODE_SET, opMode);
        if (asynchronousCallback[DcpCallbackTypes::OPERATION_INFORMATION]) {
            std::thread t(operationInformationListener, dcpId, opMode);
            t.detach();
        } else {
            operationInformationListener(dcpId, opMode);
        }
    }

    void setRuntime(int64_t unixTimeStamp) {
        if (asynchronousCallback[DcpCallbackTypes::RUNTIME]) {
            std::thread t(runtimeListener, unixTimeStamp);
            t.detach();
        } else {
            runtimeListener(unixTimeStamp);
        }
    }

    void controlPduMissed() {
        if (asynchronousCallback[DcpCallbackTypes::CONTROL_MISSED]) {
            std::thread t(missingControlPduListener);
            t.detach();
        } else {
            missingControlPduListener();
        }
    }

    void inputOutputPduMissed(uint16_t dataId) {
        if (asynchronousCallback[DcpCallbackTypes::IN_OUT_MISSED]) {
            std::thread t(missingInputOutputPduListener, dataId);
            t.detach();
        } else {
            missingInputOutputPduListener(dataId);
        }
    }

    void parameterPduMissed(uint16_t paramId) {
        if (asynchronousCallback[DcpCallbackTypes::CONTROL_MISSED]) {
            std::thread t(missingParameterPduListener, paramId);
            t.detach();
        } else {
            missingParameterPduListener(paramId);
        }
    }

    void ack(uint16_t respSeqId) {
        DcpPduAck ack = {dcpId, respSeqId};
        driver.send(ack);
    }

    void nack(uint16_t respSeqId, DcpError errorCode) {
        DcpPduNack nack = {DcpPduType::RSP_nack, dcpId, respSeqId, errorCode};
        driver.send(nack);
    }

    bool checkForError(DcpPdu &msg) {
        bool valid = true;
        DcpError error = DcpError::NONE;
        uint8_t type = (uint8_t) msg.getTypeId();
        if (!(
                // STC_*
                (type >= 0x01 && type <= 0x0A) ||
                // CFG_*
                (type >= 0x20 && type <= 0x2B) ||
                //INF_*
                (type >= 0x80 && type <= 0x82) ||
                //DAT_*
                (type >= 0xF0 && type <= 0xF1)
        )) {
#if defined(DEBUG) || defined(LOGGING)
            Log(INVALID_TYPE_ID, type);
#endif
            return false;
        }

        //check Receiver
        switch (msg.getTypeId()) {
            case DcpPduType::DAT_input_output: {
                //toDo distinguish scopes
                DcpPduDatInputOutput &aciPduData = static_cast<DcpPduDatInputOutput &>(msg);
                if (inputAssignment.count(aciPduData.getDataId()) == 0) {
#if defined(DEBUG) || defined(LOGGING)
                    Log(UNKNOWN_DATA_ID, aciPduData.getDataId());
#endif
                    return false;
                }
                break;
            }
            case DcpPduType::DAT_parameter: {
                //toDo distinguish scopes
                DcpPduDatParameter &paramPdu = static_cast<DcpPduDatParameter &>(msg);
                if (paramAssignment.count(paramPdu.getParamId()) == 0) {
#if defined(DEBUG) || defined(LOGGING)
                    Log(UNKNOWN_PARAM_ID, paramPdu.getParamId());
#endif
                    return false;
                }
                break;
            }
            default: {
                DcpPduBasic &basicPdu = static_cast<DcpPduBasic &>(msg);
                if (state != DcpState::ALIVE && dcpId != basicPdu.getReceiver()) {
#if defined(DEBUG) || defined(LOGGING)
                    Log(INVALID_RECEIVER, basicPdu.getReceiver());
#endif
                    return false;
                }
                break;
            }
        }

        //check sequence id
        /*switch (msg.getTypeId()) {
            case DcpPduType::DAT_input_output: {
                DcpPduDatInputOutput &aciPduData = static_cast<DcpPduDatInputOutput &>(msg);
                uint16_t diff = checkSeqIdInOut(aciPduData.getDataId(), aciPduData.getPduSeqId());
                if (diff > 1) {
                    //inputOutputPduMissed(aciPduData.getDataId());
                    //Log(IN_OUT_PDU_MISSED);
                } else if (diff <= 0) {
                    Log(OLD_IN_OUT_PDU_RECEIVED);
                    return false;
                }
                break;
            }
            case DcpPduType::DAT_parameter: {
                DcpPduDatParameter &aciPduParam = static_cast<DcpPduDatParameter &>(msg);
                uint16_t diff = checkSeqIdParam(aciPduParam.getParamId(), aciPduParam.getPduSeqId());
                if (diff > 1) {
                    parameterPduMissed(aciPduParam.getParamId());
                    Log(PARAM_PDU_MISSED);
                } else if (diff <= 0) {
                    Log(OLD_PARAM_PDU_RECEIVED);
                    return false;
                }
                break;
            }
            default: {
                if (state != DcpState::ALIVE) {
                    DcpPduBasic &basicPdu = static_cast<DcpPduBasic &>(msg);
                    uint16_t diff = checkSeqId(masterId, basicPdu.getPduSeqId());
                    if (diff > 1) {
                        controlPduMissed();
                        Log(CTRL_PDU_MISSED);
                    } else if (diff <= 0) {
                        Log(OLD_CTRL_PDU_RECEIVED);
                        return false;
                    }
                }
                break;
            }
        }*/

        //check support
        if (opMode != DcpOpMode::NRT) {
            switch (msg.getTypeId()) {
                case DcpPduType::STC_do_step:
#if defined(DEBUG) || defined(LOGGING)
                    Log(ONLY_NRT, opMode);
#endif
                    error = DcpError::NOT_SUPPORTED_PDU;
            }
        }

        if (error == DcpError::NONE) {
            //check logging
            switch (msg.getTypeId()) {
                case DcpPduType::CFG_set_logging:
                case DcpPduType::INF_log:
                    if (slaveDescription.Log == nullptr ||
                        (!slaveDescription.CapabilityFlags.canProvideLogOnNotification &&
                         !slaveDescription.CapabilityFlags.canProvideLogOnRequest)) {
                        error = DcpError::NOT_SUPPORTED_PDU;
                    }
            }

        }


        if (error == DcpError::NONE) {
            //check length
            switch (msg.getTypeId()) {
                case DcpPduType::DAT_input_output: {
                    DcpPduDatInputOutput &aciPduData = static_cast<DcpPduDatInputOutput &>(msg);
                    size_t correctLength = 0;
                    for (auto &pos: inputAssignment[aciPduData.getDataId()]) {
                        switch (pos.second.second) {
                            case DcpDataType::binary:
                            case DcpDataType::string:
                                correctLength += *((uint16_t *) (aciPduData.getPayload() + correctLength));
                                break;
                            default:
                                correctLength += getDcpDataTypeSize(pos.second.second);
                        }
                    }
                    if (aciPduData.getPduSize() != (correctLength + 5)) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LENGTH, (uint16_t) aciPduData.getPduSize(), (uint16_t) (correctLength + 5));
#endif
                        return false;

                    }
                    break;
                }
                case DcpPduType::DAT_parameter: {
                    DcpPduDatParameter &aciPduParam = static_cast<DcpPduDatParameter &>(msg);
                    size_t correctLength = 0;
                    for (auto &pos: paramAssignment[aciPduParam.getParamId()]) {
                        switch (pos.second.second) {
                            case DcpDataType::binary:
                            case DcpDataType::string:
                                correctLength += *((uint16_t *) (aciPduParam.getConfiguration()));
                                break;
                            default:
                                correctLength += getDcpDataTypeSize(pos.second.second);
                                break;
                        }
                    }
                    if (aciPduParam.getPduSize() != (correctLength + 5)) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LENGTH, (uint16_t) aciPduParam.getPduSize(),
                            (uint16_t) (correctLength + 5));
#endif
                        return false;
                    }
                    break;
                }
                case DcpPduType::CFG_set_parameter: {
                    DcpPduSetParameter setParameter = static_cast<DcpPduSetParameter &>(msg);
                    size_t correctLength = 0;
                    switch (setParameter.getSourceDataType()) {
                        case DcpDataType::binary:
                        case DcpDataType::string:
                            correctLength = *((uint16_t *) (setParameter.getConfiguration() + correctLength));
                            break;
                        default:
                            correctLength = getDcpDataTypeSize(setParameter.getSourceDataType());
                    }
                    if (setParameter.getSerializedSize() != (correctLength + 13)) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LENGTH, (uint16_t) setParameter.getPduSize(),
                            (uint16_t) (correctLength + 13));
#endif
                        error = DcpError::INVALID_LENGTH;
                    }
                    break;
                }
                default: {
                    if (!msg.isSizeCorrect()) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LENGTH, (uint16_t) msg.getPduSize(),
                            (uint16_t) msg.getCorrectSize());
#endif
                        error = DcpError::INVALID_LENGTH;
                    }
                    break;
                }
            }
        }

        //check state
        if (error == DcpError::NONE) {
            switch (msg.getTypeId()) {
                case DcpPduType::DAT_input_output:
                case DcpPduType::DAT_parameter: {
                    if (!stateChangePossible[state][msg.getTypeId()]) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(DATA_NOT_ALLOWED, state);
#endif
                        return false;
                    } else {
                        return true;
                    }
                    break;
                }
                default:
                    if (!stateChangePossible[state][msg.getTypeId()]) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(MSG_NOT_ALLOWED, msg.getTypeId(), state);
#endif
                        error = DcpError::PROTOCOL_ERROR_PDU_NOT_ALLOWED_IN_THIS_STATE;
                    }
                    break;
            }

        }

        //check state_id
        if (error == DcpError::NONE) {

            switch (msg.getTypeId()) {
                case DcpPduType::STC_register: {
                    DcpPduRegister registerPdu = static_cast<DcpPduRegister &>(msg);
                    if (registerPdu.getStateId() != state) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_STATE_ID, registerPdu.getStateId(), state);
#endif
                        DcpPduNack nack = {DcpPduType::RSP_nack, registerPdu.getReceiver(), registerPdu.getPduSeqId(),
                                           DcpError::INVALID_STATE_ID};
                        driver.send(nack);
                        return false;
                    }
                    break;
                }
                case DcpPduType::STC_deregister:
                case DcpPduType::STC_prepare:
                case DcpPduType::STC_configure:
                case DcpPduType::STC_initialize:
                case DcpPduType::STC_run:
                case DcpPduType::STC_do_step:
                case DcpPduType::STC_send_outputs:
                case DcpPduType::STC_stop:
                case DcpPduType::STC_reset: {
                    DcpPduBasicStateTransition &bst = static_cast<DcpPduBasicStateTransition &>(msg);
                    if (bst.getStateId() != state) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_STATE_ID, bst.getStateId(), state);
#endif
                        error = DcpError::INVALID_STATE_ID;
                    }
                    break;
                }
            }
        }

        if (error == DcpError::NONE) {
            switch (msg.getTypeId()) {
                case DcpPduType::INF_state:
                case DcpPduType::INF_error:
                    return true;
            }

        }

        //check other pdu semantics
        if (error == DcpError::NONE) {
            switch (msg.getTypeId()) {
                case DcpPduType::STC_register: {
                    DcpPduRegister registerPdu = static_cast<DcpPduRegister &>(msg);

                    uint128_t uuidFromACUD = convertToUUID(slaveDescription.uuid);
                    uint128_t &uuidFromPDU = registerPdu.getSlaveUuid();
                    if (uuidFromACUD != uuidFromPDU) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_UUID, std::string(slaveDescription.uuid),
                            convertToUUIDStr(uuidFromPDU));
#endif
                        error = DcpError::INVALID_UUID;
                    } else if (!slavedescription::isOpModeSupported(slaveDescription, registerPdu.getOpMode())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_OP_MODE, registerPdu.getOpMode());
#endif
                        error = DcpError::INVALID_OP_MODE;
                    } else if (registerPdu.getMajorVersion() != slaveDescription.dcpMajorVersion) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_MAJOR_VERSION, registerPdu.getMajorVersion(),
                            slaveDescription.dcpMajorVersion, slaveDescription.dcpMinorVersion);
#endif
                        error = DcpError::INVALID_MAJOR_VERSION;
                    } else if (registerPdu.getMinorVersion() != slaveDescription.dcpMinorVersion) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_MINOR_VERSION, registerPdu.getMinorVersion(),
                            slaveDescription.dcpMajorVersion, slaveDescription.dcpMinorVersion);
#endif
                        error = DcpError::INVALID_MINOR_VERSION;
                    }

                    if (error == DcpError::NONE) {
                        DcpPduAck ack = {registerPdu.getReceiver(), registerPdu.getPduSeqId()};
                        driver.send(ack);
                        return true;
                    } else {
                        std::cout << registerPdu.getReceiver() << std::endl;
                        DcpPduNack nack = {DcpPduType::RSP_nack, registerPdu.getReceiver(), registerPdu.getPduSeqId(),
                                           error};
                        driver.send(nack);
                        return false;
                    }
                    break;
                }
                case DcpPduType::STC_configure: {
                    for (auto &in : configuredInPos) {
                        uint16_t maxInPos = *std::max_element(std::begin(in.second), std::end(in.second));
                        if (maxInPos >= in.second.size()) {

                            std::string missingInPosition = "";
                            for (int i = 0; i < maxInPos; ++i) {
                                if (std::find(in.second.begin(), in.second.end(), i) == in.second.end()) {
                                    missingInPosition += std::to_string(i) + ", ";
                                }
                            }
                            missingInPosition = missingInPosition.substr(0, missingInPosition.size() - 2);
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIGURATION_GAP_INPUT_POS, missingInPosition, in.first,
                                maxInPos);
#endif
                            error = DcpError::INCOMPLETE_CONFIG_GAP_INPUT_POS;
                        }
                    }
                    for (auto &out : configuredOutPos) {
                        uint16_t maxOutPos = *std::max_element(std::begin(out.second), std::end(out.second));
                        if (maxOutPos >= out.second.size()) {

                            std::string missingOutPosition = "";
                            for (int i = 0; i < maxOutPos; ++i) {
                                if (std::find(out.second.begin(), out.second.end(), i) == out.second.end()) {
                                    missingOutPosition += std::to_string(i) + ", ";
                                }
                            }
                            missingOutPosition = missingOutPosition.substr(0, missingOutPosition.size() - 2);
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIGURATION_GAP_OUTPUT_POS, missingOutPosition, out.first,
                                maxOutPos);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_GAP_OUTPUT_POS;
                            }
                        }
                    }
                    for (auto &param : configuredParamPos) {
                        uint16_t maxParamPos = *std::max_element(std::begin(param.second), std::end(param.second));
                        if (maxParamPos >= param.second.size()) {

                            std::string missingParamPosition = "";
                            for (int i = 0; i < maxParamPos; ++i) {
                                if (std::find(param.second.begin(), param.second.end(), i) == param.second.end()) {
                                    missingParamPosition += std::to_string(i) + ", ";
                                }
                            }
                            missingParamPosition = missingParamPosition.substr(0, missingParamPosition.size() - 2);
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIGURATION_GAP_PARAM_POS, missingParamPosition, param.first,
                                maxParamPos);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_GAP_TUNABLE_POS;
                            }
                        }
                    }

                    for (const auto &inputAss: inputAssignment) {
                        if (sourceNetworkConfigured.count(inputAss.first) == 0) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIG_NW_INFO_INPUT, inputAss.first);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_NW_INFO_INPUT;
                            }
                        }
                    }

                    for (const auto &outputAss: outputAssignment) {
                        if (targetNetworkConfigured.count(outputAss.first) == 0) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIG_NW_INFO_OUTPUT, outputAss.first);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_NW_INFO_OUTPUT;
                            }
                        }
                    }

                    for (const auto &paramAss: paramAssignment) {
                        if (paramNetworkConfigured.count(paramAss.first) == 0) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIG_NW_INFO_TUNABLE, paramAss.first);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_NW_INFO_TUNABLE;
                            }
                        }
                    }

                    for (auto &entr : outputAssignment) {
                        if (steps.count(entr.first) == 0) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIGURATION_STEPS, entr.first);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_STEPS;
                            }
                        }
                    }

                    if (!timeResolutionSet) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INCOMPLETE_CONFIGURATION_TIME_RESOLUTION);
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INCOMPLETE_CONFIG_TIME_RESOLUTION;
                        }
                    }

                    for (const auto &inputAss: inputAssignment) {

                        if (std::find(runningScope.begin(), runningScope.end(), inputAss.first) == runningScope.end() &&
                            std::find(initializationScope.begin(), initializationScope.end(), inputAss.first) ==
                            initializationScope.end()) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIG_SCOPE, inputAss.first);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_SCOPE;
                            }
                        }
                    }

                    for (const auto &outputAss: outputAssignment) {

                        if (std::find(runningScope.begin(), runningScope.end(), outputAss.first) ==
                            runningScope.end() &&
                            std::find(initializationScope.begin(), initializationScope.end(), outputAss.first) ==
                            initializationScope.end()) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INCOMPLETE_CONFIG_SCOPE, outputAss.first);
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INCOMPLETE_CONFIG_SCOPE;
                            }
                        }
                    }

                    break;
                }
                case DcpPduType::STC_run: {
                    DcpPduRun &runPDU = static_cast<DcpPduRun &>(msg);
                    int64_t time_since_epoch = std::chrono::seconds(std::time(NULL)).count();
                    if (opMode != DcpOpMode::NRT
                        && (runPDU.getStartTime() > 0
                            && runPDU.getStartTime()
                               < time_since_epoch)) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_START_TIME, convertUnixTimestamp(runPDU.getStartTime()), currentTime());
#endif
                        error = DcpError::INVALID_START_TIME;
                        break;
                    }
#if defined(DEBUG) || defined(LOGGING)
                    else {
                        Log(START_TIME, convertUnixTimestamp(runPDU.getStartTime()));
                    }
#endif
                    break;
                }
                case DcpPduType::STC_do_step: {
                    DcpPduDoStep &doStepPDU = static_cast<DcpPduDoStep &>(msg);
                    if (!slavedescription::isStepsSupportedNRT(slaveDescription, doStepPDU.getSteps())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_STEPS, doStepPDU.getSteps(),
                            slavedescription::supportedStepsNRT(slaveDescription));
#endif
                        error = DcpError::INVALID_STEPS;
                    }
                    if (!slaveDescription.CapabilityFlags.canHandleVariableSteps) {
                        if (fixNrtStep > 0) {
                            if (fixNrtStep != doStepPDU.getSteps()) {
#if defined(DEBUG) || defined(LOGGING)
                                Log(NOT_SUPPORTED_VARIABLE_STEPS, doStepPDU.getSteps(), fixNrtStep);
#endif
                                if (error == DcpError::NONE) {
                                    error = DcpError::NOT_SUPPORTED_VARIABLE_STEPS;
                                }
                            }
                        } else {
                            fixNrtStep = doStepPDU.getSteps();
                        }
                    }
                    break;
                }
                case DcpPduType::INF_log: {
                    DcpPduInfLog &infLog = static_cast<DcpPduInfLog &>(msg);
                    if (!slavedescription::logCategoryExists(slaveDescription, infLog.getLogCategory())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LOG_CATEGORY, infLog.getLogCategory());
#endif
                        error = DcpError::INVALID_LOG_CATEGORY;
                        break;
                    } else {
                        return true;
                    }
                    break;
                }
                case DcpPduType::CFG_set_time_res: {
                    DcpPduSetTimeRes &setTimeRes = static_cast<DcpPduSetTimeRes &>(msg);
                    if (timeResolutionFix &&
                        !(numerator == setTimeRes.getNumerator() && denominator == setTimeRes.getDenominator())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(FIX_TIME_RESOLUTION);
#endif
                        error = DcpError::INVALID_TIME_RESOLUTION;
                    }

                    if (!slavedescription::isTimeResolutionSupported(slaveDescription, setTimeRes.getNumerator(),
                                                   setTimeRes.getDenominator())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_TIME_RESOLUTION, setTimeRes.getNumerator(), setTimeRes.getNumerator(),
                            slavedescription::supportedTimeResolutions(slaveDescription));
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_TIME_RESOLUTION;
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_set_steps: {
                    DcpPduSetSteps &setSteps = static_cast<DcpPduSetSteps &>(msg);
                    uint16_t dataId = setSteps.getDataId();

                    if (outputAssignment.find(dataId) != outputAssignment.end()) {

                        for (auto &e : outputAssignment[dataId]) {

                            const uint64_t vr = e.second;

                            const Output_t &output = *slavedescription::getOutput(slaveDescription, vr);

                            if (!slavedescription::isStepsSupported(slaveDescription, output, setSteps.getSteps())) {
#if defined(DEBUG) || defined(LOGGING)
                                Log(INVALID_STEPS, setSteps.getSteps(), vr, output.fixedSteps ?
                                                                            std::to_string(
                                                                                    output.defaultSteps)
                                                                                              :
                                                                            "between " +
                                                                            std::to_string(
                                                                                    *output.minSteps) +
                                                                            " and " +
                                                                            std::to_string(
                                                                                    *output.maxSteps));
#endif
                                error = DcpError::INVALID_STEPS;

                                break;
                            }

                        }
                    }

                    break;
                }
                case DcpPduType::CFG_config_input: {
                    DcpPduConfigInput &configInput = static_cast<DcpPduConfigInput &>(msg);
                    if (!slavedescription::inputExists(slaveDescription, configInput.getTargetVr())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_VALUE_REFERENCE_INPUT, configInput.getTargetVr());
#endif
                        error = DcpError::INVALID_VALUE_REFERENCE;
                        break;
                    }
                    if (!castAllowed(slavedescription::getDataType(slaveDescription, configInput.getTargetVr()),
                                     configInput.getSourceDataType())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_SOURCE_DATA_TYPE, configInput.getSourceDataType(), slavedescription::getDataType(slaveDescription,
                                                                                                   configInput.getTargetVr()));
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_SOURCE_DATA_TYPE;
                        }

                    }
                    break;
                }
                case DcpPduType::CFG_config_output: {
                    DcpPduConfigOutput &outputConfig = static_cast<DcpPduConfigOutput &>(msg);
                    if (!slavedescription::outputExists(slaveDescription, outputConfig.getSourceVr())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_VALUE_REFERENCE_OUTPUT, outputConfig.getSourceVr());
#endif
                        error = DcpError::INVALID_VALUE_REFERENCE;
                        break;
                    }
                    const Output_t &output = *slavedescription::getOutput(slaveDescription, outputConfig.getSourceVr());
                    if (steps.count(outputConfig.getDataId()) >= 1) {

                        if (!slavedescription::isStepsSupported(slaveDescription, output,
                                              steps[outputConfig.getDataId()])) {
#if defined(DEBUG) || defined(LOGGING)
                            Log(INVALID_STEPS, steps[outputConfig.getDataId()],
                                outputConfig.getSourceVr(), output.fixedSteps ?
                                                            std::to_string(output.defaultSteps) :
                                                            "between " + std::to_string(*output.minSteps) + " and " +
                                                            std::to_string(*output.maxSteps));
#endif
                            if (error == DcpError::NONE) {
                                error = DcpError::INVALID_STEPS;
                            }
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_set_target_network_information: {
                    DcpPduSetNetworkInformation &networkInfo = static_cast<DcpPduSetNetworkInformation &>(msg);

                    if (!slavedescription::isTransportProtocolSupported(slaveDescription, networkInfo.getTransportProtocol())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_TRANSPORT_PROTOCOL, networkInfo.getTransportProtocol());
#endif
                        error = DcpError::INVALID_TRANSPORT_PROTOCOL;
                        break;
                    }
                    break;
                }
                case DcpPduType::CFG_set_source_network_information: {
                    DcpPduSetNetworkInformation &networkInfo = static_cast<DcpPduSetNetworkInformation &>(msg);

                    if (!slavedescription::isTransportProtocolSupported(slaveDescription, networkInfo.getTransportProtocol())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_TRANSPORT_PROTOCOL, networkInfo.getTransportProtocol());
#endif
                        error = DcpError::INVALID_TRANSPORT_PROTOCOL;
                    }


                    switch (networkInfo.getTransportProtocol()) {
                        case DcpTransportProtocol::UDP_IPv4: {
                            DcpPduSetNetworkInformationEthernet networkInfoUdp =
                                    static_cast<DcpPduSetNetworkInformationEthernet &>(networkInfo);

                            if (!slavedescription::isUDPPortSupportedForInputOutput(slaveDescription, networkInfoUdp.getPort())) {
#if defined(DEBUG) || defined(LOGGING)
                                Log(INVALID_PORT, networkInfoUdp.getPort(),
                                    slavedescription::supportedUdpPorts(slaveDescription));
#endif
                                if (error == DcpError::NONE) {
                                    error = DcpError::INVALID_NETWORK_INFORMATION;
                                }
                            }
                            //toDo check ip address
                            break;
                        }
                        case DcpTransportProtocol::TCP_IPv4: {
                            DcpPduSetNetworkInformationEthernet networkInfoTcp =
                                    static_cast<DcpPduSetNetworkInformationEthernet &>(networkInfo);

                            if (!slavedescription::isTCPPortSupportedForInputOutput(slaveDescription, networkInfoTcp.getPort())) {
#if defined(DEBUG) || defined(LOGGING)
                                Log(INVALID_PORT, networkInfoTcp.getPort(),
                                    slavedescription::supportedTCPPorts(slaveDescription));
#endif
                                if (error == DcpError::NONE) {
                                    error = DcpError::INVALID_NETWORK_INFORMATION;
                                }
                            }
                            break;
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_set_parameter: {
                    DcpPduSetParameter &setParameter = static_cast<DcpPduSetParameter &>(msg);
                    if (!slavedescription::parameterExists(slaveDescription, setParameter.getParameterVr())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_VALUE_REFERENCE_PARAMETER, setParameter.getParameterVr());
#endif
                        error = DcpError::INVALID_VALUE_REFERENCE;
                        break;
                    }
                    if (!castAllowed(slavedescription::getDataType(slaveDescription, setParameter.getParameterVr()),
                                     setParameter.getSourceDataType())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_SOURCE_DATA_TYPE, setParameter.getSourceDataType(),
                            slavedescription::getDataType(slaveDescription, setParameter.getParameterVr()));
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_SOURCE_DATA_TYPE;
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_config_tunable_parameter: {
                    DcpPduConfigTunableParameter configTunableParameter = static_cast<DcpPduConfigTunableParameter &>(msg);

                    if (!slavedescription::parameterExists(slaveDescription, configTunableParameter.getParameterVr())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_VALUE_REFERENCE_PARAMETER, configTunableParameter.getParameterVr());
#endif
                        error = DcpError::INVALID_VALUE_REFERENCE;
                        break;
                    }
                    if (!castAllowed(slavedescription::getDataType(slaveDescription, configTunableParameter.getParameterVr()),
                                     configTunableParameter.getSourceDataType())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_SOURCE_DATA_TYPE, configTunableParameter.getSourceDataType(),
                            slavedescription::getDataType(slaveDescription, configTunableParameter.getParameterVr()));
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_SOURCE_DATA_TYPE;
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_set_param_network_information: {
                    DcpPduSetParamNetworkInformation &paramNetworkInfo = static_cast<DcpPduSetParamNetworkInformation &>(msg);

                    if (!slavedescription::isTransportProtocolSupported(slaveDescription,
                                                      paramNetworkInfo.getTransportProtocol())) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_TRANSPORT_PROTOCOL, paramNetworkInfo.getTransportProtocol());
#endif
                        error = DcpError::INVALID_TRANSPORT_PROTOCOL;
                    }

                    switch (paramNetworkInfo.getTransportProtocol()) {
                        case DcpTransportProtocol::UDP_IPv4: {
                            DcpPduSetParamNetworkInformationEthernet paramNetworkInfoUDP =
                                    static_cast<DcpPduSetParamNetworkInformationEthernet &>(paramNetworkInfo);
                            if (!slavedescription::isUDPPortSupportedForParameter(slaveDescription,
                                                                paramNetworkInfoUDP.getPort())) {
#if defined(DEBUG) || defined(LOGGING)
                                Log(INVALID_PORT, paramNetworkInfoUDP.getPort(),
                                    slavedescription::supportedUdpPortsParameter(slaveDescription));
#endif
                                if (error == DcpError::NONE) {
                                    error = DcpError::INVALID_NETWORK_INFORMATION;
                                }

                            }
                            //toDo check ip address

                            break;
                        }
                        case DcpTransportProtocol::TCP_IPv4: {
                            DcpPduSetParamNetworkInformationEthernet paramNetworkInfTCP =
                                    static_cast<DcpPduSetParamNetworkInformationEthernet &>(paramNetworkInfo);
                            if (!slavedescription::isTCPPortSupportedForParameter(slaveDescription,
                                                                paramNetworkInfTCP.getPort())) {
#if defined(DEBUG) || defined(LOGGING)
                                Log(INVALID_PORT, paramNetworkInfTCP.getPort(),
                                    slavedescription::supportedTCPPortsParameter(slaveDescription));
#endif
                                if (error == DcpError::NONE) {
                                    error = DcpError::INVALID_NETWORK_INFORMATION;
                                }

                            }
                            //toDo check ip address

                            break;
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_set_logging: {

                    DcpPduSetLogging &setLogging = static_cast<DcpPduSetLogging &>(msg);
                    if (setLogging.getLogMode() == DcpLogMode::LOG_ON_REQUEST &&
                        !slaveDescription.CapabilityFlags.canProvideLogOnRequest) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(NOT_SUPPORTED_LOG_ON_REQUEST);
#endif
                        error = DcpError::NOT_SUPPORTED_LOG_ON_REQUEST;
                    }
                    if (setLogging.getLogMode() == DcpLogMode::LOG_ON_NOTIFICATION &&
                        !slaveDescription.CapabilityFlags.canProvideLogOnNotification) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(NOT_SUPPORTED_LOG_ON_NOTIFICATION);
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::NOT_SUPPORTED_LOG_ON_NOTIFICATION;
                        }
                    }
                    //toDo find better solution
                    bool categoryFound = false;
                    for (const auto &category: slaveDescription.Log->categories) {
                        if (category.id == setLogging.getLogCategory()) {
                            categoryFound = true;
                            break;
                        }
                    }
                    if (!categoryFound) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LOG_CATEGORY, setLogging.getLogCategory());
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_LOG_CATEGORY;
                        }
                    }

                    if ((uint8_t) setLogging.getLogLevel() > 4) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LOG_LEVEL, setLogging.getLogLevel());
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_LOG_LEVEL;
                        }
                    }

                    if ((uint8_t) setLogging.getLogMode() > 1) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_LOG_MODE, setLogging.getLogMode());
#endif
                        if (error == DcpError::NONE) {
                            error = DcpError::INVALID_LOG_MODE;
                        }
                    }
                    break;
                }
                case DcpPduType::CFG_set_scope: {
                    DcpPduSetScope &setScope = static_cast<DcpPduSetScope &>(msg);
                    if ((uint8_t) setScope.getScope() > 2) {
#if defined(DEBUG) || defined(LOGGING)
                        Log(INVALID_SCOPE, setScope.getScope());
#endif
                        error = DcpError::INVALID_SCOPE;
                        break;
                    }
                    break;
                }
            }
        }

        DcpPduBasic &basic = static_cast<DcpPduBasic &>(msg);
        if (error == DcpError::NONE) {
            ack(basic.getPduSeqId());
            return true;
        } else {
            nack(basic.getPduSeqId(), error);
        }
        return false;
    }

    bool castAllowed(DcpDataType input, DcpDataType output) {
        switch (input) {
            //ALLOWED_INPUT_OUTPUT(INPUT_TYPE, uint8, uint16, uint32, uint64,
            //int8, int16, int32, int64, float32, double64, string, binary)
            ALLOWED_INPUT_OUTPUT(uint8, true, false, false, false,
                                 false, false, false, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(uint16, true, true, false, false,
                                 false, false, false, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(uint32, true, true, true, false,
                                 false, false, false, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(uint64, true, true, true, true, false,
                                 false, false, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(int8, false, false, false, false,
                                 true, false, false, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(int16, true, false, false, false,
                                 true, true, false, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(int32, true, true, false, false, true,
                                 true, true, false, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(int64, true, true, true, false, true,
                                 true, true, true, false, false, false, false)
            ALLOWED_INPUT_OUTPUT(float32, true, true, false, false,
                                 true, true, false, false, true, false, false, false)
            ALLOWED_INPUT_OUTPUT(float64, true, true, true, false,
                                 true, true, true, false, true, true, false, false)
            ALLOWED_INPUT_OUTPUT(string, false, false, false, false,
                                 false, false, false, false, false, false, true, false)
            ALLOWED_INPUT_OUTPUT(binary, false, false, false, false,
                                 false, false, false, false, false, false, false, true)
        }
        return false;
    }

    virtual void prepare() = 0;

    virtual void configure() = 0;

    virtual void initialize() = 0;

    virtual void synchronize() = 0;

    virtual void run(const int64_t startTime) = 0;

    virtual void doStep(const uint32_t steps) = 0;

    virtual void startHeartbeat() = 0;

    virtual void updateLastStateRequest() = 0;


    void clearOutputBuffer() {
        for (auto const &ent : outputBuffer) {
            delete ent.second;
        }
        outputBuffer.clear();
    }

    virtual void sendOutputs(std::vector<uint16_t> dataIdsToSend) = 0;


    void clearConfig() {
        driver.stop();

        inputAssignment.clear();
        configuredInPos.clear();

        outputAssignment.clear();
        configuredOutPos.clear();
        clearOutputBuffer();

        configuredParamPos.clear();
        paramAssignment.clear();

        segNumsOut.clear();
        segNumsIn.clear();
        dataSegNumsOut.clear();
        dataSegNumsIn.clear();

        steps.clear();
        runningScope.clear();
        initializationScope.clear();

        sourceNetworkConfigured.clear();
        targetNetworkConfigured.clear();
        paramNetworkConfigured.clear();
        timeResolutionSet = false;
        timeResolutionFix = false;
        for (auto const &timeResolution : slaveDescription.TimeRes.resolutions) {
            if (timeResolution.fixed) {
                numerator = timeResolution.numerator;
                denominator = timeResolution.denominator;
                this->timeResolutionSet = true;
                //this->timeResListener(timeResolution.numerator(), timeResolution.denominator());
                this->timeResolutionFix = true;
                break;
            } else if (timeResolution.recommended) {
                numerator = timeResolution.numerator;
                denominator = timeResolution.denominator;
                this->timeResolutionSet = true;
                //this->timeResListener(timeResolution.numerator(), timeResolution.denominator());
                break;
            }
        }
        Log(CONFIGURATION_CLEARED);
    }

    void updateStructualDependencies(uint64_t valueReference, size_t value) {
        for (auto const &dependency: structualDependencies[valueReference]) {
            uint64_t vrToUpdate = dependency.first;
            size_t pos = dependency.second;
            std::vector<size_t> newDimensions(values[vrToUpdate]->getDimensions());
            newDimensions[pos] = value;
            if (slavedescription::inputExists(slaveDescription, valueReference) ||
                    slavedescription::outputExists(slaveDescription, valueReference)) {
                values[vrToUpdate] = new MultiDimValue(slavedescription::getDataType(slaveDescription, vrToUpdate),
                                                       values[vrToUpdate]->getBaseSize(), newDimensions);
            } else {
                updatedStructure[vrToUpdate] = new MultiDimValue(slavedescription::getDataType(slaveDescription, vrToUpdate),
                                                                 values[vrToUpdate]->getBaseSize(), newDimensions);
            }
        }
    }

    void checkForUpdatedStructure(uint64_t valueReference) {
        if (updatedStructure.count(valueReference)) {
            delete values[valueReference];
            values[valueReference] = updatedStructure[valueReference];
            updatedStructure.erase(valueReference);
        }
    }

    virtual void computingFinished() = 0;

    virtual void stoppingFinished() = 0;

    virtual void preparingFinished() = 0;

    virtual void configuringFinished() = 0;

    virtual void realtimeStepFinished() = 0;

    virtual void initializingFinished() = 0;

    virtual void synchronizingFinished() = 0;

    virtual void consume(const LogTemplate &logTemplate, uint8_t *payload, size_t size) override {
        LogEntry logEntry(logTemplate, payload, size);
        if (generateLogString) {
            logEntry.applyPayloadToString();
        }
        for (const std::function<void(const LogEntry &)> &logListener: logListeners) {
            logListener(logEntry);
        }

        if (logOnNotification[logEntry.getCategory()][logEntry.getLevel()]) {
            DcpPduNtfLog ntfLog = {dcpId, logEntry.getId(), logEntry.getTime(), logEntry.serialize(),
                                   logEntry.serializedSize()};
            driver.send(ntfLog);
        };
        if (logOnRequest[logEntry.getCategory()][logEntry.getLevel()]) {
            logBuffer[logEntry.getCategory()].push_back({logEntry.serialize(), logEntry.serializedSize()});
        } else {
            delete[] payload;
        }

    }
};

#endif /* ACI_LOGIC_DRIVERMANAGERSLAVE_H_ */
