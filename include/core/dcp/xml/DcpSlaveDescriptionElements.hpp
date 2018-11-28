//
// Created by kater on 11.06.18.
//

#ifndef ACOSAR_DCPSLAVEDESCRIPTIONELEMENTS_H
#define ACOSAR_DCPSLAVEDESCRIPTIONELEMENTS_H

#include <dcp/model/DcpTypes.hpp>
#include <dcp/model/DcpConstants.hpp>
#include <memory>
#include <vector>



struct HardRealTime_t {
    bool set;
};

static HardRealTime_t make_HardRealTime() {
    return {false};
}

struct SoftRealTime_t {
    bool set;
};

static SoftRealTime_t make_SoftRealTime() {
    return {false};
}

struct NonRealTime_t {
    bool set;
    steps_t defaultSteps;
    bool fixedSteps;
    steps_t minSteps;
    steps_t maxSteps;
};

static NonRealTime_t make_NonRealTime() {
    return {false, 1, true, 1, 1};
}

struct OpMode_t {
    HardRealTime_t HardRealTime;
    SoftRealTime_t SoftRealTime;
    NonRealTime_t NonRealTime;
};

static OpMode_t make_OpMode() {
    return {make_HardRealTime(), make_SoftRealTime(), make_NonRealTime()};
}


struct BaseUnit_t {
    uint32_t kg, m, s, A, K, mol, cd, rad;
    float64_t factor, offset;
};

static BaseUnit_t make_BaseUnit(){
    return {0, 0, 0, 0, 0, 0, 0, 0, 1.0, 0.0};
}

struct DisplayUnit_t {
    std::string name;
    float64_t factor, offset;
};

static DisplayUnit_t make_DisplayUnit(std::string name){
    return {name, 1.0, 0.0};
}

struct Unit_t {
    std::string name;
    std::shared_ptr<BaseUnit_t> BaseUnit;
    std::vector<DisplayUnit_t> DisplayUnit;
};

static Unit_t make_Unit(std::string name){
    return {name, std::shared_ptr<BaseUnit_t>(nullptr), std::vector<DisplayUnit_t>()};
}

struct Resolution_t {
    numerator_t numerator;
    denominator_t denominator;
    bool fixed;
    std::shared_ptr<bool> recommended;
};

static Resolution_t make_Resolution() {
    return {1, 1000, true, std::shared_ptr<bool>(nullptr)};
}

struct ResolutionRange_t {
    numerator_t numeratorFrom;
    numerator_t numeratorTo;
    denominator_t denominator;
};

static ResolutionRange_t
make_ResolutionRange(numerator_t numeratorFrom, numerator_t numeratorTo, denominator_t denominator) {
    return {numeratorFrom, numeratorTo, denominator};
}

struct TimeRes_t {
    std::vector<Resolution_t> resolutions;
    std::vector<ResolutionRange_t> resolutionRanges;
};

static TimeRes_t make_TimeRes() {
    return {std::vector<Resolution_t>(), std::vector<ResolutionRange_t>()};
}

struct MaximumPeriodicInterval_t {
    numerator_t numerator;
    denominator_t denominator;
};

static MaximumPeriodicInterval_t make_MaximumPeriodicInterval() {
    return {1, 1};
}

struct Heartbeat_t {
    MaximumPeriodicInterval_t MaximumPeriodicInterval;
};

static Heartbeat_t make_Heartbeat() {
    return {make_MaximumPeriodicInterval()};
}

static std::shared_ptr<Heartbeat_t> make_Heartbeat_ptr() {
    std::shared_ptr<Heartbeat_t> heartbeat(new Heartbeat_t());
    heartbeat->MaximumPeriodicInterval = make_MaximumPeriodicInterval();
    return heartbeat;
}

struct Control_t {
    std::shared_ptr<std::string> host;
    std::shared_ptr<port_t> port;
};

static Control_t make_Control() {
    return {std::shared_ptr<std::string>(nullptr), std::shared_ptr<port_t>(nullptr)};
}

static Control_t make_Control(std::string host, port_t port) {
    Control_t control = make_Control();
    control.host = std::make_shared<std::string>(host);
    control.port = std::make_shared<port_t>(port);
    return control;
}

