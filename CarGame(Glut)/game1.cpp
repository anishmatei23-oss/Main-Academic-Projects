#include <GL/glut.h>
#include <ctime>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --- GLOBALS AND CONSTANTS ---

float carX = 0.0f;
const float roadWidth = 2.0f;

const int NUM_CUBES = 5;
const int NUM_BIRDS = 5;
const int NUM_TREES = 50;
const int NUM_STARS = 100;
const int NUM_PARTICLES = 50;

int score = 0;
bool isGameOver = false;

float gameSpeed = 5.0f;

bool isDay = true;
float dayNightTimer = 0.0f;
const float DAY_NIGHT_INTERVAL = 30.0f;

int prevTime = 0;
float roadOffset = 0.0f;

bool isBlasting = false;
float blastTimer = 0.0f;
const float BLAST_DURATION = 1.0f;

// --- STRUCTURES ---

struct Particle {
    float x, y, z;
    float velocityX, velocityY, velocityZ;
    float initialSize;
    float currentSize;
};

struct Obstacle {
    float x, z;
    float width, height, length;
};

struct Bird {
    float x, y, z;
    float wingAngle;
    float phase;
};

struct Tree {
    float x, z;
    char name[20];
};

struct Star {
    float x, y, z;
};

// --- DATA ARRAYS ---

Particle blastParticles[NUM_PARTICLES];
float blastOriginX, blastOriginY, blastOriginZ;
Obstacle obstacles[NUM_CUBES];
Bird birds[NUM_BIRDS];
Tree trees[NUM_TREES];
Star stars[NUM_STARS];

const char* treeNames[] = {"APPLE", "MANGO", "GUAVA", "OAK", "PINE"};
const int numTreeTypes = sizeof(treeNames) / sizeof(treeNames[0]);


// --- INITIALIZATION FUNCTIONS ---

void initObstacles() {
    for (int i = 0; i < NUM_CUBES; ++i) {
        obstacles[i].x = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * roadWidth;
        obstacles[i].z = -(float)(i * 30 + 20) - (rand() % 20);
        obstacles[i].width = 0.4f, obstacles[i].height = 0.3f, obstacles[i].length = 0.4f;
    }
}

void initBirds() {
    for (int i = 0; i < NUM_BIRDS; ++i) {
        // Birds still spawn over the scenery, but we'll adjust movement in update
        birds[i].x = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * 5.0f;
        birds[i].y = ((float)rand() / RAND_MAX * 0.3f) + 1.5f;
        birds[i].z = -(float)(i * 40 + 50) - (rand() % 30);
        birds[i].phase = ((float)rand() / RAND_MAX) * 2 * M_PI;
    }
}

void initTrees() {
    for (int i = 0; i < NUM_TREES; ++i) {
        // Trees are now exclusively placed on the right side (where the grass is)
        // Minimum x for a tree is roadWidth + buffer (0.5f)
        trees[i].x = roadWidth + 0.5f + ((float)rand() / RAND_MAX * 1.5f);
        trees[i].z = -(float)(i * 5 + rand() % 5);
        strcpy(trees[i].name, treeNames[rand() % numTreeTypes]);
    }
}

void initStars() {
    for(int i = 0; i < NUM_STARS; ++i) {
        stars[i].x = ((float)rand() / RAND_MAX - 0.5f) * 100.0f;
        stars[i].y = ((float)rand() / RAND_MAX - 0.5f) * 100.0f;
        stars[i].z = ((float)rand() / RAND_MAX - 0.5f) * -100.0f;
    }
}

void initBlastParticles() {
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        blastParticles[i].x = 0.0f;
        blastParticles[i].y = 0.0f;
        blastParticles[i].z = 0.0f;

        blastParticles[i].velocityX = ((float)rand() / RAND_MAX - 0.5f) * 5.0f;
        blastParticles[i].velocityY = ((float)rand() / RAND_MAX) * 3.0f;
        blastParticles[i].velocityZ = ((float)rand() / RAND_MAX - 0.5f) * 5.0f;

        blastParticles[i].initialSize = ((float)rand() / RAND_MAX) * 0.1f + 0.05f;
        blastParticles[i].currentSize = blastParticles[i].initialSize;
    }
}

