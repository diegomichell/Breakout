//
// breakout.c
//
// Computer Science 50
// Problem Set 4
// Create By: Diego Ivan Perez Michel

// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define HEIGHT 550
#define WIDTH 410

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 8

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

//Paddle dimensions
#define PADDLE_WIDTH 60
#define PADDLE_HEIGHT 10

#define BALL_HEIGHT 17
#define BALL_WIDTH 17

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRoundRect initPaddle(GWindow window);
GLabel initScoreboard(GWindow window);
void updateScoreboard(GWindow window, GLabel label, int points);
GObject detectCollision(GWindow window, GOval ball);
void restarPostionAndVelocityOfBall(GOval ball, double* velocityX, double* velocityY);
GLabel initLivesLabel (GWindow window);
GLabel initLivesValueLabel(GWindow window,GLabel owner, int lives);
void updateLivesValue(GWindow window, GLabel label, int lives);

int main(void)
{
    // seed pseudorandom number generator
    srand48(time(NULL));
    
    // instantiate window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // instantiate bricks
    initBricks(window);

    // instantiate ball, centered in middle of window
    GOval ball = initBall(window);

    // instantiate paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // instantiate scoreboard, centered in middle of window, just above ball
    GLabel label = initScoreboard(window);
    
    // instantiate lives label, and put it at the top left corner
    GLabel livesLabel = initLivesLabel(window);
    // instantiate a sepate label that will containt the lives value
    GLabel livesValueLabel = initLivesValueLabel(window,livesLabel,LIVES);
    
    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

    //Waits for click before start
    setLabel(label,"Click");
    waitForClick();
    setLabel(label,"0");
    
    //PADLE HEIGHT LEVEL
    double y = HEIGHT - PADDLE_HEIGHT * 4;
    
    //Ball velocity containers
    double velocityX = 0;
    double velocityY = 0;
    
    restarPostionAndVelocityOfBall(ball, &velocityX, &velocityY);
    
    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {   
        //Listen for Events
        GEvent mouseEvent = getNextEvent(MOUSE_EVENT);
        
        //BALL
        
        move(ball, velocityX, velocityY);
        
        // bounce off right edge of window
        if ((getX(ball) + getWidth(ball)) >= getWidth(window))
        {
            //printf("BOUNCE RIGHT\n");
            velocityX = -velocityX;
        }
        // bounce off left edge of window
        else if (getX(ball) <= 0)
        {
            //printf("BOUNCE LEFT\n");
            velocityX = -velocityX;
        }
         // bounce off BOTTOM edge of window
        else if (getY(ball) + getHeight(ball) >= getHeight(window))
        {
            //printf("BOUNCE BOTTOM\n");
            lives -= 1;
            updateLivesValue(window, livesValueLabel, lives);
            restarPostionAndVelocityOfBall(ball, &velocityX, &velocityY);
        }
        // bounce off TOP edge of window
        else if (getY(ball) <= 0)
        {
            velocityY = -velocityY;
        }
        
        
        // COLLISION
        GObject objectHit = detectCollision(window,ball);
        
        if (objectHit != NULL)
        {
            //the paddle was hit
            if(objectHit == paddle)
            {   
                velocityY = -velocityY;
            }
            
            if(strcmp(getType(objectHit),"GRect") == 0)
            {
                if (objectHit != paddle)
                {
                    //A brick desappears
                    removeGWindow(window,objectHit);
                    bricks -= 1;
                    
                    //You gain a point
                    points += 1;
                    
                    updateScoreboard(window,label,points);
                    
                    //BOUNCE ON BRICK
                    velocityY = -velocityY;
                    
                }
            }
        }
        
        //PADDLE
        
        //If we heard one event
        if (mouseEvent != NULL)
        {
            // if the event was movement
            if (getEventType(mouseEvent) == MOUSE_MOVED)
            {
                
                double x = getX(mouseEvent) - PADDLE_WIDTH / 2;
                
                if (x > 0 && (x + PADDLE_WIDTH) < WIDTH)
                {   
                    
                    setLocation(paddle,x,y);
                }
                
                
            }
            
        }
        
        
        //Animation speed
        pause(5);
    }

    setLocation(label,getX(label) - getWidth(label) * 4, getY(label));
    setLabel(label,"Game Over!");
    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}