static std::shared_ptr<Control_t> make_Control_ptr() {
    return std::shared_ptr<Control_t>(new Control_t());
}

static std::shared_ptr<Control_t> make_Control_ptr(std::string host, port_t port) {
    std::shared_ptr<Control_t> control(new Control_t());
    control->host = std::make_shared<std::string>(host);
    control->port = std::make_shared<port_t>(port);
    return control;
}

struct AvailablePortRange_t {
    port_t from;
    port_t to;
};

static AvailablePortRange_t make_AviablePortRange(port_t from, port_t to) {
    return {from, to};
}

struct AvailablePort_t {
    port_t port;
};

static AvailablePort_t make_AvailablePort(port_t port) {
    return {port};
}

struct DAT_t {
    std::shared_ptr<std::string> host;
    std::vector<AvailablePort_t> availablePorts;
    std::vector<AvailablePortRange_t> availablePortRanges;
};

static DAT_t make_DAT(std::string host) {
    return {std::make_shared<std::string>(host), std::vector<AvailablePort_t>(), std::vector<AvailablePortRange_t>()};
}

static DAT_t make_DAT() {
    return {std::make_shared<std::string>(nullptr), std::vector<AvailablePort_t>(), std::vector<AvailablePortRange_t>()};
}

static std::shared_ptr<DAT_t> make_DAT_ptr(std::string host) {
    std::shared_ptr<DAT_t> dat(new DAT_t());
    dat->host = std::make_shared<std::string>(host);
    dat->availablePorts = std::vector<AvailablePort_t>();
    dat->availablePortRanges = std::vector<AvailablePortRange_t>();
    return dat;
}

static std::shared_ptr<DAT_t> make_DAT_ptr() {
    std::shared_ptr<DAT_t> dat(new DAT_t());
    dat->host = std::make_shared<std::string>(nullptr);
    dat->availablePorts = std::vector<AvailablePort_t>();
    dat->availablePortRanges = std::vector<AvailablePortRange_t>();
    return dat;
}

struct Ethernet_t {
    uint32_t maxPduSize;
    std::shared_ptr<Control_t> Control;
    std::shared_ptr<DAT_t> DAT_input_output;
    std::shared_ptr<DAT_t> DAT_parameter;
};

static Ethernet_t make_UDP() {
    return {65507, std::shared_ptr<Control_t>(nullptr), std::shared_ptr<DAT_t>(nullptr),
            std::shared_ptr<DAT_t>(nullptr)};
}

static std::shared_ptr<Ethernet_t> make_UDP_ptr() {
    std::shared_ptr<Ethernet_t> udp(new Ethernet_t());
    udp->maxPduSize = 65507;
    udp->Control = std::shared_ptr<Control_t>(nullptr);
    udp->DAT_input_output = std::shared_ptr<DAT_t>(nullptr);
    udp->DAT_parameter = std::shared_ptr<DAT_t>(nullptr);
    return udp;
}

static Ethernet_t make_TCP() {
    return {4294967267, std::shared_ptr<Control_t>(nullptr), std::shared_ptr<DAT_t>(nullptr),
            std::shared_ptr<DAT_t>(nullptr)};
}

static std::shared_ptr<Ethernet_t> make_TCP_ptr() {
    std::shared_ptr<Ethernet_t> udp(new Ethernet_t());
    udp->maxPduSize = 4294967267;
    udp->Control = std::shared_ptr<Control_t>(nullptr);
    udp->DAT_input_output = std::shared_ptr<DAT_t>(nullptr);
    udp->DAT_parameter = std::shared_ptr<DAT_t>(nullptr);
    return udp;
}

enum class Direction_t {
    USB_DIR_IN, USB_DIR_OUT
};

struct DataPipe_t {
    Direction_t direction;
    uint8_t endpointAddress;
    uint8_t interval;
};

static DataPipe_t make_DataPipe(Direction_t direction, uint8_t endpointAddress, uint8_t interval) {
    return {direction, endpointAddress, interval};
}

struct USB_t {
    uint32_t maximumTransmissionUnit;
    uint8_t maxPower;
    std::vector<DataPipe_t> dataPipes;
};

