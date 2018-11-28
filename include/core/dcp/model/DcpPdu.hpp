/**************************************************************************//**
 * @file
 * This file describes the ACI PDU.
 * For more information please read the document.
 * @author Desheng Fu, Christian Kater
 ******************************************************************************/

#ifndef ACI_ACIPDU_H_
#define ACI_ACIPDU_H_

#define PDU_LENGTH_INDICATOR_SIZE 4

//Define macros
#define GET_FUN(function, type, offset) inline type& function(){return *((type*)(stream + (offset + PDU_LENGTH_INDICATOR_SIZE)));}
#define GET_FUN_PTR(function, type, offset) inline type* function(){return ((type*)(stream + (offset + PDU_LENGTH_INDICATOR_SIZE)));}
#define SIZE_HANDLING(size) virtual bool isSizeCorrect(){return this->stream_size - PDU_LENGTH_INDICATOR_SIZE == size;} \
    virtual size_t getCorrectSize(){return size;}


#define CHECK_TYPE_ID

//Include files

#include "dcp/model/DcpConstants.hpp"
#include "dcp/helper/Helper.hpp"


#include <cstdint>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <regex>
#include "dcp/helper/Helper.hpp"
class DcpPdu;
class DcpPduBasicStateTransition;
class DcpPduRun;
class DcpPduBasic;
class DcpPduNtfStateChanged;
class DcpPduDoStep;
class DcpPduSetTimeRes;
class DcpPduSetSteps;
class DcpPduRegister;
class DcpPduConfigInput;
class DcpPduConfigOutput;
class DcpPduDatInputOutput;
class DcpPduAck;
class DcpPduNack;
class DcpPduNack;
class DcpPduStateAck;
class RSP_log_ack;
class DcpPduSetNetworkInformationEthernet;
class DcpPduInfLog;



/**
 * This class describes the DCP PDU.
 * @author Desheng Fu, Christian Kater
 */
class DcpPdu {
public:

    /**
     * Creates a DcpPdu from existing byte array.
     * @param stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
     * @param stream_size number of bytes in stream
     */
    DcpPdu(unsigned char *stream, size_t pduSize) {
        this->stream = stream;
        setPduSize(pduSize);
        this->deleteStream = false;
    }


    /**
     * Get the type id.
     * @return the type id.
     */
    GET_FUN(getTypeId, DcpPduType, 0)

    ~DcpPdu() {
        if (deleteStream) {
            delete stream;
        }
    }

    /**
     * Serialize the DcpPdu
     * @return byte array containing all data with respect to Aci specification.
     */
    unsigned char *serialize() {
        return stream;
    }

    /**
    * Serialize the DcpPdu
    * @return byte array containing all data with respect to Aci specification.
    */
    unsigned char *serializePdu() {
        return stream + PDU_LENGTH_INDICATOR_SIZE;
    }

    /**
     * Number of bytes in serialized DcpPdu
     * @return size of serialized DcpPdu in bytes
     */
    size_t getSerializedSize() {
        return stream_size;
    }

    /**
     * Number of bytes in serialized DcpPdu
     * @return size of serialized DcpPdu in bytes
     */
    size_t getPduSize() {
        return stream_size - PDU_LENGTH_INDICATOR_SIZE;
    }

    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * Special case type_id == DATA: This only checks if size greate than fixed part.
     * @return stream_size is equal to the in the standard defined size
     */
    virtual bool isSizeCorrect() { return false; };

    virtual size_t getCorrectSize() { return 0; };

    /**
     * Returns a string with the hexadecimal representation of each byte of the DcpPdu
     * @return DcpPdu in hex
     */
    std::string getAsHex() {
        std::stringstream hs;
        for (size_t i = 4; i < stream_size; ++i)
            hs << std::hex << std::setw(2) << std::setfill('0')
               << (int) stream[i] << " ";
        return hs.str();
    }

    std::string to_string() {
        std::ostringstream oss;
        operator<<(oss);
        return oss.str();
    }

    /**
      * Writes the Pdu in a human readable format to the given stream.
      * @param os stream to write on.
      */
    virtual std::ostream &operator<<(std::ostream &os) {
        os << "type_id=" << getTypeId();
        return os;
    }

    void setPduSize(size_t pduSize) {
        this->stream_size = pduSize + PDU_LENGTH_INDICATOR_SIZE;
        *((uint32_t*) this->stream) = pduSize;
    }




protected:
    /**
     * byte array containg pdu data
     */
    unsigned char *stream;
    /**
     * size of pdu byte stream
     */
    size_t stream_size;
    /**
     * indicates weather stream should be deleted on distruction or not
     */
    bool deleteStream;

    DcpPdu() {}

    /**
     * creates an new DcpPdu with all fields are 0
     * @param stream_size how much bytes are needed for the DcpPdu
     * @param type type of DcpPdu
     */
    DcpPdu(size_t pduSize, DcpPduType type) {
        stream = new unsigned char[pduSize + PDU_LENGTH_INDICATOR_SIZE];
        getTypeId() = type;
        setPduSize(pduSize);
        this->deleteStream = true;
    }



};


/**
 * This class decscribes the structure for the Pdus "INF_state", "INF_error", CFG_clear_config.
 */

class DcpPduBasic : public DcpPdu {
public:

    /**
     * Get the pdu_seq_id.
     *@return the pdu seq id.
     */
    GET_FUN(getPduSeqId, uint16_t, 1)

    /**
     * Get the receiver.
     *@return the receiver.
     */
    GET_FUN(getReceiver, uint8_t, 3)

    /**
    /* Creates a AciPduBasic from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduBasic(unsigned char *stream, size_t stream_size) : DcpPdu(stream, stream_size){}

    /**
     * Creates a new DcpPduBasic object.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     */
    DcpPduBasic(const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver) : DcpPdu(4, type_id) {
        getPduSeqId() = pdu_seq_id;
        getReceiver() = receiver;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPdu::operator<<(os);
        os << " pdu_seq_id=" << getPduSeqId();
        os << " receiver=" << unsigned(getReceiver());
        return os;
    }

    /**
	 * Check if the stream_size is equal to the in the standard defined size.
	 * @return stream_size is equal to the in the standard defined size
	 */
    SIZE_HANDLING(4)

protected:

    DcpPduBasic() {}

    /**
     * Creates a new DcpPduBasic object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     */
    DcpPduBasic(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver)
            : DcpPdu(stream_size, type_id) {
        getPduSeqId() = pdu_seq_id;
        getReceiver() = receiver;
    }
};

/**
 * This class decscribes the structure for the Pdus
 */
class DcpPduInfLog : public DcpPduBasic {
public:

    /**
     * Get the log_category
     *@return the log_category.
     */
    GET_FUN(getLogCategory, uint8_t, 4)


    GET_FUN(getLogMaxNum, uint8_t, 5)


