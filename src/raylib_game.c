/*******************************************************************************************
*
*   raylib 9years gamejam template
*
*   Template originally created with raylib 4.5-dev, last time updated with raylib 4.5-dev
*
*   Template licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2022 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"

#if defined(PLATFORM_WEB)
    #define CUSTOM_MODAL_DIALOGS            // Force custom modal dialogs usage
    #include <emscripten/emscripten.h>      // Emscripten library - LLVM to JavaScript compiler
#endif

#include <stdio.h>                          // Required for: printf()
#include <stdlib.h>                         // Required for: 
#include <string.h>                         // Required for: 

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
// Simple log system to avoid printf() calls if required
// NOTE: Avoiding those calls, also avoids const strings memory usage
#define SUPPORT_LOG_INFO
#if defined(SUPPORT_LOG_INFO)
    #define LOG(...) printf(__VA_ARGS__)
#else
    #define LOG(...)
#endif

#define MAX_ENEMIES 50

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { 
    SCREEN_LOGO = 0, 
    SCREEN_TITLE, 
    SCREEN_GAMEPLAY, 
    SCREEN_ENDING
} GameScreen;

// TODO: Define your custom data types here

typedef struct Player {
    Vector2 position;
    float rotation;
    Texture texture;
    Rectangle rectangle;
    float speed;
} Player;

typedef struct Enemy {
    Vector2 position;
    float speed;
    Vector2 target;
} Enemy;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static const int screenWidth = 256;
static const int screenHeight = 256;

static unsigned int screenScale = 1; 
static unsigned int prevScreenScale = 1;

static RenderTexture2D target = { 0 };  // Initialized at init
Player player = { 0 };
Enemy enemies[MAX_ENEMIES] = { 0 };

// TODO: Define global variables here, recommended to make them static

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(void);      // Update and Draw one frame
void UpdatePlayer(Player*, float);
void DrawPlayer(Player*);
void InitEnemies(Enemy*);
void UpdateEnemies(Enemy*, Player*, float);
void DrawEnemies(Enemy*);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
#if !defined(_DEBUG)
    SetTraceLogLevel(LOG_NONE);         // Disable raylib trace log messsages
#endif

    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "raylib 9yr gamejam");
    
    // TODO: Load resources / Initialize variables at this point
    player.position = (Vector2){128, 128};
    player.speed = 50;

    InitEnemies(enemies);
    
    // Render texture to draw full screen, enables screen scaling
    // NOTE: If screen is scaled, mouse input should be scaled proportionally
    target = LoadRenderTexture(screenWidth, screenHeight);
    // SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);     // Set our game frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button
    {
        UpdateDrawFrame();
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadRenderTexture(target);
    
    // TODO: Unload all loaded resources at this point

    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//--------------------------------------------------------------------------------------------
// Module functions definition
//--------------------------------------------------------------------------------------------
// Update and draw frame
void UpdateDrawFrame(void)
{
    float dt = GetFrameTime();
    // Update
    //----------------------------------------------------------------------------------
    // Screen scale logic (x2)
    if (IsKeyPressed(KEY_ONE)) screenScale = 1;
    else if (IsKeyPressed(KEY_TWO)) screenScale = 2;
    else if (IsKeyPressed(KEY_THREE)) screenScale = 3;
    
    if (screenScale != prevScreenScale)
    {
        // Scale window to fit the scaled render texture
        SetWindowSize(screenWidth*screenScale, screenHeight*screenScale);
        
        // Scale mouse proportionally to keep input logic inside the 256x256 screen limits
        SetMouseScale(1.0f/(float)screenScale, 1.0f/(float)screenScale);
        
        prevScreenScale = screenScale;
    }

    // TODO: Update variables / Implement example logic at this point
    //----------------------------------------------------------------------------------
    UpdatePlayer(&player, dt);
    UpdateEnemies(enemies, &player, dt);
    
    // Draw
    //----------------------------------------------------------------------------------
    // Render all screen to texture (for scaling)
    BeginTextureMode(target);
        ClearBackground(RAYWHITE);
        
        // TODO: Draw screen at 256x256
        DrawRectangle(10, 10, screenWidth - 20, screenHeight - 20, SKYBLUE);

        // Draw equivalent mouse position on the target render-texture
        DrawCircleLines(GetMouseX(), GetMouseY(), 10, MAROON);

        // TODO: Draw everything that requires to be drawn at this point:
        DrawPlayer(&player);
        DrawEnemies(enemies);
        
    EndTextureMode();
    
    BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw render texture to screen scaled as required
        DrawTexturePro(target.texture, (Rectangle){ 0, 0, (float)target.texture.width, -(float)target.texture.height }, (Rectangle){ 0, 0, (float)target.texture.width*screenScale, (float)target.texture.height*screenScale }, (Vector2){ 0, 0 }, 0.0f, WHITE);

    EndDrawing();
    //----------------------------------------------------------------------------------  
}

void UpdatePlayer(Player* player, float dt)
{
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) player->position.x -= player->speed * dt;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) player->position.x += player->speed * dt;
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) player->position.y -= player->speed * dt;
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) player->position.y += player->speed * dt;

    player->rectangle = (Rectangle){player->position.x, player->position.y, 16, 16};
}

void DrawPlayer(Player* player)
{
    DrawRectangleRec(player->rectangle, BLUE);
}


void InitEnemies(Enemy* enemies)
{
    for (size_t i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].position = (Vector2){ GetRandomValue(0, 256), GetRandomValue(0, 256) };
        enemies[i].speed = 5;
        enemies[i].target = player.position;
    }
}

void UpdateEnemies(Enemy* enemies, Player* player, float dt)
{
    for (size_t i = 0; i < MAX_ENEMIES; i++)
    {
        enemies[i].position = Vector2MoveTowards(enemies[i].position, enemies[i].target, enemies[i].speed * dt);
        enemies[i].target = player->position;
    }
}

void DrawEnemies(Enemy* enemies)
{
    for (size_t i = 0; i < MAX_ENEMIES; i++)
    {
        DrawRectangleV(enemies[i].position, (Vector2){16, 16}, BLACK);
    }
}