"""
Example usage of the e4dccommlib_python_wrapper module.

Demonstrates the full device lifecycle:
  1. auto-detect a plugged-in device
  2. connect to it (auto-detects FTDI vs Opal Kelly internally)
  3. wait for FW loading to complete
  4. read basic device/model info
  5. configure a range and a user parameter, then run the model
  6. poll for acquired data
  7. clean disconnection, always executed via `finally`

Build this wrapper first (see CMakeLists.txt) so that
e4dccommlib_python_wrapper.pyd is importable, e.g. by running this script
from the same folder as the .pyd, or by adding that folder to sys.path.
"""

import sys
import time

import e4dccommlib_python_wrapper as e4dc


def check(err, action):
    """Print the outcome of a call and abort the script on error."""
    if err != e4dc.ErrorCodes.Success:
        print(f"{action} failed with error code {err}")
        sys.exit(1)
    print(f"{action}: OK")


def main():
    # --- 1. Auto-detect --------------------------------------------------
    err, device_ids = e4dc.MessageDispatcher.detectDevices()
    check(err, "detectDevices")
    if not device_ids:
        print("No e4DC devices found.")
        return
    print("Devices found:", device_ids)
    device_id = device_ids[0]

    # --- 2. Connect --------------------------------------------------------
    # modelPath must point to a folder containing config.yaml and top.bit
    # (see MessageDispatcher's constructor in messagedispatcher.cpp).
    model_path = r"C:\path\to\model"
    err, dispatcher = e4dc.MessageDispatcher.connectDevice(
        deviceId=device_id,
        modelPath=model_path,
        commBoardIdx=0,
        modelsNum=1,
        dontLoadFwFlag=False,
    )
    check(err, "connectDevice")

    try:
        # --- 3. Wait for FW loading to complete ----------------------------
        # (skip this loop if you connected with dontLoadFwFlag=True)
        while True:
            err, percentage = dispatcher.getFwLoadingProgress()
            check(err, "getFwLoadingProgress")
            print(f"FW loading: {percentage:.1f}%")
            if percentage >= 100.0:
                break
            time.sleep(0.5)

        # --- 4. Basic device/model info -------------------------------------
        err, device_id_readback = dispatcher.getDeviceId()
        check(err, "getDeviceId")
        print("Connected to device:", device_id_readback)

        err, models_num = dispatcher.getModelsNum()
        check(err, "getModelsNum")
        print("Models available:", models_num)

        err, names, ranges, defaults = dispatcher.getUserParameters()
        check(err, "getUserParameters")
        for name, rng, default in zip(names, ranges, defaults):
            print(
                f"  {name}: [{rng.min}, {rng.max}] step {rng.step}, "
                f"default {default.value}{default.getFullUnit()}"
            )

        # --- 5. Configure a range and a parameter, then run the model ------
        err, vc_current_ranges = dispatcher.getVCCurrentRanges()
        check(err, "getVCCurrentRanges")
        if vc_current_ranges:
            err = dispatcher.setVCCurrentRange(0, True)
            check(err, "setVCCurrentRange")

        if names:
            new_value = e4dc.Measurement(defaults[0].value, defaults[0].prefix, defaults[0].unit)
            err = dispatcher.setUserParameter(0, 0, new_value, True)
            check(err, "setUserParameter")

        err = dispatcher.sendCommands()
        check(err, "sendCommands")

        err = dispatcher.runModel(0, True, True)
        check(err, "runModel (start)")

        # --- 6. Poll for acquired data --------------------------------------
        for _ in range(5):
            err, available, overflow, lost = dispatcher.getQueueStatus()
            check(err, "getQueueStatus")
            print(f"Available packets: {available}, overflow: {overflow}, lost: {lost}")

            if available > 0:
                err, packets_read, data = dispatcher.getDataPackets(available)
                check(err, "getDataPackets")
                first_sample = data[0] if packets_read else "n/a"
                print(f"Read {packets_read} packets, first sample: {first_sample}")

            time.sleep(0.2)

        err = dispatcher.runModel(0, False, True)
        check(err, "runModel (stop)")

    finally:
        # --- 7. Always disconnect, even if something above raised/exited ---
        err = dispatcher.disconnectDevice()
        check(err, "disconnectDevice")


if __name__ == "__main__":
    main()
