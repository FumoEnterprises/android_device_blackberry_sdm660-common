/*
 * Copyright (C) 2021-2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/sysinfo.h>
#include <unistd.h>

#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include <android-base/logging.h>
#include <android-base/properties.h>
using android::base::SetProperty;

#define MAC_SOURCE       "/proc/device-tree/board_mac_addr/macaddresswlan"
#define MAC_TARGET_WLAN  "/data/misc/wifi/wlan_mac.bin"
#define MAC_TARGET_BT    "/data/misc/bluetooth/bt_mac.bin"

void property_override(const std::string& prop,
                       const std::string& value,
                       bool /* add */) {
    if (!android::base::SetProperty(prop, value)) {
        LOG(ERROR) << "Failed to set property " << prop;
    }
}

static bool file_exists(const char* path) {
    return access(path, F_OK) == 0;
}

static std::string mac_to_string(const std::vector<unsigned char>& mac) {
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setfill('0');
    for (unsigned char b : mac)
        oss << std::setw(2) << static_cast<int>(b);
    return oss.str();
}

static std::string mac_to_bt_string(const std::vector<unsigned char>& mac) {
    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setfill('0');

    for (size_t i = 0; i < mac.size(); ++i) {
        if (i != 0)
            oss << ":";
        oss << std::setw(2) << static_cast<int>(mac[i]);
    }
    return oss.str();
}

static void increment_mac(std::vector<unsigned char>& mac) {
    for (int i = static_cast<int>(mac.size()) - 1; i >= 0; --i) {
        if (++mac[i] != 0)
            break;
    }
}

int main() {
    bool skip_wlan = false;
    bool reboot = false;

    bool wlan_exists = file_exists(MAC_TARGET_WLAN);
    bool bt_exists   = file_exists(MAC_TARGET_BT);

    if (wlan_exists && bt_exists) {
        LOG(INFO) << "MACs already written. Skipping.";
        return 0;
    }

    skip_wlan = wlan_exists;
    reboot = !bt_exists;

    std::ifstream in(MAC_SOURCE, std::ios::binary);
    if (!in) {
        LOG(ERROR) << "Failed to open MAC source";
        return -1;
    }

    std::vector<unsigned char> mac(6);
    in.read(reinterpret_cast<char*>(mac.data()), mac.size());
    if (in.gcount() != static_cast<std::streamsize>(mac.size())) {
        LOG(ERROR) << "Failed to read MAC address";
        return -1;
    }

    if (!skip_wlan) {
        std::ofstream out(MAC_TARGET_WLAN);
        if (!out) {
            LOG(ERROR) << "Failed to open WLAN MAC output";
            return -1;
        }

        for (int i = 0; i < 3; ++i) {
            out << "Intf" << i
            << "MacAddress=" << mac_to_string(mac) << "\n";
            increment_mac(mac);
        }
        out << "END\n";
    }

    std::ofstream out_bt(MAC_TARGET_BT);
    if (!out_bt) {
        LOG(ERROR) << "Failed to open BT MAC output";
        return -1;
    }

    increment_mac(mac);
    out_bt << mac_to_bt_string(mac) << "\n";

    if (!reboot)
        return 0;

    std::this_thread::sleep_for(std::chrono::milliseconds(20000));

    property_override("sys.powerctl", "reboot", true);

    return -1; // goodbye...
}
