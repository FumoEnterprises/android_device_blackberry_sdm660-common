#!/usr/bin/env -S PYTHONPATH=../../../tools/extract-utils python3
#
# SPDX-FileCopyrightText: 2024 The LineageOS Project
# SPDX-License-Identifier: Apache-2.0
#

from extract_utils.fixups_blob import (
    blob_fixup,
    blob_fixups_user_type,
)
from extract_utils.fixups_lib import (
    lib_fixup_remove,
    lib_fixups,
    lib_fixups_user_type,
)
from extract_utils.main import (
    ExtractUtils,
    ExtractUtilsModule,
)

import extract_utils.tools
extract_utils.tools.DEFAULT_PATCHELF_VERSION = '0_9'

namespace_imports = [
    'device/blackberry/sdm660-common',
    'hardware/qcom-caf/msm8998',
    'hardware/qcom-caf/wlan',
    'vendor/qcom/opensource/dataservices',
    'vendor/qcom/opensource/data-ipa-cfg-mgr-legacy-um',
]

def lib_fixup_vendor_suffix(lib: str, partition: str, *args, **kwargs):
    return f'{lib}_{partition}' if partition == 'vendor' else None

lib_fixups: lib_fixups_user_type = {
    **lib_fixups,
    (
        'libtrueportrait',
        'vendor.qti.hardware.fm@1.0',
        'vendor.qti.hardware.mwqemadapter@1.0',
    ): lib_fixup_vendor_suffix,
}

blob_fixups: blob_fixups_user_type = {
    ('vendor/lib/vendor.qti.hardware.qteeconnector@1.0.so',
     'vendor/lib64/vendor.qti.hardware.qteeconnector@1.0.so',
     'vendor/lib64/vendor.qti.hardware.tui_comm@1.0_vendor.so'
     ): blob_fixup()
        .replace_needed('libhidlbase.so', 'libhidlbase-v32.so'),
    ('vendor/bin/hw/vendor.qti.hardware.tui_comm@1.0-service-qti',
     'vendor/bin/hw/android.hardware.drm@1.0-service.widevine'
     ): blob_fixup()
        .add_needed('libbase_shim.so'),
    #'vendor/lib64/libril-qc-hal-qmi.so': blob_fixup()
    #    .replace_needed('android.hardware.radio.config@1.0.so', 'android.hardware.radio.c_shim@1.0.so'),
    'system_ext/lib64/lib-imscamera.so': blob_fixup()
        .add_needed('libgui_shim.so'),
    'system_ext/lib64/lib-imsvideocodec.so': blob_fixup()
        .add_needed('libgui_shim.so')
        .replace_needed('libqdMetaData.so', 'libqdMetaData.system.so'),
    'vendor/etc/izat.conf': blob_fixup()
        .regex_replace('PROCESS_STATE=ENABLED', 'PROCESS_STATE=DISABLED'),
}  # fmt: skip

module = ExtractUtilsModule(
    'sdm660-common',
    'blackberry',
    blob_fixups=blob_fixups,
    lib_fixups=lib_fixups,
    namespace_imports=namespace_imports,
)

if __name__ == '__main__':
    utils = ExtractUtils.device(module)
    utils.run()
