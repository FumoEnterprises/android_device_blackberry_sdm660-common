#!/bin/bash
#
# Copyright (C) 2019 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set -e

DEVICE_COMMON=sdm660-common
VENDOR=blackberry

INITIAL_COPYRIGHT_YEAR=2025

# Load extractutils and do some sanity checks
MY_DIR="${BASH_SOURCE%/*}"
if [[ ! -d "$MY_DIR" ]]; then MY_DIR="$PWD"; fi

LINEAGE_ROOT="$MY_DIR"/../../..

PATCHELF="$(which patchelf)"

HELPER="$LINEAGE_ROOT"/vendor/lineage/build/tools/extract_utils.sh
if [ ! -f "$HELPER" ]; then
    echo "Unable to find helper script at $HELPER"
    exit 1
fi

echo $1

. "$HELPER"

# Default to sanitizing the vendor folder before extraction
CLEAN_VENDOR=true

while [ "$1" != "" ]; do
    case $1 in
        -p | --path )           shift
                                SRC=$1
                                ;;
        -s | --section )        shift
                                SECTION=$1
                                CLEAN_VENDOR=true
                                ;;
        -n | --no-cleanup )     CLEAN_VENDOR=false
                                ;;
    esac
    shift
done

if [ -z "$SRC" ]; then
    SRC=adb
fi

echo $SRC

function blob_fixup() {
    case "${1}" in
        # Fix missing symbols for GNSS
        vendor/lib64/vendor.qti.gnss@1.0_vendor.so)
            for GNSS_SHIM in $(grep -L "libshim_gnss.so" "${2}"); do
                "${PATCHELF}" --add-needed "libshim_gnss.so" "${GNSS_SHIM}"
            done
            ;;
        vendor/bin/imsrcsd)
            if ! "${PATCHELF}" --print-needed "${2}" | grep -q "libshim_logmsg.so"; then
                "${PATCHELF}" --add-needed "libshim_logmsg.so" "${2}"
            fi
            ;;
        vendor/bin/hw/android.hardware.drm@1.0-service.widevine)
            if ! "${PATCHELF}" --print-needed "${2}" | grep -q "libshim_logmsg.so"; then
                "${PATCHELF}" --add-needed "libshim_logmsg.so" "${2}"
            fi
            ;;
    esac
}


# Initialize the helper for common device
setup_vendor "$DEVICE_COMMON" "$VENDOR" "$LINEAGE_ROOT" true $CLEAN_VENDOR

extract "$MY_DIR"/proprietary-files.txt "$SRC" "$SECTION"

# Initialize the helper for device
setup_vendor "$DEVICE" "$VENDOR" "$LINEAGE_ROOT" false $CLEAN_VENDOR

extract "$MY_DIR"/../$DEVICE/proprietary-files.txt "$SRC" "$SECTION"

"$MY_DIR"/setup-makefiles.sh
