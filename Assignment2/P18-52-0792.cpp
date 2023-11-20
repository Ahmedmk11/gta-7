#define GL_SILENCE_DEPRECATION
#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include <cmath>
#include <string>
#include <iostream>
#include <random>
#include <chrono>
#include <SDL.h>
#include <SDL_mixer.h>

// Structures

struct Time {
    int minutes;
    int seconds;
};

// Class Declarations

class Vector3f;
class Camera;

// Function Declarations

Time secondsToMinutesAndSeconds(int totalSeconds);
void print(int x, int y, char* string, int font);
void playSound(int channel, int index);
void gameEnd(bool win);
void playerCollide();
void displayTime();
void drawBoundaries();
void drawJetpack();
void drawBigSmoke();
void drawBurger(int mode);
void drawLamps();
void drawFerrisWheel();
void drawCar();
void drawKiosk();
void drawSeeSaw();
void drawFence();
void setupLights();
void setupCamera();
bool checkRocketMan();
void displayCheatStatus();
void Display();
void timer(int value);
void Keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void specialKeyReleased(int key, int x, int y);
void anim();

// Variables

std::default_random_engine generator(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));

std::vector<std::string> audioFilesNames;
std::vector<char> keys;
std::vector<char> targetSequence = {'r', 'o', 'c', 'k', 'e', 't', 'm', 'a', 'n'};

float playerX = 1.0f;
float playerY = 0;
float playerZ = 5.5f;
float playerRotation = 180.0f;
bool topView = false;
bool sideView = false;
char lastKey;
int rotation = 0.0f;
float upDown = 0.5f;
float increment = 0.01f;
bool legRotation = false;
float legRotationLeft = 0.0f;
float legRotationRight = 0.0f;
float legRotationIncrement = 20.0f;
int counter = 0;
int burgers = 1;
int bigBurger = 1;
bool gameOver = false;
float r = 1;
float g = 1;
float b = 1;
bool cheatCode = false;
bool jetpack = false;
bool falling = false;
bool displayCheatActivated = false;
bool displayCheatDeactivated = false;
int cheatTime1 = -1;
int cheatTime2 = -1;
float rotateFerris = 0.0f;
float carMovement = 2.0f;
float carDirection = 1;
float lampLength1 = 1;
float lampLengthDirection1 = 1;
float lampLength2 = 1;
float lampLengthDirection2 = 1;
float fireAnimation = 1;
float fireDirection = 1;
bool objectsAnimate = false;
bool lamp1ReachedPeak = false;
float curtain1Rotation = -90.0f;
float curtain2Rotation = -90.0f;
int curtain1RotationDirection = 1;
int curtain2RotationDirection = 1;
bool openedCurtain1 = false;
float seeSawRotation = 0.0f;
int seeSawDirection = 1;
bool runningStarted = true;
bool firstLoopAfterEnd = true;
bool bigSmokeAteTheBurger = false;
int timeRemaining = 60;
Time displayedTime = secondsToMinutesAndSeconds(timeRemaining);

// Definitions

class Vector3f {
public:
    float x, y, z;

    Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
        x = _x;
        y = _y;
        z = _z;
    }

    Vector3f operator+(const Vector3f &v) const {
        return Vector3f(x + v.x, y + v.y, z + v.z);
    }

    Vector3f operator-(const Vector3f &v) const {
        return Vector3f(x - v.x, y - v.y, z - v.z);
    }

    Vector3f operator*(float n) const {
        return Vector3f(x * n, y * n, z * n);
    }

    Vector3f operator/(float n) const {
        return Vector3f(x / n, y / n, z / n);
    }

    Vector3f unit() const {
        return *this / sqrt(x * x + y * y + z * z);
    }

    Vector3f cross(const Vector3f &v) const {
        return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
};

class Camera {
public:
    Vector3f eye, center, up;

    Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
        eye = Vector3f(eyeX, eyeY, eyeZ);
        center = Vector3f(centerX, centerY, centerZ);
        up = Vector3f(upX, upY, upZ);
    }

    void moveX(float d) {
        Vector3f right = up.cross(center - eye).unit();
        eye = eye + right * d;
        center = center + right * d;
    }

    void moveY(float d) {
        eye = eye + up.unit() * d;
        center = center + up.unit() * d;
    }

    void moveZ(float d) {
        Vector3f view = (center - eye).unit();
        eye = eye + view * d;
        center = center + view * d;
    }

    void rotateX(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
        up = view.cross(right);
        center = eye + view;
    }

    void rotateY(float a) {
        Vector3f view = (center - eye).unit();
        Vector3f right = up.cross(view).unit();
        view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
        right = view.cross(up);
        center = eye + view;
    }

    void look() {
        gluLookAt(
            eye.x, eye.y, eye.z,
            center.x, center.y, center.z,
            up.x, up.y, up.z
        );
    }
};

Camera camera;

Time secondsToMinutesAndSeconds(int totalSeconds) {
    Time result;
    result.minutes = totalSeconds / 60;
    result.seconds = totalSeconds % 60;
    return result;
}

void print(int x, int y, char* string, int font) {
    int len, i;
    glRasterPos2f(x, y);
    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
        if (font == 12) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
        } else if (font == 18) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
        } else if (font == 24) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
        }
    }
}

void playSound(int channel, int index) {
    Mix_Chunk* soundEffect = Mix_LoadWAV(audioFilesNames.at(index).c_str());
    if (!soundEffect) {
        std::cerr << "Sound effect not loaded" << std::endl;
    }

    Mix_PlayChannel(channel, soundEffect, 0);
    Mix_Volume(2, 128);
}

void gameEnd(bool win) {
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1, 1, 1);
    
    std::string text;

    if (win) {
        text = "You Won!";
    } else {
        text = "You're out of time :(";
    }

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 1000);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    int textWidth = glutBitmapLength(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)text.c_str());

    print(800 - textWidth / 2, 491, const_cast<char*>(text.c_str()), 18);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void playerCollide() {
    const float playerWidth = 0.5;
    
    float leftBoundary = playerX - playerWidth;
    float rightBoundary = playerX + playerWidth;
    float frontBoundary = playerZ - playerWidth;
    float backBoundary = playerZ + playerWidth;
    
    std::cout << playerX << ", " << playerY << ", " << playerZ << std::endl;
    std::cout << leftBoundary << ", " << rightBoundary << ", " << frontBoundary << ", " << backBoundary << "\n" << std::endl;
    
    if (playerZ < -3 && playerY <= 1) {
        if (burgers == 1) {
            burgers --;
            playSound(1, 4);
            bigSmokeAteTheBurger = true;
        }
    }
}

