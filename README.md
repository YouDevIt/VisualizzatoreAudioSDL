# Sintetizzatore ADSR con SDL2

Un'applicazione interattiva scritta in C che genera forme d'onda audio, le riproduce e le visualizza graficamente con SDL2. Supporta l'inviluppo ADSR (Attack, Decay, Sustain, Release) e permette il controllo in tempo reale dei parametri tramite tastiera.

## 🎹 Funzionalità principali

- Generazione di forme d'onda: sinusoidale, quadra, triangolare, dente di sega
- Riproduzione audio con inviluppo ADSR
- Visualizzazione grafica della curva ADSR
- Controllo interattivo di frequenza, ampiezza, durata e parametri ADSR
- Visualizzazione dei parametri a schermo con SDL2_ttf

## ⌨️ Controlli da tastiera

### 🔊 Parametri audio
- `A` / `Z`: Aumenta / diminuisce ampiezza
- `F` / `V`: Aumenta / diminuisce frequenza

### 🎛️ Parametri ADSR
- `Y` / `H`: Aumenta / diminuisce Attack
- `U` / `J`: Aumenta / diminuisce Decay
- `I` / `K`: Aumenta / diminuisce Sustain
- `O` / `L`: Aumenta / diminuisce Release
- `T` / `G`: Aumenta / diminuisce Durata (fase di Sustain)

### 🎼 Forma d’onda
- `1`: Sinusoidale
- `2`: Quadra
- `3`: Triangolare
- `4`: Dente di sega

### ▶️ Riproduzione
- `M`: Riproduce il suono con i parametri correnti

## 🖥️ Visualizzazione

- La curva ADSR viene disegnata in tempo reale nella finestra.
- I parametri correnti (forma d’onda, frequenza, ampiezza, ADSR) sono mostrati a schermo.

## 🧱 Requisiti

- SDL2
- SDL2_ttf
- Font TrueType (es. `Arial.ttf`) nella cartella `fonts/`

## 🛠️ Compilazione con Visual Studio

Il progetto è già configurato per Visual Studio 2022. Per compilarlo:

1. Apri il file `VisualizzatoreAudioSDL.sln`
2. Assicurati che i percorsi siano configurati correttamente:
   - **Include path**: `include/SDL2`
   - **Library path**: `lib/`
   - **DLL**: copia le DLL SDL2 e SDL2_ttf nella cartella di output (`Release/`)
3. Compila ed esegui il progetto
