//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// http_audio_stream_session.cpp: Private implementation declarations for CSpxHttpAudioStreamSession
//
#include "stdafx.h"

#include "create_object_helpers.h"
#include "http_audio_stream_session.h"
#include "site_helpers.h"
#include "thread_service.h"
#include "try_catch_helpers.h"
#include "shared_ptr_helpers.h"
#include "error_info.h"

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace Impl {

using namespace std;
using namespace std::chrono;

void CSpxHttpAudioStreamSession::Init()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);
    // The CSpxApiFactory is created at XX_from_config. and the factory is needed when call enrollment which is after creating voice profile client.
    m_keepFactoryAlive = GetSite();

    m_fromMicrophone = false;

    m_threadService = make_shared<CSpxThreadService>();
    m_threadService->Init();
}

void CSpxHttpAudioStreamSession::Term()
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    if (m_audioPump && m_audioPump->GetState() == ISpxAudioPump::State::Processing)
    {
        SPX_DBG_TRACE_VERBOSE("[%p]CSpxHttpAudioStreamSession::Term: StopPump[%p]", (void*)this, (void*)m_audioPump.get());

        m_audioPump->StopPump();

        auto audioIsDone = m_audioIsDone;
        if (audioIsDone)
        {
            Error("Terminate the http session.");
        }
    }

    if (m_codecAdapter)
    {
        m_codecAdapter->Close();
    }

    m_threadService->Term();
    if (m_audioPump)
    {
        SpxTermAndClear(m_audioPump);
        m_audioPump = nullptr;
    }
    SpxTermAndClear(m_keepFactoryAlive);
    m_keepFactoryAlive = nullptr;
    SpxTermAndClear(m_codecAdapter);
    m_codecAdapter = nullptr;
    SpxTermAndClear(m_reco);
    m_reco = nullptr;
}

void CSpxHttpAudioStreamSession::InitFromFile(const wchar_t* pszFileName)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxAudioProcessor>(this);
    wstring filename = pszFileName;
    auto task = CreateTask([this, keepAlive, filename]() {
        SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);
        // Create the wav file pump
        auto audio_file_pump = SpxCreateObjectWithSite<ISpxAudioFile>("CSpxWavFilePump", this);
        m_audioPump = SpxQueryInterface<ISpxAudioPump>(audio_file_pump);

        // Open the WAV file
        audio_file_pump->Open(filename.c_str());
        SPX_DBG_TRACE_VERBOSE("[%p]InitFromFile Pump from file:[%p]", (void*)this, (void*)m_audioPump.get());
        });
    m_threadService->ExecuteAsync(move(task));
}

void CSpxHttpAudioStreamSession::InitFromMicrophone()
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxAudioProcessor>(this);
    auto task = CreateTask([this, keepAlive]() {
        SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);
        // Create the microphone pump
        auto site = SpxSiteFromThis(this);
        m_audioPump = SpxCreateObjectWithSite<ISpxAudioPump>("CSpxInteractiveMicrophone", site);
        SPX_DBG_TRACE_VERBOSE("[%p]InitFromMicrophone: Pump from microphone:[%p]", (void*)this, (void*)m_audioPump.get());
        m_fromMicrophone = true;
        });
    m_threadService->ExecuteAsync(move(task));
}