void displayTime() {
    std::string mins;
    std::string secs;
    
    if (displayedTime.minutes <= 9) {
        mins = "0" + std::to_string(displayedTime.minutes);
    } else {
        mins = std::to_string(displayedTime.minutes);
    }

    if (displayedTime.seconds <= 9) {
        secs = "0" + std::to_string(displayedTime.seconds);
    } else {
        secs = std::to_string(displayedTime.seconds);
    }

    std::string text = mins + ":" + secs;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 1000);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int x = 1600 - 75;
    int y = 1000 - 28;

    if (timeRemaining <= 15) {
        if (timeRemaining % 2 == 0) {
            glColor3f(1.0f, 0.0f, 0.0f);
        } else {
            glColor3f(0.0f, 0.0f, 0.0f);
        }
    } else {
        glColor3f(0.0f, 0.0f, 0.0f);
    }

    print(x, y, const_cast<char*>(text.c_str()), 18);
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void drawBoundaries() {
    float thickness = 0.05f;
    float width = 2.0f;
    float length = 10.0f;
    
    glColor3f(0, 0.4, 0);
    glPushMatrix();
    glTranslatef(0.5f * width, 0.5f * thickness, 0.5f * width);
    glScalef(width * 4, thickness, length);
    glutSolidCube(1);
    glPopMatrix();
    
    glColor3f(r, g, b);
    glPushMatrix();
    glTranslatef(width + 1.5 * width + 0.025, 1, 0.5f * width);
    glRotatef(90, 0, 0, 1.0f);
    glScalef(width, thickness, length);
    glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-width - 0.5f * width - 0.025, 1, 0.5f * width);
    glRotatef(90, 0, 0, 1.0f);
    glScalef(width, thickness, length);
    glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(1, 1, -4);
    glRotatef(90, 0, 0, 1);
    glRotatef(90, 1, 0, 0);
    glScalef(width, thickness, length-1.90);
    glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(5, 2.25, -4);
    glutSolidCube(0.5);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-3, 2.25, -4);
    glutSolidCube(0.5);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(5, 2.25, 6);
    glutSolidCube(0.5);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-3, 2.25, 6);
    glutSolidCube(0.5);
    glPopMatrix();
    
    glColor3f(1, 1, 1);
}

void drawJetpack() {

    glPushMatrix();
    glColor3f(0.6, 0.7, 0.8);

    // Main
    
    glPushMatrix();
    glTranslatef(0, 0.75, -0.35);
    glScalef(0.8, 0.5, 0.2);
    glutSolidCube(1);
    glPopMatrix();
    
    // Holes
    
    glColor3f(1, 1, 1);
    
    GLUquadric* quad1 = gluNewQuadric();
    glPushMatrix();
    glTranslatef(-0.25, 0.5, -0.35);
    glRotatef(90, 1, 0, 0);
    gluCylinder(quad1, 0.05, 0.05, 0.005, 50, 50);
    glTranslatef(0.0f, 0.0f, 0.005);
    gluDisk(quad1, 0.0, 0.05, 50, 1);
    glTranslatef(0.0f, 0.0f, -0.005);
    gluDisk(quad1, 0.0, 0.05, 50, 1);
    glPopMatrix();
    
    GLUquadric* quad2 = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0.25, 0.5, -0.35);
    glRotatef(90, 1, 0, 0);
    gluCylinder(quad2, 0.05, 0.05, 0.005, 50, 50);
    glTranslatef(0.0f, 0.0f, 0.005);
    gluDisk(quad2, 0.0, 0.05, 50, 1);
    glTranslatef(0.0f, 0.0f, -0.005);
    gluDisk(quad2, 0.0, 0.05, 50, 1);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Fire
    glColor3f(1.0, 0.3529, 0.0);
    
    glPushMatrix();
    glTranslatef(0.25, 0.495, -0.35);
    glScalef(1, fireAnimation, 1);
    glRotatef(90, 1, 0, 0);
    glutSolidCone(0.04, 0.25, 50, 50);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.25, 0.495, -0.35);
    glScalef(1, fireAnimation, 1);
    glRotatef(90, 1, 0, 0);
    glutSolidCone(0.04, 0.25, 50, 50);
    glPopMatrix();
    
    glColor3f(1, 1, 1);
    glPopMatrix();
}

