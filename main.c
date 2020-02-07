#include "raylib.h"
#include <time.h>
#include <stdlib.h>

const int max = 8;
float generatingPos;
float forwardSpeed;
float cameraPosX;
float cameraPosY;
float camSize;
int WIDTH = 1366, HEIGHT = 768;
bool GAME_ON = true;
// Color colors[8] = {RED, GREEN, BLUE, MAGENTA, YELLOW, MAROON, ORANGE, GRAY};
Color colors[8] = {(Color){255, 251, 185, 255}, (Color){255, 232, 35, 255}, (Color){255, 185, 33, 255}, (Color){204, 85, 0, 255}, (Color){137, 52, 37, 255}, (Color){255, 251, 185, 255}, (Color){200, 52, 76, 255}, (Color){210, 100, 88, 255}};

float random_generator(float, float);
void startScene();
void gameScene(bool);
void endScene();

float random_generator(float min, float max)
{
    float scale = rand() / (float)RAND_MAX;
    return (min + scale * (max - min));
}

int main()
{
    srand((unsigned int)time(NULL)); //seed random numbers
    InitWindow(WIDTH, HEIGHT, "DODGE BLOCKS");
    SetTargetFPS(60);
    startScene();
}

void gameScene(bool startVar)
{
    bool collision;

    float y_between_planes = 40.0f; //gap between two planes
    float y_min;                    //lowest position for lower block's center

    //define planes...
    typedef struct Plane
    {
        Vector3 pos;
        Vector3 size;
        Color color;
    } Plane;

    Plane plane_ground;
    Plane plane_top;

    plane_ground.pos = (Vector3){0.0f, -10.0f, 0.0f};
    plane_ground.size = (Vector3){500.0f, 10.0f, 1000.0f};
    plane_ground.color = DARKBROWN;

    plane_top = plane_ground;
    plane_top.pos = (Vector3){0.0f, 0.0f + y_between_planes, 0.0f};
    plane_top.color = BLANK;

    // Define Basic Block
    typedef struct Cube
    {
        Vector3 pos;
        Vector3 size;
        Color color;
    } Cube;

    float y_block; //height of the block
    float x_block; //width in x drxn
    float z_block; //width in z drxn
    float x_gap;   //gap between two blocks in x-drxn
    float y_gap;   //gap between two blocks in y-drxn
    int score = 0;

    Cube cube_reference;

    y_block = 37.0f;
    x_block = 5.0f;
    z_block = 10.0f;

    x_gap = 10.0f;
    y_gap = 15.0f;

    cube_reference.size = (Vector3){x_block, y_block, z_block};
    cube_reference.pos = (Vector3){0.0f, 0.0f, 0.0f};
    cube_reference.color = YELLOW;

    Cube cube[max], cube_pair[max];

    y_min = (y_block / 2) - 0.2f;

    //initialise global and necessary variables
    forwardSpeed = 0.12f;
    cameraPosX = 20.0f;
    cameraPosY = 20.0f;
    camSize = 0.5f; //in all directions
    float camMovementTime = 0.0f;
    float camMovementVelocity = forwardSpeed;
    generatingPos = (0.0f - (8.0f * (x_block + x_gap)) + cameraPosX);

    //Generate the blocks here for the first time
    //and wrap it with a colliding Box
    cube[0] = cube_reference; //for first block
    int i;
    for (i = 0; i < max - 1; i++)
    {
        cube[i].pos.y = (0.0f - random_generator((plane_ground.pos.y), (y_min)));
        cube[i + 1] = cube[i];
        cube[i + 1].pos.x = (cube[i].pos.x - x_gap - x_block);
        cube_pair[i] = cube[i];
        cube_pair[i].pos.y = cube[i].pos.y + y_block + y_gap;
    }
    cube_pair[i + 1] = cube[i + 1];
    cube_pair[i + 1].pos.y = cube[i + 1].pos.y + y_block + y_gap; //for last block

    //Bounding Boxes
    while (!WindowShouldClose())
    {
        // Define the camera to look into our 3D world
        Camera3D camera = {0};
        camera.position = (Vector3){cameraPosX, cameraPosY, 0.0f}; // Camera position
        camera.target = (Vector3){-1000.0f, 0.0f, 0.0f};           // Camera looking at point
        camera.up = (Vector3){0.0f, 1.0f, 0.0f};                   // Camera up vector (rotation towards target)
        camera.fovy = 80.0f;                                       // Camera field-of-view Y
        camera.type = CAMERA_PERSPECTIVE;                          // Camera mode type

        //Determine camera position from user using physics
        if (IsKeyDown(KEY_SPACE))
        {
            cameraPosY = cameraPosY + (GetFrameTime() * 15.0f);
            camMovementTime = 0.0f;
            camera.target = (Vector3){-1000.0f, 30.0f, 0.0f};
        }
        else
        {
            //use the school level physics for determining height
            camMovementTime = camMovementTime + GetFrameTime();
            camMovementVelocity = 0.0098f * camMovementTime + camMovementVelocity;
            cameraPosY = cameraPosY - (camMovementVelocity * camMovementTime) + (0.5f * 0.0098f * camMovementTime * camMovementTime);
        }
        //Draw bounding box for camera
        BoundingBox box[max], box_pair[max], boxCamera, boxGround, boxTop;
        boxCamera.min = (Vector3){camera.position.x - camSize,
                                  camera.position.y - camSize / 2.0f,
                                  camera.position.z - camSize};
        boxCamera.max = (Vector3){camera.position.x + camSize,
                                  camera.position.y + camSize,
                                  camera.position.z + camSize};
        //Draw bounding box for ground plane
        boxGround.min = (Vector3){plane_ground.pos.x - plane_ground.size.x,
                                  plane_ground.pos.y - plane_ground.size.y,
                                  plane_ground.pos.z - plane_ground.size.z};
        boxGround.max = (Vector3){plane_ground.pos.x + plane_ground.size.x,
                                  plane_ground.pos.y + plane_ground.size.y,
                                  plane_ground.pos.z + plane_ground.size.z};
        //Draw bounding box for top plane
        boxTop.min = (Vector3){plane_top.pos.x - plane_top.size.x,
                                  plane_top.pos.y - plane_top.size.y,
                                  plane_top.pos.z - plane_top.size.z};
        boxTop.max = (Vector3){plane_top.pos.x + plane_top.size.x,
                                  plane_top.pos.y + plane_top.size.y,
                                  plane_top.pos.z + plane_top.size.z};

        ClearBackground(SKYBLUE); //used startVar to draw something like preview
        BeginDrawing();

        BeginMode3D(camera);

        for (int i = 0; i < max; i++)
        // just for moving forward
        // and crete a bounding box for collision simultanously

        {
            cube[i].color = colors[(i % 8)];
            cube_pair[i].color = colors[(i % 8)]; // setting different color

            //Check whether a cube collides to the back of our camera
            // so that it generates back to the generating position
            if (cube[i].pos.x >= camera.position.x)
            {

                cube[i].pos.x = generatingPos;
                cube[i].pos.y = (0.0f - random_generator((plane_ground.pos.y), (y_min)));

                cube_pair[i].pos.x = generatingPos;
                cube_pair[i].pos.y = (cube[i].pos.y + y_block + y_gap);

                score += 10; //Generates score of the game
            }

            if (score == 100)
            {
                forwardSpeed = 0.23f;
            }

            cube[i].pos.x += forwardSpeed;
            cube_pair[i].pos.x += forwardSpeed;

            box[i].min = (Vector3){cube[i].pos.x - cube[i].size.x / 2,
                                   cube[i].pos.y - cube[i].size.y / 2,
                                   cube[i].pos.z - cube[i].size.z / 2};
            box[i].max = (Vector3){cube[i].pos.x + cube[i].size.x / 2,
                                   cube[i].pos.y + cube[i].size.y / 2,
                                   cube[i].pos.z + cube[i].size.z / 2};
            box_pair[i].min = (Vector3){cube_pair[i].pos.x - cube_pair[i].size.x / 2,
                                        cube_pair[i].pos.y - cube_pair[i].size.y / 2,
                                        cube_pair[i].pos.z - cube_pair[i].size.z / 2};
            box_pair[i].max = (Vector3){cube_pair[i].pos.x + cube_pair[i].size.x / 2,
                                        cube_pair[i].pos.y + cube_pair[i].size.y / 2,
                                        cube_pair[i].pos.z + cube_pair[i].size.z / 2};
            DrawCubeV(cube[i].pos, cube[i].size, cube[i].color);
            DrawCubeV(cube_pair[i].pos, cube_pair[i].size, cube_pair[i].color);
            DrawCubeWiresV(cube[i].pos, cube[i].size, Fade(BLACK, 0.2f));
            DrawCubeWiresV(cube_pair[i].pos, cube_pair[i].size, Fade(BLACK, 0.2f));
            collision = (CheckCollisionBoxes(box[i], boxCamera) || CheckCollisionBoxes(box_pair[i], boxCamera) || CheckCollisionBoxes(boxGround, boxCamera)||CheckCollisionBoxes(boxTop, boxCamera));
            if (collision)
            {
                GAME_ON = false;
            }
        }

        DrawCubeV(plane_ground.pos, plane_ground.size, plane_ground.color);
        DrawCubeV(plane_top.pos, plane_top.size, plane_top.color);

        EndMode3D();
        if (!GAME_ON || startVar)
        {
            break;
        }
        // keeping this condition in while's expression seems not working
        // Also used startVar to draw only once for preview

        DrawRectangle(10, 10, WIDTH / 4.9, HEIGHT / 13, (Color)(Fade(BLUE, 0.8f)));
        DrawText(TextFormat("SCORE :%04i", score), 20, 20, 40, BLACK);
        // DrawText(TextFormat("Passed %d",pass))
        EndDrawing();
    }
    if (!GAME_ON)
    {
        endScene();
    }

    //CloseWindow();
}
void startScene()
{
    GAME_ON = true;
    gameScene(true);
    //InitWindow(WIDTH,HEIGHT, "MyGame");
    //SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        //ClearBackground(RAYWHITE);
        BeginDrawing();
        DrawText(TextFormat("DODGE BLOCKS"), ((WIDTH / 4)), ((HEIGHT / 5)), 80, RED);
        DrawRectangle((WIDTH / 2.6), (HEIGHT / 2.5), (WIDTH / 5.2), (HEIGHT / 5), DARKPURPLE);
        DrawTriangle((Vector2){WIDTH / 2.5, HEIGHT / 2}, (Vector2){WIDTH / 4, HEIGHT / 4}, (Vector2){WIDTH / 0.5, HEIGHT / 0.5}, GREEN);
        DrawTriangle((Vector2){700, 385}, (Vector2){620, 350}, (Vector2){620, 415}, RAYWHITE);
        DrawText(TextFormat("PRESS SPACE TO PLAY"), (WIDTH / 4.5), (HEIGHT / 1.4), 60, BLACK);
        if (IsKeyPressed(KEY_SPACE))
        {
            break;
        }
        EndDrawing();
    }
    //CloseWindow();
    gameScene(false);
}
void endScene()
{

    while (!WindowShouldClose())
    {
        ClearBackground(RAYWHITE);
        BeginDrawing();
        DrawText(TextFormat("GAME OVER"), (WIDTH / 3), (HEIGHT / 4), 80, MAROON);
        DrawText(TextFormat("PRESS ENTER TO PLAY AGAIN"), (WIDTH / 6.5), (HEIGHT / 2), 60, BLACK);
        if (IsKeyPressed(KEY_ENTER))
        {
            break;
        }
        EndDrawing();
    }
    CloseWindow();
    main();
}