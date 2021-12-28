#include <cstdint>
#include <SDL2/SDL.h>

void audio_callback(void* userdata, uint8_t* stream, int len)
{
    uint64_t* samples_played = (uint64_t*)userdata;
    float* fstream = (float*)(stream);
    static const float volume = 0.2;
    static const float frequency = 200.0;

    for(int sid = 0; sid < (len / 8); ++sid)
    {
        double time = (*samples_played + sid) / 44100.0;
        fstream[2 * sid + 0] = volume * sin(frequency * 2.0 * M_PI * time); /* L */
        fstream[2 * sid + 1] = volume * sin(frequency * 2.0 * M_PI * time); /* R */
    }

    *samples_played += (len / 8);
}

int main(int argc, char* argv[])
{
    uint64_t samples_played = 0;

    if(SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "Error initializing SDL. SDL_Error: %s\n", SDL_GetError());
        return -1;
    }


    SDL_AudioSpec audio_spec_want, audio_spec;
    SDL_memset(&audio_spec_want, 0, sizeof(audio_spec_want));

    audio_spec_want.freq     = 44100;
    audio_spec_want.format   = AUDIO_F32;
    audio_spec_want.channels = 2;
    audio_spec_want.samples  = 512;
    audio_spec_want.callback = audio_callback;
    audio_spec_want.userdata = (void*)&samples_played;

    SDL_AudioDeviceID audio_device_id = SDL_OpenAudioDevice(
        NULL, 0,
        &audio_spec_want, &audio_spec,
        SDL_AUDIO_ALLOW_FORMAT_CHANGE
    );

    if(!audio_device_id)
    {
        fprintf(stderr, "Error creating SDL audio device. SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    int window_width  = 600;
    int window_height = 600;
    SDL_Window* window;
    {
        window = SDL_CreateWindow(
            "SDL Tone Generator",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            window_width, window_height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
        );

        if(!window)
        {
            fprintf(stderr, "Error creating SDL window. SDL_Error: %s\n", SDL_GetError());
            SDL_Quit();
            return -1;
        }
    }

    SDL_PauseAudioDevice(audio_device_id, 0);

    bool running = true;
    while(running)
    {
        // Process input
        SDL_Event sdl_event;
        while(SDL_PollEvent(&sdl_event) != 0)
        {
            if(sdl_event.type == SDL_QUIT)
                running = false;
        }
    }

    SDL_DestroyWindow(window);
    SDL_CloseAudioDevice(audio_device_id);
    SDL_Quit();

    return 0;
}
