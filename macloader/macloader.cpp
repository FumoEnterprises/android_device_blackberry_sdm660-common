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

#include <android-base/logging.h>

#define MAC_SOURCE          "/dev/block/by-name/boardid"
#define MAC_TARGET_WLAN     "/data/misc/wifi/wlan_mac.bin"
#define MAC_TARGET_BT       "/data/misc/bluetooth/bt_mac.bin"

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

int main() {
    bool skip_wlan = false;
    bool reboot = false;

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
