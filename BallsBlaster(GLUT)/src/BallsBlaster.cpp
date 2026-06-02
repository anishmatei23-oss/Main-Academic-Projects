#include<iostream>
#include<stdio.h>
#include <GL/glut.h>
#include<math.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#define PI 3.14159265
using namespace std;

// Background music flag
bool musicPlaying = false;

// Initialize the functions
void points_to_go();
void get_score();
int rand1();
void timer(int );
void game_over();
void drawline_between_two_points(int ,int );
void keyboard(unsigned char key, int x, int y);
void findNearestBubblePosition(double bx, double by, int &row, int &col);
void addNewRow();
// Game constants - optimized for 1080p full screen
const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;
const int BUBBLE_RADIUS = 22;
const int BUBBLE_SPACING = 40;  // Reduced spacing to fill entire screen horizontally
const int ROW_HEIGHT = 35;  // sqrt(3)/2 * spacing for hex grid
const int END_LINE_Y = 120;  // Moved down further
const int MAX_ROWS = 30;  // Increased for full screen
const int MAX_COLS_EVEN = 48;  // Even rows: 1920/40 = 48 bubbles (fills screen)
const int MAX_COLS_ODD = 47;   // Odd rows: offset by 20

// Initialize variables
int x=SCREEN_WIDTH/2, y=0, xs=SCREEN_WIDTH/2, ys=0, max1=SCREEN_HEIGHT-(9*ROW_HEIGHT), max2=SCREEN_HEIGHT;
int reset=1, c1=0, c4=0, ck, sc=0;
int turnCount=0;  // Track turns for auto-push
bool flt, gover=false;
double xl=x,yl=y,x3,y3,len,theta,theta1,lbound=0.13962634,rbound=3.00196631;
int noc[5];
int windowWidth=SCREEN_WIDTH, windowHeight=SCREEN_HEIGHT;
int comboCount=0;  // Combo counter
int lastPopTime=0;  // For combo timing
int doubleBallsActive=0;  // Double balls powerup timer
int currentPowerup=0;  // Current active powerup on shooter

// initialize some structs for supplying bubble pattern and 
// controlling the pointers
struct blast1{
    int a,b;
};
struct blast1 blast[200];
struct colour{
    double r1,g1,b1;
};
struct colour col1[6];
struct bubble{
    int color,v,vi;
    int powerup;  // 0=none, 1=fireball, 2=double, 3=piercing
};
struct bubble bubbles[30][48];  // Full screen grid: 30 rows x 48 cols max

// colors combinations for bubbles - enhanced colors
void cols(){
    // Red
    col1[0].r1=1.0; col1[0].g1=0.2; col1[0].b1=0.2;
    // Green
    col1[1].r1=0.2; col1[1].g1=1.0; col1[1].b1=0.2;
    // Blue
    col1[2].r1=0.2; col1[2].g1=0.4; col1[2].b1=1.0;
    // Yellow
    col1[3].r1=1.0; col1[3].g1=0.9; col1[3].b1=0.2;
    // Purple
    col1[4].r1=0.8; col1[4].g1=0.2; col1[4].b1=1.0;
    // Background color
    col1[5].r1=0.15; col1[5].g1=0.15; col1[5].b1=0.25;
}
static void resize(int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int rand1(){
    for(int i=0;i<5;i++){
        noc[i]=0;
    }
    for(int j=0;j<MAX_ROWS;j++){
        if(j%2==0){
            for(int i=0;i<MAX_COLS_EVEN;i++){
                if(bubbles[j][i].v==1){
                    noc[bubbles[j][i].color]++;
                }
            }
        }
       else{
            for(int i=0;i<MAX_COLS_ODD;i++){
                if(bubbles[j][i].v==1){
                    noc[bubbles[j][i].color]++;
                }
            }
       }
    }
    ck=0;
    for(int i=0;i<5;i++){
        if(noc[i]==0){
            ck++;
        }
    }
    int temp=rand()%5;
    if(ck!=5){
    while(noc[temp]==0){
        temp=rand()%5;
    }}
    return temp;
}

void score1(){
        glColor3f(0.0,0,0);
        glRasterPos2i(170,170);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'G');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'A');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'M');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'E');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,' ');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'O');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'V');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'E');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'R');
}

void circle(double xc,double yc,int R,double r,double g,double b, bool powerup=false){
    // Draw outer glow for powerups
    if(powerup){
        for(double t=0;t<=2*PI;t+=.05){
            glBegin(GL_TRIANGLES);
            glColor4d(r*0.3,g*0.3,b*0.3,0.5);
            glVertex2d(xc,yc);
            glColor4d(r*0.1,g*0.1,b*0.1,0.0);
            glVertex2d(xc+(R+8)*cos(t),yc+(R+8)*sin(t));
            glColor4d(r*0.1,g*0.1,b*0.1,0.0);
            glVertex2d(xc+(R+8)*cos(t+0.05),yc+(R+8)*sin(t+0.05));
            glEnd();
        }
    }
    
    // Main circle with gradient
    for(double t=0;t<=2*PI;t+=.08){
        glBegin(GL_TRIANGLES);
        glColor3d(r,g,b);
        glVertex2d(xc,yc);
        glColor3d(r*0.7,g*0.7,b*0.7);
        glVertex2d(xc+R*cos(t),yc+R*sin(t));
        glColor3d(r*0.5,g*0.5,b*0.5);
        glVertex2d(xc+R*cos(t+0.08),yc+R*sin(t+0.08));
        glEnd();
    }
    
    // Highlight
    for(double t=PI/4;t<=3*PI/4;t+=.1){
        glBegin(GL_TRIANGLES);
        glColor3d(r*1.2,g*1.2,b*1.2);
        glVertex2d(xc,yc);
        glColor3d(r*0.9,g*0.9,b*0.9);
        glVertex2d(xc+R*0.7*cos(t),yc+R*0.7*sin(t));
        glColor3d(r*0.9,g*0.9,b*0.9);
        glVertex2d(xc+R*0.7*cos(t+0.1),yc+R*0.7*sin(t+0.1));
        glEnd();
    }
}
void inits(){
    for(int j=0;j<7;j++){
        if(j%2==0){
            for(int i=0;i<MAX_COLS_EVEN;i++){
                bubbles[j][i].v=1;
                bubbles[j][i].vi=0;
                bubbles[j][i].color=rand()%5;
                bubbles[j][i].powerup=(rand()%15==0)?(1+rand()%3):0;  // ~7% chance for powerup
            }
        }
       else{
            for(int i=0;i<MAX_COLS_ODD;i++){
                bubbles[j][i].v=1;
                bubbles[j][i].vi=0;
                bubbles[j][i].color=rand()%5;
                bubbles[j][i].powerup=(rand()%15==0)?(1+rand()%3):0;
            }
       }
    }
}