    /**
    /* Creates a DcpPduInfLog from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduInfLog(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new DcpPduInfLog object.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param log_category
     * @param log_max_num
     */
    DcpPduInfLog(const uint16_t pdu_seq_id, const uint8_t receiver, const uint8_t log_category,
                 const uint8_t log_max_num) : DcpPduBasic(6, DcpPduType::INF_log, pdu_seq_id, receiver) {
        getLogCategory() = log_category;
        getLogMaxNum() = log_max_num;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " log_category=" << unsigned(getLogCategory());
        os << " log_max_num=" << unsigned(getLogMaxNum());

        return os;
    }

    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(6)

protected:

    DcpPduInfLog() {}

    /**
     * Creates a new DcpPduInfLog object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param log_category
     * @param log_max_num
     */
    DcpPduInfLog(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver,
                 const uint8_t log_category, const uint8_t log_max_num) : DcpPduBasic(stream_size, type_id, pdu_seq_id,
                                                                                      receiver) {
        getLogCategory() = log_category;
        getLogMaxNum() = log_max_num;
    }
};

/**
 * This class decscribes the structure for the Pdus
 */
class DcpPduSetLogging : public DcpPduBasic {
public:

    /**
     * Get the log_category
     *@return the log_category.
     */
    GET_FUN(getLogCategory, uint8_t, 4)

    GET_FUN(getLogLevel, DcpLogLevel, 5)

    GET_FUN(getLogMode, DcpLogMode, 6)

    /**
    /* Creates a DcpPduInfLog from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduSetLogging(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new DcpPduSetLogging object.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param log_category
     * @param log_max_num
     */
    DcpPduSetLogging(const uint16_t pdu_seq_id, const uint8_t receiver, const uint8_t log_category,
                     const DcpLogLevel log_level, DcpLogMode log_mode) : DcpPduBasic(7, DcpPduType::CFG_set_logging,
                                                                                     pdu_seq_id, receiver) {
        getLogCategory() = log_category;
        getLogLevel() = log_level;
        getLogMode() = log_mode;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " log_category=" << unsigned(getLogCategory());
        os << " log_level=" << getLogLevel();
        os << " log_mode=" << getLogMode();


        return os;
    }

    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(7)


};

/**
 * This class decscribes the structure for the Pdus "STC_initialize", "STC_synchronize", "STC_unregister", "STC_resynchronize", "STC_stop", "STC_reset" & "STC_sendoutputs".
 */
class DcpPduBasicStateTransition : public DcpPduBasic {
public:

    /**
     * Get thet state_id of the slaves state.
     *@return the state_id.
     */
    GET_FUN(getStateId, DcpState, 4)

    /**
    /* Creates a AciPduBasicStateTransition from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduBasicStateTransition(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size) {}

    /**
     * Creates a new DcpPduBasic object.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param state_id the state_id of the slaves state.
     */
    DcpPduBasicStateTransition(const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver,
                               const DcpState state_id) : DcpPduBasic(5, type_id, pdu_seq_id, receiver) {
        getStateId() = state_id;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " state_id=" << getStateId();
        return os;
    }

    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(5)

protected:

    DcpPduBasicStateTransition() {}

    /**
     * Creates a new DcpPduBasic object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param state_id the state_id of the slaves state.

     */
    DcpPduBasicStateTransition(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                               const uint8_t receiver, const DcpState state_id) : DcpPduBasic(stream_size, type_id,
                                                                                              pdu_seq_id, receiver) {
        getStateId() = state_id;
    }
};


/**
 * This class decscribes the structure for the Pdu "STC_run".
 */
class DcpPduRun : public DcpPduBasicStateTransition {
public:

    /**
     * Get the start_time.
     *@return the start time.
     */
    GET_FUN(getStartTime, int64_t, 5)

    /**
    /* Creates a AciPduRun from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduRun(unsigned char *stream, size_t stream_size) : DcpPduBasicStateTransition(stream, stream_size){}

    /**
     * Creates a new AciPduRun object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param start_time the start time.
     * @param state_id the state_id of the slaves state.
     */
    DcpPduRun(const uint16_t pdu_seq_id, const uint8_t receiver, const DcpState state_id, const int64_t start_time) :
            DcpPduBasicStateTransition(13, DcpPduType::STC_run, pdu_seq_id, receiver, state_id) {
        getStartTime() = start_time;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " start_time=" << getStartTime();
        return os;
    }

    /**
	 * Check if the stream_size is equal to the in the standard defined size.
	 * @return stream_size is equal to the in the standard defined size
	 */
    SIZE_HANDLING(13)

protected:
    /**
     * Creates a new AciPduRun object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param start_time the start time.
     * @param state_id the state_id of the slaves state.

     */
    DcpPduRun(const int stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver,
              const DcpState state_id, const int64_t start_time) : DcpPduBasicStateTransition(stream_size, type_id,
                                                                                              pdu_seq_id, receiver,
                                                                                              state_id) {
        getStartTime() = start_time;
    }
};

/**
 * This class decscribes the structure for the Pdu "STC_dostep".
 */
class DcpPduDoStep : public DcpPduBasicStateTransition {
public:

    /**
     * Get the steps.
     *@return the steps.
     */
    GET_FUN(getSteps, uint32_t, 5)

    /**
    /* Creates a AciPduDoStep from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduDoStep(unsigned char *stream, size_t stream_size) : DcpPduBasicStateTransition(stream, stream_size){}

    /**
     * Creates a new AciPduDoStep object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param state_id the state_id of the slaves state.
     * @param steps the steps.
     */
    DcpPduDoStep(const uint16_t pdu_seq_id, const uint8_t receiver, const DcpState state_id, const uint32_t steps) :
            DcpPduBasicStateTransition(9, DcpPduType::STC_do_step, pdu_seq_id, receiver, state_id) {
        getSteps() = steps;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " steps=" << getSteps();
        return os;
    }

    /**
   * Check if the stream_size is equal to the in the standard defined size.
   * @return stream_size is equal to the in the standard defined size
   */
    SIZE_HANDLING(9)

protected:
    /**
     * Creates a new AciPduDoStep object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param state_id the state_id of the slaves state.
     * @param steps the steps.
     */
    DcpPduDoStep(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver,
                 const DcpState state_id, const uint32_t steps) : DcpPduBasicStateTransition(stream_size, type_id,
                                                                                             pdu_seq_id, receiver,
                                                                                             state_id) {
        getSteps() = steps;
    }
};

/**
 * This class decscribes the structure for the Pdu "NTF_state_changed".
 */
class DcpPduNtfStateChanged : public DcpPdu {
public:

    /**
    * Get the sender.
    *@return the sender.
    */
    GET_FUN(getSender, uint8_t, 1)

    /**
    * Get the sender.
    *@return the sender.
    */
    GET_FUN(getStateId, DcpState, 2)