static USB_t make_USB() {
    return {1024, 255, std::vector<DataPipe_t>()};
}

struct CAN_t {
    //toDo
};

struct Bluetooth_t {
    //toDo
};

struct TransportProtocols_t {
    std::shared_ptr<Ethernet_t> UDP_IPv4;
    std::shared_ptr<CAN_t> CAN;
    std::shared_ptr<USB_t> USB;
    std::shared_ptr<Bluetooth_t> Bluetooth;
    std::shared_ptr<Ethernet_t> TCP_IPv4;
};

static TransportProtocols_t make_Drivers() {
    return {std::shared_ptr<Ethernet_t>(nullptr), std::shared_ptr<CAN_t>(nullptr), std::shared_ptr<USB_t>(nullptr),
            std::shared_ptr<Bluetooth_t>(nullptr), std::shared_ptr<Ethernet_t>(nullptr)};
}

struct CapabilityFlags_t {
    bool canAcceptConfigPdus;
    bool canHandleReset;
    bool canHandleVariableSteps;
    bool canMonitorHeartbeat;
    bool canProvideLogOnRequest;
    bool canProvideLogOnNotification;
};

static CapabilityFlags_t make_CapabilityFlags() {
    return {false, false, false, false, false, false};
}

template<typename T>
struct DataType_t {
    DcpDataType type;

    DataType_t() : type(getFixedSizeDcpDataType<T>()) {}
};

template<typename T>
struct IntegerDataType_t : public DataType_t<T> {
    std::shared_ptr<T> min;
    std::shared_ptr<T> max;
    std::shared_ptr<T> gradient;
    std::shared_ptr<std::vector<T>> start;


    IntegerDataType_t() : DataType_t<T>(), min(std::shared_ptr<T>(nullptr)),
                          max(std::shared_ptr<T>(nullptr)),
                          gradient(std::shared_ptr<T>(nullptr)), start(std::shared_ptr<std::vector<T>>(nullptr)) {}
};

template<typename T>
static IntegerDataType_t<T> make_IntegerDataType() {
    return {};
}


template<typename T>
struct FloatDataType_t : public IntegerDataType_t<T> {
    std::shared_ptr<T> nominal;
    std::shared_ptr<std::string> quantitiy;
    std::shared_ptr<std::string> unit;
    std::shared_ptr<std::string> displayUnit;

    FloatDataType_t()
            : IntegerDataType_t<T>(), nominal(std::shared_ptr<T>(nullptr)),
              quantitiy(std::shared_ptr<std::string>(nullptr)), unit(std::shared_ptr<std::string>(nullptr)),
              displayUnit(std::shared_ptr<std::string>(nullptr)) {}
};

template<typename T>
static FloatDataType_t<T> make_FloatDataType() {
    return {};
}

struct StringDataType_t {
    std::shared_ptr<uint32_t> maxSize;
    std::shared_ptr<std::string> start;

    StringDataType_t()
            : maxSize(std::shared_ptr<uint32_t>(nullptr)),
              start(std::shared_ptr<std::string>(nullptr)) {}
};

static StringDataType_t make_StringDataType() {
    return {};
}

struct BinaryStartValue {
    ~BinaryStartValue(){
        delete[] value;
    }

    uint8_t* value;
    uint32_t length;
};

struct BinaryDataType_t {
    std::shared_ptr<std::string> mimeType;
    std::shared_ptr<uint32_t> maxSize;
    std::shared_ptr<BinaryStartValue> start;

    BinaryDataType_t()
            : mimeType(std::shared_ptr<std::string>(nullptr)),
              maxSize(std::shared_ptr<uint32_t>(nullptr)),
              start(std::shared_ptr<BinaryStartValue>(nullptr)) {}
};

static BinaryDataType_t make_BinaryDataType() {
    return {};
}

enum DimensionType {
    CONSTANT, LINKED_VR
};
struct Dimension_t {
    DimensionType type;
    uint64_t value;
};

