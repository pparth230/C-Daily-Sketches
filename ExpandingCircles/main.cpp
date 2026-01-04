#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>
#include <vector>
#include <cmath>

//------------------------------------------------------------------------------------
// Perlin Noise Implementation
//------------------------------------------------------------------------------------

static const int PERLIN_PERMUTATION[512] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
    134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
    55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
    18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
    250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
    189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
    172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
    228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
    107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
    134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
    55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
    18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
    250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
    189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
    172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
    228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
    107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

inline float fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

inline float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

inline float grad(int hash, float x, float y) {
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float ofNoise(float seed, float time) {
    float x = seed;
    float y = time;

    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    x -= floor(x);
    y -= floor(y);

    float u = fade(x);
    float v = fade(y);

    int A = PERLIN_PERMUTATION[X] + Y;
    int AA = PERLIN_PERMUTATION[A];
    int AB = PERLIN_PERMUTATION[A + 1];
    int B = PERLIN_PERMUTATION[X + 1] + Y;
    int BA = PERLIN_PERMUTATION[B];
    int BB = PERLIN_PERMUTATION[B + 1];

    float result = lerp(
        lerp(grad(PERLIN_PERMUTATION[AA], x, y), grad(PERLIN_PERMUTATION[BA], x - 1, y), u),
        lerp(grad(PERLIN_PERMUTATION[AB], x, y - 1), grad(PERLIN_PERMUTATION[BB], x - 1, y - 1), u),
        v
    );

    return (result + 1.0f) * 0.5f;
}

float ofMap(float value, float inputMin, float inputMax, float outputMin, float outputMax) {
    return outputMin + ((value - inputMin) / (inputMax - inputMin)) * (outputMax - outputMin);
}

int main(void) {
    const int screenWidth = 720;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "ExpandingCircles");
    SetTargetFPS(30);

    Camera3D camera = {
        .position = { 0.0f, 0.0f, 1000.0f },
        .target = { 0.0f, 0.0f, 0.0f },
        .up = { 0.0f, 1.0f, 0.0f },
        .fovy = 45.0f,
        .projection = CAMERA_PERSPECTIVE
    };

    std::vector<float> radius_list;
    std::vector<float> max_radius_list;
    std::vector<Vector3> rotate_list;

    int frameNum = 0;

    while (!WindowShouldClose()) {
        // UPDATE
        if (frameNum % 2 == 0) {
            radius_list.push_back(1.0f);
            max_radius_list.push_back(300.0f);

            float time = frameNum * 0.0035f;
            Vector3 rotation = {
                ofMap(ofNoise(1984, time), 0, 1, -360, 360),
                ofMap(ofNoise(1103, time), 0, 1, -360, 360),
                ofMap(ofNoise(702, time), 0, 1, -360, 360)
            };
            rotate_list.push_back(rotation);
        }

        for (int i = radius_list.size() - 1; i >= 0; i--) {
            if (radius_list[i] < max_radius_list[i]) {
                radius_list[i] += 2.0f;
            } else {
                radius_list.erase(radius_list.begin() + i);
                max_radius_list.erase(max_radius_list.begin() + i);
                rotate_list.erase(rotate_list.begin() + i);
            }
        }

        // DRAW
        BeginDrawing();
        ClearBackground((Color){ 239, 239, 239, 255 });

        BeginMode3D(camera);

        rlPushMatrix();
        rlRotatef(frameNum * 1.44f, 0.0f, 1.0f, 0.0f);

        for (size_t i = 0; i < radius_list.size(); i++) {
            rlPushMatrix();

            rlRotatef(rotate_list[i].z, 0.0f, 0.0f, 1.0f);
            rlRotatef(rotate_list[i].y, 0.0f, 1.0f, 0.0f);
            rlRotatef(rotate_list[i].x, 1.0f, 0.0f, 0.0f);

            int alpha = (int)ofMap(radius_list[i], 1, max_radius_list[i], 255, 0);
            alpha = Clamp(alpha, 0, 255);

            rlColor4ub(0, 0, 0, (unsigned char)alpha);

            rlBegin(RL_LINES);
            for (int j = 0; j < 60; j++) {
                float angle1 = j * 2.0f * PI / 60.0f;
                float angle2 = (j + 1) * 2.0f * PI / 60.0f;

                float x1 = radius_list[i] * cosf(angle1);
                float y1 = radius_list[i] * sinf(angle1);
                float x2 = radius_list[i] * cosf(angle2);
                float y2 = radius_list[i] * sinf(angle2);

                rlVertex3f(x1, y1, 0);
                rlVertex3f(x2, y2, 0);
            }
            rlEnd();

            rlPopMatrix();
        }

        rlPopMatrix();
        EndMode3D();

        EndDrawing();
        frameNum++;
    }

    CloseWindow();
    return 0;
}