void drawBackground(){
    // Beautiful gradient background
    glBegin(GL_QUADS);
    // Top - darker blue
    glColor3d(0.1, 0.1, 0.3);
    glVertex2d(0, SCREEN_HEIGHT);
    glVertex2d(SCREEN_WIDTH, SCREEN_HEIGHT);
    // Middle - purple
    glColor3d(0.15, 0.1, 0.25);
    glVertex2d(SCREEN_WIDTH, SCREEN_HEIGHT/2);
    glVertex2d(0, SCREEN_HEIGHT/2);
    // Bottom - darker
    glColor3d(0.05, 0.05, 0.15);
    glVertex2d(SCREEN_WIDTH, 0);
    glVertex2d(0, 0);
    glEnd();
    
    // Add some stars/particles
    glPointSize(2);
    glBegin(GL_POINTS);
    for(int i=0;i<100;i++){
        double px = (rand()%SCREEN_WIDTH);
        double py = (rand()%SCREEN_HEIGHT);
        double bright = 0.5 + (rand()%50)/100.0;
        glColor3d(bright, bright, bright);
        glVertex2d(px, py);
    }
    glEnd();
}

 void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawBackground();

    //circle(240,320,500,1,0.75,0.8);
    cols();
    for(int j=0;j<MAX_ROWS;j++){
        if(j%2==0){
            for(int i=0;i<MAX_COLS_EVEN;i++){
                if(bubbles[j][i].v==1){
                    double bx = (i*BUBBLE_SPACING)+BUBBLE_SPACING/2;
                    double by = SCREEN_HEIGHT-(j*ROW_HEIGHT);
                    if(bx >= 0 && bx <= SCREEN_WIDTH && by >= 0 && by <= SCREEN_HEIGHT){
                        // Draw powerup indicator
                        bool isPowerup = bubbles[j][i].powerup > 0;
                        circle(bx, by, BUBBLE_RADIUS, 
                               col1[bubbles[j][i].color].r1,
                               col1[bubbles[j][i].color].g1,
                               col1[bubbles[j][i].color].b1,
                               isPowerup);
                        // Draw powerup symbol
                        if(isPowerup){
                            glLineWidth(3);
                            glBegin(GL_LINES);
                            if(bubbles[j][i].powerup == 1){  // Fireball - flame symbol
                                glColor3d(1.0, 0.5, 0.0);
                                glVertex2d(bx-8, by-8);
                                glVertex2d(bx, by+8);
                                glVertex2d(bx, by+8);
                                glVertex2d(bx+8, by-8);
                            } else if(bubbles[j][i].powerup == 2){  // Double - two circles
                                glColor3d(1.0, 1.0, 0.0);
                                glVertex2d(bx-10, by);
                                glVertex2d(bx-5, by);
                                glVertex2d(bx+5, by);
                                glVertex2d(bx+10, by);
                            } else if(bubbles[j][i].powerup == 3){  // Piercing - arrow
                                glColor3d(0.0, 1.0, 1.0);
                                glVertex2d(bx-10, by);
                                glVertex2d(bx+10, by);
                                glVertex2d(bx+5, by-5);
                                glVertex2d(bx+10, by);
                                glVertex2d(bx+5, by+5);
                                glVertex2d(bx+10, by);
                            }
                            glEnd();
                        }
                        max2=SCREEN_HEIGHT-((j+1)*ROW_HEIGHT);
                    }
                }
            }
        }
       else{
            for(int i=0;i<MAX_COLS_ODD;i++){
                if(bubbles[j][i].v==1){
                    double bx = (i*BUBBLE_SPACING)+BUBBLE_SPACING;
                    double by = SCREEN_HEIGHT-(j*ROW_HEIGHT);
                    if(bx >= 0 && bx <= SCREEN_WIDTH && by >= 0 && by <= SCREEN_HEIGHT){
                        bool isPowerup = bubbles[j][i].powerup > 0;
                        circle(bx, by, BUBBLE_RADIUS,
                               col1[bubbles[j][i].color].r1,
                               col1[bubbles[j][i].color].g1,
                               col1[bubbles[j][i].color].b1,
                               isPowerup);
                        // Draw powerup symbol
                        if(isPowerup){
                            glLineWidth(3);
                            glBegin(GL_LINES);
                            if(bubbles[j][i].powerup == 1){
                                glColor3d(1.0, 0.5, 0.0);
                                glVertex2d(bx-8, by-8);
                                glVertex2d(bx, by+8);
                                glVertex2d(bx, by+8);
                                glVertex2d(bx+8, by-8);
                            } else if(bubbles[j][i].powerup == 2){
                                glColor3d(1.0, 1.0, 0.0);
                                glVertex2d(bx-10, by);
                                glVertex2d(bx-5, by);
                                glVertex2d(bx+5, by);
                                glVertex2d(bx+10, by);
                            } else if(bubbles[j][i].powerup == 3){
                                glColor3d(0.0, 1.0, 1.0);
                                glVertex2d(bx-10, by);
                                glVertex2d(bx+10, by);
                                glVertex2d(bx+5, by-5);
                                glVertex2d(bx+10, by);
                                glVertex2d(bx+5, by+5);
                                glVertex2d(bx+10, by);
                            }
                            glEnd();
                        }
                        max2=SCREEN_HEIGHT-((j+1)*ROW_HEIGHT);
                    }
                }
            }
       }
    }
    if(ck==5) gover = true;
    if(gover){
        game_over();
    }
    // Draw end line - moved down
    glLineWidth(3);
    glColor3d(1.0, 0.2, 0.2);
    glBegin(GL_LINES);
        glVertex2d(0, END_LINE_Y);
        glVertex2d(SCREEN_WIDTH, END_LINE_Y);
    glEnd();
    
    // Draw crosshair with next ball preview
    drawline_between_two_points(xl,yl);
    
    // Draw next ball preview at shooter position
    if(ck!=5){
        bool hasPowerup = currentPowerup > 0;
        circle(xs, 40, BUBBLE_RADIUS, col1[c1].r1, col1[c1].g1, col1[c1].b1, hasPowerup);
        // Draw powerup indicator on shooter
        if(hasPowerup){
            glLineWidth(3);
            glBegin(GL_LINES);
            if(currentPowerup == 1){
                glColor3d(1.0, 0.5, 0.0);
                glVertex2d(xs-8, 32);
                glVertex2d(xs, 48);
                glVertex2d(xs, 48);
                glVertex2d(xs+8, 32);
            } else if(currentPowerup == 2){
                glColor3d(1.0, 1.0, 0.0);
                glVertex2d(xs-10, 40);
                glVertex2d(xs-5, 40);
                glVertex2d(xs+5, 40);
                glVertex2d(xs+10, 40);
            } else if(currentPowerup == 3){
                glColor3d(0.0, 1.0, 1.0);
                glVertex2d(xs-10, 40);
                glVertex2d(xs+10, 40);
                glVertex2d(xs+5, 35);
                glVertex2d(xs+10, 40);
                glVertex2d(xs+5, 45);
                glVertex2d(xs+10, 40);
            }
            glEnd();
        }
        // Draw shooting ball
        circle(x, y+40, BUBBLE_RADIUS, col1[c1].r1, col1[c1].g1, col1[c1].b1, hasPowerup);
    }

    get_score();
    glutSwapBuffers();
}

