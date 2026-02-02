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

import android.os.Bundle;
import android.os.Handler;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;

import androidx.preference.ListPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceCategory;
import androidx.preference.PreferenceFragment;
import androidx.preference.SwitchPreference;
import androidx.preference.SwitchPreferenceCompat;

import org.lineageos.settings.R;

public class HardwareKeysSettingsFragment extends PreferenceFragment implements
        Preference.OnPreferenceChangeListener, OnCheckedChangeListener {

    private static final String PREF_HWNAV = "hw_nav_key_pref";

    private SwitchPreference mHwNavType;

    private HardwareKeysUtils mHardwareKeysUtils;
    private Handler mHandler = new Handler();

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        addPreferencesFromResource(R.xml.hwkeys_settings);

        mHardwareKeysUtils = new HardwareKeysUtils(getContext());

        mHwNavType = (SwitchPreference) findPreference(PREF_HWNAV);
        mHwNavType.setOnPreferenceChangeListener(this);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        switch (preference.getKey()) {
            case PREF_HWNAV:
                mHardwareKeysUtils.setHwKeysEnabled((Boolean) newValue);
                return true;
            default:
                return false;
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        // stub
    }
}
