/*
 * Copyright 2020 The Android Open Source Project
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

#ifndef ANDROID_HARDWARE_TV_TUNER_V1_1_DVR_H_
#define ANDROID_HARDWARE_TV_TUNER_V1_1_DVR_H_

#include <fmq/MessageQueue.h>
#include <math.h>
#include <atomic>
#include <set>
#include <thread>
#include "Demux.h"
#include "Frontend.h"
#include "Tuner.h"

using namespace std;

namespace android {
namespace hardware {
namespace tv {
namespace tuner {
namespace V1_0 {
namespace implementation {

using ::android::hardware::EventFlag;
using ::android::hardware::kSynchronizedReadWrite;
using ::android::hardware::MessageQueue;
using ::android::hardware::MQDescriptorSync;

using DvrMQ = MessageQueue<uint8_t, kSynchronizedReadWrite>;

struct MediaEsMetaData {
    bool isAudio;
    int startIndex;
    int len;
    int pts;
};

class Demux;
class Filter;
class Frontend;
class Tuner;

class Dvr : public IDvr {
  public:
    Dvr();

    Dvr(DvrType type, uint32_t bufferSize, const sp<IDvrCallback>& cb, sp<Demux> demux);

    ~Dvr();

    virtual Return<void> getQueueDesc(getQueueDesc_cb _hidl_cb) override;

    virtual Return<Result> configure(const DvrSettings& settings) override;

    virtual Return<Result> attachFilter(const sp<IFilter>& filter) override;

    virtual Return<Result> detachFilter(const sp<IFilter>& filter) override;

    virtual Return<Result> start() override;

    virtual Return<Result> stop() override;

    virtual Return<Result> flush() override;

    virtual Return<Result> close() override;

    /**
     * To create a DvrMQ and its Event Flag.
     *
     * Return false is any of the above processes fails.
     */
    bool createDvrMQ();
    void sendBroadcastInputToDvrRecord(vector<uint8_t> byteBuffer);
    bool writeRecordFMQ(const std::vector<uint8_t>& data);
    bool addPlaybackFilter(uint64_t filterId, sp<IFilter> filter);
    bool removePlaybackFilter(uint64_t filterId);
    bool readPlaybackFMQ(bool isVirtualFrontend, bool isRecording);
    bool processEsDataOnPlayback(bool isVirtualFrontend, bool isRecording);
    bool startFilterDispatcher(bool isVirtualFrontend, bool isRecording);
    EventFlag* getDvrEventFlag();
    DvrSettings getSettings() { return mDvrSettings; }

  private:
    // Demux service
    sp<Demux> mDemux;

    DvrType mType;
    uint32_t mBufferSize;
    sp<IDvrCallback> mCallback;
    std::map<uint64_t, sp<IFilter>> mFilters;

    void deleteEventFlag();
    bool readDataFromMQ();
    void getMetaDataValue(int& index, uint8_t* dataOutputBuffer, int& value);
    void maySendPlaybackStatusCallback();
    void maySendRecordStatusCallback();
    PlaybackStatus checkPlaybackStatusChange(uint32_t availableToWrite, uint32_t availableToRead,
                                             uint32_t highThreshold, uint32_t lowThreshold);
    RecordStatus checkRecordStatusChange(uint32_t availableToWrite, uint32_t availableToRead,
                                         uint32_t highThreshold, uint32_t lowThreshold);
    /**
     * A dispatcher to read and dispatch input data to all the started filters.
     * Each filter handler handles the data filtering/output writing/filterEvent updating.
     */
    void startTpidFilter(vector<uint8_t> data);
    void playbackThreadLoop();

    unique_ptr<DvrMQ> mDvrMQ;
    EventFlag* mDvrEventFlag;
    /**
     * Demux callbacks used on filter events or IO buffer status
     */
    bool mDvrConfigured = false;
    DvrSettings mDvrSettings;

    // Thread handlers
    std::thread mDvrThread;

    // FMQ status local records
    PlaybackStatus mPlaybackStatus;
    RecordStatus mRecordStatus;
    /**
     * If a specific filter's writing loop is still running
     */
    std::atomic<bool> mDvrThreadRunning;
    bool mKeepFetchingDataFromFrontend;
    /**
     * Lock to protect writes to the FMQs
     */
    std::mutex mWriteLock;
    /**
     * Lock to protect writes to the input status
     */
    std::mutex mPlaybackStatusLock;
    std::mutex mRecordStatusLock;

    const bool DEBUG_DVR = false;
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace tuner
}  // namespace tv
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_TV_TUNER_V1_1_DVR_H_