void CSpxHttpAudioStreamSession::InitFromStream(shared_ptr<ISpxAudioStream> stream)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxAudioProcessor>(this);
    auto task = CreateTask([this, keepAlive, stream = stream]() {
        SPX_THROW_HR_IF(SPXERR_ALREADY_INITIALIZED, m_audioPump.get() != nullptr);

        shared_ptr<ISpxAudioPumpInit> audioPump;
        shared_ptr<ISpxAudioStreamReader> reader;

        // Create the stream pump
        auto cbFormat = stream->GetFormat(nullptr, 0);
        auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
        stream->GetFormat(waveformat.get(), cbFormat);

        if (waveformat->wFormatTag != WAVE_FORMAT_PCM)
        {
            m_codecAdapter = SpxCreateObjectWithSite<ISpxAudioStreamReader>("CSpxCodecAdapter", GetSite());
            SPX_IFTRUE_THROW_HR(m_codecAdapter == nullptr, SPXERR_GSTREAMER_NOT_FOUND_ERROR);

            reader = SpxQueryInterface<ISpxAudioStreamReader>(stream);

            auto initCallbacks = SpxQueryInterface<ISpxAudioStreamReaderInitCallbacks>(m_codecAdapter);
            initCallbacks->SetCallbacks(
                [=](uint8_t* buffer, uint32_t size) { return reader->Read(buffer, size); },
                [=]() { { reader->Close(); } });

            initCallbacks->SetPropertyCallback2(
                [=](PropertyId propertyId) {
                    return reader->GetProperty(propertyId);
                });

            auto adapterAsSetFormat = SpxQueryInterface<ISpxAudioStreamInitFormat>(m_codecAdapter);

            auto numChannelsString = GetStringValue("OutputPCMChannelCount", "1");
            auto numBitsPerSampleString = GetStringValue("OutputPCMNumBitsPerSample", "16");
            auto sampleRateString = GetStringValue("OutputPCMSamplerate", "16000");

            try
            {
                waveformat->nChannels = (uint16_t)stoi(numChannelsString);
                waveformat->wBitsPerSample = (uint16_t)stoi(numBitsPerSampleString);
                waveformat->nSamplesPerSec = (uint32_t)stoi(sampleRateString);
            }
            catch (const exception& e)
            {
                SPX_DBG_TRACE_VERBOSE("Error Parsing %s", e.what());
                SPX_DBG_TRACE_VERBOSE("Setting default output format samplerate = 16000, bitspersample = 16 and numchannels = 1");
                waveformat->nChannels = 1;
                waveformat->wBitsPerSample = 16;
                waveformat->nSamplesPerSec = 16000;
            }

            adapterAsSetFormat->SetFormat(waveformat.get());
        }

        audioPump = SpxCreateObjectWithSite<ISpxAudioPumpInit>("CSpxAudioPump", this);

        m_audioPump = SpxQueryInterface<ISpxAudioPump>(audioPump);

        // Attach the stream to the pump
        if (m_codecAdapter == nullptr)
        {
            reader = SpxQueryInterface<ISpxAudioStreamReader>(stream);
        }
        else
        {
            reader = SpxQueryInterface<ISpxAudioStreamReader>(m_codecAdapter);
        }
        audioPump->SetReader(reader);
        });
    m_threadService->ExecuteAsync(move(task));
}

RecognitionResultPtr CSpxHttpAudioStreamSession::StartStreamingAudioAndWaitForResult(bool enroll, VoiceProfileType type, vector<string>&& profileIds)
{
    SPX_DBG_TRACE_SCOPE(__FUNCTION__, __FUNCTION__);

    auto keepAlive = SpxSharedPtrFromThis<ISpxAudioProcessor>(this);
    RecognitionResultPtr result;
    auto task = CreateTask([&result, this, keepAlive, profileIds = move(profileIds), type = type, enroll = enroll]() mutable {

        if (m_reco == nullptr)
        {
            auto site = SpxSiteFromThis(this);
            m_reco = SpxCreateObjectWithSite<ISpxHttpRecoEngineAdapter>("CSpxHttpRecoEngineAdapter", site);
        }
        auto audioPump = m_audioPump;
        if (audioPump == nullptr)
        {
            auto error = ErrorInfo::FromRuntimeMessage("Error accessing audio pump");
            result = CreateErrorResult(error);
            return;
        }

        auto cbFormat = audioPump->GetFormat(nullptr, 0);
        auto waveformat = SpxAllocWAVEFORMATEX(cbFormat);
        audioPump->GetFormat(waveformat.get(), cbFormat);
        m_reco->SetFormat(waveformat.get(), type, move(profileIds), enroll);

        m_microphoneTimeoutInMS = GetMicrophoneTimeout();
        m_totalAudioinMS = 0;
        auto ptr = (ISpxAudioProcessor*)this;
        auto pISpxAudioProcessor = ptr->shared_from_this();
        m_audioIsDone = std::make_shared<std::promise<RecognitionResultPtr>>();
        auto future = (*m_audioIsDone).get_future();
        audioPump->StartPump(pISpxAudioProcessor);

        // the max time we wait for audio streaming and result back from http post is 1 minutes.
        auto status = future.wait_for(m_microphoneTimeoutInMS + (milliseconds)1min);

        if (status == future_status::ready)
        {
            result = future.get();
        }
        else
        {
            const auto error = ErrorInfo::FromRuntimeMessage("Bailed out due to wait more than 1 minutes for the result of enrollment or speaker recognition.");
            result = CreateErrorResult(error);
        }

        // each enroll or verify/identify has its own audio config, so we have to destroy the all audio input and its related member variables here.
        auto finish = shared_ptr<void>(nullptr, [this](void*) {CleanupAfterEachAudioPumping(); });
    });

    m_threadService->ExecuteSync(std::move(task));

    return result;
}