void drawBigSmoke() {
    float bodySize = 0.5;
    float headSize = 0.15;
    float hatSize = 0.155;
    float legSize = bodySize / 3;
    glPushMatrix();
    glTranslatef(playerX, playerY, playerZ);
    glRotatef(playerRotation, 0.0f, 1.0f, 0.0f);
    
    // Hat
    
    glColor3f(0.2078f, 0.2196f, 0.2549f);
    glPushMatrix();
    glTranslatef(0, legSize * 3 + bodySize + headSize, 0);
    glScalef(1.0f, 0.1f, 1.0f);
    glutSolidSphere(hatSize, 50, 50);
    glPopMatrix();
    
    GLUquadric* quad = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0, legSize * 3 + bodySize + headSize + 0.155 * 0.1, 0);
    glRotatef(-90, 1.0f, 0.0f, 0.0f); // Orient the cylinder
    gluCylinder(quad, headSize / 2, headSize / 2, hatSize, 50, 50);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Head
    
    glColor3f(0.2157f, 0.1804f, 0.0745f);
    glPushMatrix();
    glTranslatef(0, legSize * 3 + bodySize + headSize/2, 0);
    glutSolidCube(headSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Eyes
    
    glColor3f(1, 1, 1);
    GLUquadric* quad3 = gluNewQuadric();
    glPushMatrix();
    glTranslatef(-0.03, legSize * 3 + bodySize + headSize / 2, headSize - 0.07);
    glRotatef(90, 0.0f, 0.0f, 1.0f);
    gluCylinder(quad3, 0.009, 0.009, 0.005, 50, 50);
    glTranslatef(0.0f, 0.0f, 0.005);
    gluDisk(quad3, 0.0, 0.009, 50, 1);
    glTranslatef(0.0f, 0.0f, -0.005);
    gluDisk(quad3, 0.0, 0.009, 50, 1);
    glPopMatrix();
    
    glColor3f(1, 1, 1);
    GLUquadric* quad4 = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0.03, legSize * 3 + bodySize + headSize / 2, headSize - 0.07);
    glRotatef(90, 0.0f, 0.0f, 1.0f);
    gluCylinder(quad4, 0.009, 0.009, 0.005, 50, 50);
    glTranslatef(0.0f, 0.0f, 0.005);
    gluDisk(quad3, 0.0, 0.009, 50, 1);
    glTranslatef(0.0f, 0.0f, -0.005);
    gluDisk(quad3, 0.0, 0.009, 50, 1);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Mouth
    
    glColor3f(1, 1, 1);
    GLUquadric* quad5 = gluNewQuadric();
    glPushMatrix();
    glTranslatef(0, -0.04 + legSize * 3 + bodySize + headSize / 2, headSize - 0.07);
    glRotatef(90, 0.0f, 0.0f, 1.0f);
    glScalef(1, 2, 1);
    gluCylinder(quad5, 0.009, 0.009, 0.005, 50, 50);
    glTranslatef(0.0f, 0.0f, 0.005);
    gluDisk(quad5, 0.0, 0.009, 50, 1);
    glTranslatef(0.0f, 0.0f, -0.005);
    gluDisk(quad5, 0.0, 0.009, 50, 1);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Moustache & Goatee
    
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(0, -0.05 + legSize * 3 + bodySize + headSize / 2, headSize - 0.07);
    glScalef(1.0f, 1.0f, 0.07f);
    glutSolidCube(0.06);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Glasses
    
    // Right Lens
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(0.03, 0.005 + legSize * 3 + bodySize + headSize / 2, headSize - 0.07);
    glScalef(0.5f, 0.65f, 0.07f);
    glutSolidCube(0.06);
    glPopMatrix();
    
    // Left Lens
    glPushMatrix();
    glTranslatef(-0.03, 0.005 + legSize * 3 + bodySize + headSize / 2, headSize - 0.07);
    glScalef(0.5f, 0.65f, 0.07f);
    glutSolidCube(0.06);
    glPopMatrix();
    
    // Middle Part
    glPushMatrix();
    glTranslatef(0, 0.005 + legSize * 3 + bodySize + headSize / 2, headSize - 0.07);
    glScalef(1.0f, 0.15f, 0.07f);
    glutSolidCube(0.06);
    glPopMatrix();
    
    // Left Arm
    glPushMatrix();
    glTranslatef(-0.03, 0.005 + legSize * 3 + bodySize + headSize / 2, headSize - 0.07);
    glScalef(1.8f, 0.15f, 0.07f);
    glutSolidCube(0.06);
    glPopMatrix();
    
    // Right Arm
    glPushMatrix();
    glTranslatef(0.03, 0.005 + legSize * 3 + bodySize + headSize / 2, headSize - 0.07);
    glScalef(1.8f, 0.15f, 0.07f);
    glutSolidCube(0.06);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Body
    
    glColor3f(0.1647f, 0.2509f, 0.1412f);
    glPushMatrix();
    glTranslatef(0, legSize * 3 + bodySize / 2, 0);
    glutSolidCube(bodySize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Buttons
    
    for (float i = -0.2; i <= 0.3; i += 0.1) {
        glColor3f(1, 1, 1);
        GLUquadric* quad6 = gluNewQuadric();
        glPushMatrix();
        glTranslatef(0, i + legSize * 3 + bodySize / 2, bodySize - 0.25);
        glRotatef(90, 0.0f, 0.0f, 1.0f);
        glScalef(1, 1, 1);
        gluCylinder(quad6, 0.009, 0.009, 0.005, 50, 50);
        glTranslatef(0.0f, 0.0f, 0.005);
        gluDisk(quad6, 0.0, 0.009, 50, 1);
        glTranslatef(0.0f, 0.0f, -0.005);
        gluDisk(quad5, 0.0, 0.009, 50, 1);
        glPopMatrix();
        glColor3f(1, 1, 1);
    }
    
    // Limbs
    
    // Left Leg
    
    glColor3f(0.1843f, 0.1686f, 0.1725f);
    glPushMatrix();
    glTranslatef(-bodySize / 4, 0.5, 0);
    if (playerY == 0) {
        glRotatef(legRotationLeft, 1, 0, 0);
    }
    glScalef(1.0f, 5.5f, 1.0f);
    glutSolidCube(legSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Right Leg
    
    glColor3f(0.1843f, 0.1686f, 0.1725f);
    glPushMatrix();
    glTranslatef(bodySize / 4, 0.5, 0);
    if (playerY == 0){
        glRotatef(legRotationRight, 1, 0, 0);
    }
    glScalef(1.0f, 5.5f, 1.0f);
    glutSolidCube(legSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Left Arm Top
    
    glColor3f(0.1647f, 0.2509f, 0.1412f);
    glPushMatrix();
    glTranslatef(-bodySize / 1.5, legSize * 3 + bodySize - (legSize * 3) / 2, 0);
    glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0, 0.1, 0);
    glutSolidCube(legSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Left Arm Bottom
    
    glColor3f(0.2157f, 0.1804f, 0.0745f);
    glPushMatrix();
    glTranslatef(-bodySize / 1.5, legSize * 3 + bodySize - (legSize * 3) / 2, 0);
    glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0, -0.1, 0);
    glutSolidCube(legSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Left Arm Middle
    
    glColor3f(1, 1, 1);
    glPushMatrix();
    glTranslatef(-bodySize / 1.5, legSize * 3 + bodySize - (legSize * 3) / 2, 0);
    glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0, 0, 0);
    glScalef(1.0f, 0.2f, 1.0f);
    glutSolidCube(legSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Right Arm Top
    
    glColor3f(0.1647f, 0.2509f, 0.1412f);
    glPushMatrix();
    glTranslatef(bodySize / 1.5, legSize * 3 + bodySize - (legSize * 3) / 2, 0);
    glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0, 0.1, 0);
    glutSolidCube(legSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Right Arm Bottom
    
    glColor3f(0.2157f, 0.1804f, 0.0745f);
    glPushMatrix();
    glTranslatef(bodySize / 1.5, legSize * 3 + bodySize - (legSize * 3) / 2, 0);
    glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0, -0.1, 0);
    glutSolidCube(legSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Right Arm Middle
    
    glColor3f(1, 1, 1);
    glPushMatrix();
    glTranslatef(bodySize / 1.5, legSize * 3 + bodySize - (legSize * 3) / 2, 0);
    glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0, 0, 0);
    glScalef(1.0f, 0.2f, 1.0f);
    glutSolidCube(legSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    if (jetpack) {
        drawJetpack();
    }
    
    glPopMatrix();
    glColor3f(1, 1, 1);
}

void drawBurger(int mode) {
    float bunScaleFactor = 0.15f;
    float pattySize = 1.0f;
    float cheeseSize = 1.8f;
    
    float rotationFloat = rotation;
    
    glPushMatrix();
    glTranslatef(-0.85, upDown, -3.5);
    glRotatef(rotationFloat, 0, 1, 0);
    glScalef(bunScaleFactor, bunScaleFactor, bunScaleFactor);
    
    // Bottom Bun
    
    glColor3f(0.4745f, 0.3216f, 0.0322f);
    glPushMatrix();
    GLdouble eqn[4] = {0.0, -1.0, 0.0, 0.0};
    glClipPlane(GL_CLIP_PLANE0, eqn);
    glEnable(GL_CLIP_PLANE0);
    glutSolidSphere(1, 100, 100);
    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Patty
    
    glColor3f(0.1608f, 0.0556f, 0.0f);
    glPushMatrix();
    GLUquadric* quad = gluNewQuadric();
    glTranslatef(0, 0, 0);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad, pattySize, pattySize, 0.5, 50, 50);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Tomato
    
    glColor3f(0.5137f, 0.0392f, 0.0235f);
    glPushMatrix();
    GLUquadric* quad1 = gluNewQuadric();
    glTranslatef(0, 0.5, 0);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad1, pattySize, pattySize, 0.2, 50, 50);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Lettuce
    
    glColor3f(0.5647f, 0.9333f, 0.5647f);
    glPushMatrix();
    GLUquadric* quad2 = gluNewQuadric();
    glTranslatef(0, 0.7, 0);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad2, pattySize, pattySize, 0.2, 50, 50);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Cheese
    
    glColor3f(0.9647f, 0.5333f, 0.0824f);
    glPushMatrix();
    glTranslatef(0, 1, 0);
    glRotatef(45, 0, 1, 0);
    glScalef(1.0f, 0.1f, 1.0f);
    glutSolidCube(cheeseSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Top Bun
    
    glColor3f(0.4745f, 0.3216f, 0.0322f);
    glPushMatrix();
    glTranslatef(0, 0.6 + pattySize / 2, 0);
    GLdouble eqn1[4] = {0.0, 1.0, 0.0, 0.0};
    glClipPlane(GL_CLIP_PLANE0, eqn1);
    glEnable(GL_CLIP_PLANE0);
    glutSolidSphere(1, 100, 100);
    glDisable(GL_CLIP_PLANE0);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    glPopMatrix();
    glColor3f(1, 1, 1);
}