void init() {
    glClearColor(0.5f, 0.8f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);

    srand((unsigned int)time(0));

    initObstacles();
    initBirds();
    initTrees();
    initStars();
    initBlastParticles();

    prevTime = glutGet(GLUT_ELAPSED_TIME);
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float aspect = (float)w / (float)h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect, 1.0, 200.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// --- DRAWING HELPER FUNCTIONS ---

void drawWheel(float radius, float width) {
    GLUquadric* quad = gluNewQuadric();
    glPushMatrix();
    glRotatef(90, 0, 1, 0);

    // Tire (using torus for better shape)
    glColor3f(0.1f, 0.1f, 0.1f);
    glutSolidTorus(0.05, radius, 10, 20);

    // Hub and Spokes (simplified)
    glColor3f(0.5f, 0.5f, 0.5f);
    glScalef(0.01f, radius, radius);
    glutSolidCube(1.0f);

    glPopMatrix();
    gluDeleteQuadric(quad);
}

void drawTreeName(const char* name) {
    glDisable(GL_LIGHTING);
    glPushMatrix();

    // Color based on tree type
    if (strcmp(name, "APPLE") == 0) glColor3f(1.0f, 0.0f, 0.0f);
    else if (strcmp(name, "MANGO") == 0) glColor3f(1.0f, 0.8f, 0.0f);
    else if (strcmp(name, "GUAVA") == 0) glColor3f(0.0f, 1.0f, 0.0f);
    else if (strcmp(name, "OAK") == 0) glColor3f(0.7f, 0.7f, 0.7f);
    else glColor3f(0.5f, 0.5f, 0.5f);

    // Move text upward
    glTranslatef(0.0f, 1.5f, 0.0f);

    // --- BILLBOARDING: Make text face camera ---
    GLfloat m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);

    // Zero out rotation while keeping translation
    float billboardMatrix[16] = {
        m[0], 0,   0,   0,
        0,   m[5],0,   0,
        0,   0,   m[10],0,
        0,   0,   0,   1
    };
    glMultMatrixf(billboardMatrix);
    // --------------------------------------------

    // Scale text
    glScalef(0.003f, 0.003f, 0.003f);

    // Render characters
    for (int i = 0; name[i] != '\0'; i++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, name[i]);
    }

    glPopMatrix();
    glEnable(GL_LIGHTING);
}


// --- GAME OBJECT DRAWING FUNCTIONS ---