void CSpxHttpAudioStreamSession::CleanupAfterEachAudioPumping()
{
    StopPump();
    SpxTermAndClear(m_audioPump);
    m_audioPump = nullptr;
    m_fromMicrophone = false;
    m_totalAudioinMS = 0;
}

milliseconds CSpxHttpAudioStreamSession::GetMicrophoneTimeout()
{
    auto valueFromUser = GetStringValue("SPEECH-MicrophoneTimeoutInSpeakerRecognitionInMilliseconds", "0");
    if (valueFromUser == "0")
    {
        return m_microphoneTimeoutInMS;
    }

    uint32_t valueInMS = 0;
    try
    {
        valueInMS = std::stoi(valueFromUser);
    }
    catch (const logic_error& e)
    {
        string msg{"error in parsing"};
        msg += valueFromUser;
        msg += e.what();
        ThrowLogicError(msg);
    }

    return (milliseconds)valueInMS;
}
//--- ISpxAudioPumpSite
void CSpxHttpAudioStreamSession::Error(const string& msg)
{
    const auto error = ErrorInfo::FromRuntimeMessage(msg);
    auto result = CreateErrorResult(error);
    auto audioIsDone = m_audioIsDone;
    if (audioIsDone)
    {
        (*audioIsDone).set_value(result);
    }
}

void CSpxHttpAudioStreamSession::SetFormat(const SPXWAVEFORMATEX* pformat)
{
    if (m_reco == nullptr)
    {
        SPX_TRACE_ERROR("http reco engine adapter is null.");
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }

    if (pformat)
    {
        m_avgBytesPerSecond = pformat->nAvgBytesPerSec;
    }
    // all audio is done pumping.
    if (pformat == nullptr && m_audioIsDone)
    {
        m_reco->FlushAudio();
        auto result = m_reco->GetResult();
        SPX_DBG_TRACE_INFO("Audio session received the result of flush audio.");
        if (m_audioIsDone)
        {
            (*m_audioIsDone).set_value(result);
            m_audioIsDone = nullptr;
        }
    }
}

uint32_t CSpxHttpAudioStreamSession::FromBytesToMilisecond(uint32_t bytes, uint32_t bytesPerSecond)
{
    return static_cast<uint32_t>(bytes * 1000) / bytesPerSecond;
}

void CSpxHttpAudioStreamSession::ProcessAudio(const DataChunkPtr& audioChunk)
{
    if (m_reco == nullptr)
    {
        SPX_TRACE_ERROR("http reco engine adapter is null.");
        SPX_THROW_HR(SPXERR_RUNTIME_ERROR);
    }
    m_reco->ProcessAudio(audioChunk);

    // need to stop pump after we have seen enough samples.
    if (m_fromMicrophone)
    {
        m_totalAudioinMS += FromBytesToMilisecond(audioChunk->size, m_avgBytesPerSecond);
        if (m_totalAudioinMS >= m_microphoneTimeoutInMS.count())
        {
            SetFormat(nullptr);
        }
    }
}

