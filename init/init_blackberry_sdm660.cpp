/*
 * Copyright (C) 2021-2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include <fstream>
#include <string>
#include <sys/sysinfo.h>
#include <vector>

#include "vendor_init.h"

#define HEAPSTARTSIZE_PROP "dalvik.vm.heapstartsize"
#define HEAPGROWTHLIMIT_PROP "dalvik.vm.heapgrowthlimit"
#define HEAPSIZE_PROP "dalvik.vm.heapsize"
#define HEAPMINFREE_PROP "dalvik.vm.heapminfree"
#define HEAPMAXFREE_PROP "dalvik.vm.heapmaxfree"
#define HEAPTARGETUTILIZATION_PROP "dalvik.vm.heaptargetutilization"

#define GB(b) (b * 1024ull * 1024 * 1024)

typedef struct dalvik_heap_info {
    std::string heapstartsize;
    std::string heapgrowthlimit;
    std::string heapsize;
    std::string heapminfree;
    std::string heapmaxfree;
    std::string heaptargetutilization;
} dalvik_heap_info_t;

static const dalvik_heap_info_t dalvik_heap_info_6144 = {
    .heapstartsize = "16m",
    .heapgrowthlimit = "256m",
    .heapsize = "512m",
    .heapminfree = "8m",
    .heapmaxfree = "32m",
    .heaptargetutilization = "0.5",
};

static const dalvik_heap_info_t dalvik_heap_info_4096 = {
    .heapstartsize = "8m",
    .heapgrowthlimit = "256m",
    .heapsize = "512m",
    .heapminfree = "8m",
    .heapmaxfree = "16m",
    .heaptargetutilization = "0.6",
};

void property_override(std::string prop, std::string value, bool add) {
    auto pi = (prop_info *) __system_property_find(prop.c_str());
    if (pi != nullptr) {
        __system_property_update(pi, value.c_str(), value.length());
    } else if (add) {
        __system_property_add(prop.c_str(), prop.length(), value.c_str(), value.length());
    }
}

void set_dalvik_heap() {
    struct sysinfo sys;
    const dalvik_heap_info_t *dhi;

    sysinfo(&sys);

    if (sys.totalram > GB(5))
        dhi = &dalvik_heap_info_6144;
    else
        dhi = &dalvik_heap_info_4096;

    property_override(HEAPSTARTSIZE_PROP, dhi->heapstartsize, true);
    property_override(HEAPGROWTHLIMIT_PROP, dhi->heapgrowthlimit, true);
    property_override(HEAPSIZE_PROP, dhi->heapsize, true);
    property_override(HEAPTARGETUTILIZATION_PROP, dhi->heaptargetutilization, true);
    property_override(HEAPMINFREE_PROP, dhi->heapminfree, true);
    property_override(HEAPMAXFREE_PROP, dhi->heapmaxfree, true);
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
            property_override("ro.bootloader", line.substr(found), true);
            return;
        }
    }
}

void vendor_load_properties() {
    set_bootloader_prop();
    set_dalvik_heap();
}
