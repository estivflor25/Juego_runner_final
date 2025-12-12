#include "GameManager.h"

GameManager::GameManager()
    : sprite(&display),
      hud(nullptr),
      bluetooth(nullptr),
      graphics(&display),
      player(&graphics),
      obstacleCount(0),
      spawnTimer(0),
      gameOver(false),
      paused(false),
      usePartialBuffer(false)
{
    // Crear instancias dinámicamente
    hud = new HUDManager(&display);
    bluetooth = new BluetoothManager();
    
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i] = Obstacle(&graphics);
    }
}

GameManager::~GameManager() {
    if (hud) delete hud;
    if (bluetooth) delete bluetooth;
}

void GameManager::setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println(" CYBER RUNNER - INICIANDO");

    esp_task_wdt_init(30, true);
    
    displayMutex = xSemaphoreCreateMutex();
    gameStateMutex = xSemaphoreCreateMutex();
    commandQueue = xQueueCreate(20, sizeof(char*));

    display.init();
    display.setRotation(1);
    display.setSwapBytes(true);
    display.fillScreen(COLOR_SKY);

    Serial.print(" Memoria libre ANTES: ");
    Serial.println(ESP.getFreeHeap());
    
    Serial.print(" Creando buffer de ");
    Serial.print(SCREEN_WIDTH);
    Serial.print("x");
    Serial.print(SPRITE_ZONE_HEIGHT);
    Serial.print(" = ");
    Serial.print((SCREEN_WIDTH * SPRITE_ZONE_HEIGHT * 2) / 1024);
    Serial.println(" KB...");
    
    if (sprite.createSprite(SCREEN_WIDTH, SPRITE_ZONE_HEIGHT)) {
        usePartialBuffer = true;
        Serial.println(" Buffer parcial CREADO!");
    } else {
        usePartialBuffer = false;
        Serial.println(" No hay RAM suficiente para buffer");
    }

    Serial.print("Memoria libre DESPUÉS: ");
    Serial.println(ESP.getFreeHeap());

    if (bluetooth) bluetooth->init();
    if (hud) hud->draw();
    renderInitialFrame();

    Serial.println(" Sistema listo!");
    if (usePartialBuffer) {
        Serial.println(" Buffer parcial activo - Parpadeo reducido");
    }

    // Iniciar música de fondo
    sound.startBackgroundMusic();

    xTaskCreatePinnedToCore(gameLogicTask, "GameLogic", 8192, this, 2, NULL, 1);
    xTaskCreatePinnedToCore(renderTask, "Render", 8192, this, 2, NULL, 0);
    xTaskCreatePinnedToCore(inputTask, "Input", 8192, this, 4, NULL, 1);

    // Parpadeo de LED de confirmación
    for (int i = 0; i < 5; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(120);
        digitalWrite(LED_BUILTIN, LOW);
        delay(120);
    }
}

