// For basic live captioning of desktop audio, run it like so:
// parec --format=s16 --rate=16000 --channels=1 --latency-ms=100 --device=@DEFAULT_MONITOR@ | ./main - /path/to/model.april

#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include "april_api.h"



#ifndef _MSC_VER
#include <unistd.h>
#else
#include <io.h>
#include <BaseTsd.h>
#define STDIN_FILENO 0
typedef SSIZE_T ssize_t;
#endif

// attempting to add cpp libs wtf why is thing
// a cpp file and using old ass C pointer shit?!
#include <string>
#include <iostream>
#include <fstream>
std::string text_file_path = "";

#define BUFFER_SIZE 1024
int ends_with(const char *str, const char *suffix);
// In this example, the internal state is just a global struct just for testing
// In your program you can pass any pointer into userdata to access it in the
// handler
struct {
    int xyz;
} some_internal_state;

// This callback function will get called every time a new result is decoded.
// It's passed into the AprilConfig along with the userdata pointer.
void handler(void *userdata, AprilResultType result, size_t count, const AprilToken *tokens) {
    
    assert(userdata == &some_internal_state);

    switch(result){
        case APRIL_RESULT_RECOGNITION_FINAL: 
            printf("@ ");
            break;
        case APRIL_RESULT_RECOGNITION_PARTIAL:
            printf("- ");
            break;
        case APRIL_RESULT_SILENCE:
            break;
        default:
            assert(false);
            return;
    }
    // variable holding current caption text
    std::string captions = "";


    for(int t=0; t<count; t++){
        const char *text = tokens[t].token;
        printf("%s", text);
        captions += text;
        }
std::ofstream outFile(text_file_path);

    if (outFile.is_open()) {
        outFile << captions;
        outFile.close(); 
    } else {
        printf("Error opening file!\n");
    }
        printf(captions.c_str());
        captions = "";
        printf("\n");
    }


int main(int argc, char *argv[]){
  printf("args: %d", argc);
    if(argc != 3){
        printf("Usage: %s [input] [path to text file]\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    const char *input_model = "models/april-english-dev-01110_en.april";
    text_file_path = argv[2];
    
    // In the start of our program we should call aam_api_init.
    // This should only be called once.
    aam_api_init(APRIL_VERSION);

    // Next we should load the model. The model by itself doesn't allow us
    // to do much except for get the metadata. If loading the model
    // fails, NULL is returned.
    AprilASRModel model = aam_create_model(input_model);
    if(model == NULL){
        printf("Loading model %s failed!\n", input_model);
        return 1;
    }
    
    size_t model_sample_rate = aam_get_sample_rate(model);
    printf("Model name: %s\n", aam_get_name(model));
    printf("Model desc: %s\n", aam_get_description(model));
    printf("Model lang: %s\n", aam_get_language(model));
    printf("Model samplerate: %ld\n\n", model_sample_rate);


    // To do actual speech recognition, it's necessary to create a session.
    // Models and sessions are separate to allow for more efficient handling
    // of multiple sessions. For example, an application may be performing
    // recognition on 20 different audio streams by using 20 different sessions
    // and by re-using the same AprilASRModel the relative memory use is low.
    // However, it's important that the AprilASRModel does not get freed
    // before all of its sessions.
    AprilConfig config = { 0 };
    config.handler = handler;
    config.userdata = (void*)&some_internal_state;

    // By default, the session runs in synchronous mode. If you want async
    // processing, you may choose to set it to APRIL_CONFIG_FLAG_ASYNC_RT_BIT
    // here.
    config.flags = APRIL_CONFIG_FLAG_ZERO_BIT;

    AprilASRSession session = aas_create_session(model, config);


    if(argv[1][0] == '-' && argv[1][1] == 0) {
        // Reading stdin mode. It's assumed that the input data is pcm16 audio,
        // sampled in the model's sample rate.
        // You can achieve this on Linux like this:
        // $ parec --format=s16 --rate=16000 --channels=1 --latency-ms=100 | ./main - /path/to/model.april

        char data[BUFFER_SIZE];
        ssize_t r;
        for(;;){
            r = read(STDIN_FILENO, data, BUFFER_SIZE);
            
            if (r == -1) {
                aas_flush(session);
                break;
            } else
            if (r <= 0) {
                continue;
            }
            
            aas_feed_pcm16(session, (short *)data, r/2);
        }
    } else {
        // Run some blank data, mainly for memory leak testing
        char data[6400];
        memset(data, 0, 6400);
        aas_feed_pcm16(session, (short *)data, 3200);
        aas_flush(session);
    }    aas_free(session);
    aam_free(model);

    return 0;
}


int ends_with(const char *str, const char *suffix) {
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}