    /**
    /* Creates a DcpStateChangedNotification from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduNtfStateChanged(unsigned char *stream, size_t stream_size) : DcpPdu(stream, stream_size){}

    /**
     * Creates a new DcpStateChangedNotification object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param numerator the numerator.
     * @param denominator the denominator.
     */
    DcpPduNtfStateChanged(const uint8_t sender, const DcpState stateId) :
            DcpPdu(3, DcpPduType::NTF_state_changed) {
        getSender() = sender;
        getStateId() = stateId;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPdu::operator<<(os);
        os << " sender=" << unsigned(getSender());
        os << " state_id=" << getStateId();
        return os;
    }

    /**
       * Check if the stream_size is equal to the in the standard defined size.
       * @return stream_size is equal to the in the standard defined size
       */
    SIZE_HANDLING(3)
};


/**
 * This class decscribes the structure for the Pdu "CFG_set_time_res".
 */
class DcpPduSetTimeRes : public DcpPduBasic {
public:

    /**
     * Get the numerator.
     *@return the numerator.
     */
    GET_FUN(getNumerator, uint32_t, 4)

    /**
     * Get the denominator.
     *@return the denominator.
     */
    GET_FUN(getDenominator, uint32_t, 8)

    /**
    /* Creates a AciPduSetTimeRes from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduSetTimeRes(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new AciPduSetTimeRes object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param numerator the numerator.
     * @param denominator the denominator.
     */
    DcpPduSetTimeRes(const uint16_t pdu_seq_id, const uint8_t receiver, const uint32_t numerator,
                     const uint32_t denominator) :
            DcpPduBasic(12, DcpPduType::CFG_set_time_res, pdu_seq_id, receiver) {
        getNumerator() = numerator;
        getDenominator() = denominator;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " numerator=" << getNumerator();
        os << " denominator=" << getDenominator();
        return os;
    }

    /**
   * Check if the stream_size is equal to the in the standard defined size.
   * @return stream_size is equal to the in the standard defined size
   */
    SIZE_HANDLING(12)

protected:
    /**
     * Creates a new AciPduSetTimeRes object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param numerator the numerator.
     * @param denominator the denominator.
     */
    DcpPduSetTimeRes(const int stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver,
                     const uint32_t numerator, const uint32_t denominator) : DcpPduBasic(stream_size, type_id,
                                                                                         pdu_seq_id, receiver) {
        getNumerator() = numerator;
        getDenominator() = denominator;
    }
};

/**
 * This class decscribes the structure for the Pdu "CFG_set_steps".
 */
class DcpPduSetSteps : public DcpPduBasic {
public:

    /**
     * Get the steps.
     *@return the steps.
     */
    GET_FUN(getSteps, uint32_t, 4)

    /**
     * Get the data_id.
     *@return the data id.
     */
    GET_FUN(getDataId, uint16_t, 8)

    /**
    /* Creates a AciPduSetCommStepSize from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduSetSteps(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new AciPduSetCommStepSize object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param steps the steps.
     * @param data_id the data id.
     */
    DcpPduSetSteps(const uint16_t pdu_seq_id, const uint8_t receiver, const uint32_t steps, const uint16_t data_id) :
            DcpPduBasic(10, DcpPduType::CFG_set_steps, pdu_seq_id, receiver) {
        getSteps() = steps;
        getDataId() = data_id;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " steps=" << getSteps();
        os << " data_id=" << getDataId();
        return os;
    }

    /**
   * Check if the stream_size is equal to the in the standard defined size.
   * @return stream_size is equal to the in the standard defined size
   */
    SIZE_HANDLING(10)

protected:
    /**
     * Creates a new AciPduSetCommStepSize object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param steps the steps.
     * @param data_id the data id.
     */
    DcpPduSetSteps(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                   const uint8_t receiver, const uint32_t steps, const uint16_t data_id) : DcpPduBasic(stream_size,
                                                                                                       type_id,
                                                                                                       pdu_seq_id,
                                                                                                       receiver) {
        getSteps() = steps;
        getDataId() = data_id;
    }
};

/**
* This class decscribes the structure for the Pdu "CFG_set_data_id".
*/
class DcpPduSetScope : public DcpPduBasic {
public:


    /**
     * Get the data_id.
     *@return the data id.
     */
    GET_FUN(getDataId, uint16_t, 4)

    GET_FUN(getScope, DcpScope, 6);

    /**
    /* Creates a AciPduSetCommStepSize from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduSetScope(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new AciPduSetCommStepSize object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param steps the steps.
     * @param data_id the data id.
     */
    DcpPduSetScope(const uint16_t pdu_seq_id, const uint8_t receiver, const uint16_t data_id, const DcpScope scope) :
            DcpPduBasic(7, DcpPduType::CFG_set_scope, pdu_seq_id, receiver) {
        getDataId() = data_id;
        getScope() = scope;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " data_id=" << getDataId();
        os << " scope=" << getScope();

        return os;
    }

    /**
   * Check if the stream_size is equal to the in the standard defined size.
   * @return stream_size is equal to the in the standard defined size
   */
    SIZE_HANDLING(7)
};

/**
 * This class decscribes the structure for the Pdu "STC_register".
 */
class DcpPduRegister : public DcpPduBasicStateTransition {
public:

    /**
     * Get the slave_uuid .
     *@return the slave uuid .
     */
    GET_FUN(getSlaveUuid, uint128_t, 5)

    /**
     * Get the op_mode.
     *@return the op mode.
     */
    GET_FUN(getOpMode, DcpOpMode, 21)


    GET_FUN(getMajorVersion, uint8_t, 22)

    GET_FUN(getMinorVersion, uint8_t, 23)


    /**
    /* Creates a AciPduRegister from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduRegister(unsigned char *stream, size_t stream_size) : DcpPduBasicStateTransition(stream, stream_size){}

    /**
     * Creates a new AciPduRegister object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param state_id the state_id of the slaves state.
     * @param slave_uuid  the slave uuid .
     * @param op_mode the op mode.
     */
    DcpPduRegister(const uint16_t pdu_seq_id, const uint8_t receiver, const DcpState state_id,
                   const uint128_t slave_uuid, const DcpOpMode op_mode, const uint8_t majorversion,
                   const uint8_t minorVersion) :
            DcpPduBasicStateTransition(24, DcpPduType::STC_register, pdu_seq_id, receiver, state_id) {
        getSlaveUuid() = slave_uuid;
        getOpMode() = op_mode;
        getMajorVersion() = majorversion;
        getMinorVersion() = minorVersion;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " slave_uuid =" << convertToUUIDStr(getSlaveUuid());
        os << " state_id=" << getStateId();
        os << " op_mode=" << getOpMode();
        os << " major_version=" << unsigned(getMajorVersion());
        os << " minor_version=" << unsigned(getMinorVersion());
        return os;
    }