struct CommonCausality_t {
    std::shared_ptr<IntegerDataType_t<uint8_t>> Uint8;
    std::shared_ptr<IntegerDataType_t<uint16_t>> Uint16;
    std::shared_ptr<IntegerDataType_t<uint32_t>> Uint32;
    std::shared_ptr<IntegerDataType_t<uint64_t>> Uint64;
    std::shared_ptr<IntegerDataType_t<int8_t>> Int8;
    std::shared_ptr<IntegerDataType_t<int16_t>> Int16;
    std::shared_ptr<IntegerDataType_t<int32_t>> Int32;
    std::shared_ptr<IntegerDataType_t<int64_t>> Int64;
    std::shared_ptr<FloatDataType_t<float32_t>> Float32;
    std::shared_ptr<FloatDataType_t<float64_t>> Float64;
    std::shared_ptr<StringDataType_t> String;
    std::shared_ptr<BinaryDataType_t> Binary;
    std::vector<Dimension_t> dimensions;

    CommonCausality_t(const std::shared_ptr<IntegerDataType_t<uint8_t>> Uint8,
                      const std::shared_ptr<IntegerDataType_t<uint16_t>> Uint16,
                      const std::shared_ptr<IntegerDataType_t<uint32_t>> Uint32,
                      const std::shared_ptr<IntegerDataType_t<uint64_t>> Uint64,
                      const std::shared_ptr<IntegerDataType_t<int8_t>> Int8,
                      const std::shared_ptr<IntegerDataType_t<int16_t>> Int16,
                      const std::shared_ptr<IntegerDataType_t<int32_t>> Int32,
                      const std::shared_ptr<IntegerDataType_t<int64_t>> Int64,
                      const std::shared_ptr<FloatDataType_t<float32_t>> Float32,
                      const std::shared_ptr<FloatDataType_t<float64_t>> Float64,
                      const std::shared_ptr<StringDataType_t> &String, const std::shared_ptr<BinaryDataType_t> &Binary,
                      const std::vector<Dimension_t> &dimensions) : Uint8(Uint8), Uint16(Uint16), Uint32(Uint32),
                                                                    Uint64(Uint64), Int8(Int8), Int16(Int16),
                                                                    Int32(Int32), Int64(Int64), Float32(Float32),
                                                                    Float64(Float64), String(String), Binary(Binary),
                                                                    dimensions(dimensions) {}


};

template<typename T>
static CommonCausality_t make_CommonCausality() {
    if (std::is_same<uint8_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(new IntegerDataType_t<uint8_t>()),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()};
    } else if (std::is_same<uint16_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(new IntegerDataType_t<uint16_t>()),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()};
    } else if (std::is_same<uint32_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(new IntegerDataType_t<uint32_t>()),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()};
    } else if (std::is_same<uint64_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(new IntegerDataType_t<uint64_t>()),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()};
    } else if (std::is_same<int8_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(new IntegerDataType_t<int8_t>()),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()};
    } else if (std::is_same<int16_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(new IntegerDataType_t<int16_t>()),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()};
    } else if (std::is_same<int32_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(new IntegerDataType_t<int32_t>()),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()};
    } else if (std::is_same<int64_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(new IntegerDataType_t<int64_t>()),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()};
    } else if (std::is_same<float32_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(new FloatDataType_t<float32_t>()),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()};
    } else if (std::is_same<float64_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(new FloatDataType_t<float64_t>()),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()};
    }
}

template<typename T>
static std::shared_ptr<CommonCausality_t> make_CommonCausality_ptr() {
    if (std::is_same<uint8_t, T>::value) {
        return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(new IntegerDataType_t<uint8_t>()),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()));
    } else if (std::is_same<uint16_t, T>::value) {
        return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(new IntegerDataType_t<uint16_t>()),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()));
    } else if (std::is_same<uint32_t, T>::value) {
        return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(new IntegerDataType_t<uint32_t>()),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()));
    } else if (std::is_same<uint64_t, T>::value) {
        return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(new IntegerDataType_t<uint64_t>()),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()));
    } else if (std::is_same<int8_t, T>::value) {
        return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(new IntegerDataType_t<int8_t>()),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()));
    } else if (std::is_same<int16_t, T>::value) {
        return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(new IntegerDataType_t<int16_t>()),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()));
    } else if (std::is_same<int32_t, T>::value) {
        return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(new IntegerDataType_t<int32_t>()),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()));
    } else if (std::is_same<int64_t, T>::value) {
        return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(new IntegerDataType_t<int64_t>()),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()));
    } else if (std::is_same<float32_t, T>::value) {
        return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(new FloatDataType_t<float32_t>()),
                std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()));
    } else if (std::is_same<float64_t, T>::value) {
        return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
                std::shared_ptr<FloatDataType_t<float64_t>>(new FloatDataType_t<float64_t>()),
                std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(nullptr),
                std::vector<Dimension_t>()));
    }
}

