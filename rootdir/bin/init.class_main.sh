#! /vendor/bin/sh

# Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of The Linux Foundation nor
#       the names of its contributors may be used to endorse or promote
#       products derived from this software without specific prior written
#       permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#
# start ril-daemon only for targets on which radio is present
#
baseband=`getprop ro.baseband`
sgltecsfb=`getprop persist.vendor.radio.sglte_csfb`
datamode=`getprop persist.data.mode`

case "$baseband" in
    "apq" | "sda" )
    setprop ro.radio.noril yes
    stop ril-daemon
esac

case "$baseband" in
    "msm" | "csfb" | "svlte2a" | "mdm" | "mdm2" | "sglte" | "sglte2" | "dsda2" | "unknown" | "dsda3")
    start qmuxd
esac

case "$baseband" in
    "msm" | "csfb" | "svlte2a" | "mdm" | "mdm2" | "sglte" | "sglte2" | "dsda2" | "unknown" | "dsda3" | "sdm" | "sdx")
    start ipacm-diag
    start ipacm
    case "$baseband" in
        "svlte2a" | "csfb")
          start qmiproxy
        ;;
        "sglte" | "sglte2" )
          if [ "x$sgltecsfb" != "xtrue" ]; then
              start qmiproxy
          else
              setprop persist.vendor.radio.voice.modem.index 0
          fi
        ;;
        "dsda2")
          setprop persist.radio.multisim.config dsda
    esac

    #add by Luojie, set multisim property according to fsg at first boot
    firstboot_flag=`getprop persist.tct.firstboot`
    if [ -z $firstboot_flag ]; then
        device_mode="$(tar tf /dev/block/bootdevice/by-name/fsg |toybox_vendor grep '^nv/item_files/modem/mmode/device_mode$')"
        if [ -e /persist/ssss ] || [ -z $device_mode ]; then
            setprop persist.radio.multisim.config ssss
            multisim="ssss"
        else
            setprop persist.radio.multisim.config dsds
            multisim="dsds"
        fi
        setprop persist.tct.firstboot false
    else
        vsim_flag=`getprop persist.singlesim.vsim.enable`
        if [ "$vsim_flag" == "1" ]; then
            setprop persist.radio.multisim.config dsds
            multisim="dsds"
        elif [ "$vsim_flag" == "0" ]; then
            setprop persist.radio.multisim.config ssss
            multisim="ssss"
        else
            multisim=`getprop persist.radio.multisim.config`
        fi
    fi

    #MODIFIED-BEGIN by zhangjia, 2018-1-17,BUG-5879789
    #when rogers ecid replace dpm.conf with dpm_rogers.conf
    ecid=`getprop ro.ecid`
    if [ "$ecid" == "169" ]; then
        mkdir -p /data/dpm
        cp /system/etc/dpm/dpm_rogers.conf /data/dpm/dpm.conf
    else
        if [ -e /data/dpm/dpm.conf ]; then
            rm -rf /data/dpm/dpm.conf
        fi
    fi
    #MODIFIED-END by zhangjia,BUG-5879789

    # [BUGFIX]-Add-BEGIN by Xijun.Zhang,03/14/2018,Defect-6107753
    # ril-daemon will be run after set 'persist.radio.multisim.config'
    start ril-daemon
    # [BUGFIX]-Add-END by Xijun.Zhang,03/14/2018,Defect-6107753
    if [ "$multisim" = "dsds" ] || [ "$multisim" = "dsda" ]; then
        start ril-daemon2
    elif [ "$multisim" = "tsts" ]; then
        start ril-daemon2
        start ril-daemon3
    fi

    case "$datamode" in
        "tethered")
            start qti
            start port-bridge
            ;;
        "concurrent")
            start qti
            start netmgrd
            start port-bridge
            ;;
        *)
            start netmgrd
            ;;
    esac
esac

#
# Allow persistent faking of bms
# User needs to set fake bms charge in persist.bms.fake_batt_capacity
#
fake_batt_capacity=`getprop persist.bms.fake_batt_capacity`
case "$fake_batt_capacity" in
    "") ;; #Do nothing here
    * )
    echo "$fake_batt_capacity" > /sys/class/power_supply/battery/capacity
    ;;
esac
