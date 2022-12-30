#include "PulseSoundEngine.h"
#include <stdio.h>
#include <unistd.h>

namespace BR {
namespace Sfx {

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
		if(pa_mainloop_prepare(m, 1) < 0) break;
		if(pa_mainloop_poll(m) < 0) break;
		if(pa_mainloop_dispatch(m) < 0) break;

		if(serverState != ServerState::STATE_SERVER_INIT && serverState != ServerState::STATE_SERVER_NONE) break;
	}

	if(serverState != ServerState::STATE_SERVER_CONNECTED){
		throw "Could not init PA server";
	}
}

size_t PulseSoundEngine::add(int index, int16_t *buf, size_t count){
	LockGuard guard(mutex);
	for(auto& stream : streams){
		if(stream->index == index){
			return stream->add(buf, count);
		}
	}

	return 0;
}

void PulseSoundEngine::reset(){
	LockGuard guard(mutex);
	streams.clear();
}

void PulseSoundEngine::addStream(const char *name, int index){
	LockGuard guard(mutex);
	streams.push_back(std::make_unique<PulseStream>(m, name, index, sampleRate, context));
}

void PulseSoundEngine::run() {
	while(1){
		{
			if(finished) break;

			LockGuard guard(mutex);

			pa_mainloop_prepare(m, 0);
			pa_mainloop_poll(m);
			pa_mainloop_dispatch(m);

			for(auto &stream : streams){
				LockGuard guard(stream->mutex);

				int16_t buf [stream->samplesPossibleToWrite];
				int count = stream->circleBuffer.getData(buf, stream->samplesPossibleToWrite);

				if(count){
					stream->samplesPossibleToWrite -= count;
					pa_stream_write(stream->stream, buf, count * sizeof(int16_t), nullptr, 0, PA_SEEK_RELATIVE);
				}
			}

		}

		usleep(10000);
	}
}

void PulseSoundEngine::finish() {
	LockGuard guard(mutex);
	finished = 1;
}

PulseSoundEngine::~PulseSoundEngine()
{
	streams.clear();

	pa_mainloop_quit(m, 0);
	pa_mainloop_run(m, 0);
	pa_mainloop_free(m);

	printf("Pulse exit\n");
}

/* This is called whenever new data may be written to the stream */
static void stream_write_callback(pa_stream *s, size_t length, void *userdata)
{
	assert(userdata);
	(void) s;

	PulseStream *ps = (PulseStream *) userdata;

	LockGuard guard(ps->mutex);

	ps->samplesPossibleToWrite = length / sizeof(int16_t);
}

/* This routine is called whenever the stream state changes */
static void stream_state_callback(pa_stream *s, void *userdata)
{
	assert(userdata);
	PulseStream *pa = (PulseStream *) userdata;

	LockGuard guard(pa->mutex);

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


size_t PulseStream::add(int16_t *buf, size_t count){
	LockGuard guard (mutex);
	return circleBuffer.addData(buf, count);
}

PulseStream::PulseStream(pa_mainloop* m_, const char *name_, int index_, uint32_t sampleRate, pa_context *context)
	: m(m_), circleBuffer(sampleRate), name(name_), index(index_){

	pa_sample_spec sample_spec = {PA_SAMPLE_S16LE, sampleRate, 2};

	double latency = 0.2;
	double maxSamples = latency * (double) sampleRate;

	printf("Samples %u, %s, latency: %fs maxSamples: %f\n", sampleRate, name.c_str(), latency, maxSamples);

	pa_buffer_attr buffer_attr = {(uint32_t) maxSamples, (uint32_t)maxSamples, (uint32_t)-1, (uint32_t)-1, (uint32_t)-1};

	stream = pa_stream_new(context, name.c_str(), &sample_spec, nullptr);
	pa_stream_set_state_callback(stream, stream_state_callback, this);
	pa_stream_set_write_callback(stream, stream_write_callback, this);
	pa_stream_connect_playback(stream, nullptr, &buffer_attr, PA_STREAM_NOFLAGS, nullptr, nullptr);

	//Wait for connect
	while(1){
		if(pa_mainloop_prepare(m, 1) < 0) break;
		if(pa_mainloop_poll(m) < 0) break;
		if(pa_mainloop_dispatch(m) < 0) break;

		if(streamState != StreamState::STATE_STREAM_INIT && streamState != StreamState::STATE_STREAM_NONE) {
			break;
		}
	}

	if(streamState != StreamState::STATE_STREAM_CONNECTED){
		printf("Could not init PA Stream\n");
		throw "Could not init PA Stream";
	}

	printf("Pulse created\n");
}

PulseStream::~PulseStream(){
	pa_stream_disconnect(stream);

	while(1){
		{
			if(pa_mainloop_prepare(m, 1) < 0) break;
			if(pa_mainloop_poll(m) < 0) break;
			if(pa_mainloop_dispatch(m) < 0) break;

			LockGuard guard(mutex);
			if(streamState == StreamState::STATE_STREAM_FINISHED) break;
		}

		sched_yield();
	}
}

CircleBuffer::CircleBuffer(size_t sz) : samples(sz + 1), rdminus1(samples.size() -1){
}

size_t CircleBuffer::getData(int16_t *samplesToRead, size_t samplesReadSize){
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

size_t CircleBuffer::addData(int16_t *samplesToWrite, size_t samplesSize){
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

} //Sfx
}