    /**
   * Check if the stream_size is equal to the in the standard defined size.
   * @return stream_size is equal to the in the standard defined size
   */
    SIZE_HANDLING(24)

protected:
    /**
     * Creates a new AciPduRegister object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param slave_uuid  the slave uuid .
     * @param op_mode the op mode.
     */
    DcpPduRegister(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                   const uint8_t receiver, const DcpState state_id, const uint128_t slave_uuid, const DcpOpMode op_mode)
            : DcpPduBasicStateTransition(stream_size, type_id, pdu_seq_id, receiver, state_id) {
        getSlaveUuid() = slave_uuid;
        getOpMode() = op_mode;
    }
};

/**
 * This class decscribes the structure for the Pdu "CFG_config_input".
 */
class DcpPduConfigInput : public DcpPduBasic {
public:

    /**
     * Get the data_id .
     *@return the data id .
     */
    GET_FUN(getDataId, uint16_t, 4)

    /**
     * Get the pos.
     *@return the pos.
     */
    GET_FUN(getPos, uint16_t, 6)

    /**
     * Get the target_vr.
     *@return the target vr.
     */
    GET_FUN(getTargetVr, uint64_t, 8)

    /**
     * Get the source_data_type.
     *@return the source data type.
     */
    GET_FUN(getSourceDataType, DcpDataType, 16)

    /**
    /* Creates a AciPduConfigInput from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduConfigInput(unsigned char *stream, size_t stream_size): DcpPduBasic(stream, stream_size) {}

    /**
     * Creates a new AciPduConfigInput object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param pos the pos.
     * @param target_vr the target vr.
     * @param source_data_type the source data type.
     */
    DcpPduConfigInput(const uint16_t pdu_seq_id, const uint8_t receiver, const uint16_t data_id, const uint16_t pos,
                      const uint64_t target_vr, const DcpDataType source_data_type) :
            DcpPduBasic(17, DcpPduType::CFG_config_input, pdu_seq_id, receiver) {
        getDataId() = data_id;
        getPos() = pos;
        getTargetVr() = target_vr;
        getSourceDataType() = source_data_type;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " data_id =" << getDataId();
        os << " pos=" << getPos();
        os << " target_vr=" << getTargetVr();
        os << " source_data_type=" << getSourceDataType();
        return os;
    }

    /**
   * Check if the stream_size is equal to the in the standard defined size.
   * @return stream_size is equal to the in the standard defined size
   */
    SIZE_HANDLING(17)

protected:
    /**
     * Creates a new AciPduConfigInput object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param pos the pos.
     * @param target_vr the target vr.
     * @param source_data_type the source data type.
     */
    DcpPduConfigInput(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                      const uint8_t receiver, const uint16_t data_id, const uint16_t pos, const uint64_t target_vr,
                      const DcpDataType source_data_type) : DcpPduBasic(stream_size, type_id, pdu_seq_id, receiver) {
        getDataId() = data_id;
        getPos() = pos;
        getTargetVr() = target_vr;
        getSourceDataType() = source_data_type;
    }
};

/**
 * This class decscribes the structure for the Pdu "CFG_config_output".
 */
class DcpPduConfigOutput : public DcpPduBasic {
public:

    /**
     * Get the data_id .
     *@return the data id .
     */
    GET_FUN(getDataId, uint16_t, 4)

    /**
     * Get the pos.
     *@return the pos.
     */
    GET_FUN(getPos, uint16_t, 6)

    /**
     * Get the source_vr.
     *@return the source vr.
     */
    GET_FUN(getSourceVr, uint64_t, 8)

    /**
    /* Creates a AciPduConfigOutput from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduConfigOutput(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new AciPduConfigOutput object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param pos the pos.
     * @param source_vr the source vr.
     */
    DcpPduConfigOutput(const uint16_t pdu_seq_id, const uint8_t receiver, const uint16_t data_id, const uint16_t pos,
                       const uint64_t source_vr) :
            DcpPduBasic(16, DcpPduType::CFG_config_output, pdu_seq_id, receiver) {
        getDataId() = data_id;
        getPos() = pos;
        getSourceVr() = source_vr;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " data_id =" << getDataId();
        os << " pos=" << getPos();
        os << " source_vr=" << getSourceVr();
        return os;
    }
    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(16)

protected:
    /**
     * Creates a new AciPduConfigOutput object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param pos the pos.
     * @param source_vr the source vr.
     */
    DcpPduConfigOutput(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                       const uint8_t receiver, const uint16_t data_id, const uint16_t pos, const uint64_t source_vr)
            : DcpPduBasic(stream_size, type_id, pdu_seq_id, receiver) {
        getDataId() = data_id;
        getPos() = pos;
        getSourceVr() = source_vr;
    }
};

/**
 * This class decscribes the structure for the Pdu "CFG_config_tunable_parameter".
 */
class DcpPduConfigTunableParameter : public DcpPduBasic {
public:

    /**
     * Get the data_id .
     *@return the data id .
     */
    GET_FUN(getParamId, uint16_t, 4)

    /**
     * Get the pos.
     *@return the pos.
     */
    GET_FUN(getPos, uint16_t, 6)

    /**
     * Get the target_vr.
     *@return the target vr.
     */
    GET_FUN(getParameterVr, uint64_t, 8)

    /**
     * Get the source_data_type.
     *@return the source data type.
     */
    GET_FUN(getSourceDataType, DcpDataType, 16)

    /**
    /* Creates a AciPduConfigTunableParameter from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduConfigTunableParameter(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Creates a new AciPduConfigTunableParameter object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param param_id  the param_id .
     * @param pos the pos.
     * @param parameter_vr the parameter_vr.
     * @param source_data_type the source data type.
     */
    DcpPduConfigTunableParameter(const uint16_t pdu_seq_id, const uint8_t receiver, const uint16_t param_id,
                                 const uint16_t pos, const uint64_t parameter_vr, const DcpDataType source_data_type) :
            DcpPduBasic(17, DcpPduType::CFG_config_tunable_parameter, pdu_seq_id, receiver) {
        getParamId() = param_id;
        getPos() = pos;
        getParameterVr() = parameter_vr;
        getSourceDataType() = source_data_type;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " param_id=" << getParamId();
        os << " pos=" << getPos();
        os << " parameter_vr=" << getParameterVr();
        os << " source_data_type=" << getSourceDataType();
        return os;
    }

    /**
   * Check if the stream_size is equal to the in the standard defined size.
   * @return stream_size is equal to the in the standard defined size
   */
    SIZE_HANDLING(17)

protected:
    /**
     * Creates a new AciPduConfigTunableParameter object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param param_id  the param_id.
     * @param pos the pos.
     * @param parameter_vr the parameter_vr.
     * @param source_data_type the source data type.
     */
    DcpPduConfigTunableParameter(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                                 const uint8_t receiver, const uint16_t param_id, const uint16_t pos,
                                 const uint64_t parameter_vr, const DcpDataType source_data_type) : DcpPduBasic(
            stream_size, type_id, pdu_seq_id, receiver) {
        getParamId() = param_id;
        getPos() = pos;
        getParameterVr() = parameter_vr;
        getSourceDataType() = source_data_type;
    }
};