void drawCarImproved() {
    float bikeWidth = 0.2f, bikeHeight = 0.6f, bikeLength = 1.2f;
    float wheelRadius = 0.25f, wheelWidth = 0.1f;

    float bankAngle = -carX * 15.0f;

    glPushMatrix();
    glTranslatef(carX, wheelRadius, 0.0f);
    glRotatef(bankAngle, 0, 0, 1);

    // --- BIKE COMPONENTS ---
    glColor3f(0.4f, 0.4f, 0.4f);
    glPushMatrix();
    glTranslatef(0.0f, -bikeHeight * 0.05f, 0.0f);
    glScalef(bikeWidth * 1.2f, bikeHeight * 0.3f, bikeLength * 0.3f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.8f, 0.2f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, bikeHeight * 0.2f, 0.05f);
    glScalef(bikeWidth * 1.5f, bikeHeight * 0.2f, bikeLength * 0.5f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(0.0f, bikeHeight * 0.3f, -bikeLength * 0.2f);
    glScalef(bikeWidth * 1.2f, bikeHeight * 0.05f, bikeLength * 0.35f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glTranslatef(0.0f, -bikeHeight * 0.1f, -bikeLength * 0.3f);
    glRotatef(90, 0, 1, 0);
    glScalef(0.05f, 0.05f, bikeLength * 0.5f);
    glutSolidCone(0.5, 1.0, 10, 2);
    glPopMatrix();

    glColor3f(0.4f, 0.4f, 0.4f);
    glPushMatrix();
    glTranslatef(0.0f, bikeHeight * 0.45f, bikeLength * 0.35f);
    glScalef(bikeWidth * 2.0f, 0.03f, 0.03f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -wheelRadius, bikeLength * 0.4f);
    drawWheel(wheelRadius, wheelWidth);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, -wheelRadius, -bikeLength * 0.4f);
    drawWheel(wheelRadius, wheelWidth);
    glPopMatrix();

    if (!isDay) {
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 0.8f);
        glPushMatrix();
        glTranslatef(0.0f, bikeHeight * 0.2f, bikeLength * 0.55f);
        glutSolidSphere(0.08f, 10, 10);
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }

    // --- RIDER COMPONENTS (Tilted with the bike) ---
    float riderHeight = 0.8f;
    float riderShoulderWidth = bikeWidth * 2.0f;
    float leanAngle = -bankAngle * 0.5f;

    glColor3f(0.0f, 0.0f, 0.5f);
    glPushMatrix();
    glTranslatef(0.0f, bikeHeight * 0.3f + riderHeight * 0.25f, -bikeLength * 0.15f);
    glRotatef(leanAngle, 0, 0, 1);
    glScalef(bikeWidth * 1.5f, riderHeight * 0.4f, bikeLength * 0.25f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.8f, 0.6f, 0.4f);
    glPushMatrix();
    glTranslatef(0.0f, bikeHeight * 0.3f + riderHeight * 0.55f, -bikeLength * 0.15f);
    glRotatef(leanAngle, 0, 0, 1);
    glutSolidSphere(0.12f, 20, 20);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.5f);
    float armAngle = 45.0f;

    glPushMatrix();
    glTranslatef(-riderShoulderWidth * 0.5f, bikeHeight * 0.3f + riderHeight * 0.4f, -bikeLength * 0.1f);
    glRotatef(leanAngle, 0, 0, 1);
    glRotatef(armAngle, 0, 0, 1);
    glScalef(0.1f, riderHeight * 0.15f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(riderShoulderWidth * 0.5f, bikeHeight * 0.3f + riderHeight * 0.4f, -bikeLength * 0.1f);
    glRotatef(leanAngle, 0, 0, 1);
    glRotatef(-armAngle, 0, 0, 1);
    glScalef(0.1f, riderHeight * 0.15f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(0.0f, bikeHeight * 0.25f, -bikeLength * 0.05f);
    glRotatef(leanAngle, 0, 0, 1);
    glScalef(bikeWidth * 1.5f, riderHeight * 0.2f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();


    glPopMatrix();
}

void drawCubeObstacle(const Obstacle* obs) {
    glPushMatrix();
    glTranslatef(obs->x, obs->height / 2, obs->z);

    glColor3f(0.8f, 0.1f, 0.1f);
    glScalef(obs->width, obs->height, obs->length);
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawBird(Bird* bird) {
    glPushMatrix();
    glTranslatef(bird->x, bird->y, bird->z);

    bird->wingAngle = sin(bird->phase * 10.0f) * 30.0f;

    glColor3f(0.5f, 0.5f, 0.5f);
    glPushMatrix();
    glScalef(0.1f, 0.05f, 0.15f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.05f, -0.07f);
    glutSolidSphere(0.03, 10, 10);
    glPopMatrix();

    glColor3f(0.4f, 0.4f, 0.4f);

    glPushMatrix();
    glTranslatef(-0.05f, 0.0f, 0.0f);
    glRotatef(bird->wingAngle, 0, 0, 1);
    glRotatef(-90, 0, 1, 0);
    glScalef(0.1f, 0.01f, 0.05f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.05f, 0.0f, 0.0f);
    glRotatef(-bird->wingAngle, 0, 0, 1);
    glRotatef(-90, 0, 1, 0);
    glScalef(0.1f, 0.01f, 0.05f);
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();
}

void drawTree(const Tree* tree) {
    glPushMatrix();
    glTranslatef(tree->x, 0.0f, tree->z);

    // Tree Trunk
    glColor3f(0.4f, 0.2f, 0.0f);
    glPushMatrix();
    glScalef(0.1f, 1.0f, 0.1f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Tree Canopy
    glColor3f(0.0f, 0.6f, 0.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.7f, 0.0f);
    glutSolidSphere(0.5, 12, 12);
    glTranslatef(0.2f, 0.1f, 0.0f);
    glutSolidSphere(0.4, 12, 12);
    glTranslatef(-0.4f, 0.0f, 0.0f);
    glutSolidSphere(0.4, 12, 12);
    glPopMatrix();

    // Draw the name above the tree
    drawTreeName(tree->name);

    glPopMatrix();
}

void drawSun() {
    float sunAngle = (dayNightTimer / DAY_NIGHT_INTERVAL) * 2.0f * M_PI;
    float x = 50.0f * sin(sunAngle);
    float y = 50.0f * cos(sunAngle);

    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(x, y, -50.0f);
    glColor3f(1.0f, 0.8f, 0.0f);
    glutSolidSphere(2.0f, 30, 30);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawMoon() {
    if (isDay) return;

    float moonAngle = (dayNightTimer / DAY_NIGHT_INTERVAL) * 2.0f * M_PI + M_PI;
    float x = 60.0f * sin(moonAngle);
    float y = 60.0f * cos(moonAngle);

    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(x, y, -50.0f);
    glColor3f(0.9f, 0.9f, 0.9f);
    glutSolidSphere(1.5f, 30, 30);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawStars() {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    for(int i = 0; i < NUM_STARS; ++i) {
        glVertex3f(stars[i].x, stars[i].y, stars[i].z);
    }
    glEnd();
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void drawTrack() {
    glColor3f(0.2f, 0.2f, 0.2f); // Road color
    glBegin(GL_QUADS);
    glVertex3f(-roadWidth, 0.0f, 50.0f);
    glVertex3f(roadWidth, 0.0f, 50.0f);
    glVertex3f(roadWidth, 0.0f, -200.0f);
    glVertex3f(-roadWidth, 0.0f, -200.0f);
    glEnd();

    // Lane markings
    glColor3f(1, 1, 1);
    float laneLength = 3.0f;
    float gap = 5.0f;
    float startZ = fmod(roadOffset, gap) - gap;

    for (float z = startZ; z > -200; z -= gap) {
        glBegin(GL_QUADS);
        glVertex3f(-0.1f, 0.01f, z);
        glVertex3f(0.1f, 0.01f, z);
        glVertex3f(0.1f, 0.01f, z - laneLength);
        glVertex3f(-0.1f, 0.01f, z - laneLength);
        glEnd();
    }

    // Right side scenery (grass)
    glColor3f(0.0f, 0.5f, 0.0f);
    glBegin(GL_QUADS);
    glVertex3f(roadWidth, 0.0f, 50.0f);
    glVertex3f(5.0f, 0.0f, 50.0f);
    glVertex3f(5.0f, 0.0f, -200.0f);
    glVertex3f(roadWidth, 0.0f, -200.0f);
    glEnd();

    // Left side - THE RIVER 🏞️
    glColor3f(0.0f, 0.3f, 0.8f);
    glBegin(GL_QUADS);
    glVertex3f(-roadWidth, 0.0f, 50.0f);
    glVertex3f(-5.0f, 0.0f, 50.0f);
    glVertex3f(-5.0f, 0.0f, -200.0f);
    glVertex3f(-roadWidth, 0.0f, -200.0f);
    glEnd();

    // Road shoulders (dirt/sand) - still needed around the road
    glColor3f(0.4f, 0.2f, 0.0f);
    glBegin(GL_QUADS);

    // Left shoulder (next to river)
    glVertex3f(-roadWidth - 0.2f, 0.0f, 50.0f);
    glVertex3f(-roadWidth, 0.0f, 50.0f);
    glVertex3f(-roadWidth, 0.0f, -200.0f);
    glVertex3f(-roadWidth - 0.2f, 0.0f, -200.0f);

    // Right shoulder (next to grass)
    glVertex3f(roadWidth, 0.0f, 50.0f);
    glVertex3f(roadWidth + 0.2f, 0.0f, 50.0f);
    glVertex3f(roadWidth + 0.2f, 0.0f, -200.0f);
    glVertex3f(roadWidth, 0.0f, -200.0f);
    glEnd();
}

void drawBlast() {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(blastOriginX, blastOriginY, blastOriginZ);

    float alpha = 1.0f - (blastTimer / BLAST_DURATION);
    if (alpha < 0) alpha = 0;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < NUM_PARTICLES; ++i) {
        glPushMatrix();
        glTranslatef(blastParticles[i].x, blastParticles[i].y, blastParticles[i].z);
        glColor4f(1.0f, 0.1f, 0.1f, alpha);
        float sizeScale = blastParticles[i].currentSize / blastParticles[i].initialSize;
        glScalef(sizeScale, sizeScale, sizeScale);
        glutSolidSphere(blastParticles[i].initialSize, 6, 6);
        glPopMatrix();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}


void drawScore() {
    char buffer[50];
    sprintf(buffer, "Score: %d | Speed: %.1f km/h", score, gameSpeed);
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1, 1, 1);
    glRasterPos2i(10, 580);
    for (int i = 0; buffer[i] != '\0'; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buffer[i]);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

void drawGameOver() {
    const char* text = "Game Over!";
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 0.0f, 0.0f);
    glRasterPos2i(350, 300);
    const char* p = text;
    while(*p) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);
    }

    char scoreBuffer[50];
    sprintf(scoreBuffer, "Final Score: %d", score);
    glRasterPos2i(330, 280);
    p = scoreBuffer;
    while(*p) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);
    }

    char speedBuffer[50];
    sprintf(speedBuffer, "Final Speed: %.1f km/h", gameSpeed);
    glRasterPos2i(330, 260);
    p = speedBuffer;
    while(*p) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

// --- GAME LOGIC FUNCTIONS ---

bool checkCollision() {
    float playerFront = 0.0f + 1.2f / 2;
    float playerBack = 0.0f - 1.2f / 2;
    float playerLeft = carX - 0.2f / 2;
    float playerRight = carX + 0.2f / 2;

    for (int i = 0; i < NUM_CUBES; ++i) {
        float obsFront = obstacles[i].z + obstacles[i].length / 2;
        float obsBack = obstacles[i].z - obstacles[i].length / 2;
        float obsLeft = obstacles[i].x - obstacles[i].width / 2;
        float obsRight = obstacles[i].x + obstacles[i].width / 2;

        bool overlapX = (playerRight > obsLeft) && (playerLeft < obsRight);
        bool overlapZ = (playerFront > obsBack) && (playerBack < obsFront);

        if (overlapX && overlapZ) {
            return true;
        }
    }
    return false;
}

void update(int value) {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - prevTime) / 1000.0f;
    prevTime = currentTime;

    dayNightTimer += deltaTime;
    if (dayNightTimer >= DAY_NIGHT_INTERVAL) {
        isDay = !isDay;
        dayNightTimer = 0.0f;
    }
    gameSpeed = 5.0f + (float)score * 0.5f;
    float roadSpeed = gameSpeed * deltaTime;

    if (isGameOver) {
        if (isBlasting) {
             blastTimer += deltaTime;
             for (int i = 0; i < NUM_PARTICLES; ++i) {
                 blastParticles[i].x += blastParticles[i].velocityX * deltaTime;
                 blastParticles[i].y += blastParticles[i].velocityY * deltaTime;
                 blastParticles[i].z += blastParticles[i].velocityZ * deltaTime;
                 blastParticles[i].currentSize = blastParticles[i].initialSize * (1.0f - (blastTimer / BLAST_DURATION));
             }
             if (blastTimer >= BLAST_DURATION) {
                 isBlasting = false;
             }
        }
        glutPostRedisplay();
        glutTimerFunc(16, update, 0);
        return;
    }

    if (isBlasting) {
        blastTimer += deltaTime;
        for (int i = 0; i < NUM_PARTICLES; ++i) {
            blastParticles[i].x += blastParticles[i].velocityX * deltaTime;
            blastParticles[i].y += blastParticles[i].velocityY * deltaTime;
            blastParticles[i].z += blastParticles[i].velocityZ * deltaTime;
            blastParticles[i].currentSize = blastParticles[i].initialSize * (1.0f - (blastTimer / BLAST_DURATION));
        }
        if (blastTimer >= BLAST_DURATION) {
            isBlasting = false;
        }
    }

    for (int i = 0; i < NUM_CUBES; ++i) {
        obstacles[i].z += roadSpeed;
        if (obstacles[i].z > 5.0f) {
            obstacles[i].z = -150.0f - (rand() % 50);
            obstacles[i].x = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * roadWidth;
            score++;
        }
    }

    for (int i = 0; i < NUM_BIRDS; ++i) {
        birds[i].z += roadSpeed * 0.4f;
        // Keep birds flying over the road/right side scenery for realism
        birds[i].x = ((float)rand() / RAND_MAX * 2.0f - 1.0f) * (roadWidth + 2.0f) + sin(birds[i].phase + birds[i].z/10.0f) * 2.0f;
        birds[i].y = 1.5f + sin(birds[i].phase * 2.0f + birds[i].z/20.0f) * 0.5f;
        birds[i].phase += deltaTime * 5.0f;
        if (birds[i].z > 10.0f) {
            birds[i].z = -200.0f - (rand() % 100);
            birds[i].y = ((float)rand() / RAND_MAX * 0.3f) + 1.5f;
            birds[i].phase = ((float)rand() / RAND_MAX) * 2 * M_PI;
        }
    }

    roadOffset += roadSpeed;
    if (roadOffset > 100.0f) roadOffset -= 100.0f;

    for(int i = 0; i < NUM_TREES; ++i) {
        trees[i].z += roadSpeed;
        if(trees[i].z > 5.0f) {
            trees[i].z = -200.0f - (rand() % 20);
            // Re-spawn logic remains right-side only
            trees[i].x = roadWidth + 0.5f + ((float)rand() / RAND_MAX * 1.5f);
            strcpy(trees[i].name, treeNames[rand() % numTreeTypes]);
        }
    }

    if (checkCollision()) {
        if (!isBlasting) {
            isBlasting = true;
            blastTimer = 0.0f;
            blastOriginX = carX;
            blastOriginY = 0.15f;
            blastOriginZ = 0.0f;
            initBlastParticles();
        }
        isGameOver = true;
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// --- RENDERING SETUP ---

void setCamera() {
    glLoadIdentity();
    gluLookAt(carX, 1.8f, 5.0f,
              carX, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f);
}

void setLighting() {
    float blend = dayNightTimer / DAY_NIGHT_INTERVAL;
    if (!isDay) blend = 1.0f - blend;

    float ambient_level = 0.1f + 0.4f * blend;
    float diffuse_level = 0.2f + 0.6f * blend;

    if (isDay) {
        GLfloat light_pos[] = { 0.0f, 10.0f, 10.0f, 1.0f };
        GLfloat light_ambient[] = { ambient_level, ambient_level, ambient_level, 1.0f };
        GLfloat light_diffuse[] = { diffuse_level, diffuse_level, diffuse_level, 1.0f };

        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

        glDisable(GL_LIGHT1);
    } else {
        GLfloat light_pos[] = { 0.0f, 5.0f, 5.0f, 1.0f };
        GLfloat light_ambient[] = { ambient_level, ambient_level, ambient_level + 0.05f, 1.0f };
        GLfloat light_diffuse[] = { diffuse_level, diffuse_level, diffuse_level + 0.1f, 1.0f };

        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

        glEnable(GL_LIGHT1);
        GLfloat headlight_pos[] = { carX, 0.5f, 0.6f, 1.0f };
        GLfloat headlight_dir[] = { 0.0f, 0.0f, -1.0f };
        GLfloat headlight_diffuse[] = { 1.0f, 1.0f, 0.8f, 1.0f };
        glLightfv(GL_LIGHT1, GL_POSITION, headlight_pos);
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, headlight_dir);
        glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 45.0f);
        glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 20.0f);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, headlight_diffuse);
    }
}

