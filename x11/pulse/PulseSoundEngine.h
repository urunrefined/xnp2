#ifndef PULSESOUNDENGINE_H_
#define PULSESOUNDENGINE_H_

#include <iostream>
#include <memory>
#include <vector>

#include <pulse/pulseaudio.h>
#include "Thread.h"
#include <string.h>

namespace BR {
namespace Sfx {

class CircleBuffer{

public:
	std::vector<int16_t> samples;
	size_t wr = 0;
	size_t rdminus1;

	CircleBuffer(size_t sz) : samples(sz + 1), rdminus1(samples.size() -1){
	}

	size_t getData(int16_t *samplesToRead, size_t samplesReadSize){
		if(!samplesReadSize) return 0;
		size_t next = (rdminus1 + 1) % samples.size();

		if(next == wr) {
			return 0;
		}

		if(next > wr){
			//Read to the end of the samples buffer, recusive call to read the possible rest
			size_t toRead = std::min(samples.size() - next, samplesReadSize);
			memcpy(samplesToRead, samples.data() + next, toRead * sizeof(int16_t));
			rdminus1 = next + toRead - 1;
			return toRead + getData(samplesToRead + toRead, samplesReadSize - toRead);
		}
		else{
			//Read to Write Index
			size_t toRead = std::min(wr - next, samplesReadSize);
			memcpy(samplesToRead, samples.data() + next, toRead * sizeof(int16_t));
			rdminus1 = next + toRead - 1;
			return toRead;
		}
	}

	size_t addData(int16_t *samplesToWrite, size_t samplesSize){
		if(wr == rdminus1) return 0;
		if(!samplesSize) return 0;

		if(wr < rdminus1){
			//Write to rdminus1 (inclusive)
			size_t toWrite = std::min(samplesSize, rdminus1 - wr);
			memcpy(samples.data() + wr, samplesToWrite, toWrite * sizeof(int16_t));
			wr += toWrite;
			return toWrite;
		}
		else {
			//Write to end of buffer, recursive call again to write possible rest
			size_t toWrite = std::min(samplesSize, samples.size() - wr);
			memcpy(samples.data() + wr, samplesToWrite, toWrite * sizeof(int16_t));
			wr = (wr + toWrite) % samples.size();
			return toWrite + addData(samplesToWrite + toWrite, samplesSize - toWrite);
		}
	}
};

class PulseStream {
public:
	enum class StreamState {
		STATE_STREAM_NONE = 0,
		STATE_STREAM_INIT,
		STATE_STREAM_CONNECTED,
		STATE_STREAM_FINISHED,
		STATE_STREAM_FAILED,
	};

	pa_mainloop* m;

	size_t samplesPossibleToWrite = 0;

	Mutex mutex;
	CircleBuffer circleBuffer;

	pa_stream *stream = nullptr;
	const std::string name;
	const int index;

	enum StreamState streamState = StreamState::STATE_STREAM_NONE;

	size_t add(int16_t *buf, size_t count);

	PulseStream(pa_mainloop* m_, const char *name_, int index_, uint32_t sampleRate, pa_context *context);
	~PulseStream();
};

class PulseSoundEngine : public Runable {
public:
	const uint32_t sampleRate;
	std::vector<std::unique_ptr<PulseStream>> streams;
	Mutex mutex;
	int finished = 0;

	enum class ServerState {
		STATE_SERVER_NONE = 0,
		STATE_SERVER_INIT,
		STATE_SERVER_CONNECTED,
		STATE_SERVER_FINISHED,
		STATE_SERVER_FAILED,
	};

	enum ServerState serverState = ServerState::STATE_SERVER_NONE;

	pa_mainloop* m = nullptr;
	pa_context *context = nullptr;

	PulseSoundEngine(u_int32_t sampleRate_);
	~PulseSoundEngine();

	void addStream(const char *name, int index);
	size_t add(int index, int16_t *buf, size_t count);
	void reset();

	virtual void run() override;
	virtual void finish() override;
};

} //namespace Sfx
} //namespace BR

#endif /* PULSESOUNDENGINE_H_ */