void dfs(int i,int j,int c){
    // Only match same color - strict check
    if(i < 0 || i >= MAX_ROWS || j < 0) return;
    if(i%2==0 && j >= MAX_COLS_EVEN) return;
    if(i%2==1 && j >= MAX_COLS_ODD) return;
    if(bubbles[i][j].v==0 || bubbles[i][j].color != c) return;
    
    if(i%2==0){
            if(i==0){
                if(j>0 && j<MAX_COLS_EVEN-1){
            if(bubbles[i+1][j-1].v==1&&bubbles[i+1][j-1].color==c&&bubbles[i+1][j-1].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j-1;
                bubbles[i+1][j-1].vi=1;
                c4++;
                dfs(i+1,j-1,c);
            }
            if(bubbles[i+1][j].v==1&&bubbles[i+1][j].color==c&&bubbles[i+1][j].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs(i+1,j,c);
            }
            if(bubbles[i][j-1].v==1&&bubbles[i][j-1].color==c&&bubbles[i][j-1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j-1;
                bubbles[i][j-1].vi=1;
                c4++;
                dfs(i,j-1,c);
            }
            if(bubbles[i][j+1].v==1&&bubbles[i][j+1].color==c&&bubbles[i][j+1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j+1;
                bubbles[i][j+1].vi=1;
                c4++;
                dfs(i,j+1,c);
            }
        }
        if(j==0){
            if(bubbles[i+1][j].v==1&&bubbles[i+1][j].color==c&&bubbles[i+1][j].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs(i+1,j,c);
            }
            if(bubbles[i][j+1].v==1&&bubbles[i][j+1].color==c&&bubbles[i][j+1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j+1;
                bubbles[i][j+1].vi=1;
                c4++;
                dfs(i,j+1,c);
            }
        }
        if(j==MAX_COLS_EVEN-1 && i%2==0){
            if(i+1 < MAX_ROWS && j-1 >= 0 && bubbles[i+1][j-1].v==1&&bubbles[i+1][j-1].color==c&&bubbles[i+1][j-1].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j-1;
                bubbles[i+1][j-1].vi=1;
                c4++;
                dfs(i+1,j-1,c);
            }
            if(j-1 >= 0 && bubbles[i][j-1].v==1&&bubbles[i][j-1].color==c&&bubbles[i][j-1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j-1;
                bubbles[i][j-1].vi=1;
                c4++;
                dfs(i,j-1,c);
            }
        }
            }
        else if(j>0 && j<MAX_COLS_EVEN-1){
            if(bubbles[i-1][j-1].v==1&&bubbles[i-1][j-1].color==c&&bubbles[i-1][j-1].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j-1;
                bubbles[i-1][j-1].vi=1;
                c4++;
                dfs(i-1,j-1,c);
            }
            if(bubbles[i-1][j].v==1&&bubbles[i-1][j].color==c&&bubbles[i-1][j].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j;
                bubbles[i-1][j].vi=1;
                c4++;
                dfs(i-1,j,c);
            }
            if(bubbles[i+1][j-1].v==1&&bubbles[i+1][j-1].color==c&&bubbles[i+1][j-1].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j-1;
                bubbles[i+1][j-1].vi=1;
                c4++;
                dfs(i+1,j-1,c);
            }
            if(bubbles[i+1][j].v==1&&bubbles[i+1][j].color==c&&bubbles[i+1][j].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs(i+1,j,c);
            }
            if(bubbles[i][j-1].v==1&&bubbles[i][j-1].color==c&&bubbles[i][j-1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j-1;
                bubbles[i][j-1].vi=1;
                c4++;
                dfs(i,j-1,c);
            }
            if(bubbles[i][j+1].v==1&&bubbles[i][j+1].color==c&&bubbles[i][j+1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j+1;
                bubbles[i][j+1].vi=1;
                c4++;
                dfs(i,j+1,c);
            }
        }
        if(j==0){
            if(bubbles[i-1][j].v==1&&bubbles[i-1][j].color==c&&bubbles[i-1][j].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j;
                bubbles[i-1][j].vi=1;
                c4++;
                dfs(i-1,j,c);
            }
            if(bubbles[i+1][j].v==1&&bubbles[i+1][j].color==c&&bubbles[i+1][j].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs(i+1,j,c);
            }
            if(bubbles[i][j+1].v==1&&bubbles[i][j+1].color==c&&bubbles[i][j+1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j+1;
                bubbles[i][j+1].vi=1;
                c4++;
                dfs(i,j+1,c);
            }
        }
        if(j==MAX_COLS_EVEN-1 && i%2==0){
            if(i-1 >= 0 && j-1 >= 0 && bubbles[i-1][j-1].v==1&&bubbles[i-1][j-1].color==c&&bubbles[i-1][j-1].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j-1;
                bubbles[i-1][j-1].vi=1;
                c4++;
                dfs(i-1,j-1,c);
            }
            if(i+1 < MAX_ROWS && j-1 >= 0 && bubbles[i+1][j-1].v==1&&bubbles[i+1][j-1].color==c&&bubbles[i+1][j-1].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j-1;
                bubbles[i+1][j-1].vi=1;
                c4++;
                dfs(i+1,j-1,c);
            }
            if(j-1 >= 0 && bubbles[i][j-1].v==1&&bubbles[i][j-1].color==c&&bubbles[i][j-1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j-1;
                bubbles[i][j-1].vi=1;
                c4++;
                dfs(i,j-1,c);
            }
        }
    }
    else{
        if(j>0 && j<MAX_COLS_ODD-1){
            if(bubbles[i-1][j+1].v==1&&bubbles[i-1][j+1].color==c&&bubbles[i-1][j+1].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j+1;
                bubbles[i-1][j+1].vi=1;
                c4++;
                dfs(i-1,j+1,c);
            }
            if(bubbles[i-1][j].v==1&&bubbles[i-1][j].color==c&&bubbles[i-1][j].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j;
                bubbles[i-1][j].vi=1;
                c4++;
                dfs(i-1,j,c);
            }
            if(bubbles[i+1][j+1].v==1&&bubbles[i+1][j+1].color==c&&bubbles[i+1][j+1].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j+1;
                bubbles[i+1][j+1].vi=1;
                c4++;
                dfs(i+1,j+1,c);
            }
            if(bubbles[i+1][j].v==1&&bubbles[i+1][j].color==c&&bubbles[i+1][j].vi==0){
                 blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs(i+1,j,c);
            }
            if(bubbles[i][j-1].v==1&&bubbles[i][j-1].color==c&&bubbles[i][j-1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j-1;
                bubbles[i][j-1].vi=1;
                c4++;
                dfs(i,j-1,c);
            }
            if(bubbles[i][j+1].v==1&&bubbles[i][j+1].color==c&&bubbles[i][j+1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j+1;
                bubbles[i][j+1].vi=1;
                c4++;
                dfs(i,j+1,c);
            }
        }
        if(j==0){
            if(bubbles[i-1][j+1].v==1&&bubbles[i-1][j+1].color==c&&bubbles[i-1][j+1].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j+1;
                bubbles[i-1][j+1].vi=1;
                c4++;
                dfs(i-1,j+1,c);
            }
            if(bubbles[i-1][j].v==1&&bubbles[i-1][j].color==c&&bubbles[i-1][j].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j;
                bubbles[i-1][j].vi=1;
                c4++;
                dfs(i-1,j,c);
            }
            if(bubbles[i+1][j+1].v==1&&bubbles[i+1][j+1].color==c&&bubbles[i+1][j+1].vi==0){
                 blast[c4].a=i+1;
                blast[c4].b=j+1;
                bubbles[i+1][j+1].vi=1;
                c4++;
                dfs(i+1,j+1,c);
            }
            if(bubbles[i+1][j].v==1&&bubbles[i+1][j].color==c&&bubbles[i+1][j].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs(i+1,j,c);
            }
            if(bubbles[i][j+1].v==1&&bubbles[i][j+1].color==c&&bubbles[i][j+1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j+1;
                bubbles[i][j+1].vi=1;
                c4++;
                dfs(i,j+1,c);
            }
        }
        if(j==MAX_COLS_ODD-1 && i%2==1){
            if(i-1 >= 0 && j+1 < MAX_COLS_ODD && bubbles[i-1][j+1].v==1&&bubbles[i-1][j+1].color==c&&bubbles[i-1][j+1].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j+1;
                bubbles[i-1][j+1].vi=1;
                c4++;
                dfs(i-1,j+1,c);
            }
            if(i-1 >= 0 && bubbles[i-1][j].v==1&&bubbles[i-1][j].color==c&&bubbles[i-1][j].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j;
                bubbles[i-1][j].vi=1;
                c4++;
                dfs(i-1,j,c);
            }
            if(i+1 < MAX_ROWS && j+1 < MAX_COLS_ODD && bubbles[i+1][j+1].v==1&&bubbles[i+1][j+1].color==c&&bubbles[i+1][j+1].vi==0){
                 blast[c4].a=i+1;
                blast[c4].b=j+1;
                bubbles[i+1][j+1].vi=1;
                c4++;
                dfs(i+1,j+1,c);
            }
            if(i+1 < MAX_ROWS && bubbles[i+1][j].v==1&&bubbles[i+1][j].color==c&&bubbles[i+1][j].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs(i+1,j,c);
            }
            if(j-1 >= 0 && bubbles[i][j-1].v==1&&bubbles[i][j-1].color==c&&bubbles[i][j-1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j-1;
                bubbles[i][j-1].vi=1;
                c4++;
                dfs(i,j-1,c);
            }
        }
    }
}

void dfs1(int i,int j){
    if(i < 0 || i >= MAX_ROWS || j < 0) return;
    if(i%2==0 && j >= MAX_COLS_EVEN) return;
    if(i%2==1 && j >= MAX_COLS_ODD) return;
    if(bubbles[i][j].v==0) return;
    
    if(i==0){
                flt=false;
            }
    if(i%2==0){
            if(i==0){
                if(j>0 && j<MAX_COLS_EVEN-1){
            if(bubbles[i+1][j-1].v==1&&bubbles[i+1][j-1].vi==0){

                blast[c4].a=i+1;
                blast[c4].b=j-1;
                bubbles[i+1][j-1].vi=1;
                c4++;
                dfs1(i+1,j-1);
            }
            if(bubbles[i+1][j].v==1&&bubbles[i+1][j].vi==0){

                blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs1(i+1,j);
            }
            if(bubbles[i][j-1].v==1&&bubbles[i][j-1].vi==0){

                blast[c4].a=i;
                blast[c4].b=j-1;
                bubbles[i][j-1].vi=1;
                c4++;
                dfs1(i,j-1);
            }
            if(bubbles[i][j+1].v==1&&bubbles[i][j+1].vi==0){

                blast[c4].a=i;
                blast[c4].b=j+1;
                bubbles[i][j+1].vi=1;
                c4++;
                dfs1(i,j+1);
            }
        }
        if(j==0){
            if(bubbles[i+1][j].v==1&&bubbles[i+1][j].vi==0){

                blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs1(i+1,j);
            }
            if(bubbles[i][j+1].v==1&&bubbles[i][j+1].vi==0){

                blast[c4].a=i;
                blast[c4].b=j+1;
                bubbles[i][j+1].vi=1;
                c4++;
                dfs1(i,j+1);
            }
        }
        if(j==MAX_COLS_EVEN-1 && i%2==0){
            if(i+1 < MAX_ROWS && j-1 >= 0 && bubbles[i+1][j-1].v==1&&bubbles[i+1][j-1].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j-1;
                bubbles[i+1][j-1].vi=1;
                c4++;
                dfs1(i+1,j-1);
            }
            if(j-1 >= 0 && bubbles[i][j-1].v==1&&bubbles[i][j-1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j-1;
                bubbles[i][j-1].vi=1;
                c4++;
                dfs1(i,j-1);
            }
        }
            }
        else if(j>0 && j<MAX_COLS_EVEN-1){
            if(bubbles[i-1][j-1].v==1&&bubbles[i-1][j-1].vi==0){

                blast[c4].a=i-1;
                blast[c4].b=j-1;
                bubbles[i-1][j-1].vi=1;
                c4++;
                dfs1(i-1,j-1);
            }
            if(bubbles[i-1][j].v==1&&bubbles[i-1][j].vi==0){

                blast[c4].a=i-1;
                blast[c4].b=j;
                bubbles[i-1][j].vi=1;
                c4++;
                dfs1(i-1,j);
            }
            if(bubbles[i+1][j-1].v==1&&bubbles[i+1][j-1].vi==0){

                blast[c4].a=i+1;
                blast[c4].b=j-1;
                bubbles[i+1][j-1].vi=1;
                c4++;
                dfs1(i+1,j-1);
            }
            if(bubbles[i+1][j].v==1&&bubbles[i+1][j].vi==0){

                blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs1(i+1,j);
            }
            if(bubbles[i][j-1].v==1&&bubbles[i][j-1].vi==0){

                blast[c4].a=i;
                blast[c4].b=j-1;
                bubbles[i][j-1].vi=1;
                c4++;
                dfs1(i,j-1);
            }
            if(bubbles[i][j+1].v==1&&bubbles[i][j+1].vi==0){

                blast[c4].a=i;
                blast[c4].b=j+1;
                bubbles[i][j+1].vi=1;
                c4++;
                dfs1(i,j+1);
            }
        }
        if(j==0){
            if(bubbles[i-1][j].v==1&&bubbles[i-1][j].vi==0){

                blast[c4].a=i-1;
                blast[c4].b=j;
                bubbles[i-1][j].vi=1;
                c4++;
                dfs1(i-1,j);
            }
            if(bubbles[i+1][j].v==1&&bubbles[i+1][j].vi==0){

                blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs1(i+1,j);
            }
            if(bubbles[i][j+1].v==1&&bubbles[i][j+1].vi==0){

                blast[c4].a=i;
                blast[c4].b=j+1;
                bubbles[i][j+1].vi=1;
                c4++;
                dfs1(i,j+1);
            }
        }
        if(j==MAX_COLS_EVEN-1 && i%2==0){
            if(i-1 >= 0 && j-1 >= 0 && bubbles[i-1][j-1].v==1&&bubbles[i-1][j-1].vi==0){

                blast[c4].a=i-1;
                blast[c4].b=j-1;
                bubbles[i-1][j-1].vi=1;
                c4++;
                dfs1(i-1,j-1);
            }
            if(i+1 < MAX_ROWS && j-1 >= 0 && bubbles[i+1][j-1].v==1&&bubbles[i+1][j-1].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j-1;
                bubbles[i+1][j-1].vi=1;
                c4++;
                dfs1(i+1,j-1);
            }
            if(j-1 >= 0 && bubbles[i][j-1].v==1&&bubbles[i][j-1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j-1;
                bubbles[i][j-1].vi=1;
                c4++;
                dfs1(i,j-1);
            }
        }
    }
    else{
        if(j>0 && j<MAX_COLS_ODD-1){
            if(bubbles[i-1][j+1].v==1&&bubbles[i-1][j+1].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j+1;
                bubbles[i-1][j+1].vi=1;
                c4++;
                dfs1(i-1,j+1);
            }
            if(bubbles[i-1][j].v==1&&bubbles[i-1][j].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j;
                bubbles[i-1][j].vi=1;
                c4++;
                dfs1(i-1,j);
            }
            if(bubbles[i+1][j+1].v==1&&bubbles[i+1][j+1].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j+1;
                bubbles[i+1][j+1].vi=1;
                c4++;
                dfs1(i+1,j+1);
            }
            if(bubbles[i+1][j].v==1&&bubbles[i+1][j].vi==0){
                 blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs1(i+1,j);
            }
            if(bubbles[i][j-1].v==1&&bubbles[i][j-1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j-1;
                bubbles[i][j-1].vi=1;
                c4++;
                dfs1(i,j-1);
            }
            if(bubbles[i][j+1].v==1&&bubbles[i][j+1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j+1;
                bubbles[i][j+1].vi=1;
                c4++;
                dfs1(i,j+1);
            }
        }
        if(j==0){
            if(bubbles[i-1][j+1].v==1&&bubbles[i-1][j+1].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j+1;
                bubbles[i-1][j+1].vi=1;
                c4++;
                dfs1(i-1,j+1);
            }
            if(bubbles[i-1][j].v==1&&bubbles[i-1][j].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j;
                bubbles[i-1][j].vi=1;
                c4++;
                dfs1(i-1,j);
            }
            if(bubbles[i+1][j+1].v==1&&bubbles[i+1][j+1].vi==0){
                 blast[c4].a=i+1;
                blast[c4].b=j+1;
                bubbles[i+1][j+1].vi=1;
                c4++;
                dfs1(i+1,j+1);
            }
            if(bubbles[i+1][j].v==1&&bubbles[i+1][j].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs1(i+1,j);
            }
            if(bubbles[i][j+1].v==1&&bubbles[i][j+1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j+1;
                bubbles[i][j+1].vi=1;
                c4++;
                dfs1(i,j+1);
            }
        }
        if(j==MAX_COLS_ODD-1 && i%2==1){
            if(i-1 >= 0 && j+1 < MAX_COLS_ODD && bubbles[i-1][j+1].v==1&&bubbles[i-1][j+1].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j+1;
                bubbles[i-1][j+1].vi=1;
                c4++;
                dfs1(i-1,j+1);
            }
            if(i-1 >= 0 && bubbles[i-1][j].v==1&&bubbles[i-1][j].vi==0){
                blast[c4].a=i-1;
                blast[c4].b=j;
                bubbles[i-1][j].vi=1;
                c4++;
                dfs1(i-1,j);
            }
            if(i+1 < MAX_ROWS && j+1 < MAX_COLS_ODD && bubbles[i+1][j+1].v==1&&bubbles[i+1][j+1].vi==0){
                 blast[c4].a=i+1;
                blast[c4].b=j+1;
                bubbles[i+1][j+1].vi=1;
                c4++;
                dfs1(i+1,j+1);
            }
            if(i+1 < MAX_ROWS && bubbles[i+1][j].v==1&&bubbles[i+1][j].vi==0){
                blast[c4].a=i+1;
                blast[c4].b=j;
                bubbles[i+1][j].vi=1;
                c4++;
                dfs1(i+1,j);
            }
            if(j-1 >= 0 && bubbles[i][j-1].v==1&&bubbles[i][j-1].vi==0){
                blast[c4].a=i;
                blast[c4].b=j-1;
                bubbles[i][j-1].vi=1;
                c4++;
                dfs1(i,j-1);
            }
        }
    }
}

void remove1(){
    // Calculate combo bonus
    int currentTime = GetTickCount();
    if(currentTime - lastPopTime < 1000){  // Within 1 second = combo
        comboCount++;
    } else {
        comboCount = 1;
    }
    lastPopTime = currentTime;
    
    // Score with combo multiplier
    int baseScore = c4;
    int comboBonus = (comboCount > 1) ? (comboCount - 1) * 5 : 0;
    sc += baseScore + comboBonus;
    
    // Sound effect - different pitch based on combo
    int frequency = 400 + (comboCount * 50);
    if(frequency > 1000) frequency = 1000;
    Beep(frequency, 80);
    
    for(int i=0;i<c4;i++){
        bubbles[blast[i].a][blast[i].b].v=0;
    }
}

void float1(){
    c4=0;
    for(int j=0;j<MAX_ROWS;j++){
        if(j%2==0){
            for(int i=0;i<MAX_COLS_EVEN;i++){
                if(bubbles[j][i].v==1 && bubbles[j][i].vi==0){
                    c4=0;
                    flt=true;
                    dfs1(j,i);
                    if(flt){
                        remove1();
                    }
                }
            }
        }
       else{
            for(int i=0;i<MAX_COLS_ODD;i++){
                if(bubbles[j][i].v==1 && bubbles[j][i].vi==0){
                    c4=0;
                    flt=true;
                    dfs1(j,i);
                    if(flt){
                        remove1();
                    }
                }
            }
       }
    }
}

// Fireball powerup - blasts 3x3 area
void fireballBlast(int row, int col){
    Beep(600, 150);  // Fireball sound
    int popped = 0;
    for(int dr=-1; dr<=1; dr++){
        for(int dc=-1; dc<=1; dc++){
            int r = row + dr;
            int c = col + dc;
            if(r >= 0 && r < MAX_ROWS){
                if(r%2==0 && c >= 0 && c < MAX_COLS_EVEN && bubbles[r][c].v==1){
                    bubbles[r][c].v=0;
                    popped++;
                } else if(r%2==1 && c >= 0 && c < MAX_COLS_ODD && bubbles[r][c].v==1){
                    bubbles[r][c].v=0;
                    popped++;
                }
            }
        }
    }
    sc += popped;
}

void burst(int i,int j,int c){
    // Check bounds
    if(i < 0 || i >= MAX_ROWS || j < 0) return;
    if(i%2==0 && j >= MAX_COLS_EVEN) return;
    if(i%2==1 && j >= MAX_COLS_ODD) return;
    
    // Check if bubble exists
    if(bubbles[i][j].v==0){
        Beep(200, 50);  // Miss sound
        return;
    }
    
    // Handle powerups - activate if hit bubble has one
    int powerup = bubbles[i][j].powerup;
    if(powerup > 0){
        if(powerup == 1){  // Fireball - activate immediately
            fireballBlast(i, j);
            bubbles[i][j].v=0;
            float1();  // Check for floating bubbles
            glutPostRedisplay();
            return;
        } else if(powerup == 2){  // Double balls - activate for 5 shots
            currentPowerup = 2;
            doubleBallsActive = 5;
            Beep(500, 100);  // Double balls sound - lower pitch
            bubbles[i][j].powerup = 0;  // Remove powerup from bubble
        } else if(powerup == 3){  // Piercing - activate for next shot
            currentPowerup = 3;
            Beep(800, 120);  // Piercing sound - higher pitch
            bubbles[i][j].powerup = 0;
        }
    }
    
    // Only burst if colors match (unless piercing)
    if(currentPowerup != 3 && bubbles[i][j].color != c){
        Beep(200, 50);  // Sound for miss
        return;
    }
    
    c4=0;
    blast[c4].a=i;
    blast[c4].b=j;
    bubbles[i][j].vi=1;
    c4++;
    dfs(i,j,c);

    if(c4>=3 || currentPowerup == 3){  // Piercing always pops
        remove1();
        for(int j=0;j<MAX_ROWS;j++){
            if(j%2==0){
                for(int i=0;i<MAX_COLS_EVEN;i++){
                    bubbles[j][i].vi=0;
                }
            }
           else{
                for(int i=0;i<MAX_COLS_ODD;i++){
                    bubbles[j][i].vi=0;
                }
           }
        }
        float1();
    }
    for(int j=0;j<MAX_ROWS;j++){
        if(j%2==0){
            for(int i=0;i<MAX_COLS_EVEN;i++){
                bubbles[j][i].vi=0;
            }
        }
       else{
            for(int i=0;i<MAX_COLS_ODD;i++){
                bubbles[j][i].vi=0;
            }
       }
    }
    glutPostRedisplay();
}

void get_score()
{
    int a[10];
    a[0]=0;
    int c=0;
    int ss=sc;
    if(sc==0) c=1;
    for(int i=0;ss!=0;i++)
    {
        a[i]=ss%10;
        ss=ss/10;
        c++;
    }
    
    // Enhanced score display
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2d(SCREEN_WIDTH-300, SCREEN_HEIGHT-50);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'S');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'C');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'O');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'R');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'E');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,':');
    for(int i=c-1;i>=0;i--)
    {
         glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,48+a[i]);
    }
    
    // Turn counter
    glRasterPos2d(50, SCREEN_HEIGHT-50);
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'T');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'u');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'r');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'n');
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,':');
    int turns = turnCount % 5;
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 48 + (5-turns));
    
    // Combo display
    if(comboCount > 1){
        glRasterPos2d(50, SCREEN_HEIGHT-100);
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'C');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'O');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'M');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'B');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'O');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'!');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,' ');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'x');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 48 + comboCount);
    }
}

