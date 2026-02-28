/*
 * Copyright (C) 2021-2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <unistd.h>

#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <unordered_set>

#include <android-base/logging.h>
#include <android-base/properties.h>

#define MAC_SOURCE          "/dev/block/by-name/boardid"
#define MAC_TARGET_WLAN     "/data/misc/wifi/wlan_mac.bin"
#define MAC_TARGET_BT       "/data/misc/bluetooth/bt_mac.bin"

#define TAB_PATH           "/dev/block/by-name/traceability"

using android::base::SetProperty;

static const std::unordered_set<int> singlesim_prds = {
    63824001,
    63824009,
    63824013,
    63824015,
    63824017,
    63824021,
    63824031,
    63824035,
    63824036,
    63824039,
    63824041,
    63824042,
    63824043,
    63824045,
    63824046,
    63824050,
    63824501,
    63824509,
    63824513,
    63824515,
    63824521,
    63824535,
    63824539,
    63824541,
    63824542,
    63824545,
    63824547,
    63825001,
    63825002,
    63825003,
    63825004,
    63825005,
    63825006,
    63825007,
    63825014,
    63825015
};

void property_override(const std::string& prop,
                       const std::string& value,
                       bool /* add */) {
    if (!android::base::SetProperty(prop, value)) {
        LOG(ERROR) << "Failed to set property " << prop;
    }
}

static bool file_nonempty(const char* path) {
    struct stat st;
    return stat(path, &st) == 0 && st.st_size > 0;
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

int read_mac(const char* path, const char magic[],
             const size_t magic_len, std::vector<unsigned char>& mac,
             const uint8_t padding) {
    std::ifstream in_bt(path, std::ios::binary);
    if (!in_bt) {
        LOG(ERROR) << "Failed to open MAC source";
        return -1;
    }

    std::vector<char> window(magic_len);
    size_t pos = 0;

    if (!in_bt.read(window.data(), magic_len))
        return -1;

    while (true) {
        if (std::memcmp(window.data(), magic, magic_len) == 0) {
            mac.resize(6);
            in_bt.ignore(padding);
            if (!in_bt.read(reinterpret_cast<char*>(mac.data()), 6))
                return -1;
            return 0;
        }

        char next;
        if (!in_bt.get(next))
            break;

        std::memmove(window.data(), window.data() + 1, magic_len - 1);
        window[magic_len - 1] = next;
    }

    LOG(ERROR) << magic << " MAC not found";
    return -1;
}

static bool set_dualsim(const char* path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        LOG(ERROR) << "Failed to open partition";
        return false;
    }

    std::string data((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());

    const std::string key = "PRD-";
    size_t pos = data.rfind(key);
    if (pos == std::string::npos) {
        LOG(ERROR) << "PRD start not present";
        return false;
    }

    pos += key.size();

    size_t end = pos;
    while (end < data.size() && data[end] != '\0')
        ++end;

    std::string variant = data.substr(pos, end - pos);

    LOG(INFO) << "Detected variant: " << variant;

    // remove '-', convert to int, and match from an array of dual sim or single sim prds
    variant.erase(std::remove(variant.begin(), variant.end(), '-'), variant.end());

    int prd = std::stoi(variant);
    bool dualsim = !(singlesim_prds.count(prd) > 0);

    if (dualsim) {
        property_override("persist.radio.multisim.config", "dsds", true);
        LOG(INFO) << "Dual SIM variant detected";
        return true;
    }

    LOG(INFO) << "Single SIM variant detected";

    return false;
}

int main() {
    bool skip_wlan = false;
    bool reboot = false;

    // Even if the MACs exist, we still check if the device is dsds/single sim
    if (!set_dualsim(TAB_PATH))
        property_override("persist.radio.multisim.config", "none", true);

    bool wlan_exists = file_nonempty(MAC_TARGET_WLAN);
    bool bt_exists   = file_nonempty(MAC_TARGET_BT);

#ifndef MACLOADER_DEBUG
    if (wlan_exists && bt_exists) {
        LOG(INFO) << "MACs already written. Skipping.";
        return 0;
    }

    skip_wlan = wlan_exists;
    reboot = !bt_exists;
#else
    skip_wlan = false;
    reboot = false;
#endif

    if (!skip_wlan) {
        std::ofstream out(MAC_TARGET_WLAN);
        if (!out) {
            LOG(ERROR) << "Failed to open WLAN MAC output";
            return -1;
        }

        std::vector<unsigned char> mac_wlan;
        if (read_mac(MAC_SOURCE, "macaddresswlan", 14, mac_wlan, 2) == 0) {
            for (int i = 0; i < 3; ++i) {
                out << "Intf" << i
                << "MacAddress=" << mac_to_string(mac_wlan) << "\n";
                increment_mac(mac_wlan);
            }
            out << "END\n";
#ifdef MACLOADER_DEBUG
            LOG(ERROR) << "Read WLAN MAC: " << mac_to_bt_string(mac_wlan);
#endif
        }
    }

    std::ofstream out_bt(MAC_TARGET_BT);
    if (!out_bt) {
        LOG(ERROR) << "Failed to open BT MAC output";
        return -1;
    }

    std::vector<unsigned char> mac_bt;
    if (read_mac(MAC_SOURCE, "macaddressbt", 12, mac_bt, 0) == 0) {
        out_bt << mac_to_bt_string(mac_bt) << "\n";
    }

#ifdef MACLOADER_DEBUG
    LOG(ERROR) << "Read BT MAC: " << mac_to_bt_string(mac_bt);
#endif

    if (!reboot)
        return 0;

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    return -1; // goodbye...
}
