/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "android.hardware.thermal@1.0-impl"

#include <errno.h>
#include <math.h>

#include <vector>

#include <log/log.h>
#include "Thermal.h"

namespace android {
namespace hardware {
namespace thermal {
namespace V1_0 {
namespace implementation {

static const Temperature_1_0 kTemp_1_0 = {
        .type = static_cast<::android::hardware::thermal::V1_0::TemperatureType>(
                TemperatureType::SKIN),
        .name = "test temperature sensor",
        .currentValue = 30.8,
        .throttlingThreshold = 48.0,
        .shutdownThreshold = 60.0,
        .vrThrottlingThreshold = 49.0,
};


static const CoolingDevice_1_0 kCooling_1_0 = {
        .type = ::android::hardware::thermal::V1_0::CoolingType::FAN_RPM,
        .name = "test cooling device",
        .currentValue = 100.0,
};


static const CpuUsage kCpuUsage = {
        .name = "cpu_name",
        .active = 0,
        .total = 0,
        .isOnline = true,
};

// Methods from ::android::hardware::thermal::V1_0::IThermal follow.
Return<void> Thermal::getTemperatures(getTemperatures_cb _hidl_cb) {
    ThermalStatus status;
    status.code = ThermalStatusCode::SUCCESS;
    std::vector<Temperature_1_0> temperatures = {kTemp_1_0};
    _hidl_cb(status, temperatures);
    return Void();
}

Return<void> Thermal::getCpuUsages(getCpuUsages_cb _hidl_cb) {
    ThermalStatus status;
    status.code = ThermalStatusCode::SUCCESS;
    std::vector<CpuUsage> cpu_usages = {kCpuUsage};
    _hidl_cb(status, cpu_usages);
    return Void();
}

Return<void> Thermal::getCoolingDevices(getCoolingDevices_cb _hidl_cb) {
    ThermalStatus status;
    status.code = ThermalStatusCode::SUCCESS;
    std::vector<CoolingDevice_1_0> cooling_devices = {kCooling_1_0};
    _hidl_cb(status, cooling_devices);
    return Void();
}


}  // namespace implementation
}  // namespace V1_0
}  // namespace thermal
}  // namespace hardware
}  // namespace android