/**
 * This class decscribes the structure for the Pdu "CFG_config_tunable_parameter".
 */
class DcpPduSetParameter : public DcpPduBasic {
public:

    /**
     * Get the data_id .
     *@return the data id .
     */
    GET_FUN(getParameterVr, uint64_t, 4)

    GET_FUN(getSourceDataType, DcpDataType, 12)

    /**
     * Get the payload.
     *@return the payload.
     */
    GET_FUN_PTR(getConfiguration, uint8_t, 13)


    /**
    /* Creates a AciPduSetParameter from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduSetParameter(unsigned char *stream, size_t stream_size) :DcpPduBasic(stream, stream_size) {}

    /**
     * Creates a new AciPduSetParameter object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param parameter_vr  the parameters value reference.
     * @param value the value.
     * @param value_size the size of value.
     */
    DcpPduSetParameter(const uint16_t pdu_seq_id, const uint8_t receiver, const uint64_t parameter_vr,
                       const DcpDataType sourceDataType, const uint8_t *configuration, const size_t configuration_size)
            :
            DcpPduBasic(13 + configuration_size, DcpPduType::CFG_set_parameter, pdu_seq_id, receiver) {
        getParameterVr() = parameter_vr;
        getSourceDataType() = sourceDataType;
        memcpy(getConfiguration(), configuration, configuration_size);
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " parameter_vr=" << getParameterVr();
        os << " source_data_type=" << getSourceDataType();
        os << " configuration=";
        for (size_t i = 13; i < stream_size; ++i)
            os << std::hex << std::setw(2) << std::setfill('0')
               << (int) stream[i] << " ";

        return os;
    }

    /**
      * Check if the stream_size is at least as much as the in the standard defined fixed part of an PARAM PDU.
      * @return tream_size is at least as much as the in the standard defined fixed part of an PARAM PDU
      */
    virtual bool isSizeCorrect() {
        return this->stream_size >= 13;
    }

    /**
      * Returns the minimum stream_size as the in the standard defined fixed part of an PARAM PDU.
      * @return the minimum stream_size as the in the standard defined fixed part of an PARAM PDU.
      */
    virtual size_t getCorrectSize() {
        return 13;
    }

protected:

};

/**
 * This class decscribes the structure for the Pdu "DAT_input_output".
 */
class DcpPduDatInputOutput : public DcpPdu {
public:

    /**
     * Get the pdu_seq_id.
     *@return the pdu seq id.
     */
    GET_FUN(getPduSeqId, uint16_t, 1)

    /**
     * Get the data_id.
     *@return the data id.
     */
    GET_FUN(getDataId, uint16_t, 3)

    /**
     * Get the payload.
     *@return the payload.
     */
    GET_FUN_PTR(getPayload, uint8_t, 5)



    /**
    /* Creates a AciPduData from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduDatInputOutput(unsigned char *stream, size_t stream_size) : DcpPdu(stream, stream_size){}

    /**
     * Creates a new AciPduData object.
     * @param pdu_seq_id the pdu seq id.
     * @param data_id the data id.
     * @param payload the payload from which the data will be copied into this pdu.
     * @param payload_size the length in bytes of the payload.

     */
    DcpPduDatInputOutput(const uint16_t pdu_seq_id, const uint16_t data_id, const uint8_t *payload,
                         const size_t payload_size) :
            DcpPdu(5 + payload_size, DcpPduType::DAT_input_output) {
        getPduSeqId() = pdu_seq_id;
        getDataId() = data_id;
        memcpy(getPayload(), payload, payload_size);
    }

    /**
	 * Creates a new AciPduData object.
	 * @param pdu_seq_id the pdu seq id.
	 * @param data_id the data id.
	 * @param payload the payload.
	 */
    DcpPduDatInputOutput(const uint16_t pdu_seq_id, uint16_t data_id, uint16_t payloadSize) :
            DcpPdu(5 + payloadSize, DcpPduType::DAT_input_output) {
        getPduSeqId() = pdu_seq_id;
        getDataId() = data_id;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPdu::operator<<(os);
        os << " pdu_seq_id=" << getPduSeqId();
        os << " data_id=" << getDataId();

        os << " payload=";
        for (size_t i = 5; i < stream_size; ++i)
            os << std::hex << std::setw(2) << std::setfill('0')
               << (int) stream[i] << " ";

        return os;
    }

    /**
   * Check if the stream_size is at least as much as the in the standard defined fixed part of an DATA PDU.
   * @return tream_size is at least as much as the in the standard defined fixed part of an DATA PDU
   */
    virtual bool isSizeCorrect() {
        return this->stream_size >= 5;
    }

    /**
      * Returns the minimum stream_size as the in the standard defined fixed part of an DAT_input_output PDU.
      * @return the minimum stream_size as the in the standard defined fixed part of an DAT_input_output PDU.
      */
    virtual size_t getCorrectSize() {
        return 5;
    }
};


/**
 * This class decscribes the structure for the Pdu "DAT_parameter".
 */
class DcpPduDatParameter : public DcpPdu {
public:

    /**
     * Get the pdu_seq_id.
     *@return the pdu seq id.
     */
    GET_FUN(getPduSeqId, uint16_t, 1)

    /**
     * Get the data_id.
     *@return the data id.
     */
    GET_FUN(getParamId, uint16_t, 3)

    /**
     * Get the payload.
     *@return the payload.
     */
    GET_FUN_PTR(getConfiguration, uint8_t, 5)

    /**
    /* Creates a AciPduParam from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduDatParameter(unsigned char *stream, size_t stream_size) : DcpPdu(stream, stream_size){}

    /**
     * Creates a new AciPduParam object.
     * @param pdu_seq_id the pdu seq id.
     * @param param_id the param_id.
     * @param payload the payload from which the data will be copied into this pdu.
     * @param payload_size the length in bytes of the payload.

     */
    DcpPduDatParameter(const uint16_t pdu_seq_id, const uint16_t param_id, const uint8_t *configuration,
                       const size_t configuration_size) :
            DcpPdu(5 + configuration_size, DcpPduType::DAT_input_output) {
        getPduSeqId() = pdu_seq_id;
        getParamId() = param_id;
        memcpy(getConfiguration(), configuration, configuration_size);
    }

    /**
     * Creates a new AciPduParam object.
     * @param pdu_seq_id the pdu seq id.
     * @param param_id the data id.
     * @param payload the payload.
     */
    DcpPduDatParameter(const uint16_t pdu_seq_id, uint16_t param_id, uint16_t configuration_size) :
            DcpPdu(5 + configuration_size, DcpPduType::DAT_input_output) {
        getPduSeqId() = pdu_seq_id;
        getParamId() = param_id;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPdu::operator<<(os);
        os << " pdu_seq_id=" << getPduSeqId();
        os << " param_id=" << getParamId();

        os << " payload=";
        for (size_t i = 5; i < stream_size; ++i)
            os << std::hex << std::setw(2) << std::setfill('0')
               << (int) stream[i] << " ";

        return os;
    }