/**
* Restars the position and velocity of the ball.
*/
void restarPostionAndVelocityOfBall(GOval ball, double* velocityX, double* velocityY)
{
    double rand = drand48();
    
    if (rand >= 0 && rand < 0.33)
    {
         *velocityX = -1;
         *velocityY = 1.5;   
    }
    else if (rand >= 0.33 && rand < 0.66)
    {
        *velocityX = 2;
        *velocityY = 1.75;
    }
    else
    {
        *velocityX = 1.25;
        *velocityY = 1;
    }
    
    double x = (WIDTH - BALL_WIDTH) / 2;
    double y = (HEIGHT - BALL_HEIGHT) / 2;
            
    setLocation(ball,x,y);
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    double x = 10;
    double y = 40;
    char* colors[] = {"RED","BLUE","GREEN","CYAN","ORANGE"};
    
    for(int v = 0; v < ROWS; v++)
    {       
        
        for(int h =0; h < COLS;h++)
        {
            GRect rect = newGRect(x,y,40,10);
            setColor(rect,colors[v]);
            setFilled(rect, true);
            
            add(window,rect);  
                  
            x+= 50;
            
        }
        
         y+= 20;
         x = 10;
         
         //Pause for animation purpose.
         pause(100);
    }
    
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    //Properties
    double x = (getWidth(window) - BALL_WIDTH) / 2;
    double y = (getHeight(window) - BALL_HEIGHT) / 2;
    
    GOval ball = newGOval(x, y, BALL_WIDTH, BALL_HEIGHT);
   
    setFilled(ball,true);
    
    add(window,ball);
    
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRoundRect initPaddle(GWindow window)
{
    //Properties
    double x = (WIDTH - PADDLE_WIDTH) / 2;
    double y = HEIGHT - PADDLE_HEIGHT * 4;
    double corner = 10;
    char* color = "GREEN";
    
    GRoundRect paddle = newGRoundRect(x,y,PADDLE_WIDTH,PADDLE_HEIGHT,corner);
    setColor(paddle,color);
    setFilled(paddle,true);
    
    add(window,paddle);
    
    return paddle;
}

/**
 * Instantiates, configures, and returns label for scoreboard.
 */
GLabel initScoreboard(GWindow window)
{
    GLabel label = newGLabel("0");
    setFont(label, "SansSerif-36");
    setColor(label, "GRAY");
    
    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
    
    add(window,label);
    
    return label;
}

/**
* Inits the live's label
*/
GLabel initLivesLabel (GWindow window) {
  
    GLabel label = newGLabel("Lives:");
    setFont(label, "SansSerif-16");
    setColor(label, "GRAY");
    
    // top left corner in window
    double x = 10;
    double y = 20;
    setLocation(label, x, y);
    
    add(window,label);
    
    return label;
}

/**
* Inits the live's value label
*/
GLabel initLivesValueLabel (GWindow window, GLabel owner, int lives)
{   
    //Buffer
    char s[1];
    sprintf(s, "%i", lives);
    
    GLabel label = newGLabel(s);
    setFont(label, "SansSerif-16");
    setColor(label, "ORANGE");
    
    // top left corner in window
    double x = (getX(owner) + getWidth(owner)) + 5;
    double y = 20;
    setLocation(label, x, y);
    
    add(window,label);
    
    return label;
}

/**
 * Updates live's value label
 */
void updateLivesValue(GWindow window, GLabel label, int lives)
{
    // update label
    char s[1];
    sprintf(s, "%i", lives);
    setLabel(label, s);
}

/**
 * Updates scoreboard's label, keeping it centered in window.
 */
void updateScoreboard(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label in window
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GOval ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}
