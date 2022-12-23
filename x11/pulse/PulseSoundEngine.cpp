#include "PulseSoundEngine.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

namespace BR {
namespace Sfx {

/* This is called whenever new data may be written to the stream */
static void stream_write_callback(pa_stream *s, size_t length, void *userdata)
{
	assert(userdata);
	(void) s;

	PulseSoundEngine *pa = (PulseSoundEngine *) userdata;

	pa->samplesPossibleToWrite = length / sizeof(int16_t);
}

/* This routine is called whenever the stream state changes */
static void stream_state_callback(pa_stream *s, void *userdata)
{
	assert(userdata);
	PulseStream *pa = (PulseStream *) userdata;

	switch (pa_stream_get_state(s)) {
		case PA_STREAM_CREATING:
			printf("PA_STREAM_CREATING\n");
			break;

		case PA_STREAM_TERMINATED:
			pa->streamState = PulseStream::StreamState::STATE_STREAM_FINISHED;
			printf("PA_STREAM_TERMINATED\n");
			break;

		case PA_STREAM_READY:
			pa->streamState = PulseStream::StreamState::STATE_STREAM_CONNECTED;
			printf("PA_STREAM_READY\n");
			break;

		case PA_STREAM_FAILED:
			pa->streamState = PulseStream::StreamState::STATE_STREAM_FAILED;
			printf("PA_STREAM_FAILED\n");
			break;

		default:
			printf("PA_STREAM_UNKNOWN\n");
			break;
	}
}

static void context_state_cb(pa_context* c, void* userdata)
{
	assert(userdata);
	PulseSoundEngine *pa = (PulseSoundEngine *) userdata;

	switch (pa_context_get_state(c)) {
		case PA_CONTEXT_CONNECTING:
			printf("PA_CONTEXT_CONNECTING\n");
			pa->serverState = PulseSoundEngine::ServerState::STATE_SERVER_INIT;
			break;

		case PA_CONTEXT_AUTHORIZING:
			printf("PA_CONTEXT_AUTHORIZING\n");
			break;

		case PA_CONTEXT_SETTING_NAME:
			printf("PA_CONTEXT_SETTING_NAME\n");
			break;

		case PA_CONTEXT_READY: {
            printf("PA_CONTEXT_READY\n");

			pa_sample_spec sample_spec = {PA_SAMPLE_S16LE, pa->sampleRate, 2};
			pa_buffer_attr buffer_attr = {4096 * sizeof(int16_t), (uint32_t)-1, 1, (uint32_t)-1, (uint32_t)-1};

			pa->pulseStream.stream = pa_stream_new(pa->context, "Audio", &sample_spec, nullptr);

			pa_stream_set_state_callback(pa->pulseStream.stream, stream_state_callback, pa->pulseStream.stream);
			pa_stream_set_write_callback(pa->pulseStream.stream, stream_write_callback, userdata);
			pa_stream_connect_playback(pa->pulseStream.stream, nullptr, &buffer_attr, PA_STREAM_NOFLAGS, nullptr, nullptr);

			pa->serverState = PulseSoundEngine::ServerState::STATE_SERVER_CONNECTED;
            }
            break;

		case PA_CONTEXT_TERMINATED:
			printf("PA_CONTEXT_TERMINATED\n");
			pa->serverState = PulseSoundEngine::ServerState::STATE_SERVER_FINISHED;
			break;

		case PA_CONTEXT_FAILED:
			printf("PA_CONTEXT_FAILED\n");
			pa->serverState = PulseSoundEngine::ServerState::STATE_SERVER_FAILED;
			break;

		default:
			printf("PA_CONTEXT_UNKNOWN\n");
			break;
	}
}

PulseSoundEngine::PulseSoundEngine(u_int32_t sampleRate_) : sampleRate(sampleRate_)
{
	printf("Compiled with libpulse %s\nLinked with libpulse %s\n", pa_get_headers_version(), pa_get_library_version());

	if (!(m = pa_mainloop_new())) {
		throw "pa_mainloop_new failed";
	}

	pa_mainloop_api *mainloop_api = nullptr;
	mainloop_api = pa_mainloop_get_api(m);

	if(!mainloop_api){
		throw "pa_mainloop_get_api failed";
	}

	if (!(context = pa_context_new(mainloop_api, "XNP2 Katze"))) {
		throw "pa_mainloop_get_api failed";
	}

	pa_context_set_state_callback(context, context_state_cb, this);

	if (pa_context_connect(context, nullptr, PA_CONTEXT_NOFLAGS, nullptr) < 0) {
		throw "pa_context_connect failed";
	}

	//Init Server
	while(1){
		if(pa_mainloop_prepare(m, 100) < 0) break;
		if(pa_mainloop_poll(m) < 0) break;
		if(pa_mainloop_dispatch(m) < 0) break;

		if(serverState != ServerState::STATE_SERVER_INIT && serverState != ServerState::STATE_SERVER_NONE) break;
	}

	if(serverState != ServerState::STATE_SERVER_CONNECTED){
		throw "Could not init PA server";
	}
}

bool PulseSoundEngine::tick(){
	if(pa_mainloop_prepare(m, 0) < 0) return false;
	if(pa_mainloop_poll(m) < 0) return false;
	if(pa_mainloop_dispatch(m) < 0) return false;

	return true;
}

size_t PulseSoundEngine::add(int16_t *buf, size_t count){
	//printf("Count %zu, Possible %zu", count, samplesPossibleToWrite);

	if(count > samplesPossibleToWrite){
		count = samplesPossibleToWrite;
	}

	if(count){
		samplesPossibleToWrite -= count;
		pa_stream_write(pulseStream.stream, buf, count * sizeof(int16_t), nullptr, 0, PA_SEEK_RELATIVE);
	}

	return count;
}

PulseSoundEngine::~PulseSoundEngine()
{
	pa_mainloop_quit(m, 0);
	pa_mainloop_run(m, 0);
	pa_mainloop_free(m);

	printf("Pulse exit\n");
}

} //Sfx
}