static CommonCausality_t make_CommonCausality_String() {
    return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
            std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
            std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
            std::shared_ptr<StringDataType_t>(new StringDataType_t()), std::shared_ptr<BinaryDataType_t>(nullptr),
            std::vector<Dimension_t>()};
}

static std::shared_ptr<CommonCausality_t> make_CommonCausality_String_ptr() {
    return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
            std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
            std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
            std::shared_ptr<StringDataType_t>(new StringDataType_t()), std::shared_ptr<BinaryDataType_t>(nullptr),
            std::vector<Dimension_t>()));
}

static CommonCausality_t make_CommonCausality_Binary() {
    return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
            std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
            std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
            std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(new BinaryDataType_t()),
            std::vector<Dimension_t>()};
}

static std::shared_ptr<CommonCausality_t> make_CommonCausality_Binary_ptr() {
    return std::shared_ptr<CommonCausality_t>(new CommonCausality_t(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr),
            std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr),
            std::shared_ptr<FloatDataType_t<float32_t>>(nullptr),
            std::shared_ptr<FloatDataType_t<float64_t>>(nullptr),
            std::shared_ptr<StringDataType_t>(nullptr), std::shared_ptr<BinaryDataType_t>(new BinaryDataType_t()),
            std::vector<Dimension_t>()));
}

struct Output_t {
    std::shared_ptr<IntegerDataType_t<uint8_t>> Uint8;
    std::shared_ptr<IntegerDataType_t<uint16_t>> Uint16;
    std::shared_ptr<IntegerDataType_t<uint32_t>> Uint32;
    std::shared_ptr<IntegerDataType_t<uint64_t>> Uint64;
    std::shared_ptr<IntegerDataType_t<int8_t>> Int8;
    std::shared_ptr<IntegerDataType_t<int16_t>> Int16;
    std::shared_ptr<IntegerDataType_t<int32_t>> Int32;
    std::shared_ptr<IntegerDataType_t<int64_t>> Int64;
    std::shared_ptr<FloatDataType_t<float32_t>> Float32;
    std::shared_ptr<FloatDataType_t<float64_t>> Float64;
    std::shared_ptr<StringDataType_t> String;
    std::shared_ptr<BinaryDataType_t> Binary;
    std::vector<Dimension_t> dimensions;
    steps_t defaultSteps;
    bool fixedSteps;
    steps_t minSteps;
    steps_t maxSteps;

    Output_t() : Uint8(std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr)),
                 Uint16(std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr)),
                 Uint32(std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr)),
                 Uint64(std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr)),
                 Int8(std::shared_ptr<IntegerDataType_t<int8_t>>(nullptr)),
                 Int16(std::shared_ptr<IntegerDataType_t<int16_t>>(nullptr)),
                 Int32(std::shared_ptr<IntegerDataType_t<int32_t>>(nullptr)),
                 Int64(std::shared_ptr<IntegerDataType_t<int64_t>>(nullptr)),
                 Float32(std::shared_ptr<FloatDataType_t<float32_t>>(nullptr)),
                 Float64(std::shared_ptr<FloatDataType_t<float64_t>>(nullptr)),
                 String(std::shared_ptr<StringDataType_t>(nullptr)),
                 Binary(std::shared_ptr<BinaryDataType_t>(nullptr)),
                 dimensions(std::vector<Dimension_t>()),
                 defaultSteps(1), fixedSteps(true), minSteps(1), maxSteps(1) {}

};

