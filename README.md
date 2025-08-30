# VisualizzatoreAudioSDL

Un'app interattiva in C con SDL2 che genera forme d'onda audio e le visualizza in tempo reale.

## Controlli
- `SPACE`: Cambia forma d'onda (Sine, Square, Triangle, Sawtooth)
- `A/Z`: Aumenta/Diminuisce ampiezza
- `F/V`: Aumenta/Diminuisce frequenza

## Requisiti
- SDL2
- SDL2_ttf
- Arial.ttf (in fonts/)

## Compilazione
Usa Visual Studio 2022 e configura:
- Include path: `include/SDL2`
- Lib path: `lib/`
- DLL in cartella di output
