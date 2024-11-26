
#include <time.h>
#include "raylib.h"

#define NUM_ROADS 4

typedef struct {
    int greenTime;
    int orangeTime;
    int redTime;
} ROAD;

int trafficTimes[4][4][2] = {
    {{20, 5}, {20, 5}, {20, 5}, {20, 5}},
    {{30, 5}, {30, 5}, {30, 5}, {30, 5}},
    {{30, 5}, {40, 5}, {30, 5}, {30, 5}},
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}}
};

Rectangle trafficLights[NUM_ROADS] = {
    {150, 200, 100, 300},
    {350, 200, 100, 300},
    {550, 200, 100, 300},
    {750, 200, 100, 300}
};




int autoTime() {
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    return tm_info->tm_hour * 3600 + tm_info->tm_min * 60 + tm_info->tm_sec;
}

int trafficSetting(int time_in_seconds) {
    if (time_in_seconds >= 5 * 3600 && time_in_seconds < 9 * 3600)
        return 2;
    else if (time_in_seconds >= 9 * 3600 && time_in_seconds < 11 * 3600)
        return 3;
    else if (time_in_seconds >= 11 * 3600 && time_in_seconds < 17 * 3600)
        return 1;
    else if (time_in_seconds >= 17 * 3600 && time_in_seconds < 20 * 3600)
        return 3;
    else if (time_in_seconds >= 20 * 3600 && time_in_seconds < 23 * 3600)
        return 1;
    else
        return 1;
}

int redTime(int road, int time, int indentation) {
    int redTime = 0;
    int setting = trafficSetting(time);

    if (indentation != 0) {
        int OneSettingTime = 0;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 2; j++) {
                OneSettingTime += trafficTimes[setting - 1][i][j];
            }
        }
        int nextSetting = trafficSetting(time + OneSettingTime);

        for (int i = 0; i < 4; i++) {
            if (i < road - 1) {
                for (int j = 0; j < 2; j++) {
                    redTime += trafficTimes[nextSetting - 1][i][j];
                }
            }
            if (i > road - 1) {
                for (int j = 0; j < 2; j++) {
                    redTime += trafficTimes[setting - 1][i][j];
                }
            }
        }
    } else {
        for (int i = 0; i < road - 1; i++) {
            for (int j = 0; j < 2; j++) {
                redTime += trafficTimes[setting - 1][i][j];
            }
        }
    }
    return redTime;
}

void updateTrafficLights(int* modes, int* sampleTime, int* m, ROAD* roads) {
    for (int i = 0; i < NUM_ROADS; i++) {
        if (sampleTime[i] == 0) {
            if (modes[i] == 0) {  // Red -> Green
                modes[i] = 1;
                sampleTime[i] = roads[i].greenTime;
            } else if (modes[i] == 1) {  // Green -> Orange
                modes[i] = 2;
                sampleTime[i] = roads[i].orangeTime;
            } else if (modes[i] == 2) {  // Orange -> Red
                modes[i] = 0;
                sampleTime[i] = roads[i].redTime;
                m[i]++;  // Increment iteration for redTime calculation
            }
        }
    }
}
//Seven Sagments Code
void DrawSevenSegment(int digit, int x, int y, int size, Color color) {
    Rectangle segments[7] = {
        {x + size, y, size * 2, size},           // A
        {x + size * 3, y + size, size, size * 2}, // B
        {x + size * 3, y + size * 4, size, size * 2}, // C
        {x + size, y + size * 6, size * 2, size}, // D
        {x, y + size * 4, size, size * 2},       // E
        {x, y + size, size, size * 2},           // F
        {x + size, y + size * 3, size * 2, size}  // G
    };

    bool segmentMap[10][7] = {
        {1, 1, 1, 1, 1, 1, 0}, // 0
        {0, 1, 1, 0, 0, 0, 0}, // 1
        {1, 1, 0, 1, 1, 0, 1}, // 2
        {1, 1, 1, 1, 0, 0, 1}, // 3
        {0, 1, 1, 0, 0, 1, 1}, // 4
        {1, 0, 1, 1, 0, 1, 1}, // 5
        {1, 0, 1, 1, 1, 1, 1}, // 6
        {1, 1, 1, 0, 0, 0, 0}, // 7
        {1, 1, 1, 1, 1, 1, 1}, // 8
        {1, 1, 1, 1, 0, 1, 1}  // 9
    };

    for (int i = 0; i < 7; i++) {
        DrawRectangleRec(segments[i], segmentMap[digit][i] ? color : BLACK);
    }
}

void DrawNumber(int number, int x, int y, int size, Color color) {
    int tens = (number / 10) % 10;
    int units = number % 10;

    DrawSevenSegment(tens, x, y, size, color);
    DrawSevenSegment(units, x + size * 5, y, size, color);
}


int main() {
    const int screenWidth = 1000;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Traffic Light Simulator");
    SetTargetFPS(60);

    ROAD roads[NUM_ROADS];
    int modes[NUM_ROADS] = {0, 2, 2, 2};  // 0: Red, 1: Green, 2: Orange
    int sampleTime[NUM_ROADS] = {0,0,0,0};
    int m[NUM_ROADS] = {1,0,0,0};

    int setting = trafficSetting(autoTime());
    for (int i = 0; i < NUM_ROADS; i++) {
        roads[i].greenTime = trafficTimes[setting - 1][i][0];
        roads[i].orangeTime = trafficTimes[setting - 1][i][1];
        roads[i].redTime = redTime(i + 1, autoTime(), m[i]);
        
    }

  
        
while (!WindowShouldClose()) {
    int timeNow = autoTime();
    setting = trafficSetting(timeNow);

    // Update redTime for each road
    for (int i = 0; i < NUM_ROADS; i++) {
        roads[i].redTime = redTime(i + 1, timeNow, m[i]);
    }

    // Decrement all sampleTime values simultaneously
    for (int i = 0; i < NUM_ROADS; i++) {
        if (sampleTime[i] > 0) {
            sampleTime[i]--;
        }
    }

    
    updateTrafficLights(modes, sampleTime, m, roads);
 

    BeginDrawing();
    ClearBackground(BLACK);

    for (int i = 0; i < NUM_ROADS; i++) {
        DrawRectangleRounded(trafficLights[i], 0.2, 16, GRAY);

        float centerX = trafficLights[i].x + trafficLights[i].width / 2;
        float centerY = trafficLights[i].y + trafficLights[i].height / 4;

        DrawCircle(centerX, centerY, 25, modes[i] == 0 ? RED : BLACK);
        DrawCircle(centerX, centerY + 70, 25, modes[i] == 2 ? ORANGE : BLACK);
        DrawCircle(centerX, centerY + 140, 25, modes[i] == 1 ? GREEN : BLACK);

        DrawNumber(sampleTime[i], trafficLights[i].x + 15, trafficLights[i].y + trafficLights[i].height + 20, 10,modes[i] == 0 ? RED:modes[i] == 2 ? ORANGE : GREEN);
        
    }

    EndDrawing();
    WaitTime(1.0); 
}

    CloseWindow();
    return 0;
}