template<typename T>
static Output_t make_Output() {
    Output_t o = {};

    if (std::is_same<uint8_t, T>::value) {
        o.Uint8 = std::shared_ptr<IntegerDataType_t<uint8_t>>(new IntegerDataType_t<uint8_t>());
    } else if (std::is_same<uint16_t, T>::value) {
        o.Uint16 = std::shared_ptr<IntegerDataType_t<uint16_t>>(new IntegerDataType_t<uint16_t>());
    } else if (std::is_same<uint32_t, T>::value) {
        o.Uint32 = std::shared_ptr<IntegerDataType_t<uint32_t>>(new IntegerDataType_t<uint32_t>());
    } else if (std::is_same<uint64_t, T>::value) {
        o.Uint64 = std::shared_ptr<IntegerDataType_t<uint64_t>>(new IntegerDataType_t<uint64_t>());
    } else if (std::is_same<int8_t, T>::value) {
        o.Int8 = std::shared_ptr<IntegerDataType_t<int8_t>>(new IntegerDataType_t<int8_t>());
    } else if (std::is_same<int16_t, T>::value) {
        o.Int16 = std::shared_ptr<IntegerDataType_t<int16_t>>(new IntegerDataType_t<int16_t>());
    } else if (std::is_same<int32_t, T>::value) {
        o.Int32 = std::shared_ptr<IntegerDataType_t<int32_t>>(new IntegerDataType_t<int32_t>());
    } else if (std::is_same<int64_t, T>::value) {
        o.Int64 = std::shared_ptr<IntegerDataType_t<int64_t>>(new IntegerDataType_t<int64_t>());
    } else if (std::is_same<float32_t, T>::value) {
        o.Float32 = std::shared_ptr<FloatDataType_t<float32_t>>(new FloatDataType_t<float32_t>());
    } else if (std::is_same<float64_t, T>::value) {
        o.Float64 = std::shared_ptr<FloatDataType_t<float64_t>>(new FloatDataType_t<float64_t>());
    }
    o.defaultSteps = 1;
    o.fixedSteps = true;
    o.minSteps = 1;
    o.maxSteps = 1;
    return o;
}

template<typename T>
static std::shared_ptr<Output_t> make_Output_ptr() {
    std::shared_ptr<Output_t> o (new Output_t());

    if (std::is_same<uint8_t, T>::value) {
        o->Uint8 = std::shared_ptr<IntegerDataType_t<uint8_t>>(new IntegerDataType_t<uint8_t>());
    } else if (std::is_same<uint16_t, T>::value) {
        o->Uint16 = std::shared_ptr<IntegerDataType_t<uint16_t>>(new IntegerDataType_t<uint16_t>());
    } else if (std::is_same<uint32_t, T>::value) {
        o->Uint32 = std::shared_ptr<IntegerDataType_t<uint32_t>>(new IntegerDataType_t<uint32_t>());
    } else if (std::is_same<uint64_t, T>::value) {
        o->Uint64 = std::shared_ptr<IntegerDataType_t<uint64_t>>(new IntegerDataType_t<uint64_t>());
    } else if (std::is_same<int8_t, T>::value) {
        o->Int8 = std::shared_ptr<IntegerDataType_t<int8_t>>(new IntegerDataType_t<int8_t>());
    } else if (std::is_same<int16_t, T>::value) {
        o->Int16 = std::shared_ptr<IntegerDataType_t<int16_t>>(new IntegerDataType_t<int16_t>());
    } else if (std::is_same<int32_t, T>::value) {
        o->Int32 = std::shared_ptr<IntegerDataType_t<int32_t>>(new IntegerDataType_t<int32_t>());
    } else if (std::is_same<int64_t, T>::value) {
        o->Int64 = std::shared_ptr<IntegerDataType_t<int64_t>>(new IntegerDataType_t<int64_t>());
    } else if (std::is_same<float32_t, T>::value) {
        o->Float32 = std::shared_ptr<FloatDataType_t<float32_t>>(new FloatDataType_t<float32_t>());
    } else if (std::is_same<float64_t, T>::value) {
        o->Float64 = std::shared_ptr<FloatDataType_t<float64_t>>(new FloatDataType_t<float64_t>());
    }
    o->defaultSteps = 1;
    o->fixedSteps = true;
    o->minSteps = 1;
    o->maxSteps = 1;
    return o;
}