static void key(int key, int x1, int y1)
{
    switch(key){
        case GLUT_KEY_LEFT : 
            if(reset==1) {
                xs-=60;
                if(xs<BUBBLE_RADIUS) xs=BUBBLE_RADIUS;
                x=xs;
                Beep(100, 30);  // Movement sound
                glutPostRedisplay();
            }
            break;
        case GLUT_KEY_RIGHT : 
            if(reset==1) {
                xs+=60;
                if(xs>SCREEN_WIDTH-BUBBLE_RADIUS) xs=SCREEN_WIDTH-BUBBLE_RADIUS;
                x=xs;
                Beep(100, 30);  // Movement sound
                glutPostRedisplay();
            }
            break;
        case GLUT_KEY_F11:
            glutFullScreen();
            break;
        case 27:  // ESC key
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void mouse(int x1,int y1){
    if(max2>END_LINE_Y+50){
        xl=x1;yl=y1;
        glutPostRedisplay();
    }
}

void drawline_between_two_points(int x1,int y1){
    // Convert mouse coordinates to game coordinates
    double mx = (double)x1 * SCREEN_WIDTH / windowWidth;
    double my = SCREEN_HEIGHT - ((double)y1 * SCREEN_HEIGHT / windowHeight);
    
    theta=atan2((my-40), (mx-xs));
    if(theta>rbound) theta=rbound;
    if(theta<lbound) theta=lbound;
    
    // Draw crosshair line with next ball color
    double x2=xs+(150*cos(theta)), y2=40+(150*sin(theta));
    
    // Main aiming line
    glLineWidth(4);
    glBegin(GL_LINES);
        glColor3d(col1[c1].r1*0.8, col1[c1].g1*0.8, col1[c1].b1*0.8);
        glVertex2d(xs, 40);
        glColor3d(col1[c1].r1, col1[c1].g1, col1[c1].b1);
        glVertex2d(x2, y2);
    glEnd();
    
    // Crosshair circle at end
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glColor3d(col1[c1].r1, col1[c1].g1, col1[c1].b1);
    for(double t=0; t<=2*PI; t+=0.2){
        glVertex2d(x2+15*cos(t), y2+15*sin(t));
    }
    glEnd();
    
    // Center dot
    glPointSize(6);
    glBegin(GL_POINTS);
    glColor3d(col1[c1].r1, col1[c1].g1, col1[c1].b1);
    glVertex2d(x2, y2);
    glEnd();
}

void addNewRow(){
    // Add a new row at the top (row 0), push everything else down
    for(int j=MAX_ROWS-1; j>=1; j--){
        if(j%2==0){
            for(int i=0; i<MAX_COLS_EVEN; i++){
                bubbles[j][i].v = bubbles[j-1][i].v;
                bubbles[j][i].color = bubbles[j-1][i].color;
                bubbles[j][i].powerup = bubbles[j-1][i].powerup;
                bubbles[j][i].vi = 0;
            }
        } else {
            for(int i=0; i<MAX_COLS_ODD; i++){
                bubbles[j][i].v = bubbles[j-1][i].v;
                bubbles[j][i].color = bubbles[j-1][i].color;
                bubbles[j][i].powerup = bubbles[j-1][i].powerup;
                bubbles[j][i].vi = 0;
            }
        }
    }
    
    // Create new row at top
    if(0%2==0){
        for(int i=0; i<MAX_COLS_EVEN; i++){
            bubbles[0][i].v=1;
            bubbles[0][i].vi=0;
            bubbles[0][i].color=rand()%5;
            bubbles[0][i].powerup=(rand()%15==0)?(1+rand()%3):0;
        }
    } else {
        for(int i=0; i<MAX_COLS_ODD; i++){
            bubbles[0][i].v=1;
            bubbles[0][i].vi=0;
            bubbles[0][i].color=rand()%5;
            bubbles[0][i].powerup=(rand()%15==0)?(1+rand()%3):0;
        }
    }
    
    max1 += ROW_HEIGHT;
    max2 += ROW_HEIGHT;
    
    // Sound effect for new row
    Beep(300, 100);
}

// Find nearest grid position for bubble placement
void findNearestBubblePosition(double bx, double by, int &row, int &col){
    row = (SCREEN_HEIGHT - (int)by) / ROW_HEIGHT;
    if(row < 0) row = 0;
    if(row >= MAX_ROWS) row = MAX_ROWS-1;
    
    if(row % 2 == 0){
        // Even row
        col = (int)(bx / BUBBLE_SPACING);
        if(col < 0) col = 0;
        if(col >= MAX_COLS_EVEN) col = MAX_COLS_EVEN-1;
    } else {
        // Odd row - offset
        col = (int)((bx - BUBBLE_SPACING/2) / BUBBLE_SPACING);
        if(col < 0) col = 0;
        if(col >= MAX_COLS_ODD) col = MAX_COLS_ODD-1;
    }
}

void timer(int val){
    if(reset==0 && y<max1){
        x+=12*cos(theta1);  // Scaled movement speed
        y+=12*sin(theta1);
    }
    else if(reset==0 && y>=max1){
        // Ball reached max height - find collision point
        // Find nearest bubble to attach to - check entire screen
        double minDist = 999999;
        int hitRow = -1, hitCol = -1;
        bool found = false;
        
        // Check all existing bubbles across entire screen
        for(int j=0; j<MAX_ROWS; j++){
            if(j%2==0){
                for(int i=0; i<MAX_COLS_EVEN; i++){
                    if(bubbles[j][i].v==1){
                        double bx = (i*BUBBLE_SPACING)+BUBBLE_SPACING/2;
                        double by = SCREEN_HEIGHT-(j*ROW_HEIGHT);
                        double dist = sqrt((x-bx)*(x-bx) + (y-by)*(y-by));
                        if(dist < BUBBLE_RADIUS*2.5 && dist < minDist){
                            minDist = dist;
                            hitRow = j;
                            hitCol = i;
                            found = true;
                        }
                    }
                }
            } else {
                for(int i=0; i<MAX_COLS_ODD; i++){
                    if(bubbles[j][i].v==1){
                        double bx = (i*BUBBLE_SPACING)+BUBBLE_SPACING;
                        double by = SCREEN_HEIGHT-(j*ROW_HEIGHT);
                        double dist = sqrt((x-bx)*(x-bx) + (y-by)*(y-by));
                        if(dist < BUBBLE_RADIUS*2.5 && dist < minDist){
                            minDist = dist;
                            hitRow = j;
                            hitCol = i;
                            found = true;
                        }
                    }
                }
            }
        }
        
        if(found && hitRow >= 0 && hitCol >= 0){
            // Attach to existing bubble - find best position
            int newRow, newCol;
            findNearestBubblePosition(x, y, newRow, newCol);
            
            // Try to place in nearest empty spot
            if(newRow < MAX_ROWS){
                if(newRow%2==0 && newCol < MAX_COLS_EVEN){
                    if(bubbles[newRow][newCol].v==0){
                        bubbles[newRow][newCol].v=1;
                        bubbles[newRow][newCol].vi=0;
                        bubbles[newRow][newCol].color=c1;
                        bubbles[newRow][newCol].powerup=currentPowerup;  // Transfer powerup
                        currentPowerup=0;  // Reset after use
                        burst(newRow, newCol, c1);
                    } else {
                        // Try adjacent positions
                        bool placed = false;
                        for(int offset = 1; offset <= 3 && !placed; offset++){
                            if(newCol >= offset && bubbles[newRow][newCol-offset].v==0){
                                bubbles[newRow][newCol-offset].v=1;
                                bubbles[newRow][newCol-offset].vi=0;
                                bubbles[newRow][newCol-offset].color=c1;
                                bubbles[newRow][newCol-offset].powerup=currentPowerup;
                                currentPowerup=0;
                                burst(newRow, newCol-offset, c1);
                                placed = true;
                            } else if(newCol+offset < MAX_COLS_EVEN && bubbles[newRow][newCol+offset].v==0){
                                bubbles[newRow][newCol+offset].v=1;
                                bubbles[newRow][newCol+offset].vi=0;
                                bubbles[newRow][newCol+offset].color=c1;
                                bubbles[newRow][newCol+offset].powerup=currentPowerup;
                                currentPowerup=0;
                                burst(newRow, newCol+offset, c1);
                                placed = true;
                            }
                        }
                        if(!placed){
                            // Activate powerup on hit bubble if it has one
                            if(bubbles[hitRow][hitCol].powerup > 0){
                                currentPowerup = bubbles[hitRow][hitCol].powerup;
                                bubbles[hitRow][hitCol].powerup = 0;
                            }
                            burst(hitRow, hitCol, c1);
                        }
                    }
                } else if(newRow%2==1 && newCol < MAX_COLS_ODD){
                    if(bubbles[newRow][newCol].v==0){
                        bubbles[newRow][newCol].v=1;
                        bubbles[newRow][newCol].vi=0;
                        bubbles[newRow][newCol].color=c1;
                        bubbles[newRow][newCol].powerup=currentPowerup;
                        currentPowerup=0;
                        burst(newRow, newCol, c1);
                    } else {
                        bool placed = false;
                        for(int offset = 1; offset <= 3 && !placed; offset++){
                            if(newCol >= offset && bubbles[newRow][newCol-offset].v==0){
                                bubbles[newRow][newCol-offset].v=1;
                                bubbles[newRow][newCol-offset].vi=0;
                                bubbles[newRow][newCol-offset].color=c1;
                                bubbles[newRow][newCol-offset].powerup=currentPowerup;
                                currentPowerup=0;
                                burst(newRow, newCol-offset, c1);
                                placed = true;
                            } else if(newCol+offset < MAX_COLS_ODD && bubbles[newRow][newCol+offset].v==0){
                                bubbles[newRow][newCol+offset].v=1;
                                bubbles[newRow][newCol+offset].vi=0;
                                bubbles[newRow][newCol+offset].color=c1;
                                bubbles[newRow][newCol+offset].powerup=currentPowerup;
                                currentPowerup=0;
                                burst(newRow, newCol+offset, c1);
                                placed = true;
                            }
                        }
                        if(!placed){
                            if(bubbles[hitRow][hitCol].powerup > 0){
                                currentPowerup = bubbles[hitRow][hitCol].powerup;
                                bubbles[hitRow][hitCol].powerup = 0;
                            }
                            burst(hitRow, hitCol, c1);
                        }
                    }
                }
            }
            // Handle double balls powerup - shoot second ball immediately
            if(doubleBallsActive > 0){
                doubleBallsActive--;
                if(doubleBallsActive == 0){
                    currentPowerup = 0;
                    c1=rand1();
                    reset=1;x=xs;y=0;
                } else {
                    // Shoot second ball immediately for double balls
                    x=xs;y=0;  // Reset position
                    reset=0;  // Don't reset, shoot again
                    theta1=theta;  // Use same angle
                    Beep(150, 40);  // Second shot sound
                }
            } else {
                c1=rand1();
                if(currentPowerup == 3) currentPowerup = 0;  // Reset piercing after use
                reset=1;x=xs;y=0;
            }
            turnCount++;
            if(turnCount%5==0) addNewRow();
        } else {
            // No bubble hit, place at top based on x position
            int newRow, newCol;
            findNearestBubblePosition(x, SCREEN_HEIGHT-50, newRow, newCol);
            if(newRow < MAX_ROWS){
                if(newRow%2==0 && newCol < MAX_COLS_EVEN){
                    bubbles[newRow][newCol].v=1;
                    bubbles[newRow][newCol].vi=0;
                    bubbles[newRow][newCol].color=c1;
                    bubbles[newRow][newCol].powerup=currentPowerup;
                    currentPowerup=0;
                    burst(newRow, newCol, c1);
                } else if(newRow%2==1 && newCol < MAX_COLS_ODD){
                    bubbles[newRow][newCol].v=1;
                    bubbles[newRow][newCol].vi=0;
                    bubbles[newRow][newCol].color=c1;
                    bubbles[newRow][newCol].powerup=currentPowerup;
                    currentPowerup=0;
                    burst(newRow, newCol, c1);
                }
            }
        // Handle double balls powerup
        if(doubleBallsActive > 0){
            doubleBallsActive--;
            if(doubleBallsActive == 0){
                currentPowerup = 0;
                c1=rand1();
                reset=1;x=xs;y=0;
            } else {
                // Shoot second ball immediately
                x=xs;y=0;
                reset=0;
                theta1=theta;
                Beep(150, 40);
            }
        } else {
            c1=rand1();
            if(currentPowerup == 3) currentPowerup = 0;
            reset=1;x=xs;y=0;
        }
        turnCount++;
        if(turnCount%5==0) addNewRow();
        }
    }
    if(reset==0){
        glutPostRedisplay();
        points_to_go();
    }
}

void game_over(){
        //glPointSize(10);
        glColor3f(1.0,0,0);
        glRasterPos2i(170,170);
        if(ck!=5){
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'G');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'A');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'M');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'E');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,' ');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'O');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'V');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'E');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'R');}
        else{
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'C');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'O');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'N');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'G');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'R');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'A');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'T');
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'S');
        }
        // Removed glFlush() - using double buffering
}


