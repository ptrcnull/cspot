#ifndef CHUNKEDAUDIOSTREAM_H
#define CHUNKEDAUDIOSTREAM_H

#include <ivorbisfile.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <array>
#include <unistd.h>
#include <pthread.h>
#include "MercuryManager.h"
#include "Task.h"
#include "AudioSink.h"
#include "AudioChunk.h"

#define SPOTIFY_HEADER_SIZE 167

enum class Whence
{
    START,
    CURRENT,
    END
};

class ChunkedAudioStream : public Task
{
private:
    // Vorbis related
    OggVorbis_File vorbisFile;
    ov_callbacks vorbisCallbacks;
    int currentSection;

    // Audio chunking
    std::vector<uint8_t> audioKey;
    std::vector<std::shared_ptr<AudioChunk>> chunks;

    // Audio data
    uint32_t duration;

    bool loadingChunks = false;
    uint16_t lastSequenceId = 0;

    std::shared_ptr<MercuryManager> manager;
    std::vector<uint8_t> fileId;
    uint32_t startPositionMs;

    void requestChunk(size_t chunkIndex);
    void fetchTraillingPacket();
    void runTask();

public:
    ChunkedAudioStream(std::vector<uint8_t> fileId, std::vector<uint8_t> audioKey, uint32_t duration, std::shared_ptr<MercuryManager> manager, uint32_t startPositionMs);
    int requestedChunkIndex = 0;
    size_t pos = SPOTIFY_HEADER_SIZE; // size of some spotify header
    uint32_t fileSize;
    uint32_t readBeforeSeek = 0;
    bool loadedMeta = false;
    bool isPaused = false;
    bool isRunning = false;
    bool finished = false;
    std::shared_ptr<AudioSink> audioSink;

    pthread_mutex_t seekMutex;
    std::vector<uint8_t> read(size_t bytes);
    void seekMs(uint32_t positionMs);
    void seek(size_t pos, Whence whence);
};

#endif