static Output_t make_Output_String() {
    Output_t o = {};
    o.String = std::shared_ptr<StringDataType_t>(new StringDataType_t());
    o.defaultSteps = 1;
    o.fixedSteps = true;
    o.minSteps = 1;
    o.maxSteps = 1;
    return o;
}

static std::shared_ptr<Output_t> make_Output_String_ptr() {
    std::shared_ptr<Output_t> o (new Output_t());
    o->String = std::shared_ptr<StringDataType_t>(new StringDataType_t());
    o->defaultSteps = 1;
    o->fixedSteps = true;
    o->minSteps = 1;
    o->maxSteps = 1;
    return o;
}

static Output_t make_Output_Binary() {
    Output_t o = {};
    o.Binary = std::shared_ptr<BinaryDataType_t>(new BinaryDataType_t());
    o.defaultSteps = 1;
    o.fixedSteps = true;
    o.minSteps = 1;
    o.maxSteps = 1;
    return o;
}

static std::shared_ptr<Output_t> make_Output_Binary_ptr() {
    std::shared_ptr<Output_t> o (new Output_t());
    o->Binary = std::shared_ptr<BinaryDataType_t>(new BinaryDataType_t());
    o->defaultSteps = 1;
    o->fixedSteps = true;
    o->minSteps = 1;
    o->maxSteps = 1;
    return o;
}

struct StructuralParameter_t {
    std::shared_ptr<IntegerDataType_t<uint8_t>> Uint8;
    std::shared_ptr<IntegerDataType_t<uint16_t>> Uint16;
    std::shared_ptr<IntegerDataType_t<uint32_t>> Uint32;
    std::shared_ptr<IntegerDataType_t<uint64_t>> Uint64;
};

template<typename T>
static StructuralParameter_t make_StructuralParameter() {
    if (std::is_same<uint8_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(new IntegerDataType_t<uint8_t>()),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr)};
    } else if (std::is_same<uint16_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(new IntegerDataType_t<uint16_t>()),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr)};
    } else if (std::is_same<uint32_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(new IntegerDataType_t<uint32_t>()),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(nullptr)};
    } else if (std::is_same<uint64_t, T>::value) {
        return {std::shared_ptr<IntegerDataType_t<uint8_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint16_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint32_t>>(nullptr),
                std::shared_ptr<IntegerDataType_t<uint64_t>>(new IntegerDataType_t<uint64_t>())};
    }
}

enum class Variability {
    FIXED, TUNABLE, DISCRETE, CONTINUOUS,
};

struct Variable_t {
    std::string name;
    valueReference_t valueReference;
    std::shared_ptr<std::string> description;
    std::shared_ptr<Variability> variability;
    std::shared_ptr<double> preEdge;
    std::shared_ptr<double> postEdge;
    std::shared_ptr<uint32_t> maxConsecMissedPdus;
    std::shared_ptr<CommonCausality_t> Input;
    std::shared_ptr<Output_t> Output;
    std::shared_ptr<CommonCausality_t> Parameter;
    std::shared_ptr<StructuralParameter_t> StructuralParameter;
};

static Variable_t
make_Variable_input(std::string name, valueReference_t valueReference, std::shared_ptr<CommonCausality_t> Input) {
    return {name, valueReference, std::shared_ptr<std::string>(), std::shared_ptr<Variability>(),
            std::shared_ptr<double>(), std::shared_ptr<double>(),
            std::shared_ptr<uint32_t>(), std::move(Input), std::shared_ptr<Output_t>(nullptr),
            std::shared_ptr<CommonCausality_t>(nullptr), std::shared_ptr<StructuralParameter_t>(nullptr)};
}

static Variable_t
make_Variable_output(std::string name, valueReference_t valueReference, std::shared_ptr<Output_t> Output) {
    return {name, valueReference, std::shared_ptr<std::string>(), std::shared_ptr<Variability>(),
            std::shared_ptr<double>(), std::shared_ptr<double>(),
            std::shared_ptr<uint32_t>(), std::shared_ptr<CommonCausality_t>(nullptr), std::move(Output),
            std::shared_ptr<CommonCausality_t>(nullptr), std::shared_ptr<StructuralParameter_t>(nullptr)};
}

