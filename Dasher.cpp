
//Include the libraries that we are going to use.
//Lets first include the Raylib library. It is an external library, so #include uses "" instead of <> as it is not part of the C++ Standard Library.
#include "raylib.h"

// VARIABLES
//*****************************************************************************************************************************
//This struct is used to store the info for the animations of all the sprites used in the game
struct AnimData
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};



// FUNCTIONS
//*****************************************************************************************************************************
//Functions for checking if the character is on ground
bool IsOnGround(AnimData data, int windowHeight)
{
    return data.pos.y >= windowHeight - data.rec.height;
}

// Updates the position of the image used as a sprite for the animation
AnimData UpdateAnimation(AnimData data, float deltaTime, int maxFrame)
{
    data.runningTime += deltaTime;
    if(data.runningTime >= data.updateTime)
    {
        data.runningTime = 0;
        data.rec.x = data.frame * data.rec.width;                                                           
        data.frame = (data.frame + 1) % (maxFrame + 1);
    }
    return data;
}

// Sets all the values of the game to initial state
void ResetGame(AnimData &scarfyData, AnimData nebulae[], int size, float &finishLine, int window[], int &velocity, int &gameState, int &nebVel)
{
    // Reset player character
    scarfyData.pos = {window[0] / 2 - (scarfyData.rec.width) / 2, window[1] - scarfyData.rec.height};
    scarfyData.frame = 0;
    scarfyData.runningTime = 0;

    // Reset hazards
    for (int i = 0; i < size; i++)
    {
        nebulae[i].pos.x = window[0] + (GetRandomValue(600,800) * i);
        nebulae[i].frame = 0;
        nebulae[i].runningTime = 0;
    }

    // Reset finish line
    finishLine = nebulae[size - 1].pos.x;

    // Reset velocity and state
    velocity = 0;
    gameState = 0; //0 -> Initial State, 1 -> Playing, 2 -> Game Over, 3-> Winning the game
    
}

// Background update its positions
void UpdateBackground(float &bgX, float &mgX, float &fgX, const float deltaTime, const float bgScale, Texture2D textures[]) 
{
    float speeds[3] = {20, 150, 300};
    float *positions[3] = {&bgX, &mgX, &fgX};

    for (int i = 0; i < 3; i++) {
        *positions[i] -= speeds[i] * deltaTime;
        if (*positions[i] <= -textures[i].width * bgScale) *positions[i] = 0.0f;
    }
}

//Draw the backgrounds with he new positions
void DrawBackgrounds(float bgX, float mgX, float fgX, const float bgScale, Texture2D textures[]) {
    Vector2 positions[3][2] = {
        {{bgX, 0.0}, {bgX + textures[0].width * bgScale, 0.0}},
        {{mgX, 0.0}, {mgX + textures[1].width * bgScale, 0.0}},
        {{fgX, 0.0}, {fgX + textures[2].width * bgScale, 0.0}}
    };

    for (int i = 0; i < 3; i++) {
        DrawTextureEx(textures[i], positions[i][0], 0.0, bgScale, WHITE);
        DrawTextureEx(textures[i], positions[i][1], 0.0, bgScale, WHITE);
    }
}

//*****************************************************************************************************************************
// Main Function

