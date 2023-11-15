#define GL_SILENCE_DEPRECATION
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include <cmath>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)

float playerX = 1.0f;
float playerZ = 5.5f;
float playerRotation = 180.0f;
bool topView = false;
bool sideView = false;
char lastKey;
int rotation = 0.0f;
float upDown = 0.5f;
float increment = 0.01f;

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

void drawGenericBoundary(char rotation) {
    float thickness = 0.05f;
    float width = 2.0f;
    float length = 10.0f;
    glPushMatrix();
    switch (rotation) {
    case 'b':
        glTranslatef(0.5f * width, 0.5f * thickness, 0.5f * width);
        glScalef(width * 4, thickness, length);
        glColor3f(1.0f, 0.0f, 0.0f);
        break;
    case 'r':
        glTranslatef(width + 1.5 * width + 0.025, 1, 0.5f * width);
        glRotatef(90, 0, 0, 1.0f);
        glScalef(width, thickness, length);
        break;
    case 'l':
        glTranslatef(-width - 0.5f * width - 0.025, 1, 0.5f * width);
        glRotatef(90, 0, 0, 1.0f);
        glScalef(width, thickness, length);
        break;
    case 'e':
        glTranslatef(1, 1, -4);
        glRotatef(90, 0, 0, 1);
        glRotatef(90, 1, 0, 0);
        glScalef(width, thickness, length-1.90);
        break;
    default:
        break;
    }
    
    glutSolidCube(1);
    glPopMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
}


void drawBigSmoke() {
    float bodySize = 0.5;
    float headSize = 0.15;
    float hatSize = 0.155;
    float legSize = bodySize / 3;
    glPushMatrix();
    glTranslatef(playerX, 0, playerZ);
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
    
    // Body
    
    glColor3f(0.1647f, 0.2509f, 0.1412f);
    glPushMatrix();
    glTranslatef(0, legSize * 3 + bodySize / 2, 0);
    glutSolidCube(bodySize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Limbs
    
    // Left Leg
    
    glColor3f(0.1843f, 0.1686f, 0.1725f);
    glPushMatrix();
    glTranslatef(-bodySize / 4, 0.25, 0);
    glScalef(1.0f, 3.0f, 1.0f);
    glutSolidCube(legSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Right Leg
    
    glColor3f(0.1843f, 0.1686f, 0.1725f);
    glPushMatrix();
    glTranslatef(bodySize / 4, 0.25, 0);
    glScalef(1.0f, 3.0f, 1.0f);
    glutSolidCube(legSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Left Arm
    
    glColor3f(0.1647f, 0.2509f, 0.1412f);
    glPushMatrix();
    glTranslatef(-bodySize / 1.5, legSize * 3 + bodySize - (legSize * 3) / 2, 0);
    glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
    glScalef(1.0f, 3.0f, 1.0f);
    glutSolidCube(legSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    // Right Arm
    
    glColor3f(0.1647f, 0.2509f, 0.1412f);
    glPushMatrix();
    glTranslatef(bodySize / 1.5, legSize * 3 + bodySize - (legSize * 3) / 2, 0);
    glRotatef(-45.0f, 1.0f, 0.0f, 0.0f);
    glScalef(1.0f, 3.0f, 1.0f);
    glutSolidCube(legSize);
    glPopMatrix();
    glColor3f(1, 1, 1);
    
    
    glPopMatrix();
    glColor3f(1, 1, 1);
}

void drawBurger() {
    float bunScaleFactor = 0.15f;
    float pattySize = 1.0f;
    float cheeseSize = 1.8f;
    
    float rotationFloat = rotation;
    
    glPushMatrix();
    glTranslatef(1, upDown, -3.5); // testing
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

void Display() {
    setupCamera();
    setupLights();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    drawGenericBoundary('b');
    drawGenericBoundary('l');
    drawGenericBoundary('r');
    drawGenericBoundary('e');
    drawBigSmoke();
    
    drawBurger();

    glFlush();
}

void Keyboard(unsigned char key, int x, int y) {
    float d = 0.5;
    float a = 1.0;
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
    case 27:
        exit(EXIT_SUCCESS);
    }

    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    const float step = 0.5f;
    const float playerWidth = 0.25;

    float leftBoundary = -3.025f + playerWidth;
    float rightBoundary = 5.025f - playerWidth;
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
            } else {
                playerX = leftBoundary;
            }
            break;
        case GLUT_KEY_RIGHT:
            playerRotation = 90.0f;
            if (lastKey == 'u' && abs(playerZ - endBoundary) <= 0.167) {
                playerZ = endBoundary + 0.167;
            }
            lastKey = 'r';
            if (playerX + step <= rightBoundary) {
                playerX += step;
            } else {
                playerX = rightBoundary;
            }
            break;
        case GLUT_KEY_UP:
            playerRotation = 180.0f;
            if (lastKey == 'r' && abs(playerX - rightBoundary) <= 0.167) {
                playerX = rightBoundary - 0.167;
            }
            if (lastKey == 'l' && abs(playerX - leftBoundary) <= 0.167) {
                playerX = leftBoundary + 0.167;
            }
            lastKey = 'u';
            if (playerZ - step >= endBoundary) {
                playerZ -= step;
            } else {
                playerZ = endBoundary;
            }
            break;
        case GLUT_KEY_DOWN:
            playerRotation = 0.0f;
            if (lastKey == 'r' && abs(playerX - rightBoundary) <= 0.167) {
                playerX = rightBoundary - 0.167;
            }
            if (lastKey == 'l' && abs(playerX - leftBoundary) <= 0.167) {
                playerX = leftBoundary + 0.167;
            }
            lastKey = 'd';
            if (playerZ + step <= startBoundary) {
                playerZ += step;
            } else {
                playerZ = startBoundary;
            }
            break;
    }
    
    glutPostRedisplay();
}

void anim() {
    rotation += 10;
    
    upDown += increment;
    
    if (upDown <= 0.4 || upDown >= 0.6) {
        increment *= -1;
    }
    
    glutPostRedisplay();
}


int main(int argc, char** argv) {
    glutInit(&argc, argv);

    glutInitWindowSize(1600, 1000);
    glutInitWindowPosition(50, 50);

    glutCreateWindow("Grand Theft Auto: Who Ate My Burger?");
    glutDisplayFunc(Display);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(specialKeys);
    glutIdleFunc(anim);

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
    
    return 0;
}