static Variable_t
make_Variable_parameter(std::string name, valueReference_t valueReference,
                        std::shared_ptr<CommonCausality_t> Parameter) {
    return {name, valueReference, std::shared_ptr<std::string>(), std::shared_ptr<Variability>(),
            std::shared_ptr<double>(), std::shared_ptr<double>(),
            std::shared_ptr<uint32_t>(), std::shared_ptr<CommonCausality_t>(nullptr), std::shared_ptr<Output_t>(nullptr),
            std::move(Parameter), std::shared_ptr<StructuralParameter_t>(nullptr)};
}

static Variable_t
make_Variable_structuralParameter(std::string name, valueReference_t valueReference,
                                  std::shared_ptr<StructuralParameter_t> StructuralParameter) {
    return {name, valueReference, std::shared_ptr<std::string>(), std::shared_ptr<Variability>(),
            std::shared_ptr<double>(), std::shared_ptr<double>(),
            std::shared_ptr<uint32_t>(), std::shared_ptr<CommonCausality_t>(nullptr), std::shared_ptr<Output_t>(nullptr),
            std::shared_ptr<CommonCausality_t>(nullptr),
            std::move(StructuralParameter)};
}

struct Category_t {
    uint8_t id;
    std::string name;
};

static Category_t make_Category(uint8_t id, const std::string &name) {
    return {id, name};
}

struct Template_t {
    uint8_t id;
    uint8_t category;
    uint8_t level;
    std::string msg;
};

static Template_t make_Template(uint8_t id, uint8_t category, uint8_t level, const std::string &msg) {
    return {id, category, level, msg};
}

struct Log_t {
    std::vector<Category_t> categories;
    std::vector<Template_t> templates;
};

static Log_t make_Log() {
    return {};
}

static std::shared_ptr<Log_t> make_Log_ptr() {
    return std::shared_ptr<Log_t>(new Log_t());
}

enum VariableNamingConvention {
    FLAT, STRUCTURED
};

struct SlaveDescription_t {
    OpMode_t OpMode;
    std::vector<Unit_t> UnitDefinitions;
    TimeRes_t TimeRes;
    std::shared_ptr<Heartbeat_t> Heartbeat;
    TransportProtocols_t TransportProtocols;
    CapabilityFlags_t CapabilityFlags;
    std::vector<Variable_t> variables;
    std::shared_ptr<Log_t> Log;
    uint8_t dcpMajorVersion;
    uint8_t dcpMinorVersion;
    std::string dcpSlaveName;
    std::string uuid;
    std::shared_ptr<std::string> description;
    std::shared_ptr<std::string> author;
    std::shared_ptr<std::string> version;
    std::shared_ptr<std::string> copyright;
    std::shared_ptr<std::string> license;
    std::shared_ptr<std::string> generationTool;
    std::shared_ptr<std::string> generationDateAndTime;
    std::shared_ptr<VariableNamingConvention> variableNamingConvention;
};

static SlaveDescription_t
make_SlaveDescription(uint8_t dcpMajorVersion, uint8_t dcpMinorVersion, const std::string dcpSlaveName,
                      const std::string uuid) {
    return {make_OpMode(), std::vector<Unit_t>(), make_TimeRes(), std::shared_ptr<Heartbeat_t>(nullptr), make_Drivers(),
            make_CapabilityFlags(), std::vector<Variable_t>(), std::shared_ptr<Log_t>(
                    nullptr), dcpMajorVersion, dcpMinorVersion, dcpSlaveName, uuid,
            std::shared_ptr<std::string>(nullptr), std::shared_ptr<std::string>(nullptr), std::shared_ptr<std::string>(
                    nullptr), std::shared_ptr<std::string>(nullptr), std::shared_ptr<std::string>(
                    nullptr), std::shared_ptr<std::string>(nullptr), std::shared_ptr<std::string>(nullptr),
            std::shared_ptr<VariableNamingConvention>(nullptr)};
}


#endif //ACOSAR_DCPSLAVEDESCRIPTIONELEMENTS_H