void GameManager::gameLogicTask(void* parameter) {
    GameManager* game = (GameManager*)parameter;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(16);
    esp_task_wdt_add(NULL);
    
    while (true) {
        if (!game->gameOver && !game->paused) {
            game->updateGame();
        }
        esp_task_wdt_reset();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void GameManager::renderTask(void* parameter) {
    GameManager* game = (GameManager*)parameter;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(8);
    esp_task_wdt_add(NULL);
    
    while (true) {
        if (!game->gameOver) {
            game->renderGameOptimized();
            // Actualizar música de fondo
            game->sound.updateBackgroundMusic();
        }
        esp_task_wdt_reset();
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void GameManager::inputTask(void* parameter) {
    GameManager* game = (GameManager*)parameter;
    char* cmd = nullptr;
    esp_task_wdt_add(NULL);
    
    Serial.println(" Tarea de Input iniciada");
    while (true) {
        if (xQueueReceive(commandQueue, &cmd, pdMS_TO_TICKS(1)) == pdTRUE) {
            if (cmd) {
                String s = String(cmd);
                s.trim();
                game->processCommandFast(s);
                free(cmd);
                cmd = nullptr;
            }
        }
        esp_task_wdt_reset();
        taskYIELD();
    }
}

void GameManager::renderInitialFrame() {
    if (xSemaphoreTake(displayMutex, portMAX_DELAY)) {
        display.fillRect(0, HUD_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - HUD_HEIGHT, COLOR_SKY);
        display.fillRect(0, GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - GROUND_Y, COLOR_GROUND);
        player.draw(&display);
        xSemaphoreGive(displayMutex);
    }
    Serial.println(" Frame inicial dibujado");
}

void GameManager::updateGame() {
    player.update();
    graphics.updateAnimation();
    
    // Actualizar obstáculos
    for (int i = 0; i < obstacleCount; i++) {
        if (obstacles[i].isActive()) obstacles[i].update();
    }
    
    // Detectar colisiones
    for (int i = 0; i < obstacleCount; i++) {
        if (!obstacles[i].isActive()) continue;
        if (obstacles[i].checkCollision(player.getScreenX(), player.getScreenY(), 
            player.getWidth(), player.getHeight())) {
            char side = obstacles[i].getCollisionSide(player.getScreenX(), 
                player.getScreenY(), player.getWidth(), player.getHeight());
            if (side == 'L' || side == 'R') {
                if (hud) hud->loseLife();
                obstacles[i].deactivate();
                Serial.println(" Colisión!");
            }
        }
    }
    
    // Eliminar obstáculos fuera de pantalla
    for (int i = 0; i < obstacleCount; i++) {
        if (!obstacles[i].isActive()) continue;
        if (obstacles[i].getX() + obstacles[i].getWidth() < 0) {
            obstacles[i].deactivate();
        }
    }
    
    // Compactar array de obstáculos
    int writeIdx = 0;
    for (int i = 0; i < obstacleCount; i++) {
        if (obstacles[i].isActive()) {
            if (i != writeIdx) obstacles[writeIdx] = obstacles[i];
            writeIdx++;
        }
    }
    obstacleCount = writeIdx;
    
    // Generar nuevos obstáculos
    spawnTimer++;
    if (spawnTimer >= SPAWN_RATE && obstacleCount < MAX_OBSTACLES) {
        int h = random(35, 90);
        float spawnX = SCREEN_WIDTH + random(20, 140);
        float spawnY = GROUND_Y - h;
        obstacles[obstacleCount].activate(spawnX, spawnY, 40, h);
        obstacleCount++;
        spawnTimer = 0;
    }
    
    // Incrementar puntuación
    if (spawnTimer % 10 == 0 && hud) hud->addScore(1);
    
    // Verificar Game Over
    if (hud && hud->isGameOver()) {
        gameOver = true;
        showGameOver();
    }
}

void GameManager::renderGameOptimized() {
    if (!xSemaphoreTake(displayMutex, pdMS_TO_TICKS(8))) return;
    
    if (usePartialBuffer) {
        static bool staticDrawn = false;
        if (!staticDrawn) {
            display.fillRect(0, HUD_HEIGHT, SCREEN_WIDTH, 
                           SPRITE_ZONE_Y - HUD_HEIGHT, COLOR_SKY);
            display.fillRect(0, GROUND_Y, SCREEN_WIDTH, 
                           SCREEN_HEIGHT - GROUND_Y, COLOR_GROUND);
            staticDrawn = true;
        }
        
        graphics.drawBackground(&sprite);
        
        // Dibujar obstáculos en el sprite
        for (int i = 0; i < obstacleCount; i++) {
            if (obstacles[i].isActive()) {
                int adjY = (int)obstacles[i].getY() - SPRITE_ZONE_Y;
                if (adjY >= -obstacles[i].getHeight() && adjY < SPRITE_ZONE_HEIGHT) {
                    obstacles[i].draw(&sprite);
                }
            }
        }
        
        // Dibujar jugador en el sprite
        int adjPlayerY = (int)player.getScreenY() - SPRITE_ZONE_Y;
        if (adjPlayerY >= -PLAYER_HEIGHT && adjPlayerY < SPRITE_ZONE_HEIGHT) {
            player.draw(&sprite);
        }
        
        sprite.pushSprite(0, SPRITE_ZONE_Y);
        
    } else {
        // Renderizado directo sin buffer
        display.startWrite();
        display.setAddrWindow(0, HUD_HEIGHT, SCREEN_WIDTH, GROUND_Y - HUD_HEIGHT);
        display.pushBlock(COLOR_SKY, (SCREEN_WIDTH) * (GROUND_Y - HUD_HEIGHT));
        display.setAddrWindow(0, GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - GROUND_Y);
        display.pushBlock(COLOR_GROUND, (SCREEN_WIDTH) * (SCREEN_HEIGHT - GROUND_Y));
        
        for (int i = 0; i < obstacleCount; i++) {
            if (obstacles[i].isActive()) obstacles[i].draw(&display);
        }
        
        player.draw(&display);
        display.endWrite();
    }
    
    xSemaphoreGive(displayMutex);
    if (hud) hud->draw();
}

void GameManager::processCommandFast(const String& cmd) {
    // Comandos del joystick
    if (cmd.startsWith("d0") || cmd.startsWith("d1")) {
        int comma = cmd.indexOf(',');
        if (comma == -1) return;
        
        int valX = cmd.substring(2, comma).toInt();
        if (cmd.startsWith("d0") && valX > 900) {
            if (gameOver) resetGame();
            else player.jump();
        }
        return;
    }
    
    String s = cmd;
    s.toUpperCase();
    
    if (gameOver) {
        if (s == "A" || s == "START") resetGame();
        return;
    }
    
    if (s == "A" || s == "JUMP") player.jump();
    else if (s == "B" || s == "DASH") player.dash();
    else if (s == "START") {
        paused = !paused;
        Serial.println(paused ? " PAUSADO" : "REANUDADO");
    }
    else if (s == "RESET") resetGame();
}

void GameManager::showGameOver() {
    // Detener música de fondo y reproducir sonido de Game Over
    sound.stopBackgroundMusic();
    sound.playSound(SOUND_GAMEOVER);
    
    if (xSemaphoreTake(displayMutex, portMAX_DELAY)) {
        display.fillScreen(TFT_BLACK);
        display.setTextColor(TFT_RED);
        display.setTextSize(4);
        display.setCursor(120, 120);
        display.print("GAME OVER");
        
        display.setTextColor(COLOR_TEXT);
        display.setTextSize(2);
        display.setCursor(140, 180);
        display.print("Puntos: ");
        if (hud) display.print(hud->getScore());
        
        display.setCursor(100, 220);
        display.print("Presiona A para reiniciar");
        
        xSemaphoreGive(displayMutex);
    }
    
    Serial.print("💀 GAME OVER! Score: ");
    if (hud) Serial.println(hud->getScore());
}

void GameManager::resetGame() {
    Serial.println(" REINICIANDO...");
    
    for (int i = 0; i < obstacleCount; i++) obstacles[i].deactivate();
    obstacleCount = 0;
    player.reset();
    if (hud) hud->reset();
    spawnTimer = 0;
    gameOver = false;
    paused = false;
    
    if (xSemaphoreTake(displayMutex, portMAX_DELAY)) {
        display.fillScreen(COLOR_SKY);
        display.fillRect(0, GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - GROUND_Y, COLOR_GROUND);
        player.draw(&display);
        xSemaphoreGive(displayMutex);
    }
    
    if (hud) hud->draw();
    sound.playSound(SOUND_START);
    sound.startBackgroundMusic();  // Reiniciar música de fondo
    Serial.println(" ¡NUEVO JUEGO!");
}