    /**
   * Check if the stream_size is at least as much as the in the standard defined fixed part of an DATA PDU.
   * @return tream_size is at least as much as the in the standard defined fixed part of an DATA PDU
   */
    virtual bool isSizeCorrect() {
        return this->stream_size >= 5;
    }

    /**
      * Returns the minimum stream_size as the in the standard defined fixed part of an DAT_parameter PDU.
      * @return the minimum stream_size as the in the standard defined fixed part of an DAT_parameter PDU.
      */
    virtual size_t getCorrectSize() {
        return 5;
    }
};

/**
 * This class decscribes the structure for the Pdu "RSP_ack".
 */
class DcpPduAck : public DcpPdu {
public:

    /**
     * Get the sender.
     *@return the sender.
     */
    GET_FUN(getSender, uint8_t, 1)

    /**
     * Get the resp_seq_id.
     *@return the resp seq id.
     */
    GET_FUN(getRespSeqId, uint16_t, 2)

    /**
    /* Creates a AciPduAck from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduAck(unsigned char *stream, size_t stream_size) :DcpPdu(stream, stream_size) {}

    /**
     * Creates a new AciPduAck object.
     * @param sender the sender.
     * @param resp_seq_id the resp seq id.
     */
    DcpPduAck(const uint8_t sender, const uint16_t resp_seq_id) : DcpPdu(4, DcpPduType::RSP_ack) {
        getSender() = sender;
        getRespSeqId() = resp_seq_id;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPdu::operator<<(os);
        os << " sender=" << unsigned(getSender());
        os << " resp_seq_id=" << getRespSeqId();
        return os;
    }

    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(4)

protected:

    DcpPduAck() {}

    /**
     * Creates a new AciPduAck object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param sender the sender.
     * @param resp_seq_id the resp seq id.
     */
    DcpPduAck(const size_t stream_size, const DcpPduType type_id, const uint8_t sender, const uint16_t resp_seq_id)
            : DcpPdu(stream_size, type_id) {
        getSender() = sender;
        getRespSeqId() = resp_seq_id;
    }
};

/**
 * This class decscribes the structure for the Pdus "RSP_nack" & "RSP_error_ack".
 */
class DcpPduNack : public DcpPduAck {
public:

    /**
     * Get the error_code.
     *@return the error code.
     */
    GET_FUN(getErrorCode, DcpError, 4)

    /**
    /* Creates a AciPduNack from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduNack(unsigned char *stream, size_t stream_size) : DcpPduAck(stream, stream_size){}

    /**
     * Creates a new AciPduNack object.
     * @param type_id the type id.
     * @param sender the sender.
     * @param resp_seq_id the resp seq id.
     * @param error_code the error code.
     */
    DcpPduNack(const DcpPduType type, const uint8_t sender, const uint16_t resp_seq_id, const DcpError error_code)
            : DcpPduAck(6, type, sender, resp_seq_id) {
        getErrorCode() = error_code;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduAck::operator<<(os);
        os << " error_code=" << getErrorCode();
        return os;
    }

    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(6)

protected:
    /**
     * Creates a new AciPduNack object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param sender the sender.
     * @param resp_seq_id the resp seq id.
     * @param error_code the error code.
     */
    DcpPduNack(const size_t stream_size, const DcpPduType type_id, const uint8_t sender, const uint16_t resp_seq_id,
               const DcpError error_code) : DcpPduAck(stream_size, type_id, sender, resp_seq_id) {
        getErrorCode() = error_code;
    }
};

/**
 * This class decscribes the structure for the Pdu "MSG_state_ack".
 */
class DcpPduStateAck : public DcpPduAck {
public:

    /**
     * Get the state_id.
     *@return the state id.
     */
    GET_FUN(getStateId, DcpState, 4)

    /**
    /* Creates a AciPduStateAck from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduStateAck(unsigned char *stream, size_t stream_size) : DcpPduAck(stream, stream_size){}

    /**
     * Creates a new AciPduStateAck object.
     * @param sender the sender.
     * @param resp_seq_id the resp seq id.
     * @param state_id the state id.
     */
    DcpPduStateAck(const uint8_t sender, const uint16_t resp_seq_id, const DcpState state_id) :
            DcpPduAck(5, DcpPduType::RSP_state_ack, sender, resp_seq_id) {
        getStateId() = state_id;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduAck::operator<<(os);
        os << " state_id=" << getStateId();
        return os;
    }

    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(5)

protected:
    /**
     * Creates a new AciPduStateAck object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param sender the sender.
     * @param resp_seq_id the resp seq id.
     * @param state_id the state id.
     */
    DcpPduStateAck(const size_t stream_size, const DcpPduType type_id, const uint8_t sender, const uint16_t resp_seq_id,
                   const DcpState state_id) : DcpPduAck(stream_size, type_id, sender, resp_seq_id) {
        getStateId() = state_id;
    }
};

/**
 * This class decscribes the structure for the Pdus "CFG_set_target_network_information" & "CFG_set_source_network_information".
 */
class DcpPduSetNetworkInformation : public DcpPduBasic {
public:

    /**
     * Get the data_id .
     *@return the data id .
     */
    GET_FUN(getDataId, uint16_t, 4)

    /**
     * Get the transport_protocol.
     *@return the transport protocol.
     */
    GET_FUN(getTransportProtocol, DcpTransportProtocol, 6)


    GET_FUN_PTR(getNetworkInformation, uint8_t, 7)

    /**
    /* Creates a AciPduSetNetworkInformation from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduSetNetworkInformation(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " data_id =" << getDataId();
        os << " transport_protocol=" << getTransportProtocol();
        return os;
    }


protected:

    DcpPduSetNetworkInformation() {}

    /**
     * Creates a new AciPduSetNetworkInformation object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param transport_protocol the transport protocol.
     */
    DcpPduSetNetworkInformation(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                                const uint8_t receiver, const uint16_t data_id,
                                const DcpTransportProtocol transport_protocol) : DcpPduBasic(stream_size, type_id,
                                                                                             pdu_seq_id, receiver) {
        getDataId() = data_id;
        getTransportProtocol() = transport_protocol;
    }

    SIZE_HANDLING(7)
};

/**
 * This class decscribes the structure for the Pdus "CFG_set_target_network_information" for UDP.
 */
class DcpPduSetNetworkInformationEthernet : public DcpPduSetNetworkInformation {
public:

    /**
     * Get the port.
     *@return the port.
     */
    GET_FUN(getPort, uint16_t, 7)