void keyboard(unsigned char key, int x, int y){
    if(key==27) exit(0);  // ESC to exit
}

void mymouse(int button,int state,int x1,int y1){
    if(button==GLUT_LEFT_BUTTON&&state==GLUT_DOWN){
        if(max2<=END_LINE_Y){
            gover=true;
        }
        if(!gover){
            theta1=theta;
            reset=0;
            max1=max2;
            points_to_go();
        }
    }
}

void points_to_go(){
    if(reset==0){
        if(x<BUBBLE_RADIUS){x=BUBBLE_RADIUS;theta1=PI-theta1;}
        if(x>SCREEN_WIDTH-BUBBLE_RADIUS){x=SCREEN_WIDTH-BUBBLE_RADIUS;theta1=PI-theta1;}
        if(y>SCREEN_HEIGHT-50){
            y=SCREEN_HEIGHT-50;
        // Place bubble at top based on x position
        int newRow = 0, newCol;
        findNearestBubblePosition(x, y, newRow, newCol);
        if(newRow < MAX_ROWS){
            if(newRow%2==0 && newCol < MAX_COLS_EVEN){
                bubbles[newRow][newCol].v=1;
                bubbles[newRow][newCol].vi=0;
                bubbles[newRow][newCol].color=c1;
                bubbles[newRow][newCol].powerup=currentPowerup;
                currentPowerup=0;
                burst(newRow, newCol, c1);
            } else if(newRow%2==1 && newCol < MAX_COLS_ODD){
                bubbles[newRow][newCol].v=1;
                bubbles[newRow][newCol].vi=0;
                bubbles[newRow][newCol].color=c1;
                bubbles[newRow][newCol].powerup=currentPowerup;
                currentPowerup=0;
                burst(newRow, newCol, c1);
            }
        }
        // Handle double balls powerup
        if(doubleBallsActive > 0){
            doubleBallsActive--;
            if(doubleBallsActive == 0){
                currentPowerup = 0;
                c1=rand1();
                reset=1;x=xs;y=0;
            } else {
                // Shoot second ball immediately
                x=xs;y=0;
                reset=0;
                theta1=theta;
                Beep(150, 40);
            }
        } else {
            c1=rand1();
            if(currentPowerup == 3) currentPowerup = 0;
            reset=1;x=xs;y=0;
        }
            turnCount++;
            if(turnCount%5==0) addNewRow();
        }
    }
    if(reset==0)
        glutTimerFunc(16,timer,0);  // ~60 FPS
}

