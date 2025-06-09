#include "whisper/whisper.h"

#define DR_WAV_IMPLEMENTATION
#include "whisper/examples/dr_wav.h"

#include <cmath>
#include <fstream>
#include <cstdio>
#include <string>
#include <thread>
#include <vector>

#include <iostream>
#include "json/json.hpp"
#include <stdio.h>

#include "dart_api_dl.h"

// Receives NativePort ID from Flutter code
static Dart_Port_DL dart_port = 0;

void debug_print(const char *message)
{
    if (!dart_port)
        return;
    Dart_CObject msg;
    msg.type = Dart_CObject_kString;
    msg.value.as_string = (char *)message;
    Dart_PostCObject_DL(dart_port, &msg);
}

using json = nlohmann::json;

void print(std::string value)
{
    std::cout << value << std::endl;
}

char *jsonToChar(json jsonData)
{
    std::string result = jsonData.dump();
    char *ch = new char[result.size() + 1];
    strcpy(ch, result.c_str());
    return ch;
}

std::string charToString(char *value)
{
    std::string result(value);
    return result;
}

char *stringToChar(std::string value)
{
    char *ch = new char[value.size() + 1];
    strcpy(ch, value.c_str());
    return ch;
}

// //  500 -> 00:05.000
// // 6000 -> 01:00.000
// std::string to_timestamp(int64_t t)
// {
//     int64_t sec = t / 100;
//     int64_t msec = t - sec * 100;
//     int64_t min = sec / 60;
//     sec = sec - min * 60;

//     char buf[32];
//     snprintf(buf, sizeof(buf), "%02d:%02d.%03d", (int)min, (int)sec, (int)msec);

//     return std::string(buf);
// }

// Terminal color map. 10 colors grouped in ranges [0.0, 0.1, ..., 0.9]
// Lowest is red, middle is yellow, highest is green.
const std::vector<std::string> k_colors = {
    "\033[38;5;196m",
    "\033[38;5;202m",
    "\033[38;5;208m",
    "\033[38;5;214m",
    "\033[38;5;220m",
    "\033[38;5;226m",
    "\033[38;5;190m",
    "\033[38;5;154m",
    "\033[38;5;118m",
    "\033[38;5;82m",
};

//  500 -> 00:05.000
// 6000 -> 01:00.000
std::string to_timestamp(int64_t t, bool comma = false)
{
    int64_t msec = t * 10;
    int64_t hr = msec / (1000 * 60 * 60);
    msec = msec - hr * (1000 * 60 * 60);
    int64_t min = msec / (1000 * 60);
    msec = msec - min * (1000 * 60);
    int64_t sec = msec / 1000;
    msec = msec - sec * 1000;

    char buf[32];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d%s%03d", (int)hr, (int)min, (int)sec, comma ? "," : ".", (int)msec);

    return std::string(buf);
}

int timestamp_to_sample(int64_t t, int n_samples)
{
    return std::max(0, std::min((int)n_samples - 1, (int)((t * WHISPER_SAMPLE_RATE) / 100)));
}

// command-line parameters
struct whisper_params
{
    int32_t seed = -1; // RNG seed, not used currently
    int32_t n_threads = std::min(4, (int32_t)std::thread::hardware_concurrency());

    int32_t n_processors = 1;
    int32_t offset_t_ms = 0;
    int32_t offset_n = 0;
    int32_t duration_ms = 0;
    int32_t max_context = -1;
    int32_t max_len = 0;
    int32_t best_of = 5;
    int32_t beam_size = -1;

    float word_thold = 0.01f;
    float entropy_thold = 2.40f;
    float logprob_thold = -1.00f;

    bool verbose = false;
    bool print_special_tokens = false;
    bool speed_up = false;
    bool translate = false;
    bool diarize = false;
    bool no_fallback = false;
    bool output_txt = false;
    bool output_vtt = false;
    bool output_srt = false;
    bool output_wts = false;
    bool output_csv = false;
    bool print_special = false;
    bool print_colors = false;
    bool print_progress = false;
    bool no_timestamps = false;
    bool split_on_word = false;
    bool print_realtime = false;

