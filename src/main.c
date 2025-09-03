#include <SDL.h>
#include <SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 512
#define WINDOW_WIDTH 512
#define WINDOW_HEIGHT 480

float frequency = 440.0f;
float amplitude = 0.5f;
float attack = 0.1f, decay = 0.1f, sustain = 0.7f, release = 0.1f;
float durata = 0.5f; // durata della fase di sustain

bool play_sound = false;
Uint32 start_time = 0;

enum Waveform { SINE, SQUARE, TRIANGLE, SAW };
enum Waveform waveform = SINE;

const char* waveform_names[] = { "Sine Wave", "Square Wave", "Triangle Wave", "Sawtooth Wave" };

float get_adsr_gain(float t) {
    if (t < attack) return t / attack;
    else if (t < attack + decay) return 1.0f - ((t - attack) / decay) * (1.0f - sustain);
    else if (t < attack + decay + durata) return sustain;
    else if (t < attack + decay + durata + release) return sustain * (1.0f - ((t - attack - decay - durata) / release));
    else return 0.0f;
}

float generate_wave(float phase) {
    switch (waveform) {
    case SINE: return sinf(phase);
    case SQUARE: return phase < (float)M_PI ? 1.0f : -1.0f;
    case TRIANGLE: return 2.0f * fabsf(phase / (float)M_PI - 1.0f) - 1.0f;
    case SAW: return 2.0f * (phase / (float)(2.0 * M_PI)) - 1.0f;
    default: return 0.0f;
    }
}

void audio_callback(void* userdata, Uint8* stream, int len) {
    float* buffer = (float*)stream;
    int samples = len / sizeof(float);
    static float phase = 0.0f;

    for (int i = 0; i < samples; i++) {
        float t = (SDL_GetTicks() - start_time) / 1000.0f;
        float gain = play_sound ? get_adsr_gain(t) : 0.0f;
        buffer[i] = amplitude * gain * generate_wave(phase);
        phase += (float)(2.0 * M_PI * frequency / SAMPLE_RATE);
        if (phase > (float)(2.0 * M_PI)) phase -= (float)(2.0 * M_PI);
    }
}

void draw_text(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color color = { 255, 255, 255, 255 };
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void draw_adsr(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    int x = 0;
    int a = (int)(attack * 100);
    int d = (int)(decay * 100);
    int s = (int)(durata * 100);
    int r = (int)(release * 100);

    int y_base = WINDOW_HEIGHT - 1;
    int y_peak = y_base - 400;
    int y_sustain = y_base - (int)(400 * sustain);

    SDL_RenderDrawLine(renderer, x, y_base, x + a, y_peak);
    x += a;
    SDL_RenderDrawLine(renderer, x, y_peak, x + d, y_sustain);
    x += d;
    SDL_RenderDrawLine(renderer, x, y_sustain, x + s, y_sustain);
    x += s;
    SDL_RenderDrawLine(renderer, x, y_sustain, x + r, y_base);

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "Waveform: %s", waveform_names[waveform]);
    draw_text(renderer, font, buffer, 10, 10);

    snprintf(buffer, sizeof(buffer), "Attack: %.1fs  Decay: %.1fs  Sustain: %.1f  Release: %.1fs  Durata: %.1fs", attack, decay, sustain, release, durata);
    draw_text(renderer, font, buffer, 10, 30);

    snprintf(buffer, sizeof(buffer), "Freq: %.1f Hz  Amp: %.2f", frequency, amplitude);
    draw_text(renderer, font, buffer, 10, 50);

    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("ADSR Synth", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("fonts/Arial.ttf", 16);
    if (!font) {
        printf("Errore nel caricamento del font: %s\n", TTF_GetError());
        return 1;
    }

    SDL_AudioSpec spec;
    spec.freq = SAMPLE_RATE;
    spec.format = AUDIO_F32SYS;
    spec.channels = 1;
    spec.samples = BUFFER_SIZE;
    spec.callback = audio_callback;

    SDL_OpenAudio(&spec, NULL);

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_y: attack += 0.1f; break;
                case SDLK_h: attack -= 0.1f; if (attack < 0.0f) attack = 0.0f; break;
                case SDLK_u: decay += 0.1f; break;
                case SDLK_j: decay -= 0.1f;  if (decay < 0.0f) decay = 0.0f; break;
                case SDLK_i: sustain += 0.1f; if (sustain > 1.0f) sustain = 1.0f; break;
                case SDLK_k: sustain -= 0.1f; if (sustain < 0.0f) sustain = 0.0f; break;
                case SDLK_o: release += 0.1f; break;
                case SDLK_l: release -= 0.1f; if (release < 0.0f) release = 0.0f; break;
                case SDLK_t: durata += 0.1f; break;
                case SDLK_g: durata -= 0.1f; if (durata < 0) durata = 0; break;
                case SDLK_a: amplitude += 0.1f; if (amplitude > 1.0f) amplitude = 1.0f; break;
                case SDLK_z: amplitude -= 0.1f; if (amplitude < 0.0f) amplitude = 0.0f; break;
                case SDLK_f: frequency += 10.0f; break;
                case SDLK_v: frequency -= 10.0f; if (frequency < 20.0f) frequency = 20.0f; break;
                case SDLK_m:
                    start_time = SDL_GetTicks();
                    play_sound = true;
                    SDL_PauseAudio(0);
                    break;
                case SDLK_1: waveform = SINE; break;
                case SDLK_2: waveform = SQUARE; break;
                case SDLK_3: waveform = TRIANGLE; break;
                case SDLK_4: waveform = SAW; break;
                }
            }
        }

        draw_adsr(renderer, font);
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
