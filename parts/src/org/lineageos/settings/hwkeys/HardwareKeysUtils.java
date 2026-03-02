/*
 * Copyright (C) 2018 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.lineageos.settings.hwkeys;

import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.UserHandle;
import android.os.SystemClock;
import android.view.KeyEvent;
import java.lang.IllegalArgumentException;
import java.lang.reflect.Method;
import java.util.List;

public final class HardwareKeysUtils {

    private static HardwareKeysUtils mInstance;
    private Handler mHandler = new Handler();
    private Context mContext;

    public static HardwareKeysUtils getInstance() {
        if (mInstance == null) {
            throw new IllegalArgumentException("Trying to get instance without initializing!");
        }
        return mInstance;
    }

    public HardwareKeysUtils(final Context context) {
        mContext = context;
    }

    protected void setHwKeysEnabled(boolean enabled) {
        try {
            Class<?> sp = Class.forName("android.os.SystemProperties");
            Method set = sp.getMethod("set", String.class, String.class);
            set.invoke(null, "persist.sys.hwkeys", enabled ? "1" : "0");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    protected void setBothEnabled(boolean enabled) {
        try {
            Class<?> sp = Class.forName("android.os.SystemProperties");
            Method set = sp.getMethod("set", String.class, String.class);
            set.invoke(null, "persist.sys.hwkeys.both", enabled ? "1" : "0");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