void CSpxHttpAudioStreamSession::StopPump()
{
    auto audioPump = m_audioPump;
    if (audioPump && audioPump->GetState() == ISpxAudioPump::State::Processing)
    {
        audioPump->StopPump();
    }
}

string CSpxHttpAudioStreamSession::CreateVoiceProfile(VoiceProfileType type, string&& locale)
{
    auto site = SpxSiteFromThis(this);
    auto reco = SpxCreateObjectWithSite<ISpxHttpRecoEngineAdapter>("CSpxHttpRecoEngineAdapter", site);
    return reco->CreateVoiceProfile(type, move(locale));
}

RecognitionResultPtr CSpxHttpAudioStreamSession::ModifyVoiceProfile(bool reset, VoiceProfileType type, std::string&& id)
{
    auto keepAlive = SpxSharedPtrFromThis<ISpxAudioProcessor>(this);
    RecognitionResultPtr result;
    auto task = CreateTask([this, keepAlive, &result, reset=reset, type = type, id = move(id)]() mutable {
        auto site = SpxSiteFromThis(this);
        auto reco = SpxCreateObjectWithSite<ISpxHttpRecoEngineAdapter>("CSpxHttpRecoEngineAdapter", site);
        result = reco->ModifyVoiceProfile(reset, type, move(id));
        SpxTermAndClear(reco);
        });
    m_threadService->ExecuteSync(move(task));

    return result;
}

packaged_task<void()> CSpxHttpAudioStreamSession::CreateTask(function<void()> func)
{
    // Creates a packaged task that propagates all exceptions
    // to the user thread and then user callback.
    auto keepAlive = SpxSharedPtrFromThis<ISpxHttpAudioStreamSession>(this);

    // Catches all exceptions and sends them to the user thread.
    return packaged_task<void()>([this, keepAlive, func]() {
        string error;
        SPXAPI_TRY()
        {
            func();
            return;
        }
        SPXAPI_CATCH_ONLY()
            Error(error);
        });
}


shared_ptr<ISpxNamedProperties> CSpxHttpAudioStreamSession::GetParentProperties() const
{
    return SpxQueryInterface<ISpxNamedProperties>(GetSite());
}

shared_ptr<ISpxRecognitionResult> CSpxHttpAudioStreamSession::CreateIntermediateResult(const wchar_t* text, uint64_t offset, uint64_t duration)
{
    UNUSED(text);
    UNUSED(offset);
    UNUSED(duration);

    SPX_DBG_ASSERT(false);
    return nullptr;
}

std::shared_ptr<ISpxRecognitionResult> CSpxHttpAudioStreamSession::CreateFinalResult(ResultReason reason, NoMatchReason noMatchReason, const wchar_t* text, uint64_t offset, uint64_t duration, const wchar_t*)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);
    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitFinalResult(reason, noMatchReason, text, offset, duration);

    return result;
}

std::shared_ptr<ISpxRecognitionResult> CSpxHttpAudioStreamSession::CreateKeywordResult(const double confidence, const uint64_t offset, const uint64_t duration, const wchar_t* keyword, ResultReason reason, std::shared_ptr<ISpxAudioDataStream> stream)
{
    UNUSED(confidence);
    UNUSED(offset);
    UNUSED(duration);
    UNUSED(keyword);
    UNUSED(reason);
    UNUSED(stream);

    SPX_DBG_ASSERT(false);
    return nullptr;
}

std::shared_ptr<ISpxRecognitionResult> CSpxHttpAudioStreamSession::CreateErrorResult(const std::shared_ptr<ISpxErrorInformation>& error)
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);
    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitErrorResult(error);

    return result;
}

std::shared_ptr<ISpxRecognitionResult> CSpxHttpAudioStreamSession::CreateEndOfStreamResult()
{
    auto result = SpxCreateObjectWithSite<ISpxRecognitionResult>("CSpxRecognitionResult", this);
    auto initResult = SpxQueryInterface<ISpxRecognitionResultInit>(result);
    initResult->InitEndOfStreamResult();

    return result;
}

}}}}
