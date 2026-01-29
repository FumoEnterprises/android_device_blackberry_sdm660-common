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

#define MAC_SOURCE_WLAN  "/proc/device-tree/board_mac_addr/macaddresswlan"
#define MAC_SOURCE_BT    "/dev/block/by-name/boardid"
#define MAC_TARGET_WLAN  "/data/misc/wifi/wlan_mac.bin"
#define MAC_TARGET_BT    "/data/misc/bluetooth/bt_mac.bin"

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

int read_bt_mac(const char* path, std::vector<unsigned char>& mac_bt) {
    const char magic[] = "macaddressbt";
    constexpr size_t magic_len = sizeof(magic) - 1;

    std::ifstream in_bt(path, std::ios::binary);
    if (!in_bt) {
        LOG(ERROR) << "Failed to open WLAN MAC source";
        return -1;
    }

    std::vector<char> window(magic_len);
    size_t pos = 0;

    if (!in_bt.read(window.data(), magic_len))
        return -1;

    while (true) {
        if (std::memcmp(window.data(), magic, magic_len) == 0) {
            mac_bt.resize(6);
            if (!in_bt.read(reinterpret_cast<char*>(mac_bt.data()), 6))
                return -1;
            return 0;
        }

        char next;
        if (!in_bt.get(next))
            break;

        std::memmove(window.data(), window.data() + 1, magic_len - 1);
        window[magic_len - 1] = next;
    }

    LOG(ERROR) << "BT MAC not found";
    return -1;
}

int main() {
    bool skip_wlan = false;
    bool reboot = false;

    bool wlan_exists = file_nonempty(MAC_TARGET_WLAN);
    bool bt_exists   = file_nonempty(MAC_TARGET_BT);

    if (wlan_exists && bt_exists) {
        LOG(INFO) << "MACs already written. Skipping.";
        return 0;
    }

    skip_wlan = wlan_exists;
    reboot = !bt_exists;

    std::ifstream in(MAC_SOURCE_WLAN, std::ios::binary);
    if (!in) {
        LOG(ERROR) << "Failed to open WLAN MAC source";
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

    std::vector<unsigned char> mac_bt;
    if (read_bt_mac(MAC_SOURCE_BT, mac_bt) == 0) {
        out_bt << mac_to_bt_string(mac_bt) << "\n";
    }

    if (!reboot)
        return 0;

    std::this_thread::sleep_for(std::chrono::milliseconds(5000));

    return -1; // goodbye...
}