void drawLamps() {
    
    for (int i = 0; i < 2; i ++) {
        glPushMatrix();
        if (i == 0) {
            glTranslatef(-2, 0, -3);
            glScalef(1, lampLength1, 1);
        } else {
            glTranslatef(-2, 0, 5);
            glScalef(1, lampLength2, 1);
        }
        
        glColor3f(0.41f, 0.34f, 0.20f);
        glPushMatrix();
        glTranslatef(0, 1.25, 0);
        glScalef(0.18f, 2.4f, 0.18f);
        glutSolidCube(1);
        glPopMatrix();

        glColor3f(0.41f, 0.34f, 0.20f);
        glPushMatrix();
        glTranslatef(0, 2.4f, 0);
        glScalef(0.85f, 0.18f, 0.18f);
        glutSolidCube(1);
        glPopMatrix();
        
        glColor3f(0.41f, 0.34f, 0.20f);
        glPushMatrix();
        glTranslatef(-0.375, 2.25f, 0);
        glScalef(0.05f, 0.15f, 0.05f);
        glutSolidCube(1);
        glPopMatrix();
        
        glColor3f(0.41f, 0.34f, 0.20f);
        glPushMatrix();
        glTranslatef(0.375, 2.25f, 0);
        glScalef(0.05f, 0.15f, 0.05f);
        glutSolidCube(1);
        glPopMatrix();
        
        glColor3f(0.2078f, 0.2196f, 0.2549f);
        glPushMatrix();
        glTranslatef(0.375, 2.15f, 0);
        glutSolidSphere(0.1, 50, 50);
        glPopMatrix();
        
        glColor3f(0.2078f, 0.2196f, 0.2549f);
        glPushMatrix();
        glTranslatef(-0.375, 2.15f, 0);
        glutSolidSphere(0.1, 50, 50);
        glPopMatrix();
        
        glPopMatrix();
    }
    glColor3f(1, 1, 1);
}

void drawFerrisWheel() {
    float legsRadius = 0.05f;
    
    glPushMatrix();
    glTranslatef(0.2, 1, -2.8);
    glRotatef(60, 0, 1, 0);
    glScalef(-2, 2, 2);
    
    // Stand
    
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(-0.12, -0.22, 1.205);
    glScalef(0.5, 0.5, 2.5);
    glutSolidCube(1);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Legs
    
    glColor3f(1, 1, 1);
    glPushMatrix();
    GLUquadric* quad1 = gluNewQuadric();
    glTranslatef(0.05, 0.05, 0.6);
    glRotatef(-90, 1, 0, 0);
    glRotatef(15, 1, 0, 0);
    gluCylinder(quad1, legsRadius, legsRadius, 1.8, 50, 50);
    glPopMatrix();
    
    glPushMatrix();
    GLUquadric* quad2 = gluNewQuadric();
    glTranslatef(0.05, 0.05, 1.8);
    glRotatef(-90, 1, 0, 0);
    glRotatef(-15, 1, 0, 0);
    gluCylinder(quad2, legsRadius, legsRadius, 1.8, 50, 50);
    glPopMatrix();
    
    glPushMatrix();
    GLUquadric* quad4 = gluNewQuadric();
    glTranslatef(0, 1.5, 1.2);
    glRotatef(90, 0, 1, 0);
    glScalef(14, 14, 0.05);
    gluCylinder(quad4, legsRadius, legsRadius, 1.8, 50, 50);
    glPopMatrix();

    glPushMatrix();
    GLUquadric* quad5 = gluNewQuadric();
    glTranslatef(0, 1.5, 1.2);
    glRotatef(90, 0, 1, 0);
    glScalef(21, 21, 0.05);
    gluCylinder(quad5, legsRadius, legsRadius, 1.8, 50, 50);
    glPopMatrix();

    glPushMatrix();
    GLUquadric* quad6 = gluNewQuadric();
    glTranslatef(0, 1.5, 1.2);
    glRotatef(90, 0, 1, 0);
    glScalef(28, 28, 0.05);
    gluCylinder(quad6, legsRadius, legsRadius, 1.8, 50, 50);
    glPopMatrix();
    
    glPushMatrix();
    GLUquadric* quad7 = gluNewQuadric();
    glTranslatef(0.05, 0.05, 1.2);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad7, legsRadius, legsRadius, 1.8, 50, 50);
    glPopMatrix();
    
    // Spinning Part
    
    glPushMatrix();
    glTranslatef(0.35, 1.5, 1.2);
    glRotatef(rotateFerris, 1, 0, 0);
    glTranslatef(-0.35, -1.5, -1.2);
    
    // Center
    
    glPushMatrix();
    GLUquadric* quad3 = gluNewQuadric();
    glTranslatef(0, 1.5, 1.2);
    glRotatef(90, 0, 1, 0);
    glScalef(7, 7, 0.05);
    gluDisk(quad3, 0, legsRadius, 50, 50);
    gluCylinder(quad3, legsRadius, legsRadius, 8.2, 50, 50);
    glPushMatrix();
    glTranslatef(0, 0, 8.2);
    gluDisk(quad3, 0, legsRadius, 50, 50);
    glPopMatrix();
    glPopMatrix();
    
    // Arms
    
    glColor3f(0.2745f, 0.1255f, 0.1765f);
    glPushMatrix();
    GLUquadric* quad8 = gluNewQuadric();
    glTranslatef(0.35, 1.5, 0);
    gluCylinder(quad8, legsRadius, legsRadius, 2.4, 50, 50);
    glPopMatrix();
    
    glPushMatrix();
    GLUquadric* quad9 = gluNewQuadric();
    glTranslatef(0.35, 1.5 - 1.1, 1.2);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad9, legsRadius, legsRadius, 2.2, 50, 50);
    glPopMatrix();
    
    // Spheres
    
    glColor3f(0.4745f, 0.3216f, 0.0322f);
    glPushMatrix();
    glTranslatef(0.35, 1.6, -0.18);
    glScalef(0.2, 0.4, 0.2);
    glutSolidSphere(1, 100, 100);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.35, 1.6, 2.58);
    glScalef(0.2, 0.4, 0.2);
    glutSolidSphere(1, 100, 100);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.35, 3, 1.2);
    glScalef(0.2, 0.4, 0.2);
    glutSolidSphere(1, 100, 100);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0.35, 0, 1.2);
    glScalef(0.2, 0.4, 0.2);
    glutSolidSphere(1, 100, 100);
    glPopMatrix();
    
    glPopMatrix();
    
    glColor3f(1, 1, 1);
    glPopMatrix();
}

