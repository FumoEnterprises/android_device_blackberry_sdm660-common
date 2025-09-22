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
        'com.qualcomm.qti.dpm.api@1.0',
        'vendor.qti.hardware.fm@1.0',
        'vendor.qti.imsrtpservice@3.0',
        'vendor.qti.hardware.qccsyshal@1.0',
        'vendor.qti.hardware.qccvndhal@1.0',
    ): lib_fixup_vendor_suffix,
}

# todo: merge into tuples?

blob_fixups: blob_fixups_user_type = {
    'vendor/lib/vendor.qti.imsrtpservice@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.imsrtpservice@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/com.quicinc.cne.api@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/com.quicinc.cne.api@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.hardware.data.latency@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/com.quicinc.cne.server@2.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/com.quicinc.cne.server@2.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/com.qualcomm.qti.imscmservice@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/com.qualcomm.qti.imscmservice@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/com.qualcomm.qti.imscmservice@1.1_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/com.qualcomm.qti.imscmservice@1.1_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/com.qualcomm.qti.ant@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/com.qualcomm.qti.ant@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib64/com.qualcomm.qti.ant@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib/com.qualcomm.qti.ant@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib64/com.qualcomm.qti.dpm.api@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/com.qualcomm.qti.dpm.api@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/vendor.qti.gnss@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.hardware.perf@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/vendor.qti.hardware.perf@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.hardware.tui_comm@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/vendor.qti.hardware.fm@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.hardware.fm@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/vendor.qti.hardware.tui_comm@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.gnss@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib/com.qualcomm.qti.imscmservice@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib64/com.qualcomm.qti.imscmservice@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib/com.qualcomm.qti.imsrtpservice@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib64/com.qualcomm.qti.imsrtpservice@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib/vendor.qti.imsrtpservice@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib64/vendor.qti.imsrtpservice@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib64/libdpmframework.so': blob_fixup()
        .replace_needed('libcutils.so', 'libcutils-v29.so'),
    'system/lib/lib-imsvideocodec.so': blob_fixup()
        .add_needed('libgui_shim.so'),
    'system/lib64/lib-imsvideocodec.so': blob_fixup()
        .add_needed('libgui_shim.so'),
    'vendor/lib/hw/android.hardware.bluetooth@1.0-impl-qti.so': blob_fixup()
        .add_needed('libbase_shim.so'),
    'vendor/lib64/hw/android.hardware.bluetooth@1.0-impl-qti.so': blob_fixup()
        .add_needed('libbase_shim.so'),
    'system/lib/lib-imscamera.so': blob_fixup()
        .add_needed('libgui_shim.so'),
    'system/lib64/lib-imscamera.so': blob_fixup()
        .add_needed('libgui_shim.so'),
    'vendor/lib/vendor.qti.hardware.radio.uim@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.hardware.radio.uim@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/vendor.qti.hardware.radio.am@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.hardware.radio.am@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/vendor.qti.hardware.radio.qcrilhook@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.hardware.radio.qcrilhook@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/vendor.qti.hardware.radio.uim_remote_client@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.hardware.radio.uim_remote_client@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/vendor.qti.hardware.radio.lpa@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.hardware.radio.lpa@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/vendor.qti.hardware.radio.ims@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.hardware.radio.ims@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/vendor.qti.hardware.radio.qtiradio@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.hardware.radio.qtiradio@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/vendor.qti.hardware.radio.uim_remote_server@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.qti.hardware.radio.uim_remote_server@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.display.color@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/vendor.display.postproc@1.0_vendor.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib/libantradio.so': blob_fixup()
        .add_needed('libnativehelper_shim.so'),
    'system/lib64/libantradio.so': blob_fixup()
        .add_needed('libnativehelper_shim.so'),
    'system/lib/lib-imsvt.so': blob_fixup()
        .add_needed('libgui_shim.so'),
    'system/lib64/lib-imsvt.so': blob_fixup()
        .add_needed('libgui_shim.so'),
    'system/lib/libimsmedia_jni.so': blob_fixup()
        .add_needed('libgui_shim.so'),
    'system/lib64/libimsmedia_jni.so': blob_fixup()
        .add_needed('libgui_shim.so'),
    'system/lib/vendor.display.color@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib64/vendor.display.color@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib/vendor.display.postproc@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib64/vendor.display.postproc@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib/vendor.qti.hardware.fm@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib64/vendor.qti.hardware.fm@1.0.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib/com.qualcomm.qti.imscmservice@1.1.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'system/lib64/com.qualcomm.qti.imscmservice@1.1.so': blob_fixup()
        .add_needed('libhidlbase-v32.so'),
    'vendor/bin/hw/android.hardware.drm@1.0-service.widevine': blob_fixup()
        .add_needed('libbase_shim.so')
        .add_needed('libhidlbase-v32.so'),
    'vendor/bin/imsrcsd': blob_fixup()
        .add_needed('libhidlbase-v32.so')
        .add_needed('libbase_shim.so'),
    'vendor/lib/lib-imsrcs-v2.so': blob_fixup()
        .add_needed('libbase_shim.so')
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/lib-imsrcs-v2.so': blob_fixup()
        .add_needed('libbase_shim.so')
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib/lib-uceservice.so': blob_fixup()
        .add_needed('libbase_shim.so')
        .add_needed('libhidlbase-v32.so'),
    'vendor/lib64/lib-uceservice.so': blob_fixup()
        .add_needed('libbase_shim.so')
        .add_needed('libhidlbase-v32.so'),
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
