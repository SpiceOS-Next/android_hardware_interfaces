/*
 * Copyright (C) 2021 The Android Open Source Project
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

package android.hardware.gnss;

import android.hardware.gnss.IGnssAntennaInfoCallback;

/**
 * Extended interface for GNSS antenna information support.
 *
 * @hide
 */
@VintfStability
interface IGnssAntennaInfo {
    /**
     * Registers the callback routines with the HAL.
     *
     * @param callback Handle to the GnssAntennaInfo callback interface.
     */
    void setCallback(in IGnssAntennaInfoCallback callback);

    /**
     * Stops updates from the HAL, and unregisters the callback routines.
     * After a call to close(), the previously registered callbacks must be
     * considered invalid by the HAL.
     * If close() is invoked without a previous setCallback, this function must perform
     * no work.
     */
    void close();
}
