#ifndef PULSESOUNDENGINE_H_
#define PULSESOUNDENGINE_H_

#include <iostream>
#include <vector>

#include <pulse/pulseaudio.h>

namespace BR {
namespace Sfx {

class PulseStream {
public:
	enum class StreamState {
		STATE_STREAM_NONE = 0,
		STATE_STREAM_INIT,
		STATE_STREAM_CONNECTED,
		STATE_STREAM_FINISHED,
		STATE_STREAM_FAILED,
	};

	pa_stream *stream = nullptr;

	enum StreamState streamState = StreamState::STATE_STREAM_NONE;

	PulseStream(){
	}
};

class PulseSoundEngine {
public:
	const uint32_t sampleRate;

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

	PulseStream pulseStream;

	size_t samplesPossibleToWrite = 0;

	PulseSoundEngine(u_int32_t sampleRate_);
	~PulseSoundEngine();

	size_t add(int16_t *buf, size_t i);
	bool tick();
};

} //namespace Sfx
} //namespace BR

#endif /* PULSESOUNDENGINE_H_ */
