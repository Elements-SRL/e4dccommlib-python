#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <cstdint>
#include <string>
#include <vector>

#include "messagedispatcher.h"

namespace py = pybind11;

/* -----------------------------------------------------------------------
 *  Helper macros (same convention as cl384_python_wrapper's main.cpp)
 * ----------------------------------------------------------------------- */

#define GENERAL_GET(fname, inType) \
.def(#fname, [](MessageDispatcher &self) { \
        inType inArg; \
        auto err = self.fname(inArg); \
        return std::make_tuple(err, inArg); \
})

#define GET_U32(fname) GENERAL_GET(fname, uint32_t)
#define GET_DOUBLE(fname) GENERAL_GET(fname, double)
#define GET_STRING(fname) GENERAL_GET(fname, std::string)
#define GET_MEASUREMENT_VEC(fname) GENERAL_GET(fname, std::vector<Measurement_t>)
#define GET_RANGED_MEASUREMENT_VEC(fname) GENERAL_GET(fname, std::vector<RangedMeasurement_t>)

    /* -----------------------------------------------------------------------
 *  Trampoline
 * -----------------------------------------------------------------------
 * Unlike NanionTemperatureControlInterface (nanionTClib), MessageDispatcher
 * declares a long list of pure virtual methods (see messagedispatcher.h),
 * plus one protected pure virtual (setModelsCoders) and one virtual-but-not-
 * pure method with a default body (getDeviceInfo). A trampoline IS required
 * here so pybind11 can dispatch to Python overrides and so the class is
 * concrete enough to be constructed at all.
 *
 * IMPORTANT: this must inherit *publicly* from MessageDispatcher.
 * cl384_python_wrapper's PyMessageDispatcher uses plain
 * `class PyMessageDispatcher : MessageDispatcher` (private/default
 * inheritance). That compiles the trampoline's own member functions fine,
 * but pybind11's holder (std::unique_ptr<MessageDispatcher>) needs an
 * accessible upcast from PyMessageDispatcher* wherever py::init<...>()
 * constructs one - with private inheritance that upcast is only usable from
 * inside PyMessageDispatcher's own member functions, not from pybind11's
 * generic class-registration code, so it is not something to copy here.
 */
    class PyMessageDispatcher : public MessageDispatcher {
public:
    using MessageDispatcher::MessageDispatcher;

    ErrorCodes_t disconnectDevice() override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, disconnectDevice, );
    }
    ErrorCodes_t getFwLoadingProgress(double &percentage) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, getFwLoadingProgress, percentage);
    }
    ErrorCodes_t init() override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, init, );
    }
    ErrorCodes_t deinit() override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, deinit, );
    }
    ErrorCodes_t connect(uint32_t modelsNum, bool dontLoadFwFlag) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, connect, modelsNum, dontLoadFwFlag);
    }
    ErrorCodes_t disconnect() override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, disconnect, );
    }
    ErrorCodes_t turnLedOn(uint16_t ledIndex, bool on) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, turnLedOn, ledIndex, on);
    }
    ErrorCodes_t resetDevice(bool reset, bool applyFlag = true) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, resetDevice, reset, applyFlag);
    }
    ErrorCodes_t sendCommands() override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, sendCommands, );
    }
    ErrorCodes_t runModel(uint32_t modelIdx, bool flag, bool applyFlag = false) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, runModel, modelIdx, flag, applyFlag);
    }
    ErrorCodes_t setUserParameter(uint32_t modelIdx, uint32_t parameterIdx, Measurement_t value, bool applyFlag = false) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, setUserParameter, modelIdx, parameterIdx, value, applyFlag);
    }
    ErrorCodes_t setVCCurrentRange(uint16_t currentRangeIdx, bool applyFlag = true) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, setVCCurrentRange, currentRangeIdx, applyFlag);
    }
    ErrorCodes_t setCCCurrentRange(uint16_t currentRangeIdx, bool applyFlag = true) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, setCCCurrentRange, currentRangeIdx, applyFlag);
    }
    ErrorCodes_t setVCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag = true) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, setVCVoltageRange, voltageRangeIdx, applyFlag);
    }
    ErrorCodes_t setCCVoltageRange(uint16_t voltageRangeIdx, bool applyFlag = true) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, setCCVoltageRange, voltageRangeIdx, applyFlag);
    }
    ErrorCodes_t setSamplingRate(uint16_t samplingRateIdx, bool applyFlag = true) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, setSamplingRate, samplingRateIdx, applyFlag);
    }
    ErrorCodes_t setDebugBit(uint16_t wordOffset, uint16_t bitOffset, bool status) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, setDebugBit, wordOffset, bitOffset, status);
    }
    ErrorCodes_t getQueueStatus(uint32_t *availableDataPackets, bool *bufferOverflowFlag, bool *dataLossFlag) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, getQueueStatus, availableDataPackets, bufferOverflowFlag, dataLossFlag);
    }
    ErrorCodes_t getDataPackets(float *&data, uint32_t packetsNumber, uint32_t *packetsRead) override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, getDataPackets, data, packetsNumber, packetsRead);
    }
    ErrorCodes_t purgeData() override {
        PYBIND11_OVERRIDE_PURE(ErrorCodes_t, MessageDispatcher, purgeData, );
    }

    /* getDeviceInfo is virtual but NOT pure - the base class already
     * provides a default body that returns ErrorFeatureNotImplemented - so
     * this uses PYBIND11_OVERRIDE (falls back to the base implementation
     * when Python hasn't overridden it), not PYBIND11_OVERRIDE_PURE. */
    ErrorCodes_t getDeviceInfo(uint8_t &deviceVersion, uint8_t &deviceSubversion, uint32_t &firmwareVersion) override {
        PYBIND11_OVERRIDE(ErrorCodes_t, MessageDispatcher, getDeviceInfo, deviceVersion, deviceSubversion, firmwareVersion);
    }