    /**
    * Get the ip_address .
    *@return the ip address .
    */
    GET_FUN(getIpAddress, uint32_t, 9)

    /**
    /* Creates a AciPduSetNetworkInformationUdp from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduSetNetworkInformationEthernet(unsigned char *stream, size_t stream_size) : DcpPduSetNetworkInformation(stream, stream_size){}

    /**
     * Creates a new AciPduSetTargetNetworkInformationUdp object.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param transport_protocol the transport protocol.
     * @param ip_address  the ip address .
     * @param port the port.
     */
    DcpPduSetNetworkInformationEthernet(const DcpPduType type_id, const uint16_t pdu_seq_id, const uint8_t receiver,
                                   const uint16_t data_id, const uint16_t port, const uint32_t ip_address, DcpTransportProtocol transportProtocol) :
            DcpPduSetNetworkInformation(13, type_id, pdu_seq_id, receiver, data_id, transportProtocol) {
        getIpAddress() = ip_address;
        getPort() = port;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduSetNetworkInformation::operator<<(os);
        os << " ip_address =" << getIpAddress();
        os << " port=" << getPort();
        return os;
    }

    /**
     * Check if the stream_size is equal to the in the standard defined size.
     * @return stream_size is equal to the in the standard defined size
     */
    SIZE_HANDLING(13)

protected:
    /**
     * Creates a new AciPduSetNetworkInformationUdp object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param transport_protocol the transport protocol.
     * @param ip_address  the ip address .
     * @param port the port.
     */
    DcpPduSetNetworkInformationEthernet(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                                   const uint8_t receiver, const uint16_t data_id,
                                   const DcpTransportProtocol transport_protocol, const uint32_t ip_address,
                                   const uint16_t port) :
            DcpPduSetNetworkInformation(stream_size, type_id, pdu_seq_id, receiver, data_id, transport_protocol) {
        getIpAddress() = ip_address;
        getPort() = port;
    }
};


/**
 * This class decscribes the structure for the Pdus "CFG_set_target_network_information" & "MSG_set_source_network_information".
 */
class DcpPduSetParamNetworkInformation : public DcpPduBasic {
public:

    /**
     * Get the data_id .
     *@return the data id .
     */
    GET_FUN(getParamId, uint16_t, 4)

    /**
     * Get the transport_protocol.
     *@return the transport protocol.
     */
    GET_FUN(getTransportProtocol, DcpTransportProtocol, 6)

    GET_FUN_PTR(getNetworkInformation, uint8_t, 7)

    /**
    /* Creates a AciPduSetParamNetworkInformation from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduSetParamNetworkInformation(unsigned char *stream, size_t stream_size) : DcpPduBasic(stream, stream_size){}

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduBasic::operator<<(os);
        os << " param_id=" << getParamId();
        os << " transport_protocol=" << getTransportProtocol();
        return os;
    }


protected:

    DcpPduSetParamNetworkInformation() {}

    /**
     * Creates a new AciPduSetParamNetworkInformation object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param param_id  the param_id .
     * @param transport_protocol the transport protocol.
     */
    DcpPduSetParamNetworkInformation(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                                     const uint8_t receiver, const uint16_t param_id,
                                     const DcpTransportProtocol transport_protocol) : DcpPduBasic(stream_size, type_id,
                                                                                                  pdu_seq_id,
                                                                                                  receiver) {
        getParamId() = param_id;
        getTransportProtocol() = transport_protocol;
    }
};

/**
 * This class decscribes the structure for the Pdus "CFG_set_source_network_information" for UDP.
 */
class DcpPduSetParamNetworkInformationEthernet : public DcpPduSetParamNetworkInformation {
public:

    /**
     * Get the port.
     *@return the port.
     */
    GET_FUN(getPort, uint16_t, 7)

    /**
    * Get the ip_address .
    *@return the ip address .
    */
    GET_FUN(getIpAddress, uint32_t, 9)

    /**
    /* Creates a AciPduSetSourceNetworkInformationUdp from existing byte array.
    /* stream byte array containg pdu data. Will not be deleted on DcpPdu destructor.
    /* stream_size number of bytes in stream.
    */
    DcpPduSetParamNetworkInformationEthernet(unsigned char *stream, size_t stream_size) : DcpPduSetParamNetworkInformation(stream, stream_size){}

    /**
     * Creates a new AciPduSetSourceNetworkInformationUdp object.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param param_id  the param_id .
     * @param transport_protocol the transport protocol.
     * @param ip_address  the ip address .
     * @param port the port.
     */
    DcpPduSetParamNetworkInformationEthernet(const uint16_t pdu_seq_id, const uint8_t receiver, const uint16_t param_id,
                                        const uint16_t port, const uint32_t ipAddress, DcpTransportProtocol transportProtocol) :
            DcpPduSetParamNetworkInformation(13, DcpPduType::CFG_set_param_network_information, pdu_seq_id, receiver,
                                             param_id, transportProtocol) {
        getPort() = port;
        getIpAddress() = ipAddress;
    }

    /**
     * Writes the Pdu in a human readable format to the given stream.
     * @param os stream to write on.
     */
    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduSetParamNetworkInformation::operator<<(os);
        os << " port=" << getPort();
        os << " ip_address=" << getIpAddress();


        return os;
    }

    /**
    * Check if the stream_size is equal to the in the standard defined size.
    * @return stream_size is equal to the in the standard defined size
    */
    SIZE_HANDLING(13)

protected:
    /**
     * Creates a new AciPduSetNetworkInformationUdp object.
     * @param stream_size the number of bytes of the Pdu.
     * @param type_id the type id.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param transport_protocol the transport protocol.
     * @param ip_address  the ip address .
     * @param port the port.
     */
    DcpPduSetParamNetworkInformationEthernet(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                                        const uint8_t receiver, const uint16_t param_id,
                                        const DcpTransportProtocol transport_protocol, const uint16_t port) :
            DcpPduSetParamNetworkInformation(stream_size, type_id, pdu_seq_id, receiver, param_id, transport_protocol) {
        getPort() = port;
    }
};

class DcpPduLogAck : public DcpPduAck {
public:
    GET_FUN_PTR(getPayload, uint8_t, 4)

    DcpPduLogAck(unsigned char *stream, size_t stream_size) : DcpPduAck(stream, stream_size){}

    DcpPduLogAck(const uint8_t sender, const uint16_t resp_seq_id, uint8_t *payload, size_t payload_size) : DcpPduAck(
            4 + payload_size, DcpPduType::RSP_log_ack, sender, resp_seq_id) {
        memcpy(getPayload(), payload, payload_size);
    }

    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPduAck::operator<<(os);
        os << " payload=";
        for (size_t i = 4; i < stream_size; ++i)
            os << std::hex << std::setw(2) << std::setfill('0')
               << (int) stream[i] << " ";