void drawCar() {
    float wheelRadius = 0.05;
    float wheelLength = 3.4;
    
    glPushMatrix();
    glTranslatef(4.2, 0, carMovement);
    glRotatef(-90, 0, 1, 0);
    glScalef(0.5, 0.5, 0.5);
    
    // Car wheel 1
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(0, wheelRadius * 9, -0.75);

    GLUquadric* quad1 = gluNewQuadric();
    glTranslatef(-0.8, 0, 1.2);
    glRotatef(0, 0, 1, 0);
    glScalef(7, 7, 0.1);
    gluDisk(quad1, 0, wheelRadius, 50, 50);
    gluCylinder(quad1, wheelRadius, wheelRadius, wheelLength, 50, 50);
    glPushMatrix();
    glTranslatef(0, 0, wheelLength);
    gluDisk(quad1, 0, wheelRadius, 50, 50);
    glPopMatrix();

    glColor3f(0.7529f, 0.7529f, 0.7529f);
    GLUquadric* smallDiskQuad1 = gluNewQuadric();
    glTranslatef(0, 0, wheelLength + 0.1);
    gluDisk(smallDiskQuad1, 0, wheelRadius * 0.5, 50, 50);

    glPopMatrix();

    // Car wheel 2
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(0.8, wheelRadius * 9, -0.75);

    // Wheel body
    GLUquadric* quad2 = gluNewQuadric();
    glTranslatef(0, 0, 1.2);
    glRotatef(0, 0, 1, 0);
    glScalef(7, 7, 0.1);
    gluDisk(quad2, 0, wheelRadius, 50, 50);
    gluCylinder(quad2, wheelRadius, wheelRadius, wheelLength, 50, 50);
    glPushMatrix();
    glTranslatef(0, 0, wheelLength);
    gluDisk(quad2, 0, wheelRadius, 50, 50);
    glPopMatrix();

    glColor3f(0.7529f, 0.7529f, 0.7529f);
    GLUquadric* smallDiskQuad2 = gluNewQuadric();
    glTranslatef(0, 0, wheelLength + 0.1);
    gluDisk(smallDiskQuad2, 0, wheelRadius * 0.5, 50, 50);

    glPopMatrix();

    // Car wheel 3
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(0, wheelRadius * 9, -0.5);
    glScalef(1, 1, -1);
    
    // Wheel body
    GLUquadric* quad3 = gluNewQuadric();
    glTranslatef(-0.8, 0, 0);
    glRotatef(0, 0, 1, 0);
    glScalef(7, 7, 0.1);
    gluDisk(quad3, 0, wheelRadius, 50, 50);
    gluCylinder(quad3, wheelRadius, wheelRadius, wheelLength, 50, 50);
    glPushMatrix();
    glTranslatef(0, 0, wheelLength);
    gluDisk(quad3, 0, wheelRadius, 50, 50);
    glPopMatrix();

    glColor3f(0.7529f, 0.7529f, 0.7529f);
    GLUquadric* smallDiskQuad3 = gluNewQuadric();
    glTranslatef(0, 0, wheelLength + 0.1);
    gluDisk(smallDiskQuad3, 0, wheelRadius * 0.5, 50, 50);

    glPopMatrix();

    // Car wheel 4
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(0.8, wheelRadius * 9, -1.25);
    glScalef(1, 1, -1);

    // Wheel body
    GLUquadric* quad4 = gluNewQuadric();
    glTranslatef(0, 0, -0.75);
    glRotatef(0, 0, 1, 0);
    glScalef(7, 7, 0.1);
    gluDisk(quad4, 0, wheelRadius, 50, 50);
    gluCylinder(quad4, wheelRadius, wheelRadius, wheelLength, 50, 50);
    glPushMatrix();
    glTranslatef(0, 0, wheelLength);
    gluDisk(quad4, 0, wheelRadius, 50, 50);
    glPopMatrix();

    glColor3f(0.7529f, 0.7529f, 0.7529f);
    GLUquadric* smallDiskQuad4 = gluNewQuadric();
    glTranslatef(0, 0, wheelLength + 0.1);
    gluDisk(smallDiskQuad4, 0, wheelRadius * 0.5, 50, 50);
    glPopMatrix();
    
    // Car Body
    
    glColor3f(0.7333f, 0.1608f, 0.2431f);
    glPushMatrix();
    glTranslatef(0, 1 + (wheelRadius * 6), 0);
    glScalef(2.8, 1, 1.8);
    glutSolidCube(1);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(0, 1 + (wheelRadius * 6) + 1, 0);
    glRotatef(90, 1, 0, 0);

    GLfloat vertices[] = {
        -0.625f, -0.5f, -0.9f, // 0
         0.625f, -0.5f, -0.9f, // 1
         0.625f,  0.5f, -0.9f, // 2
        -0.625f,  0.5f, -0.9f, // 3
        -1.25f,  -0.5f, 0.9f,  // 4
         1.25f,  -0.5f, 0.9f,  // 5
         1.25f,   0.5f, 0.9f,  // 6
        -1.25f,   0.5f, 0.9f   // 7
    };

    GLubyte indices[] = {
        0, 1, 2, 3, // front face
        4, 5, 6, 7, // back face
        0, 1, 5, 4, // bottom face
        2, 3, 7, 6, // top face
        0, 4, 7, 3, // left face
        1, 5, 6, 2  // right face
    };

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, indices);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPopMatrix();
    
    glColor3f(1, 1, 1);
    glPopMatrix();
}