    std::string language = "id";
    std::string prompt;
    std::string model = "models/ggml-model-whisper-small.bin";
    std::string audio = "samples/jfk.wav";
    std::vector<std::string> fname_inp = {};
    std::vector<std::string> fname_outp = {};
};

struct whisper_print_user_data
{
    const whisper_params *params;

    const std::vector<std::vector<float>> *pcmf32s;
};

json transcribe(json jsonBody)
{
    whisper_params params;

    params.n_threads = jsonBody["threads"];
    params.verbose = jsonBody["is_verbose"];
    params.translate = jsonBody["is_translate"];
    params.language = jsonBody["language"];
    params.print_special_tokens = jsonBody["is_special_tokens"];
    params.no_timestamps = jsonBody["is_no_timestamps"];
    params.model = jsonBody["model"];
    params.audio = jsonBody["audio"];
    params.split_on_word = jsonBody["split_on_word"];
    json jsonResult;
    jsonResult["@type"] = "transcribe";

    if (whisper_lang_id(params.language.c_str()) == -1)
    {
        jsonResult["@type"] = "error";
        jsonResult["message"] = "error: unknown language";
        return jsonResult;
    }

    if (params.seed < 0)
    {
        params.seed = time(NULL);
    }

    // whisper init
    struct whisper_context *ctx = whisper_init_from_file(params.model.c_str());

    // struct whisper_context *ctx = whisper_init(params.model.c_str());
    std::string text_result = "";
    const auto fname_inp = params.audio;
    // WAV input
    std::vector<float> pcmf32;
    {
        drwav wav;
        if (!drwav_init_file(&wav, fname_inp.c_str(), NULL))
        {
            jsonResult["@type"] = "error";
            jsonResult["message"] = " failed to open WAV file ";
            return jsonResult;
        }

        if (wav.channels != 1 && wav.channels != 2)
        {
            jsonResult["@type"] = "error";
            jsonResult["message"] = "must be mono or stereo";
            return jsonResult;
        }

        if (wav.sampleRate != WHISPER_SAMPLE_RATE)
        {
            jsonResult["@type"] = "error";
            jsonResult["message"] = "WAV file  must be 16 kHz";
            return jsonResult;
        }

        if (wav.bitsPerSample != 16)
        {
            jsonResult["@type"] = "error";
            jsonResult["message"] = "WAV file  must be 16 bit";
            return jsonResult;
        }

        int n = wav.totalPCMFrameCount;

        std::vector<int16_t> pcm16;
        pcm16.resize(n * wav.channels);
        drwav_read_pcm_frames_s16(&wav, n, pcm16.data());
        drwav_uninit(&wav);

        // convert to mono, float
        pcmf32.resize(n);
        if (wav.channels == 1)
        {
            for (int i = 0; i < n; i++)
            {
                pcmf32[i] = float(pcm16[i]) / 32768.0f;
            }
        }
        else
        {
            for (int i = 0; i < n; i++)
            {
                pcmf32[i] = float(pcm16[2 * i] + pcm16[2 * i + 1]) / 65536.0f;
            }
        }
    }

    // print some info about the processing
    {
        // printf("\n");
        if (!whisper_is_multilingual(ctx))
        {
            if (params.language != "en" || params.translate)
            {
                params.language = "en";
                params.translate = false;
            }
        }
    }
    // run the inference
    {
        whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

        wparams.print_realtime = params.print_realtime;
        wparams.print_progress = false;
        wparams.print_timestamps = !params.no_timestamps;
        wparams.translate = params.translate;
        wparams.language = params.language.c_str();
        wparams.n_threads = params.n_threads;
        wparams.split_on_word = params.split_on_word;

        if (params.split_on_word) {
            wparams.max_len = 1;
            wparams.token_timestamps = true;
        }



//        if (!wparams.print_realtime)
//        {
//            if (whisper_full(ctx, wparams, pcmf32.data(), pcmf32.size()) != 0)
//            {
//                jsonResult["@type"] = "error";
//                jsonResult["message"] = "failed to process audio";
//                return jsonResult;
//            }
//
//            const int n_segments = whisper_full_n_segments(ctx);
//
//            std::vector<json> segmentsJson = {};
//
//            for (int i = 0; i < n_segments; ++i)
//            {
//                const char *text = whisper_full_get_segment_text(ctx, i);
//
//                std::string str(text);
//                text_result += str;
//                if (params.no_timestamps)
//                {
//                    // printf("%s", text);
//                    // fflush(stdout);
//                } else {
//                    json jsonSegment;
//                    const int64_t t0 = whisper_full_get_segment_t0(ctx, i);
//                    const int64_t t1 = whisper_full_get_segment_t1(ctx, i);
//
//                    // printf("[%s --> %s]  %s\n", to_timestamp(t0).c_str(), to_timestamp(t1).c_str(), text);
//
//                    jsonSegment["from_ts"] = t0;
//                    jsonSegment["to_ts"] = t1;
//                    jsonSegment["text"] = text;
//
//                    segmentsJson.push_back(jsonSegment);
//                }
//            }
//
//            if (!params.no_timestamps) {
//                jsonResult["segments"] = segmentsJson;
//            }
//        } else {
            // TODO: add realtime transcription implementation

            // TODO: add realtime transcription implementation

//        debug_print("GGG Processing audio in chunks...\n");

            // Assuming we have a function to process audio in chunks
            const int chunk_size = 1024; // Define the size of each chunk
            int total_frames = pcmf32.size();
            for (int offset = 0; offset < total_frames; offset += chunk_size) {
                int frames_to_process = std::min(chunk_size, total_frames - offset);
                if (whisper_full(ctx, wparams, pcmf32.data() + offset, frames_to_process) != 0)
                {
                    jsonResult["@type"] = "error";
                    jsonResult["message"] = "failed to process audio";
                    return jsonResult;
                }

                // Process segments after each chunk
                const int n_segments = whisper_full_n_segments(ctx);
                for (int i = 0; i < n_segments; ++i)
                {
                    const char *text = whisper_full_get_segment_text(ctx, i);
                    std::string str(text);
                    // text_result += str;
                    return str;

                    if (!params.no_timestamps)
                    {
                        json jsonSegment;
                        const int64_t t0 = whisper_full_get_segment_t0(ctx, i);
                        const int64_t t1 = whisper_full_get_segment_t1(ctx, i);

                        jsonSegment["from_ts"] = t0;
                        jsonSegment["to_ts"] = t1;
                        jsonSegment["text"] = text;

                        // print c++


                        jsonResult["segments"].push_back(jsonSegment);


                        print("123123123\n");
                        fprintf(stderr, "[%s --> %s]  %s\n", to_timestamp(t0).c_str(), to_timestamp(t1).c_str(), text);
                        printf("[%s --> %s]  %s\n", to_timestamp(t0).c_str(), to_timestamp(t1).c_str(), text);
                    }
                }
            }
        }
//    }
    jsonResult["text"] = text_result;

    whisper_free(ctx);
    return jsonResult;
}

// Ensure that the function is not-mangled; exported as a pure C function
extern "C" void set_dart_port(Dart_Port_DL port)
{
    dart_port = port;
}

extern "C"
{
    char *request(char *body)
    {
        json jsonBody = json::parse(body);
        json jsonResult;

        if (jsonBody["@type"] == "getTextFromWavFile")
        {
            return jsonToChar(transcribe(jsonBody));
        }

        if (jsonBody["@type"] == "getVersion")
        {
            jsonResult["@type"] = "version";
            jsonResult["message"] = "version lib v0.0.0";
            return jsonToChar(jsonResult);
        }

        jsonResult["@type"] = "error";
        jsonResult["message"] = "method not found";
        return jsonToChar(jsonResult);
    }

    int main()
    {
        json jsonBody;
        jsonBody["@type"] = "al";
        print(transcribe(jsonBody).dump());
        return 0;
    }
}