int main()
{
    //*****************************************************************************************************************************
    //Window Dimensions

    int window[2] = {1024, 760};     

    //First action is to create a popup window where we can see the game.
    InitWindow(window[0],window[1],"Dasher-Dapper by TonyHmt");

    //*****************************************************************************************************************************
    //Game play Variables
    int gameState = 0;
    const int gravity = 4000;
    int jumpVel = -1500;
    int velocity = 0, jumps = 0;
    const int maxJumps = 2;
    bool inAir = false;
    Texture2D textures[3] = {
        LoadTexture("textures/farbuildings.png"),
        LoadTexture("textures/backbuildings.png"),
        LoadTexture("textures/foreground.png")
    };
    const float bgScale = 4.0;
    float bgX = 0, mgX = 0, fgX = 0;

    //*****************************************************************************************************************************
    //Player Character    
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    AnimData scarfyData = {{0, 0, scarfy.width / 6, scarfy.height}, {window[0] / 2, window[1] - scarfy.height}, 0, 1.0 / 12.0, 0.0};


    //*****************************************************************************************************************************
    //Hazards
    Texture2D nebula = LoadTexture("textures/nebula.png");
    const int sizeOfNebulae = 20;
    AnimData nebulae[sizeOfNebulae];
    int nebVel = -600;
    for (int i = 0; i < sizeOfNebulae; i++) {
        nebulae[i] = {{0, 0, (float)nebula.width / 8, (float)nebula.height / 8}, {(float)window[0] + (600 * i), (float)window[1] - nebula.height / 8}, 0, 1.0 / 16.0, 0.0};
    }
    float finishLine = nebulae[sizeOfNebulae - 1].pos.x;
     
    //We are going to use a function from Raylib to set the frames per second of the program to be 60, just to prevent this program to run extremely fast.
    SetTargetFPS(60);

    //We are adding the operator "!" before the WindowShouldClose() call, becase we want the while be running while it is false. Once true, we will finish the while loop.
    while (!WindowShouldClose())
    {
        //This function set up the PopUp window to let us draw stuff into it every frame.
        BeginDrawing();
        ClearBackground(BLACK);
        
        const float dT = GetFrameTime();

        //With this switch, we will be changing the game's screen based on the game state.
        switch (gameState)
        {
            case 0:
                //***********************************************INTRO*********************************************************************************
                //*************************************************************************************************************************************
                    //Setting a black background                    
                    DrawText("", 10, window[1]/2 - 20, 20, WHITE);
                    DrawText("Press ENTER to start...", 10, window[1]/2 + 20, 20, WHITE);

                    // if the player presses enter
                    if (IsKeyPressed(KEY_ENTER))
                    {
                        gameState = 1; // we change the state to playing
                    }
                break; 
        
            case 1:
                //***********************************************GAME*********************************************************************************
                //************************************************************************************************************************************
                {
                                    
                    UpdateBackground(bgX, mgX, fgX, dT, bgScale, textures);
                    DrawBackgrounds(bgX, mgX, fgX, bgScale, textures);
                    
                    if (IsOnGround(scarfyData, window[1])) 
                    {
                        velocity = 0;
                        inAir = false;
                        jumps = 0;
                    } else {
                        velocity += gravity * dT;
                        inAir = true;
                    }

                    if (IsKeyPressed(KEY_SPACE) && jumps < maxJumps) 
                    {
                        velocity = jumpVel;
                        jumps++;
                    }

                    scarfyData.pos.y += velocity * dT;
                    scarfyData = inAir ? scarfyData : UpdateAnimation(scarfyData, dT, 5);

                    for (int i = 0; i < sizeOfNebulae; i++) 
                    {
                        nebulae[i].pos.x += nebVel * dT;
                        nebulae[i] = UpdateAnimation(nebulae[i], dT, 7);
                        DrawTextureRec(nebula, nebulae[i].rec, nebulae[i].pos, WHITE);
                    }

                    DrawTextureRec(scarfy, scarfyData.rec, scarfyData.pos, WHITE);
                    
                    bool collision = false;
                    for (AnimData neb : nebulae) 
                    {
                        Rectangle nebRec = {neb.pos.x + 20, neb.pos.y + 20, neb.rec.width - 40, neb.rec.height - 40};
                        Rectangle scarfyRec = {scarfyData.pos.x, scarfyData.pos.y, scarfyData.rec.width, scarfyData.rec.height};
                        if (CheckCollisionRecs(nebRec, scarfyRec)) collision = true;
                    }

                    finishLine += nebVel * dT;  

                    if (collision) gameState = 2;
                    if (scarfyData.pos.x >= finishLine) gameState = 3;
                                        
                    break;
            
                    };
                                        
            
                break;

            case 2:
                //***********************************************GAME OVER****************************************************************************
                //************************************************************************************************************************************
                {
                DrawText("Game Over!", window[0] / 2 - 50, window[1] / 2, 20, RED);
                DrawText("Press R to restart", window[0] / 2 - 50, window[1] / 2 + 40, 20, WHITE);
                if (IsKeyPressed(KEY_R)) ResetGame(scarfyData, nebulae, sizeOfNebulae, finishLine, window, velocity, gameState, nebVel);
                break;
                }

                break;

            case 3:
                //***********************************************WIN**********************************************************************************
                //************************************************************************************************************************************
                DrawText("You Win!", window[0] / 2 - 50, window[1] / 2, 20, GREEN);
                DrawText("Press R to restart", window[0] / 2 - 50, window[1] / 2 + 40, 20, WHITE);
                if (IsKeyPressed(KEY_R)) ResetGame(scarfyData, nebulae, sizeOfNebulae, finishLine, window, velocity, gameState, nebVel);
                break;

            default:
                break;
        }
          
        //Stop drawing
        EndDrawing();
    }

    
    CloseWindow();
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(textures[0]);
    UnloadTexture(textures[1]);
    UnloadTexture(textures[2]);
    return 0;
    
}

