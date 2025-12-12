#ifndef SOUND_MANAGER_H
#define SOUND_MANAGER_H

#include <Arduino.h>

// Pin del buzzer 
#define BUZZER_PIN 25

// Canales PWM del ESP32
#define BUZZER_CHANNEL 0

// Notas musicales en Hz
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988
#define NOTE_C6  1047

// Tipos de sonidos
enum SoundEffect {
    SOUND_JUMP,
    SOUND_DASH,
    SOUND_COLLISION,
    SOUND_GAMEOVER,
    SOUND_POINT,
    SOUND_START
};

class SoundManager {
private:
    bool enabled;
    bool isPlaying;
    int volume;  
    unsigned long backgroundMusicTimer;
    bool backgroundMusicActive;
    
public:
    SoundManager();
    
    void init();
    void enable(bool state);
    void setVolume(int vol);  
    int getVolume() const { return volume; }
    
    // Reproducir efectos
    void playSound(SoundEffect effect);
    
    // Música de fondo
    void updateBackgroundMusic();
    void startBackgroundMusic();
    void stopBackgroundMusic();
    
    // Funciones de bajo nivel
    void playTone(int frequency, int duration);
    void playMelody(const int* notes, const int* durations, int length);
    void stopSound();
    
    bool isEnabled() const { return enabled; }
};

#endif