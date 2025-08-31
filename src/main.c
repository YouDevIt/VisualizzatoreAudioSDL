#include <SDL.h>
#include <SDL_ttf.h>
#include <math.h>
#include <stdio.h>

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 2048
#define MAX_AMPLITUDE 32000
#define MIN_AMPLITUDE 1000
#define MIN_FREQUENCY 20.0
#define MAX_FREQUENCY 2000.0
#define VISUAL_BUFFER_SIZE 512

#define PI 3.141592653589793
#define TWO_PI 6.283185307179586

// Variabili globali
double phase = 0.0;
double phase_inc = 0.0;
int waveform_type = 0;
int amplitude = 16000;
double frequency = 440.0;
float visual_buffer[VISUAL_BUFFER_SIZE];
float visual_amplitude = 0.0f;

const char* waveform_names[] = {
    "Sine", "Square", "Triangle", "Sawtooth"
};

typedef double(*wave_function_t)(double phase);

double quad_wave(double phase){
    return (fmod(phase, TWO_PI) < PI) ? 1.0 : -1.0;
}

double trig_wave(double phase){
    return fabs((fmod(phase, TWO_PI) / PI) - 1.0) * 2.0 - 1.0;
}

double sawtooth_wave(double phase){
    return (fmod(phase, TWO_PI) / PI) - 1.0;
}

const wave_function_t wave_function[] = {quad_wave, trig_wave,sawtooth_wave,sin};

// Callback audio
void audio_callback(void* userdata, Uint8* stream, int len) {
    Sint16* buffer = (Sint16*)stream;
    int samples = len / 4;
    float max_sample = 0.0f;

    for (int i = 0; i < samples; i++) {
        double value;

        wave_function[waveform_type % 4](phase);

        Sint16 sample = (Sint16)(value * amplitude);
        buffer[i * 2] = sample;
        buffer[i * 2 + 1] = sample;

        if (i < VISUAL_BUFFER_SIZE) {
            visual_buffer[i] = value;
        }

        if (fabs(value) > max_sample) max_sample = fabs(value);

        phase += phase_inc;
        if (phase >= TWO_PI) phase -= TWO_PI;
    }

    visual_amplitude = max_sample;
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Audio Visualizer SDL2",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        640, 480, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("fonts/Arial.ttf", 24);
    if (!font) {
        printf("Errore nel caricamento del font: %s\n", TTF_GetError());
        return 1;
    }

    SDL_AudioSpec audio_spec = {
        .freq = SAMPLE_RATE,
        .format = AUDIO_S16SYS,
        .channels = 2,
        .samples = BUFFER_SIZE,
        .callback = audio_callback
    };

    SDL_OpenAudio(&audio_spec, NULL);
    phase_inc = TWO_PI * frequency / SAMPLE_RATE;
    SDL_PauseAudio(0);

    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;

            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                        waveform_type = (waveform_type + 1) % 4;
                        break;
                    case SDLK_a:
                        amplitude += 1000;
                        if (amplitude > MAX_AMPLITUDE) amplitude = MAX_AMPLITUDE;
                        break;
                    case SDLK_z:
                        amplitude -= 1000;
                        if (amplitude < MIN_AMPLITUDE) amplitude = MIN_AMPLITUDE;
                        break;
                    case SDLK_f:
                        frequency += 20.0;
                        if (frequency > MAX_FREQUENCY) frequency = MAX_FREQUENCY;
                        break;
                    case SDLK_v:
                        frequency -= 20.0;
                        if (frequency < MIN_FREQUENCY) frequency = MIN_FREQUENCY;
                        break;
                }
                phase_inc = TWO_PI * frequency / SAMPLE_RATE;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Oscilloscopio
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        for (int i = 0; i < VISUAL_BUFFER_SIZE - 1; i++) {
            int x1 = i * (640 / VISUAL_BUFFER_SIZE);
            int y1 = 240 - (int)(visual_buffer[i] * 100);
            int x2 = (i + 1) * (640 / VISUAL_BUFFER_SIZE);
            int y2 = 240 - (int)(visual_buffer[i + 1] * 100);
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        }

        // Testo forma d’onda
        char label[128];
        snprintf(label, sizeof(label), "Wave: %s | Amp: %d | Freq: %.1f Hz",
                 waveform_names[waveform_type], amplitude, frequency);

        SDL_Color white = {255, 255, 255};
        SDL_Surface* text_surface = TTF_RenderText_Solid(font, label, white);
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_Rect text_rect = {10, 10, text_surface->w, text_surface->h};
        SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
        SDL_FreeSurface(text_surface);
        SDL_DestroyTexture(text_texture);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_CloseAudio();
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
