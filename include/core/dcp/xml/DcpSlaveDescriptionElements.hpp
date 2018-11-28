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

static std::shared_ptr<BaseUnit_t> make_BaseUnit_ptr(){
    std::shared_ptr<BaseUnit_t> ptr = std::make_shared<BaseUnit_t>();
    ptr->kg = 0;
    ptr->m = 0;
    ptr->s = 0;
    ptr->A = 0;
    ptr->K = 0;
    ptr->mol = 0;
    ptr->cd = 0;
    ptr->rad = 0;
    ptr->factor = 1.0;
    ptr->offset = 0;
    return ptr;
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

static std::shared_ptr<Heartbeat_t> make_Heartbeat_ptr() {
    std::shared_ptr<Heartbeat_t> heartbeat(new Heartbeat_t());
    heartbeat->MaximumPeriodicInterval = make_MaximumPeriodicInterval();
    return heartbeat;
}

struct Control_t {
    std::shared_ptr<std::string> host;
    std::shared_ptr<port_t> port;
};

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

static std::shared_ptr<DAT_t> make_DAT_ptr(std::string host) {
    std::shared_ptr<DAT_t> dat(new DAT_t());
    dat->host = std::make_shared<std::string>(host);
    dat->availablePorts = std::vector<AvailablePort_t>();
    dat->availablePortRanges = std::vector<AvailablePortRange_t>();
    return dat;
}

static std::shared_ptr<DAT_t> make_DAT_ptr() {
    std::shared_ptr<DAT_t> dat(new DAT_t());
    dat->host = std::make_shared<std::string>("");
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

static std::shared_ptr<Ethernet_t> make_UDP_ptr() {
    std::shared_ptr<Ethernet_t> udp(new Ethernet_t());
    udp->maxPduSize = 65507;
    udp->Control = std::shared_ptr<Control_t>(nullptr);
    udp->DAT_input_output = std::shared_ptr<DAT_t>(nullptr);
    udp->DAT_parameter = std::shared_ptr<DAT_t>(nullptr);
    return udp;
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
    uint32_t maxPduSize;
    std::shared_ptr<uint8_t> maxPower;
    std::vector<DataPipe_t> dataPipes;
};

static std::shared_ptr<USB_t> make_USB_ptr(){
    std::shared_ptr<USB_t> ptr = std::make_shared<USB_t>();
    ptr->maxPduSize = 1024;
    ptr->maxPower = std::shared_ptr<uint8_t>(nullptr);
    ptr->dataPipes = std::vector<DataPipe_t>();
    return ptr;
}

struct Address_t {
    std::string bdAddr;
    uint8_t port;
    std::shared_ptr<std::string> alias;
};

static Address_t make_Address(std::string bdAddr, uint8_t port){
    return {bdAddr, port, std::shared_ptr<std::string>(nullptr)};
}

struct Bluetooth_t {
    uint32_t maxPduSize;
    std::vector<Address_t> addresses;
};

static std::shared_ptr<Bluetooth_t> make_Bluetooth_ptr(){
    std::shared_ptr<Bluetooth_t> ptr = std::make_shared<Bluetooth_t>();
    ptr->maxPduSize = 1011;
    ptr->addresses = std::vector<Address_t>();
    return ptr;
}

struct TransportProtocols_t {
    std::shared_ptr<Ethernet_t> UDP_IPv4;
    bool CAN;
    std::shared_ptr<USB_t> USB;
    std::shared_ptr<Bluetooth_t> Bluetooth;
    std::shared_ptr<Ethernet_t> TCP_IPv4;
};

static TransportProtocols_t make_TransportProtocols() {
    return {std::shared_ptr<Ethernet_t>(nullptr), false, std::shared_ptr<USB_t>(nullptr),
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
struct SimpleIntegerDataType {
    std::shared_ptr<T> min;
    std::shared_ptr<T> max;
    std::shared_ptr<T> gradient;
    SimpleIntegerDataType() : min(std::shared_ptr<T>(nullptr)),
                   max(std::shared_ptr<T>(nullptr)),
                   gradient(std::shared_ptr<T>(nullptr)) {}
};

template<typename T>
struct IntegerDataType_t : public SimpleIntegerDataType<T> {

    std::shared_ptr<std::vector<T>> start;

    IntegerDataType_t() : SimpleIntegerDataType<T>(),  start(std::shared_ptr<std::vector<T>>(nullptr)) {}
};

template<typename T>
struct SimpleFloatDataType_t : public SimpleIntegerDataType<T> {
    std::shared_ptr<T> nominal;
    std::shared_ptr<std::string> quantitiy;
    std::shared_ptr<std::string> unit;
    std::shared_ptr<std::string> displayUnit;

    SimpleFloatDataType_t()
            : SimpleIntegerDataType<T>(), nominal(std::shared_ptr<T>(nullptr)),
              quantitiy(std::shared_ptr<std::string>(nullptr)), unit(std::shared_ptr<std::string>(nullptr)),
              displayUnit(std::shared_ptr<std::string>(nullptr)) {}
};


template<typename T>
struct FloatDataType_t : public SimpleFloatDataType_t<T> {
    std::shared_ptr<std::vector<T>> start;
    FloatDataType_t()
            : SimpleFloatDataType_t<T>(), start(std::shared_ptr<std::vector<T>>(nullptr)) {}
};

struct SimpleStringDataType_t {
    std::shared_ptr<uint32_t> maxSize;

    SimpleStringDataType_t()
            : maxSize(std::shared_ptr<uint32_t>(nullptr)){}
};

struct StringDataType_t: public SimpleStringDataType_t {
    std::shared_ptr<std::string> start;

    StringDataType_t()
            : SimpleStringDataType_t(),
              start(std::shared_ptr<std::string>(nullptr)) {}
};

struct BinaryStartValue {
    ~BinaryStartValue(){
        delete[] value;
    }

    uint8_t* value;
    uint32_t length;
};

struct SimpleBinaryDataType_t {
    std::shared_ptr<std::string> mimeType;
    std::shared_ptr<uint32_t> maxSize;

    SimpleBinaryDataType_t()
            : mimeType(std::shared_ptr<std::string>(nullptr)),
              maxSize(std::shared_ptr<uint32_t>(nullptr)) {}
};

struct BinaryDataType_t : public SimpleBinaryDataType_t{
    std::shared_ptr<BinaryStartValue> start;

    BinaryDataType_t()
            : SimpleBinaryDataType_t(),
              start(std::shared_ptr<BinaryStartValue>(nullptr)) {}
};

enum class DimensionType {
    CONSTANT, LINKED_VR
};
struct Dimension_t {
    DimensionType type;
    uint64_t value;
};

static Dimension_t make_Dimension(DimensionType type, uint64_t value){
    return {type, value};
}

enum class DependenciesKind {
    DEPENDENT, LINEAR
};

struct Dependency_t {
    valueReference_t vr;
    DependenciesKind dependenciesKind;
};

static Dependency_t make_Dependency(valueReference_t vr, DependenciesKind dependenciesKind){
    return {vr, dependenciesKind};
}

struct DependencyState_t {
    bool none;
    std::vector<Dependency_t> dependecies;
};

static DependencyState_t make_DependecyState(){
    return {false, std::vector<Dependency_t>()};
}

struct Dependencies_t {
    DependencyState_t Initialization;
    DependencyState_t Run;
};

static std::shared_ptr<Dependencies_t> make_Dependencies_ptr(){
    std::shared_ptr<Dependencies_t> ptr = std::make_shared<Dependencies_t>();
    ptr->Initialization = make_DependecyState();
    ptr->Run = make_DependecyState();
    return ptr;
}

struct SimpleType_t{
    std::string name;
    std::shared_ptr<std::string> description;
    std::shared_ptr<SimpleIntegerDataType<uint8_t>> Uint8;
    std::shared_ptr<SimpleIntegerDataType<uint16_t>> Uint16;
    std::shared_ptr<SimpleIntegerDataType<uint32_t>> Uint32;
    std::shared_ptr<SimpleIntegerDataType<uint64_t>> Uint64;
    std::shared_ptr<SimpleIntegerDataType<int8_t>> Int8;
    std::shared_ptr<SimpleIntegerDataType<int16_t>> Int16;
    std::shared_ptr<SimpleIntegerDataType<int32_t>> Int32;
    std::shared_ptr<SimpleIntegerDataType<int64_t>> Int64;
    std::shared_ptr<SimpleFloatDataType_t<float32_t>> Float32;
    std::shared_ptr<SimpleFloatDataType_t<float64_t>> Float64;
    std::shared_ptr<SimpleStringDataType_t> String;
    std::shared_ptr<SimpleBinaryDataType_t> Binary;
};

template<typename T>
static SimpleType_t make_SimpleType(std::string name){
    SimpleType_t simpleType = {name, std::shared_ptr<std::string>(nullptr),
                         std::shared_ptr<SimpleIntegerDataType<uint8_t>>(nullptr),
                         std::shared_ptr<SimpleIntegerDataType<uint16_t>>(nullptr),
                         std::shared_ptr<SimpleIntegerDataType<uint32_t>>(nullptr),
                         std::shared_ptr<SimpleIntegerDataType<uint64_t>>(nullptr),
                         std::shared_ptr<SimpleIntegerDataType<int8_t>>(nullptr),
                         std::shared_ptr<SimpleIntegerDataType<int16_t>>(nullptr),
                         std::shared_ptr<SimpleIntegerDataType<int32_t>>(nullptr),
                         std::shared_ptr<SimpleIntegerDataType<int64_t>>(nullptr),
                         std::shared_ptr<SimpleFloatDataType_t<float32_t>>(nullptr),
                         std::shared_ptr<SimpleFloatDataType_t<float64_t>>(nullptr),
                         std::shared_ptr<SimpleStringDataType_t>(nullptr),
                         std::shared_ptr<SimpleBinaryDataType_t>(nullptr),};

    if (std::is_same<uint8_t, T>::value) {
        simpleType.Uint8 = std::shared_ptr<SimpleIntegerDataType<uint8_t>>(new IntegerDataType_t<uint8_t>());
    } else if (std::is_same<uint16_t, T>::value) {
        simpleType.Uint16 = std::shared_ptr<SimpleIntegerDataType<uint16_t>>(new IntegerDataType_t<uint16_t>());
    } else if (std::is_same<uint32_t, T>::value) {
        simpleType.Uint32 = std::shared_ptr<SimpleIntegerDataType<uint32_t>>(new IntegerDataType_t<uint32_t>());
    } else if (std::is_same<uint64_t, T>::value) {
        simpleType.Uint64 = std::shared_ptr<SimpleIntegerDataType<uint64_t>>(new IntegerDataType_t<uint64_t>());
    } else if (std::is_same<int8_t, T>::value) {
        simpleType.Int8 = std::shared_ptr<SimpleIntegerDataType<int8_t>>(new IntegerDataType_t<int8_t>());
    } else if (std::is_same<int16_t, T>::value) {
        simpleType.Int16 = std::shared_ptr<SimpleIntegerDataType<int16_t>>(new IntegerDataType_t<int16_t>());
    } else if (std::is_same<int32_t, T>::value) {
        simpleType.Int32 = std::shared_ptr<SimpleIntegerDataType<int32_t>>(new IntegerDataType_t<int32_t>());
    } else if (std::is_same<int64_t, T>::value) {
        simpleType.Int64 = std::shared_ptr<SimpleIntegerDataType<int64_t>>(new IntegerDataType_t<int64_t>());
    } else if (std::is_same<float32_t, T>::value) {
        simpleType.Float32 = std::shared_ptr<SimpleFloatDataType_t<float32_t>>(new FloatDataType_t<float32_t>());
    } else if (std::is_same<float64_t, T>::value) {
        simpleType.Float64 = std::shared_ptr<SimpleFloatDataType_t<float64_t>>(new FloatDataType_t<float64_t>());
    }
    return simpleType;
}

static SimpleType_t make_SimpleType_String(std::string name){
    SimpleType_t simpleType = {name, std::shared_ptr<std::string>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint8_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint16_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint32_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint64_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int8_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int16_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int32_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int64_t>>(nullptr),
                               std::shared_ptr<SimpleFloatDataType_t<float32_t>>(nullptr),
                               std::shared_ptr<SimpleFloatDataType_t<float64_t>>(nullptr),
                               std::shared_ptr<SimpleStringDataType_t>(nullptr),
                               std::shared_ptr<SimpleBinaryDataType_t>(nullptr),};

    simpleType.String = std::make_shared<SimpleStringDataType_t>();
    return simpleType;
}


static SimpleType_t make_SimpleType_Binary(std::string name){
    SimpleType_t simpleType = {name, std::shared_ptr<std::string>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint8_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint16_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint32_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<uint64_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int8_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int16_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int32_t>>(nullptr),
                               std::shared_ptr<SimpleIntegerDataType<int64_t>>(nullptr),
                               std::shared_ptr<SimpleFloatDataType_t<float32_t>>(nullptr),
                               std::shared_ptr<SimpleFloatDataType_t<float64_t>>(nullptr),
                               std::shared_ptr<SimpleStringDataType_t>(nullptr),
                               std::shared_ptr<SimpleBinaryDataType_t>(nullptr),};

    simpleType.Binary = std::make_shared<SimpleBinaryDataType_t>();
    return simpleType;
}


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
                                                                    dimensions(dimensions){}


};


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
    std::shared_ptr<steps_t> minSteps;
    std::shared_ptr<steps_t> maxSteps;
    bool initialization;
    std::shared_ptr<Dependencies_t> Dependencies;


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
                 defaultSteps(1), fixedSteps(true), minSteps(std::shared_ptr<steps_t>(nullptr)),
                 maxSteps(std::shared_ptr<steps_t>(nullptr)), initialization(false),
                 Dependencies(std::shared_ptr<Dependencies_t>(nullptr)) {}

};

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
    o->dimensions = std::vector<Dimension_t>();
    o->defaultSteps = 1;
    o->fixedSteps = true;
    o->minSteps = std::shared_ptr<steps_t>(nullptr);
    o->maxSteps = std::shared_ptr<steps_t>(nullptr);
    o->initialization = false;
    o->Dependencies = std::shared_ptr<Dependencies_t>(nullptr);
    return o;
}

static std::shared_ptr<Output_t> make_Output_String_ptr() {
    std::shared_ptr<Output_t> o (new Output_t());
    o->String = std::shared_ptr<StringDataType_t>(new StringDataType_t());
    o->dimensions = std::vector<Dimension_t>();
    o->defaultSteps = 1;
    o->fixedSteps = true;
    o->minSteps = std::shared_ptr<steps_t>(nullptr);
    o->maxSteps = std::shared_ptr<steps_t>(nullptr);
    o->initialization = false;
    o->Dependencies = std::shared_ptr<Dependencies_t>(nullptr);
    return o;
}

static std::shared_ptr<Output_t> make_Output_Binary_ptr() {
    std::shared_ptr<Output_t> o (new Output_t());
    o->Binary = std::shared_ptr<BinaryDataType_t>(new BinaryDataType_t());
    o->dimensions = std::vector<Dimension_t>();
    o->defaultSteps = 1;
    o->fixedSteps = true;
    o->minSteps = std::shared_ptr<steps_t>(nullptr);
    o->maxSteps = std::shared_ptr<steps_t>(nullptr);
    o->initialization = false;
    o->Dependencies = std::shared_ptr<Dependencies_t>(nullptr);
    return o;
}

struct StructuralParameter_t {
    std::shared_ptr<IntegerDataType_t<uint8_t>> Uint8;
    std::shared_ptr<IntegerDataType_t<uint16_t>> Uint16;
    std::shared_ptr<IntegerDataType_t<uint32_t>> Uint32;
    std::shared_ptr<IntegerDataType_t<uint64_t>> Uint64;
};

template<typename T>
static std::shared_ptr<StructuralParameter_t> make_StructuralParameter_ptr() {
    std::shared_ptr<StructuralParameter_t> structParameter = std::make_shared<StructuralParameter_t>();
    if (std::is_same<uint8_t, T>::value) {
        structParameter->Uint8 = std::shared_ptr<IntegerDataType_t<uint8_t>>(new IntegerDataType_t<uint8_t>());
    } else if (std::is_same<uint16_t, T>::value) {
        structParameter->Uint16 = std::shared_ptr<IntegerDataType_t<uint16_t>>(new IntegerDataType_t<uint16_t>());
    } else if (std::is_same<uint32_t, T>::value) {
        structParameter->Uint32 = std::shared_ptr<IntegerDataType_t<uint32_t>>(new IntegerDataType_t<uint32_t>());
    } else if (std::is_same<uint64_t, T>::value) {
        structParameter->Uint64 = std::shared_ptr<IntegerDataType_t<uint64_t>>(new IntegerDataType_t<uint64_t>());
    }
    return structParameter;
}

enum class Variability {
    FIXED, TUNABLE, DISCRETE, CONTINUOUS,
};

struct Variable_t {
    std::string name;
    valueReference_t valueReference;
    std::shared_ptr<std::string> description;
    Variability variability;
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
    return {name, valueReference, std::shared_ptr<std::string>(), Variability::CONTINUOUS,
            std::shared_ptr<double>(), std::shared_ptr<double>(),
            std::shared_ptr<uint32_t>(), std::move(Input), std::shared_ptr<Output_t>(nullptr),
            std::shared_ptr<CommonCausality_t>(nullptr), std::shared_ptr<StructuralParameter_t>(nullptr)};
}

static Variable_t
make_Variable_output(std::string name, valueReference_t valueReference, std::shared_ptr<Output_t> Output) {
    return {name, valueReference, std::shared_ptr<std::string>(), Variability::CONTINUOUS,
            std::shared_ptr<double>(), std::shared_ptr<double>(),
            std::shared_ptr<uint32_t>(), std::shared_ptr<CommonCausality_t>(nullptr), std::move(Output),
            std::shared_ptr<CommonCausality_t>(nullptr), std::shared_ptr<StructuralParameter_t>(nullptr)};
}

static Variable_t
make_Variable_parameter(std::string name, valueReference_t valueReference,
                        std::shared_ptr<CommonCausality_t> Parameter) {
    return {name, valueReference, std::shared_ptr<std::string>(), Variability::CONTINUOUS,
            std::shared_ptr<double>(), std::shared_ptr<double>(),
            std::shared_ptr<uint32_t>(), std::shared_ptr<CommonCausality_t>(nullptr), std::shared_ptr<Output_t>(nullptr),
            std::move(Parameter), std::shared_ptr<StructuralParameter_t>(nullptr)};
}

static Variable_t
make_Variable_structuralParameter(std::string name, valueReference_t valueReference,
                                  std::shared_ptr<StructuralParameter_t> StructuralParameter) {
    return {name, valueReference, std::shared_ptr<std::string>(), Variability::CONTINUOUS,
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

static std::shared_ptr<Log_t> make_Log_ptr() {
    return std::shared_ptr<Log_t>(new Log_t());
}

enum VariableNamingConvention {
    FLAT, STRUCTURED
};

struct SlaveDescription_t {
    OpMode_t OpMode;
    std::vector<Unit_t> UnitDefinitions;
    std::vector<SimpleType_t> TypeDefinitions;
    TimeRes_t TimeRes;
    std::shared_ptr<Heartbeat_t> Heartbeat;
    TransportProtocols_t TransportProtocols;
    CapabilityFlags_t CapabilityFlags;
    std::vector<Variable_t> Variables;
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
    VariableNamingConvention variableNamingConvention;
};

static SlaveDescription_t
make_SlaveDescription(uint8_t dcpMajorVersion, uint8_t dcpMinorVersion, const std::string dcpSlaveName,
                      const std::string uuid) {
    return {make_OpMode(), std::vector<Unit_t>(), std::vector<SimpleType_t>(), make_TimeRes(), std::shared_ptr<Heartbeat_t>(nullptr),
            make_TransportProtocols(),
            make_CapabilityFlags(), std::vector<Variable_t>(), std::shared_ptr<Log_t>(
                    nullptr), dcpMajorVersion, dcpMinorVersion, dcpSlaveName, uuid,
            std::shared_ptr<std::string>(nullptr), std::shared_ptr<std::string>(nullptr), std::shared_ptr<std::string>(
                    nullptr), std::shared_ptr<std::string>(nullptr), std::shared_ptr<std::string>(
                    nullptr), std::shared_ptr<std::string>(nullptr), std::shared_ptr<std::string>(nullptr),
            VariableNamingConvention::FLAT};
}

static std::shared_ptr<SlaveDescription_t>
make_SlaveDescription_ptr(uint8_t dcpMajorVersion, uint8_t dcpMinorVersion, const std::string dcpSlaveName,
                      const std::string uuid) {
    std::shared_ptr<SlaveDescription_t> ptr = std::make_shared<SlaveDescription_t>();
    ptr->OpMode = make_OpMode();
    ptr->UnitDefinitions = std::vector<Unit_t>();
    ptr->TypeDefinitions = std::vector<SimpleType_t>();
    ptr->TimeRes = make_TimeRes();
    ptr->Heartbeat = std::shared_ptr<Heartbeat_t>(nullptr);
    ptr->TransportProtocols = make_TransportProtocols();
    ptr->CapabilityFlags = make_CapabilityFlags();
    ptr->Variables = std::vector<Variable_t>();
    ptr->Log = std::shared_ptr<Log_t>(nullptr);
    ptr->dcpMajorVersion = dcpMajorVersion;
    ptr->dcpMinorVersion = dcpMinorVersion;
    ptr->dcpSlaveName = dcpSlaveName;
    ptr->uuid = uuid;
    ptr->description = std::shared_ptr<std::string>(nullptr);
    ptr->author = std::shared_ptr<std::string>(nullptr);
    ptr->version = std::shared_ptr<std::string>(nullptr);
    ptr->copyright = std::shared_ptr<std::string>(nullptr);
    ptr->license = std::shared_ptr<std::string>(nullptr);
    ptr->generationTool = std::shared_ptr<std::string>(nullptr);
    ptr->generationDateAndTime = std::shared_ptr<std::string>(nullptr);
    ptr->variableNamingConvention = VariableNamingConvention::FLAT;
    return ptr;
}


#endif //ACOSAR_DCPSLAVEDESCRIPTIONELEMENTS_H