// Simple arcade-style music using Beep
void playBackgroundMusic(){
    static int musicCounter = 0;
    static int lastMusicTime = 0;
    int currentTime = GetTickCount();
    
    // Play a simple arcade-style melody every 3 seconds (less frequent to avoid interference)
    if(currentTime - lastMusicTime > 3000){
        musicCounter++;
        int note = musicCounter % 8;
        switch(note){
            case 0: Beep(523, 80); break;  // C
            case 1: Beep(587, 80); break;  // D
            case 2: Beep(659, 80); break;  // E
            case 3: Beep(698, 80); break;  // F
            case 4: Beep(784, 80); break;  // G
            case 5: Beep(880, 80); break;  // A
            case 6: Beep(988, 80); break;  // B
            case 7: Beep(1047, 120); break; // C (high)
        }
        lastMusicTime = currentTime;
    }
}

static void idle(void)
{
    // Play background music
    if(musicPlaying){
        playBackgroundMusic();
    }
    glutPostRedisplay();
}



int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutInitWindowPosition(0,0);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

    glutCreateWindow("BallsBlaster - Enhanced Bubble Shooter");
    glutFullScreen();  // Start in fullscreen
    srand(time(NULL));  // Initialize random seed
    inits();
    c1=rand1();  // Initialize first bubble color after bubbles are set up
    
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutSpecialFunc(key);
    glutMouseFunc(mymouse);
    glutPassiveMotionFunc(mouse);
    glutKeyboardFunc(keyboard);
    
    glClearColor(0.05,0.05,0.15,1);
    gluOrtho2D(0,SCREEN_WIDTH,0,SCREEN_HEIGHT);
    
    // Start background music
    musicPlaying = true;
    glutIdleFunc(idle);  // Enable idle function for music
    
    glutMainLoop();
    return EXIT_SUCCESS;
}