protected:
    /* Protected + pure virtual: still has to be overridden for
     * PyMessageDispatcher to be a concrete, instantiable type, but it is
     * never meant to be called from Python, so it is not exposed via
     * .def(...) below. */
    void setModelsCoders() override {
        PYBIND11_OVERRIDE_PURE(void, MessageDispatcher, setModelsCoders, );
    }
};

/* -----------------------------------------------------------------------
 *  Zero-copy numpy view over the float* buffer returned by getDataPackets
 * ----------------------------------------------------------------------- */

class FloatBuffer {
public:
    FloatBuffer(float *data, size_t size) : data(data), size(size) {}
    py::buffer_info get_buffer() {
        return py::buffer_info(
            data,                                       // Pointer to buffer
            sizeof(float),                               // Size of one scalar
            py::format_descriptor<float>::format(),      // Python struct-style format descriptor
            1,                                           // Number of dimensions
            { size },                                    // Buffer dimensions
            { sizeof(float) }                            // Strides (in bytes) for each index
            );
    }
private:
    float *data;
    size_t size;
};

PYBIND11_MODULE(e4dccommlib_python_wrapper, m) {
    m.doc() = "pybind11 bindings for e4dccommlib (Nanion e4 Dynamic Clamp communication library)";

    /* ---------------------------------------------------------------
     *  Enums
     * --------------------------------------------------------------- */

    py::enum_<ErrorCodes_t>(m, "ErrorCodes")
        .value("Success", Success)
        .value("ErrorNoDeviceFound", ErrorNoDeviceFound)
        .value("ErrorListDeviceFailed", ErrorListDeviceFailed)
        .value("ErrorDeviceNotFound", ErrorDeviceNotFound)
        .value("ErrorEepromAlreadyConnected", ErrorEepromAlreadyConnected)
        .value("ErrorEepromConnectionFailed", ErrorEepromConnectionFailed)
        .value("ErrorEepromDisconnectionFailed", ErrorEepromDisconnectionFailed)
        .value("ErrorEepromNotConnected", ErrorEepromNotConnected)
        .value("ErrorEepromReadFailed", ErrorEepromReadFailed)
        .value("ErrorEepromNotRecognized", ErrorEepromNotRecognized)
        .value("ErrorInitializationFailed", ErrorInitializationFailed)
        .value("ErrorDeviceTypeNotRecognized", ErrorDeviceTypeNotRecognized)
        .value("ErrorDeviceAlreadyConnected", ErrorDeviceAlreadyConnected)
        .value("ErrorDeviceProgrammingFailed", ErrorDeviceProgrammingFailed)
        .value("ErrorDeviceNotConnected", ErrorDeviceNotConnected)
        .value("ErrorDeviceConnectionFailed", ErrorDeviceConnectionFailed)
        .value("ErrorFtdiConfigurationFailed", ErrorFtdiConfigurationFailed)
        .value("ErrorDeviceDisconnectionFailed", ErrorDeviceDisconnectionFailed)
        .value("ErrorProgrammingChannelNotStarted", ErrorProgrammingChannelNotStarted)
        .value("ErrorSendMessageFailed", ErrorSendMessageFailed)
        .value("ErrorCommandNotImplemented", ErrorCommandNotImplemented)
        .value("ErrorValueOutOfRange", ErrorValueOutOfRange)
        .value("WarningNotEnoughAvailable", WarningNotEnoughAvailable)
        .value("ErrorInvalidProtocolParameters", ErrorInvalidProtocolParameters)
        .value("WarningNothingTodo", WarningNothingTodo)
        .value("ErrorFeatureNotImplemented", ErrorFeatureNotImplemented)
        .value("ErrorUpgradesNotAvailable", ErrorUpgradesNotAvailable)
        .value("ErrorExpiredDevice", ErrorExpiredDevice)
        .value("ErrorMemoryInitialization", ErrorMemoryInitialization)
        .value("ErrorUnknown", ErrorUnknown)
        .export_values();

    py::enum_<UnitPfx_t>(m, "UnitPfx")
        .value("UnitPfxFemto", UnitPfxFemto)
        .value("UnitPfxPico", UnitPfxPico)
        .value("UnitPfxNano", UnitPfxNano)
        .value("UnitPfxMicro", UnitPfxMicro)
        .value("UnitPfxMilli", UnitPfxMilli)
        .value("UnitPfxNone", UnitPfxNone)
        .value("UnitPfxKilo", UnitPfxKilo)
        .value("UnitPfxMega", UnitPfxMega)
        .value("UnitPfxGiga", UnitPfxGiga)
        .value("UnitPfxTera", UnitPfxTera)
        .value("UnitPfxPeta", UnitPfxPeta)
        .value("UnitPfxNum", UnitPfxNum)
        .export_values();

    py::enum_<DeviceTypes_t>(m, "DeviceTypes")
        .value("DevicePatchliner_EL04f_spartan6_V01", DevicePatchliner_EL04f_spartan6_V01)
        .value("DevicePatchliner_EL07ab_spartan6_V01", DevicePatchliner_EL07ab_spartan6_V01)
        .value("DevicePatchliner_EL07ab_artix7_V01", DevicePatchliner_EL07ab_artix7_V01)
        .value("DevicePatchliner_EL07ab_artix7_V02", DevicePatchliner_EL07ab_artix7_V02)
        .value("DevicePatchliner_EL07cd_artix7_V01", DevicePatchliner_EL07cd_artix7_V01)
        .value("DevicePatchliner_EL07cd_artix7_V02", DevicePatchliner_EL07cd_artix7_V02)
        .value("DeviceSyncropatch_EL07ab_V01", DeviceSyncropatch_EL07ab_V01)
        .value("DeviceUnknown", DeviceUnknown)
        .value("DevicesNum", DevicesNum)
        .export_values();
    /* Not consumed by any MessageDispatcher public method directly (device
     * type is auto-detected inside connectDevice), exposed for reference. */

    py::enum_<MessageDispatcher::ConnectionStatus_t>(m, "ConnectionStatus")
        .value("Connected", MessageDispatcher::Connected)
        .value("Calibrating", MessageDispatcher::Calibrating)
        .value("Paused", MessageDispatcher::Paused)
        .value("Disconnected", MessageDispatcher::Disconnected)
        .export_values();

    /* ---------------------------------------------------------------
     *  Value types
     * ---------------------------------------------------------------
     * Measurement_t and RangedMeasurement_t are plain aggregates (no
     * user-declared constructor) with default member initializers on
     * prefix/unit, so we bind a default py::init<>() plus a convenience
     * factory lambda instead of py::init<...> (which requires an actual
     * constructor). Both structs define several operator overloads
     * (==, !=, +, -, *, /, >=, ...) in e4dccommlib_global_addendum.h;
     * only the ones confirmed above are bound below - check that header
     * before relying on any other operator from Python. */

    py::class_<Measurement_t>(m, "Measurement", py::module_local())
        .def(py::init<>())
        .def(py::init([](double value, UnitPfx_t prefix, std::string unit) {
                 return Measurement_t{value, prefix, unit};
             }), py::arg("value"), py::arg("prefix") = UnitPfxNone, py::arg("unit") = std::string())
        .def_readwrite("value", &Measurement_t::value)
        .def_readwrite("prefix", &Measurement_t::prefix)
        .def_readwrite("unit", &Measurement_t::unit)
        .def("getNoPrefixValue", &Measurement_t::getNoPrefixValue)
        .def("getPrefix", &Measurement_t::getPrefix)
        .def("getFullUnit", &Measurement_t::getFullUnit)
        .def("multiplier", &Measurement_t::multiplier);

    py::class_<RangedMeasurement_t>(m, "RangedMeasurement", py::module_local())
        .def(py::init<>())
        .def(py::init([](double min, double max, double step, UnitPfx_t prefix, std::string unit) {
                 return RangedMeasurement_t{min, max, step, prefix, unit};
             }), py::arg("min"), py::arg("max"), py::arg("step"), py::arg("prefix") = UnitPfxNone, py::arg("unit") = std::string())
        .def_readwrite("min", &RangedMeasurement_t::min)
        .def_readwrite("max", &RangedMeasurement_t::max)
        .def_readwrite("step", &RangedMeasurement_t::step)
        .def_readwrite("prefix", &RangedMeasurement_t::prefix)
        .def_readwrite("unit", &RangedMeasurement_t::unit)
        .def("steps", &RangedMeasurement_t::steps)
        .def("getPrefix", &RangedMeasurement_t::getPrefix)
        .def("getFullUnit", &RangedMeasurement_t::getFullUnit)
        .def("multiplier", &RangedMeasurement_t::multiplier)
        .def("getMin", &RangedMeasurement_t::getMin)
        .def("getXth", &RangedMeasurement_t::getXth)
        .def("getClosestIndex", &RangedMeasurement_t::getClosestIndex)
        .def("includes", &RangedMeasurement_t::includes)
        .def("__eq__", [](const RangedMeasurement_t &a, const RangedMeasurement_t &b) { return a == b; })
        .def("__ne__", [](const RangedMeasurement_t &a, const RangedMeasurement_t &b) { return a != b; });

    /* ---------------------------------------------------------------
     *  MessageDispatcher
     * --------------------------------------------------------------- */

    py::class_<MessageDispatcher, PyMessageDispatcher>(m, "MessageDispatcher", py::module_local())
        .def(py::init<std::string, std::string>(), py::arg("deviceId"), py::arg("modelPath"))

        /* Connection methods */
        .def_static("detectDevices", []() {
            std::vector<std::string> deviceIds;
            ErrorCodes_t err = MessageDispatcher::detectDevices(deviceIds);
            return std::make_tuple(err, deviceIds);
        }, "Detect plugged in devices")
        .def_static("connectDevice", [](std::string deviceId, std::string modelPath, uint16_t commBoardIdx, uint16_t modelsNum, bool dontLoadFwFlag) {
            MessageDispatcher *dispatcher = nullptr;
            ErrorCodes_t err = MessageDispatcher::connectDevice(deviceId, modelPath, commBoardIdx, modelsNum, dontLoadFwFlag, dispatcher);
            return std::make_tuple(err, dispatcher);
        }, py::arg("deviceId"), py::arg("modelPath"), py::arg("commBoardIdx"), py::arg("modelsNum"), py::arg("dontLoadFwFlag") = false,
                    py::return_value_policy::take_ownership,
                    "Auto-detects the concrete device type behind deviceId, constructs the matching\n"
                    "MessageDispatcher subclass and connects to it. Ownership of the returned\n"
                    "dispatcher is taken by Python.")
        .def("disconnectDevice", &MessageDispatcher::disconnectDevice)
        GET_DOUBLE(getFwLoadingProgress)
        .def("init", &MessageDispatcher::init)
        .def("deinit", &MessageDispatcher::deinit)
        .def("connect", &MessageDispatcher::connect, py::arg("modelsNum"), py::arg("dontLoadFwFlag"))
        .def("disconnect", &MessageDispatcher::disconnect)

        /* Tx methods */
        .def("turnLedOn", &MessageDispatcher::turnLedOn, py::arg("ledIndex"), py::arg("on"))
        .def("resetDevice", &MessageDispatcher::resetDevice, py::arg("reset"), py::arg("applyFlag") = true)
        .def("sendCommands", &MessageDispatcher::sendCommands)
        .def("runModel", &MessageDispatcher::runModel, py::arg("modelIdx"), py::arg("flag"), py::arg("applyFlag") = false)
        .def("setUserParameter", &MessageDispatcher::setUserParameter,
             py::arg("modelIdx"), py::arg("parameterIdx"), py::arg("value"), py::arg("applyFlag") = false)
        .def("setVCCurrentRange", &MessageDispatcher::setVCCurrentRange, py::arg("currentRangeIdx"), py::arg("applyFlag") = true)
        .def("setCCCurrentRange", &MessageDispatcher::setCCCurrentRange, py::arg("currentRangeIdx"), py::arg("applyFlag") = true)
        .def("setVCVoltageRange", &MessageDispatcher::setVCVoltageRange, py::arg("voltageRangeIdx"), py::arg("applyFlag") = true)
        .def("setCCVoltageRange", &MessageDispatcher::setCCVoltageRange, py::arg("voltageRangeIdx"), py::arg("applyFlag") = true)
        .def("setSamplingRate", &MessageDispatcher::setSamplingRate, py::arg("samplingRateIdx"), py::arg("applyFlag") = true)
        .def("setDebugBit", &MessageDispatcher::setDebugBit, py::arg("wordOffset"), py::arg("bitOffset"), py::arg("status"))

        /* Rx methods */
        GET_STRING(getDeviceId)
        .def("getDeviceInfo", [](MessageDispatcher &self) {
            uint8_t deviceVersion = 0, deviceSubversion = 0;
            uint32_t firmwareVersion = 0;
            auto err = self.getDeviceInfo(deviceVersion, deviceSubversion, firmwareVersion);
            return std::make_tuple(err, deviceVersion, deviceSubversion, firmwareVersion);
        })
        .def("getQueueStatus", [](MessageDispatcher &self) {
            uint32_t availableDataPackets = 0;
            bool bufferOverflowFlag = false;
            bool dataLossFlag = false;
            auto err = self.getQueueStatus(&availableDataPackets, &bufferOverflowFlag, &dataLossFlag);
            return std::make_tuple(err, availableDataPackets, bufferOverflowFlag, dataLossFlag);
        })
        .def("getDataPackets", [](MessageDispatcher &self, uint32_t packetsNumber) {
            /* data is set by the callee to point at its own internal buffer
             * (allocated/owned by the connected device's MessageDispatcher
             * subclass), so the initial value passed in is irrelevant. The
             * returned FloatBuffer is a *non-owning* view: keep the
             * dispatcher object alive for as long as you use it, same
             * caveat as cl384_python_wrapper's I16Buffer/F64Buffer. */
            float *data = nullptr;
            uint32_t packetsRead = 0;
            auto err = self.getDataPackets(data, packetsNumber, &packetsRead);
            return std::make_tuple(err, packetsRead, FloatBuffer(data, packetsRead));
        }, py::arg("packetsNumber"))
        .def("purgeData", &MessageDispatcher::purgeData)
        .def("convertVoltageValueToFloat", [](MessageDispatcher &self, uint16_t intValue) {
            double fltValue = 0.0;
            auto err = self.convertVoltageValue(intValue, fltValue);
            return std::make_tuple(err, fltValue);
        }, py::arg("intValue"))
        .def("convertVoltageValueToInt", [](MessageDispatcher &self, double fltValue) {
            uint16_t intValue = 0;
            auto err = self.convertVoltageValue(fltValue, intValue);
            return std::make_tuple(err, intValue);
        }, py::arg("fltValue"))
        GET_RANGED_MEASUREMENT_VEC(getVCCurrentRanges)
        GET_RANGED_MEASUREMENT_VEC(getCCCurrentRanges)
        GET_RANGED_MEASUREMENT_VEC(getVCVoltageRanges)
        GET_RANGED_MEASUREMENT_VEC(getCCVoltageRanges)
        GET_MEASUREMENT_VEC(getSamplingRates)
        GET_U32(getModelsNum)
        .def("getUserParameters", [](MessageDispatcher &self) {
            std::vector<std::string> names;
            std::vector<RangedMeasurement_t> ranges;
            std::vector<Measurement_t> defaultValues;
            auto err = self.getUserParameters(names, ranges, defaultValues);
            return std::make_tuple(err, names, ranges, defaultValues);
        });

    /* ---------------------------------------------------------------
     *  Buffer helper types
     * --------------------------------------------------------------- */

    py::class_<FloatBuffer>(m, "FloatBuffer", py::buffer_protocol(), py::module_local())
        .def_buffer(&FloatBuffer::get_buffer);
}