void drawKiosk() {
    float baseLength = 0.1;
    float kioskLength1 = 0.15;
    float kioskLength2 = 0.6;
    float kioskLength3 = 0.8;
    
    glPushMatrix();
    glTranslatef(-2, 0, 2.8);
    glScalef(0.75, 1, 0.75);
    
    // Base
    
    glColor3f(0.2627f, 0.2431f, 0.2196f);
    glPushMatrix();
    glTranslatef(0, 0.05 + baseLength / 2, 0);
    glScalef(18, 1, 18);
    glutSolidCube(baseLength);
    glPopMatrix();
    
    // Lower Part
    
    glColor3f(0.2745f, 0.1255f, 0.1765f);
    glPushMatrix();
    glTranslatef(0, 0.05 + baseLength + kioskLength1 / 2, 0);
    glScalef(10, 1, 10);
    glutSolidCube(kioskLength1);
    glPopMatrix();
    
    // Kiosk Lower
    
    glColor3f(0.549f, 0.5294f, 0.502f);
    glPushMatrix();
    glTranslatef(0, 0.05 + baseLength + kioskLength1 + kioskLength2 / 2, 0);
    glScalef(2.5, 1, 2.5);
    glutSolidCube(kioskLength2);
    glPopMatrix();
    
    // Kiosk Middle
    
    glColor3f(0.2745f, 0.1255f, 0.1765f);
    glPushMatrix();
    glTranslatef(0, 0.05 + baseLength + kioskLength1 + kioskLength2 + kioskLength3 / 2, 0);
    glScalef(1.875, 1, 1.875);
    glutSolidCube(kioskLength3);
    glPopMatrix();
    
    // Kiosk Upper
    
    glColor3f(0.4275f, 0.2471f, 0.3216f);
    glPushMatrix();
    glTranslatef(0, 0.05 + baseLength + kioskLength1 + kioskLength2 + kioskLength3 + kioskLength1 / 2, 0);
    glScalef(12, 1, 12);
    glutSolidCube(kioskLength1);
    glPopMatrix();
    
    // Kiosk Glass
    
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(0.35, 0.05 + baseLength + kioskLength1 + kioskLength2 + kioskLength3 / 2, 0.75);
    glScalef(0.65, 0.65, 0.02);
    glutSolidCube(1);
    glPopMatrix();
    
    glColor3f(0, 0, 0);
    glPushMatrix();
    glTranslatef(-0.35, 0.05 + baseLength + kioskLength1 + kioskLength2 + kioskLength3 / 2, 0.75);
    glScalef(0.65, 0.65, 0.02);
    glutSolidCube(1);
    glPopMatrix();
    
    // Curtain
    
    glColor3f(0.5, 0.5, 0.5);
    glPushMatrix();
    glTranslatef(0.35, 0.05 + baseLength + kioskLength1 + kioskLength2 + kioskLength3 / 2, 0.77);
    glTranslatef(0, 0.35, 0);
    glRotatef(curtain1Rotation, 1, 0, 0);
    glTranslatef(0, -0.35, 0);
    glScalef(0.65, 0.65, 0.02);
    glutSolidCube(1);
    glPopMatrix();
    
    glColor3f(0.5, 0.5, 0.5);
    glPushMatrix();
    glTranslatef(-0.35, 0.05 + baseLength + kioskLength1 + kioskLength2 + kioskLength3 / 2, 0.77);
    glTranslatef(0, 0.35, 0);
    glRotatef(curtain2Rotation, 1, 0, 0);
    glTranslatef(0, -0.35, 0);
    glScalef(0.65, 0.65, 0.02);
    glutSolidCube(1);
    glPopMatrix();
    
    glColor3f(1, 1, 1);
    glPopMatrix();
}

void drawSeeSaw() {
    float seeSawLength = 1;
    
    glPushMatrix();
    glTranslatef(-2, 0, 0);
    glRotatef(90, 0, 1, 0);
    
    glPushMatrix();
    glTranslatef(0, 1, 0);
    glRotatef(seeSawRotation, 0, 0, 1);
    glTranslatef(0, -1, 0);
    
    // Main
    
    glColor3f(0.5922, 0.4, 0.2118);
    glPushMatrix();
    glTranslatef(0, 0.15 + 0.5, 0);
    glScalef(2.5, 0.2, 0.5);
    glutSolidCube(seeSawLength);
    glPopMatrix();
    
    // Arms
    
    glColor3f(0.5, 0.5, 0.5);
    glPushMatrix();
    glTranslatef(0.75, 0.15 + 0.5 + 0.2, 0);
    glScalef(0.1, 0.3, 0.5);
    glutSolidCube(seeSawLength);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-0.75, 0.15 + 0.5 + 0.2, 0);
    glScalef(0.1, 0.3, 0.5);
    glutSolidCube(seeSawLength);
    glPopMatrix();
    
    glPopMatrix();
    
    // Base
    
    glPushMatrix();
    GLUquadric* quad1 = gluNewQuadric();
    glTranslatef(0, 0, 0);
    glRotatef(-90, 1, 0, 0);
    gluCylinder(quad1, 0.12, 0.12, 0.6, 50, 50);
    glPopMatrix();
    
    glColor3f(1, 1, 1);
    glPopMatrix();
}

void drawFence() {
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glScalef(1, 0.5, 1);
    
    glColor3f(0.8, 0.6627, 0.3961);
    
    for (int i = 0; i <= 1; i++) {
        glPushMatrix();
        if (i == 1) {
            glTranslatef(0, 0.5, 0);
        }
        
        // Fence Right 1
        
        glPushMatrix();
        glTranslatef(3, 0.175, 5.5 - 2.25);
        glScalef(0.2, 0.25, 5.5);
        glutSolidCube(1);
        glPopMatrix();
        
        // Fence Up
        
        glPushMatrix();
        glTranslatef(1.275, 0.175, 1 - 0.7);
        glScalef(3.65, 0.25, 0.4);
        glutSolidCube(1);
        glPopMatrix();
        
        // Fence Right 2
        
        glPushMatrix();
        glTranslatef(-0.5, 0.175, 0.5 - 2.25);
        glScalef(0.2, 0.25, 4.5);
        glutSolidCube(1);
        glPopMatrix();
        
        // Fence Left
        
        glPushMatrix();
        glTranslatef(-1.2, 0.175, 1);
        glScalef(0.2, 0.25, 10);
        glutSolidCube(1);
        glPopMatrix();
        
        glPopMatrix();
    }
    
    glColor3f(1, 1, 1);
    glPopMatrix();
}