        return os;
    }

    /**
  * Check if the stream_size is at least as much as the in the standard defined fixed part of an RSP_log_ack PDU.
  * @return stream_size is at least as much as the in the standard defined fixed part of an RSP_log_ack PDU
  */
    virtual bool isSizeCorrect() {
        return this->stream_size >= 4;
    }

    /**
      * Returns the minimum stream_size as the in the standard defined fixed part of an RSP_log_ack PDU.
      * @return the minimum stream_size as the in the standard defined fixed part of an RSP_log_ack PDU.
      */
    virtual size_t getCorrectSize() {
        return 4;
    }

};

class DcpPduNtfLog : public DcpPdu {
public:
    GET_FUN(getSender, uint8_t, 1)

    GET_FUN_PTR(getLogEntryPayload, uint8_t, 2)

    GET_FUN(getTime, uint64_t, 2)

    GET_FUN(getTemplateId, uint8_t, 10)

    GET_FUN_PTR(getPayload, uint8_t, 11)


    DcpPduNtfLog(unsigned char *stream, size_t stream_size) : DcpPdu(stream, stream_size){}

    DcpPduNtfLog(const uint8_t sender, const uint8_t templateId, const int64_t time, const uint8_t *payload,
                 const size_t payload_size) : DcpPdu(11 + payload_size, DcpPduType::NTF_log) {
        getSender() = sender;
        getTemplateId() = templateId;
        getTime() = time;
        memcpy(getPayload(), payload, payload_size);
    }

    DcpPduNtfLog(const uint8_t sender, const uint8_t templateId, const int64_t time,
                 size_t payload_size) : DcpPdu(11 + payload_size, DcpPduType::NTF_log) {
        getSender() = sender;
        getTemplateId() = templateId;
        getTime() = time;
    }

    virtual std::ostream &operator<<(std::ostream &os) {
        DcpPdu::operator<<(os);
        os << " payload=";
        for (size_t i = 11; i < stream_size; ++i)
            os << std::hex << std::setw(2) << std::setfill('0')
               << (int) stream[i] << " ";
        return os;
    }

    /**
  * Check if the stream_size is at least as much as the in the standard defined fixed part of an RSP_log_ack PDU.
  * @return stream_size is at least as much as the in the standard defined fixed part of an RSP_log_ack PDU
  */
    virtual bool isSizeCorrect() {
        return this->stream_size >= 11;
    }

    /**
      * Returns the minimum stream_size as the in the standard defined fixed part of an RSP_log_ack PDU.
      * @return the minimum stream_size as the in the standard defined fixed part of an RSP_log_ack PDU.
      */
    virtual size_t getCorrectSize() {
        return 11;
    }

};

static DcpPdu *makeDcpPdu(unsigned char *stream, size_t stream_size) {
    DcpPduType &type_id = *((DcpPduType *) (stream + PDU_LENGTH_INDICATOR_SIZE));
    switch (type_id) {
        case DcpPduType::STC_configure:
        case DcpPduType::STC_initialize:
        case DcpPduType::STC_stop:
        case DcpPduType::STC_reset:
        case DcpPduType::STC_deregister:
        case DcpPduType::STC_send_outputs:
        case DcpPduType::STC_prepare:
            return new DcpPduBasicStateTransition(stream, stream_size);
        case DcpPduType::STC_run:
            return new DcpPduRun(stream, stream_size);
        case DcpPduType::INF_state:
            return new DcpPduBasic(stream, stream_size);
        case DcpPduType::INF_error:
            return new DcpPduBasic(stream, stream_size);
        case DcpPduType::INF_log:
            return new DcpPduInfLog(stream,stream_size);
        case DcpPduType::NTF_state_changed:
            return new DcpPduNtfStateChanged(stream, stream_size);
        case DcpPduType::NTF_log:
            return new DcpPduNtfLog(stream, stream_size);
        case DcpPduType::STC_do_step:
            return new DcpPduDoStep(stream, stream_size);
        case DcpPduType::CFG_set_time_res:
            return new DcpPduSetTimeRes(stream, stream_size);
        case DcpPduType::CFG_set_steps:
            return new DcpPduSetSteps(stream, stream_size);
        case DcpPduType::CFG_set_scope:
            return new DcpPduSetScope(stream, stream_size);
        case DcpPduType::STC_register:
            return new DcpPduRegister(stream, stream_size);
        case DcpPduType::CFG_config_input:
            return new DcpPduConfigInput(stream, stream_size);
        case DcpPduType::CFG_config_output:
            return new DcpPduConfigOutput(stream, stream_size);
        case DcpPduType::CFG_config_clear:
            return new DcpPduBasic(stream, stream_size);
        case DcpPduType::CFG_config_tunable_parameter:
            return new DcpPduConfigTunableParameter(stream, stream_size);
        case DcpPduType::CFG_set_parameter:
            return new DcpPduSetParameter(stream, stream_size);
        case DcpPduType::CFG_set_logging:
            return new DcpPduSetLogging(stream, stream_size);
        case DcpPduType::DAT_input_output:
            return new DcpPduDatInputOutput(stream, stream_size);
        case DcpPduType::DAT_parameter:
            return new DcpPduDatParameter(stream, stream_size);
        case DcpPduType::RSP_ack:
            return new DcpPduAck(stream, stream_size);
        case DcpPduType::RSP_nack:
            return new DcpPduNack(stream, stream_size);
        case DcpPduType::RSP_error_ack:
            return new DcpPduNack(stream, stream_size);
        case DcpPduType::RSP_state_ack:
            return new DcpPduStateAck(stream, stream_size);
        case DcpPduType::RSP_log_ack:
            return new DcpPduLogAck(stream, stream_size);
        case DcpPduType::CFG_set_target_network_information: {
            DcpTransportProtocol &tp = *((DcpTransportProtocol *) (stream + 10));
            switch (tp) {
                case DcpTransportProtocol::UDP_IPv4:
                    return new DcpPduSetNetworkInformationEthernet(stream, stream_size);
                default:
                    return new DcpPduSetNetworkInformation(stream, stream_size);
            }

        }
        case DcpPduType::CFG_set_source_network_information: {
            DcpTransportProtocol &tp = *((DcpTransportProtocol *) (stream + 10));
            switch (tp) {
                case DcpTransportProtocol::UDP_IPv4:
                    return new DcpPduSetNetworkInformationEthernet(stream, stream_size);
                default:
                    return new DcpPduSetNetworkInformation(stream, stream_size);
            }

        }
        case DcpPduType::CFG_set_param_network_information: {
            DcpTransportProtocol &tp = *((DcpTransportProtocol *) (stream + 4));
            switch (tp) {
                case DcpTransportProtocol::UDP_IPv4:
                    return new DcpPduSetParamNetworkInformationEthernet(stream, stream_size);
                default:
                    return new DcpPduSetNetworkInformation(stream, stream_size);
            }

        }
    }
    return new DcpPdu(stream, stream_size);
}


#endif /* ACI_ACIPDU_H_ */