void display() {
    float blend = dayNightTimer / DAY_NIGHT_INTERVAL;
    if (!isDay) blend = 1.0f - blend;

    float r = 0.5f * blend + 0.02f * (1.0f - blend);
    float g = 0.8f * blend + 0.02f * (1.0f - blend);
    float b = 1.0f * blend + 0.1f * (1.0f - blend);
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setCamera();
    setLighting();

    if (isDay) {
        drawSun();
    } else {
        drawMoon();
        drawStars();
    }

    glEnable(GL_LIGHTING);

    if (isBlasting) {
        drawBlast();
    }

    drawTrack();

    for (int i = 0; i < NUM_TREES; ++i) {
        drawTree(&trees[i]);
    }

    for (int i = 0; i < NUM_CUBES; ++i) {
        drawCubeObstacle(&obstacles[i]);
    }

    glDisable(GL_LIGHTING);
    for (int i = 0; i < NUM_BIRDS; ++i) {
        drawBird(&birds[i]);
    }
    glEnable(GL_LIGHTING);

    if (!isBlasting) {
        drawCarImproved();
    }

    if (isGameOver) {
        drawGameOver();
    } else {
        drawScore();
    }

    glutSwapBuffers();
}


void keyboard(int key, int x, int y) {
    if (isGameOver) return;
    float step = 0.2f;
    switch (key) {
        case GLUT_KEY_LEFT:
            carX -= step;
            if (carX < -roadWidth) carX = -roadWidth;
            break;
        case GLUT_KEY_RIGHT:
            carX += step;
            if (carX > roadWidth) carX = roadWidth;
            break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Racing Bike Game");

    init();

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutSpecialFunc(keyboard);
    glutTimerFunc(16, update, 0);

    glutMainLoop();
    return 0;
}