void setupLights() {
    GLfloat ambientMaterial[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    GLfloat diffuseMaterial[] = { 0.6f, 0.6f, 0.6f, 1.0f };
    GLfloat specularMaterial[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat shininess[] = { 50 };
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientMaterial);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseMaterial);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularMaterial);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat lightPosition[] = { -7.0f, 6.0f, 3.0f, 0.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
}

void setupCamera() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 640 / 480, 0.001, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.look();
}

bool checkRocketMan() {
    bool containsSequence = false;
       for (size_t i = 0; i <= keys.size() - targetSequence.size(); ++i) {
           bool isMatch = true;
           for (size_t j = 0; j < targetSequence.size(); ++j) {
               if (keys[i + j] != targetSequence[j]) {
                   isMatch = false;
                   break;
               }
           }
           if (isMatch) {
               containsSequence = true;
               break;
           }
       }

    return containsSequence;
}

void displayCheatStatus() {
    std::string text;
    if (displayCheatActivated) {
        text = "Cheated Activated!";
    } else {
        text = "Cheated Deactivated!";
    }

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1600, 0, 1000);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    int x = 40;
    int y = 950;
    
    print(x, y, const_cast<char*>(text.c_str()), 18);
    
    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glVertex2f(x - 5, y + 25);
    glVertex2f(x + 200, y + 25);
    glVertex2f(x + 200, y - 15);
    glVertex2f(x - 5, y - 15);
    glEnd();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Display() {
    if (firstLoopAfterEnd && timeRemaining <= 0) {
        firstLoopAfterEnd = false;
        Mix_HaltMusic();
        if (burgers == 0) {
            playSound(3, 1);
        } else {
            playSound(3, 2);
        }
    }
    
    setupCamera();
    setupLights();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (timeRemaining <= 0) {
        gameEnd(burgers == 0);
    } else {
        displayTime();
        drawBoundaries();
        drawBigSmoke();
        if (!bigSmokeAteTheBurger) {
            drawBurger(0);
        }
        drawLamps();
        drawFerrisWheel();
        drawCar();
        drawKiosk();
        drawSeeSaw();
        drawFence();
    }
    if (displayCheatActivated || displayCheatDeactivated) {
        displayCheatStatus();
    }
    glFlush();
}

