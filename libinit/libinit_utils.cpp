/*
 * Copyright (C) 2021-2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <fstream>
#include <vector>

#include <libinit_utils.h>

void property_override(std::string prop, std::string value, bool add) {
    auto pi = (prop_info *) __system_property_find(prop.c_str());
    if (pi != nullptr) {
        __system_property_update(pi, value.c_str(), value.length());
    } else if (add) {
        __system_property_add(prop.c_str(), prop.length(), value.c_str(), value.length());
    }
}

void set_bootloader_prop(void) {
    std::string file = "/sys/devices/soc0/images";
    std::ifstream fp(file);
    if (!fp) {
        return;
    }

    std::string line;
    std::size_t found;
    while (std::getline(fp, line)) {
        // "  CRM:  00:BOOT.BF.3.3-00214"
        found = line.rfind("BOOT.");
        if (found != line.npos) {
            // "BOOT.BF.3.3-00214"
            property_override("ro.bootloader", line.substr(found));
            return;
        }
    }
}
