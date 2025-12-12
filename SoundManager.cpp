#include "SoundManager.h"

SoundManager::SoundManager() 
    : enabled(true), isPlaying(false), volume(80), 
      backgroundMusicTimer(0), backgroundMusicActive(false) {}

void SoundManager::init() {

    ledcSetup(BUZZER_CHANNEL, 5000, 8);  
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
    ledcWrite(BUZZER_CHANNEL, 0);  
    
    Serial.println(" SoundManager inicializado en GPIO " + String(BUZZER_PIN));
}

void SoundManager::enable(bool state) {
    enabled = state;
    if (!enabled) {
        stopSound();
        stopBackgroundMusic();
    }
    Serial.println(enabled ? " Sonido ACTIVADO" : " Sonido DESACTIVADO");
}

void SoundManager::setVolume(int vol) {
    volume = constrain(vol, 0, 100);
    Serial.print(" Volumen: ");
    Serial.println(volume);
}

void SoundManager::playTone(int frequency, int duration) {
    if (!enabled) return;
    ledcChangeFrequency(BUZZER_CHANNEL, frequency, 8);

    int pwm = map(volume, 0, 100, 50, 200); 
    pwm = constrain(pwm, 0, 255);
    ledcWrite(BUZZER_CHANNEL, pwm);
    
    delay(duration);
    ledcWrite(BUZZER_CHANNEL, 0);  
    delay(20);  
}

void SoundManager::playMelody(const int* notes, const int* durations, int length) {
    if (!enabled) return;
    
    for (int i = 0; i < length; i++) {
        if (notes[i] == 0) {
            delay(durations[i]);  
        } else {
            playTone(notes[i], durations[i]);
        }
    }
}

void SoundManager::stopSound() {
    ledcWrite(BUZZER_CHANNEL, 0);
    isPlaying = false;
}

void SoundManager::startBackgroundMusic() {
    if (!enabled) return;
    backgroundMusicActive = true;
    backgroundMusicTimer = millis();
}

void SoundManager::stopBackgroundMusic() {
    backgroundMusicActive = false;
    stopSound();
}

void SoundManager::updateBackgroundMusic() {
    if (!enabled || !backgroundMusicActive) return;
    
    unsigned long currentTime = millis();
    static unsigned long lastNoteTime = 0;
    
    if (currentTime - lastNoteTime >= 600) {
        lastNoteTime = currentTime;
        
        static int noteSequence[] = {NOTE_G4, NOTE_C5, NOTE_E5, NOTE_G5, NOTE_E5, NOTE_C5};
        static int sequenceIndex = 0;
        
        int note = noteSequence[sequenceIndex % 6];
        sequenceIndex++;
        
        int originalVolume = volume;
        volume = map(volume, 0, 100, 20, 60);  
        
        ledcChangeFrequency(BUZZER_CHANNEL, note, 8);
        int pwm = map(volume, 0, 100, 30, 150);
        pwm = constrain(pwm, 0, 255);
        ledcWrite(BUZZER_CHANNEL, pwm);
        
        volume = originalVolume;
        delayMicroseconds(500);
    }
}

void SoundManager::playSound(SoundEffect effect) {
    if (!enabled) return;
    
    switch (effect) {
        case SOUND_JUMP: {
            playTone(NOTE_C4, 50);
            playTone(NOTE_E4, 50);
            playTone(NOTE_G4, 80);
            break;
        }
        
        case SOUND_DASH: {
            playTone(NOTE_C5, 30);
            playTone(NOTE_D5, 30);
            playTone(NOTE_E5, 30);
            playTone(NOTE_F5, 30);
            playTone(NOTE_G5, 50);
            break;
        }
        
        case SOUND_COLLISION: {
            playTone(NOTE_A4, 150);
            playTone(NOTE_F4, 150);
            playTone(NOTE_A4, 200);
            break;
        }
        
        case SOUND_GAMEOVER: {
            int notes[] = {NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4};
            int durations[] = {150, 150, 150, 150, 150, 150, 200, 400};
            playMelody(notes, durations, 8);
            break;
        }
        
        case SOUND_POINT: {
            playTone(NOTE_E5, 80);
            playTone(NOTE_A5, 120);
            break;
        }
        
        case SOUND_START: {
            int notes[] = {NOTE_E4, NOTE_G4, NOTE_C5, NOTE_E5};
            int durations[] = {100, 100, 100, 200};
            playMelody(notes, durations, 4);
            break;
        }
    }
}