void timer(int value) {
    if (timeRemaining > 0) {
        timeRemaining--;
        displayedTime = secondsToMinutesAndSeconds(timeRemaining);
    }
    
    if (timeRemaining % 3 == 0) {
        float min = 0;
        float max = 1;
        std::uniform_real_distribution<float> distribution(min, max);
        r = distribution(generator);
        g = distribution(generator);
        b = distribution(generator);
    }
    
    if (timeRemaining > 0) {
        glutTimerFunc(1000, timer, 0);
    }
    
    if (jetpack) {
        displayCheatDeactivated = false;
        if (cheatTime1 - timeRemaining >= 5) {
            displayCheatActivated = false;
        }
    }
    if (!jetpack) {
        displayCheatActivated = false;
        if (cheatTime2 - timeRemaining >= 5) {
            displayCheatDeactivated = false;
        }
    }
    
    glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
    float flyingStep = 0.1f;
    float d = 0.5;
    float a = 1.0;
    
    if (key == 9) {
        cheatCode = !cheatCode;
        std::cout << cheatCode << std::endl;
    }
    if (!cheatCode) {
        switch (key) {
        case 'w':
            camera.moveY(d);
            break;
        case 's':
            camera.moveY(-d);
            break;
        case 'a':
            camera.moveX(d);
            break;
        case 'd':
            camera.moveX(-d);
            break;
        case 'q':
            camera.moveZ(d);
            break;
        case 'e':
            camera.moveZ(-d);
            break;
        case 'm':
            playSound(5, 5);
            if (!topView && !sideView) {
                camera = Camera(0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
                topView = true;
            } else if (topView) {
                camera = Camera(10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
                topView = false;
                sideView = true;
            } else if (sideView) {
                camera = Camera(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
                sideView = false;
            }
            break;
        case 'v':
            playSound(5, 5);
            camera = Camera(0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);
            break;
        case 'b':
            playSound(5, 5);
            camera = Camera(10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
            break;
        case 'n':
            playSound(5, 5);
            camera = Camera(0.0f, 0.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
            break;
        case 'i':
            camera.rotateX(a);
            break;
        case 'k':
            camera.rotateX(-a);
            break;
        case 'j':
            camera.rotateY(a);
            break;
        case 'l':
            camera.rotateY(-a);
            break;
        case 'o':
            if (!objectsAnimate) {
                objectsAnimate = true;
                playSound(4, 3);
            }
            break;
        case 'p':
            objectsAnimate = false;
            Mix_HaltChannel(4);
            break;
        case ' ':
            if (jetpack) {
                playerY += flyingStep;
            }
            break;
        case 'z':
            if (jetpack) {
                if (playerY - flyingStep > 0) {
                    playerY -= flyingStep;
                } else {
                    playerY = 0;
                }
            }
            break;
        case 13:
            jetpack = false;
            falling = true;
            Mix_HaltChannel(7);
            displayCheatDeactivated = true;
            cheatTime2 = timeRemaining;
            break;
        }
    } else {
        keys.push_back(key);
        if (keys.size() >= 9) {
            if (checkRocketMan()) {
                keys.clear();
                jetpack = true;
                cheatCode = false;
                displayCheatActivated = true;
                cheatTime1 = timeRemaining;
                playSound(6, 5);
                playSound(7, 7);
            }
        }
    }
    
    if (key == 27) {
        exit(EXIT_SUCCESS);
    }
    
    playerCollide();
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    float step = 0.5f;
    if (jetpack) {
        step = 1;
    }
    const float playerWidth = 0.25;

    float leftBoundary = -1.25 + playerWidth;
    float rightBoundary = 3 - playerWidth;
    float startBoundary = 5.5f;
    float endBoundary = -4.0f + playerWidth;

    switch (key) {
        case GLUT_KEY_LEFT:
            playerRotation = -90.0f;
            if (lastKey == 'u' && abs(playerZ - endBoundary) <= 0.167) {
                playerZ = endBoundary + 0.167;
            }
            lastKey = 'l';
            if (playerX - step >= leftBoundary) {
                playerX -= step;
                legRotation = true;
            } else {
                playerX = leftBoundary;
            }
            break;
        case GLUT_KEY_RIGHT:
            playerRotation = 90.0f;
            legRotation = true;
            if (lastKey == 'u' && abs(playerZ - endBoundary) <= 0.167) {
                playerZ = endBoundary + 0.167;
            }
            lastKey = 'r';
            if (playerX + step > -0.75 - playerWidth && playerZ - playerWidth < 0.5) {
                playerX = -0.75 - playerWidth;
            } else {
                if (playerX + step <= rightBoundary) {
                    playerX += step;
                    legRotation = true;
                } else {
                    playerX = rightBoundary;
                }
            }
            break;
        case GLUT_KEY_UP:
            playerRotation = 180.0f;
            legRotation = true;
            if (lastKey == 'r' && abs(playerX - rightBoundary) <= 0.167) {
                playerX = rightBoundary - 0.167;
            }
            if (lastKey == 'l' && abs(playerX - leftBoundary) <= 0.167) {
                playerX = leftBoundary + 0.167;
            }
            lastKey = 'u';
            if (playerZ - step < 0.5 + playerWidth && playerX >= -0.5) {
                playerZ = 0.5 + playerWidth;
            } else {
                if (playerZ - step >= endBoundary) {
                    playerZ -= step;
                    legRotation = true;
                } else {
                    playerZ = endBoundary;
                }
            }
            break;
        case GLUT_KEY_DOWN:
            playerRotation = 0.0f;
            legRotation = true;
            if (lastKey == 'r' && abs(playerX - rightBoundary) <= 0.167) {
                playerX = rightBoundary - 0.167;
            }
            if (lastKey == 'l' && abs(playerX - leftBoundary) <= 0.167) {
                playerX = leftBoundary + 0.167;
            }
            lastKey = 'd';
            if (playerZ + step <= startBoundary) {
                playerZ += step;
                legRotation = true;
            } else {
                playerZ = startBoundary;
            }
            break;
    }
    
    playerCollide();
    glutPostRedisplay();
}

void specialKeyReleased(int key, int x, int y) {
    if (key == GLUT_KEY_UP || key == GLUT_KEY_DOWN || key == GLUT_KEY_LEFT || key == GLUT_KEY_RIGHT) {
        legRotation = false;
        legRotationLeft = 0;
        legRotationRight = 0;
    }
}

void anim() {
    rotation += 10;
    
    fireAnimation += fireDirection * 0.04;
    if (fireAnimation >= 1.2 || fireAnimation <= 0.4) {
        fireDirection *= -1;
    }
    
    if (objectsAnimate) {
        rotateFerris += 10;
        
        carMovement += carDirection * 0.1;
        if (carMovement >= 5 || carMovement <= 2) {
            carDirection *= -1;
        }
        
        lampLength1 += lampLengthDirection1 * 0.04;
        if (lampLength1 >= 1.6 || lampLength1 <= 1) {
            lampLengthDirection1 *= -1;
            lamp1ReachedPeak = true;
        }
        
        if (lamp1ReachedPeak) {
            lampLength2 += lampLengthDirection2 * 0.04;
            if (lampLength2 >= 1.6 || lampLength2 <= 1) {
                lampLengthDirection2 *= -1;
            }
        }
        
        curtain1Rotation += curtain1RotationDirection * 10;
            if (curtain1Rotation >= 0 || curtain1Rotation <= -90) {
                curtain1RotationDirection *= -1;
                openedCurtain1 = true;
            }
            
            if (openedCurtain1) {
                curtain2Rotation += curtain2RotationDirection * 10;
                if (curtain2Rotation >= 0 || curtain2Rotation <= -90) {
                    curtain2RotationDirection *= -1;
                }
            }
        
        seeSawRotation += seeSawDirection * 2;
        if (seeSawRotation >= 20 || seeSawRotation <= -20) {
            seeSawDirection *= -1;
        }
    }
    
    counter ++;
    
    upDown += increment;
    
    if (upDown <= 0.4 || upDown >= 0.6) {
        increment *= -1;
    }
    
    if (legRotationLeft <= -20 || legRotationLeft >= 20) {
        legRotationIncrement *= -1;
    }
    
    if (legRotation && counter % 5 == 0) {
        legRotationLeft += legRotationIncrement;
        legRotationRight -= legRotationIncrement;
    }
    
    if (legRotation && runningStarted) {
        if (playerY == 0) {
            playSound(2, 6);
        }
        runningStarted = false;
    }
    
    if (!legRotation) {
        Mix_HaltChannel(2);
        runningStarted = true;
    }
    
    if (falling) {
        if (playerY <= 0) {
            falling = false;
            playerY = 0;
        } else {
            playerY -= 0.1;
        }
    }
    
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitWindowSize(1600, 1000);
    glutInitWindowPosition(50, 50);
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Initialization error" << std::endl;
        return 1;
    }
        
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL Mixer Initialization error" << std::endl;
        return 1;
    }

    std::vector<std::string> audioFiles = {
        "/assets/background.wav", // done
        "/assets/win.wav", // done
        "/assets/lose.wav", // done
        "/assets/animations.wav", // done
        "/assets/goal.wav", // done
        "/assets/menu.wav", // done
        "/assets/running.wav", // done
        "/assets/jetpack.wav", // done
    };
    
    for (int i = 0; i < audioFiles.size(); i++) {
        std::string path = __FILE__ + audioFiles.at(i);
        std::string substring = "P18-52-0792.cpp/";
        size_t found = path.find(substring);
        if (found != std::string::npos) {
            path.erase(found, substring.length());
        }
        audioFilesNames.push_back(path);
    }
    
    Mix_Music* audio = Mix_LoadMUS(audioFilesNames.at(0).c_str());
    if (!audio) {
        std::cerr << "Loading music failed" << std::endl;
    }
    
    Mix_VolumeMusic(5);
    if (Mix_PlayMusic(audio, -1) == -1) {
        std::cerr << "Playing music failed" << std::endl;
    }

    glutCreateWindow("Grand Theft Auto: Who Ate My Burger?");
    glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeyReleased);
    glutIdleFunc(anim);
    glutTimerFunc(1000, timer, 0);

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);

    glShadeModel(GL_SMOOTH);
    glutMainLoop();
    
    Mix_CloseAudio();
    SDL_Quit();
    
    return 0;
}
