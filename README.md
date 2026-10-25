# april-asr

[aprilasr](https://github.com/abb128/april-asr) is a minimal library that provides an API for offline streaming speech-to-text applications

[Documentation](https://abb128.github.io/april-asr/concepts.html)


## Language support
>The core library is written in C, and has a C API. [Python](https://abb128.github.io/april-asr/python.html) and [C#](https://abb128.github.io/april-asr/csharp.html) bindings are available."
I extended the example.cpp to make this for my specific use case, targeting users using OBS to stream English captions.

## Models
>A few models are available, listed [here](https://abb128.github.io/april-asr/models.html). 
>The English models are based on [csukuangfj's trained icefall model](https://huggingface.co/csukuangfj/icefall-asr-librispeech-lstm-transducer-stateless2-2022-09-03/tree/main/exp) as the base, and trained with some extra data.

This repo includes a copy of the English model from the aforementioned site.

## What do?
This uses a local llm model to write captions to a file that can be read by OBS. At this very moment, it only works on linux, but my first priority will be getting a Windows version availble, feel free to send a PR!

## Needed software/tools
- make
- cmake
- pulseaudio
  - Most linux distributions come with this, but you can try your package manager for pipewire-pulse or pulseaudio.
  - You need access to the ```parecord``` binary/command. looks like it's [pacat](https://wiki.archlinux.org/title/Pacat) for Arch and (pulseaudio-utils)[https://manpages.debian.org/testing/pulseaudio-utils/parecord.1.en.html] for debian.

## How to run

This is a console app that accepts microphone input and the path of a text file as arguments.

### Linux
Building 
- Get source code: ```git clone https://github.com/polynomialpossum/obs_captioner.git```
- Change to project directory: ```cd obs_captioner```
- Install Onnx runtime: ```./download_onnx_linux_x64.sh```
- Make build directory: ```mkdir build```
- Change to build directory ```cd build```
- Use Cmake to build: ```cmake -DCMAKE_BUILD_TYPE=Release ..```
- Use make to build?: ```make -j4```
Running
- You should now have an obs_captions executable in the project directory (the directory above you currently most likely).
- run the following command but replace PATH_TO_TEXT_FILE with the path of the text file to send the captions:
  -```parec --format=s16 --rate=16000 --channels=1 --latency-ms=100 --device=@DEFAULT_MONITOR@ | ./obs_captions - PATH_TO_TEXT_FILE```
  - TIP: You can change ```--device=@DEFAULT_MONITOR@``` to whatever you want. You can get a list of available devices with ```pactl list short sinks```
  - If working correctly, you should immediately see the terminal spitting out captions! The way this works is by saving over the text file on updates. So if you look at the text file and it contains "nothing" don't panic! It may have stopped on a pause in speech.

### Windows (coming soon!)

This is what the original april-asr library had for instructions and is probably a good starting point.

## Building on Windows (msvc)
Create a folder called `lib` in the april-asr folder.

Download [onnxruntime-win-x64-1.13.1.zip](https://github.com/microsoft/onnxruntime/releases/download/v1.13.1/onnxruntime-win-x64-1.13.1.zip) and extract the insides of the onnxruntime-win-x64-1.13.1 folder to the `lib` folder

Run cmake to configure and generate Visual Studio project files. Make sure you select x64 as the target if you have downloaded the x64 version of ONNXRuntime.

Open the `ALL_BUILD.vcxproj` and everything should build. The output will be in the Release or Debug folders.

When running main.exe you may receive an error message like this:
> The application was unable to start correctly (0xc000007b)

To fix this, you need to make onnxruntime.dll available. One way to do this is to copy onnxruntime.dll from lib/lib/onnxruntime.dll to build/Debug and build/Release. You may need to distribute the dll together with your application.

## Acknowledgements

I found this library through the [livecaptions](https://github.com/abb128/LiveCaptions). Please check it out and use it if it fits your use case! I love this application, just needed something a little more customized and tailored to me. Great job!

