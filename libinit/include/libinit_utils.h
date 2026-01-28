/*
 * Copyright (C) 2021-2022 The LineageOS Project
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <string>

void property_override(std::string prop, std::string value, bool add = true);

void set_bootloader_prop(void);
