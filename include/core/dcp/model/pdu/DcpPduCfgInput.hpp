//
// Created by Christian Kater on 19.01.19.
//

#ifndef DCPLIB_DCPPDUCFGINPUT_HPP
#define DCPLIB_DCPPDUCFGINPUT_HPP

#include <dcp/model/pdu/DcpPduBasic.hpp>

/**
 * This class decscribes the structure for the Pdu "CFG_config_input".
 */
class DcpPduCfgInput : public DcpPduBasic {
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
    DcpPduCfgInput(unsigned char *stream, size_t stream_size): DcpPduBasic(stream, stream_size) {}

    /**
     * Creates a new AciPduConfigInput object.
     * @param pdu_seq_id the pdu seq id.
     * @param receiver the receiver.
     * @param data_id  the data id .
     * @param pos the pos.
     * @param target_vr the target vr.
     * @param source_data_type the source data type.
     */
    DcpPduCfgInput(const uint16_t pdu_seq_id, const uint8_t receiver, const uint16_t data_id, const uint16_t pos,
                   const uint64_t target_vr, const DcpDataType source_data_type) :
            DcpPduBasic(17, DcpPduType::CFG_input, pdu_seq_id, receiver) {
        getDataId() = data_id;
        getPos() = pos;
        getTargetVr() = target_vr;
        getSourceDataType() = source_data_type;
    }

#if defined(DEBUG) || defined(LOGGING)
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
#endif

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
    DcpPduCfgInput(const size_t stream_size, const DcpPduType type_id, const uint16_t pdu_seq_id,
                   const uint8_t receiver, const uint16_t data_id, const uint16_t pos, const uint64_t target_vr,
                   const DcpDataType source_data_type) : DcpPduBasic(stream_size, type_id, pdu_seq_id, receiver) {
        getDataId() = data_id;
        getPos() = pos;
        getTargetVr() = target_vr;
        getSourceDataType() = source_data_type;
    }
};
#endif //DCPLIB_DCPPDUCFGINPUT_HPP
