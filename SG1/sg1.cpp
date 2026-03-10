/*
  This program plots different 2D functions.
*/

#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <iostream>
//#include "glut.h" //MSVC local library install
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include "glut.h" //system-wide install (or compiler default path)
#endif

double circle = atan(1) * 8;
double halfCircle = atan(1) * 4;
double tau = circle; // 2 * PI = TAU
double pi = halfCircle; // TAU / 2 = PI
double epsilon = 1e-15;

//How often should the drawing algorithm sample the function.
double step = 0.05;


int defaultW = 1000, defaultH = 1000;

unsigned char prevKey;

/*
   Nicomedes' Conchoid
   $x = a + b \cdot cos(t), y = a \cdot tg(t) + b \cdot sin(t)$. or
   $x = a - b \cdot cos(t), y = a \cdot tg(t) - b \cdot sin(t)$. where
   $t \in (-\pi / 2, \pi / 2)$
*/
void Display1() {
    double xmax, ymax, xmin, ymin;
    /*
      Nicomedes' Conchoid is a family of functions. These two parameters,
      a and b, choose a specific 2D function from that family.
      It has two branches, so, for each y, we have two x values.
      Therefore, we're actually drawing two lines, not one.
     */
    double a = 1, b = 2;

    /*
      We shadow the global variable with a different step size,
      as values different from 0.05
      would yield a significantly different plot.
    */
    double step = 0.05;

    /*
      First, we compute the points of the function, so we can determine
      the maximal extend of the drawing.
     */
    xmax = a - b - 1;
    xmin = a + b + 1;
    ymax = ymin = 0;
    for (double t = -pi / 2 + step; t < pi / 2; t += step) {
        double x1, y1, x2, y2;
        x1 = a + b * cos(t);
        xmax = (xmax < x1) ? x1 : xmax;
        xmin = (xmin > x1) ? x1 : xmin;

        x2 = a - b * cos(t);
        xmax = (xmax < x2) ? x2 : xmax;
        xmin = (xmin > x2) ? x2 : xmin;

        y1 = a * tan(t) + b * sin(t);
        ymax = (ymax < y1) ? y1 : ymax;
        ymin = (ymin > y1) ? y1 : ymin;

        y2 = a * tan(t) - b * sin(t);
        ymax = (ymax < y2) ? y2 : ymax;
        ymin = (ymin > y2) ? y2 : ymin;
    }
    //We care about the maximal extent on each axis (from the origin).
    xmax = (fabs(xmax) > fabs(xmin)) ? fabs(xmax) : fabs(xmin);
    ymax = (fabs(ymax) > fabs(ymin)) ? fabs(ymax) : fabs(ymin);

    /*
      Since we have, for x and for y, the maximal absolute values,
      dividing the coordinates of the points by these values will
      ensure we draw the whole function inside [-1, 1]^2, the default
      OpenGL screen.
     */

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (double t = -pi / 2 + step; t < pi / 2; t += step) {
        double x1, y1, x2, y2; //You might get some warnings in your IDE. Why?
        x1 = (a + b * cos(t)) / xmax;
        x2 = (a - b * cos(t)) / xmax;
        y1 = (a * tan(t) + b * sin(t)) / ymax;
        y2 = (a * tan(t) - b * sin(t)) / ymax;

        glVertex2d(x1, y1);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (double t = -pi / 2 + step; t < pi / 2; t += step) {
        double x1, y1, x2, y2;
        x1 = (a + b * cos(t)) / xmax;
        x2 = (a - b * cos(t)) / xmax;
        y1 = (a * tan(t) + b * sin(t)) / ymax;
        y2 = (a * tan(t) - b * sin(t)) / ymax;

        glVertex2d(x2, y2);
    }
    glEnd();
}

// $f(x) = \left| sin(x) \right| \cdot e^{-sin(x)}, x \in \left[ 0, 8 \cdot \pi \right]$, 
void Display2() {
    /*
      We can determine how far the function extends
      (and thus the needed scaling factors)
      by looking at the function and doing a bit of Calculus.
    */
    double xmax = 8 * pi;
    double ymax = exp(1.1); //Why 1.1?

    glColor3f(1, 0.1, 0.1);
    glBegin(GL_LINE_STRIP);
    for (double x = 0; x < xmax; x += step) {
        double x1, y1;
        x1 = x / xmax;
        y1 = (fabs(sin(x)) * exp(-sin(x))) / ymax;
        glVertex2d(x1, y1);
    }
    glEnd();
}

/* 1)
   \( f(x) =
     \left\{
       \begin{array}{cl}
         1              & x = 0   \\
         \frac{d(x)}{x} & x \gt 0 \\
       \end{array}
     \right.
   \)
 */
void Display3() {
    double xMin = 0, xMax = 20;
    double yMin = 1000, yMax = 0;
    
    for (double x = xMin; x <= xMax; x += step) {
        double dx = fabs(x - round(x));
        double y;
        if (x == 0)
            y = 1;
        else
            y = dx / x;
        
        yMax = (yMax < y) ? y : yMax;
        yMin = (yMin > y) ? y : yMin;
    }

    yMax = (fabs(yMax) > fabs(yMin)) ? fabs(yMax) : fabs(yMin);
 
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (double x = xMin; x <= xMax; x += step) {
        double x1, y1;
        if (x == 0) {
            y1 = 1 / yMax;
        }
        else {
            double dx = fabs(x - round(x));
            y1 = (dx / x) / yMax;
        }
        x1 = x / xMax;
        glVertex2d(x1 * 0.9, y1 * 0.9);
    }
    glEnd();
}

//3) function arguments e.g.: f(a, b, t), where a and b are function family parameters, and the is the driving variables.
void plot(double (*x)(double, double, double), double (*y)(double, double, double), double a, double b, double intervalStart, double intervalEnd, double step = 0.01, double scaleX = 1, double scaleY = 1, GLint primitive = GL_LINE_STRIP) {

    // glColor3f(1.0f, 0.08f, 0.58f);
    glBegin(primitive);
    for (double t = intervalStart; t < intervalEnd; t += step) {
        double x1, y1;
        x1 = x(t, a, b) / scaleX;
        y1 = y(t, a, b) / scaleY;

        //variabile pentru a colora graficul
        float factor = (float)(t / tau);
        float r = 1.0f + factor * (0.2f - 1.0f);
        float g = 0.0f + factor * (0.1f - 0.0f);
        float b_col = 0.0f + factor * (0.1f - 0.0f);

        glColor3f(r, g, b_col);

        glVertex2d(x1 * 0.95, y1 * 0.95);
    }

    glEnd();
}

/*
  2) Circle Concoid (Limaçon, Pascal's Snail):
  \(x = 2 \cdot (a \cdot cos(t) + b) \cdot cos(t), \; y = 2 \cdot (a \cdot cos(t) + b) \cdot sin(t), \; t \in (-\pi, \pi)\) .
  For this plot, \(a = 0.3, \; b = 0.2\) .
*/

double xCircleConcoid(double t, double a, double b) {
    return 2 * (a * cos(t) + b) * cos(t);
}

double yCircleConcoid(double t, double a, double b) {
    return 2 * (a * cos(t) + b) * sin(t);
}


void Display4() {
    double a = 0.3, b = 0.2;
    
    double xMin = 1000, xMax = 0;
    double yMin = 1000, yMax = 0;

    for (double t = -pi + epsilon; t < pi; t += step) {
        double x1, y1;
        x1 = xCircleConcoid(t, a, b);
        y1 = yCircleConcoid(t, a, b);

        xMax = (xMax < x1) ? x1 : xMax;
        xMin = (xMin > x1) ? x1 : xMin;

        yMax = (yMax < y1) ? y1 : yMax;
        yMin = (yMin > y1) ? y1 : yMin;
    }

    yMax = (fabs(yMax) > fabs(yMin)) ? fabs(yMax) : fabs(yMin);
    xMax = (fabs(xMax) > fabs(xMin)) ? fabs(xMax) : fabs(xMin);

    /*
    glColor3f(1.0f, 0.08f, 0.58f);
    glBegin(GL_LINE_LOOP);
    for (double t = -pi + epsilon; t < pi; t += step) {
        double x1, y1;
        x1 = xCircleConcoid(t, a, b) / xMax;
        y1 = yCircleConcoid(t, a, b) / yMax;
        glVertex2d(x1 * 0.95, y1 * 0.95);
    }

    glEnd();
    */

    plot(xCircleConcoid, yCircleConcoid, a, b, -pi + epsilon, pi, step, xMax, yMax, GL_LINE_LOOP);
}

/*
  2) Cicloid:
  \(x = a \cdot t - b \cdot sin(t), \; y = a - b \cdot cos(t), \; t \in \mathbb{R} \) .
  For this plot, \(a = 0.1, \; b = 0.2\) .
*/

double xCicloid(double t, double a, double b) {
    return a * t - b * sin(t);
}

double yCicloid(double t, double a, double b) {
    return a - b * cos(t);
}

void Display5() {
    double a = 0.1, b = 0.2;

    
    double tMin = -pi * 3 ;
    double tMax = pi * 3;

    double xMin = a * tMin - b, xMax = a * tMax + b;

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (double t = tMin; t < tMax; t+=step) {
        double x1, y1;

        x1 = xCicloid(t, a, b) / xMax;
        y1 = yCicloid(t, a, b);

        glVertex2d(x1 * 0.95, y1 * 0.95);
    }

    glEnd();
    

   // plot(xCicloid, yCicloid, a, b, tMin, tMax, step, xMax, yMax, GL_LINE_STRIP);

}

/*
  2) Epicicloid:
  \(x = (a + b) \cdot cos\left( \frac{b}{a} \cdot t \right) - b \cdot cos\left(t + \frac{b}{a}\cdot t \right) \)
  \(y = (a + b) \cdot sin\left( \frac{b}{a} \cdot t \right) - b \cdot sin\left(t + \frac{b}{a}\cdot t \right) \)
  \( t \in \left[ 0, 2\pi \right] \) .
  For this plot, \(a = 0.1, \; b = 0.3\) .
*/
double xEpicicloid(double t, double a, double b) {
    return (a + b) * cos(b / a * t) - b * cos(t + b / a * t);
}

double yEpicicloid(double t, double a, double b) {
    return (a + b) * sin(b / a * t) - b * sin(t + b / a * t);
}

void Display6() {
    double a = 0.1, b = 0.3;

    
    double yMin = 1000, yMax = 0;
    double xMin = 1000, xMax = 0;

    for (double t = 0; t <= tau; t += step) {
        double x1, y1;

        x1 = xEpicicloid(t, a, b);
        y1 = yEpicicloid(t, a, b);

        xMax = (xMax < x1) ? x1 : xMax;
        xMin = (xMin > x1) ? x1 : xMin;

        yMax = (yMax < y1) ? y1 : yMax;
        yMin = (yMin > y1) ? y1 : yMin;
    }

    yMax = (fabs(yMax) > fabs(yMin)) ? fabs(yMax) : fabs(yMin);
    xMax = (fabs(xMax) > fabs(xMin)) ? fabs(xMax) : fabs(xMin);

    /*
    glColor3f(1.0f, 0.08f, 0.58f);
    glBegin(GL_LINE_STRIP);
    for (double t = 0; t <= tau; t += step) {
        double x1, y1;

        x1 = xEpicicloid(t, a, b) / xMax;
        y1 = yEpicicloid(t, a, b) / yMax;

        glVertex2d(x1 * 0.95, y1 * 0.95);
    }
    glEnd();
    */

    plot(xEpicicloid, yEpicicloid, a, b, 0, tau, step, xMax, yMax, GL_LINE_STRIP);

}

/*
  2) Hipocicloid:
  \(x = (a - b) \cdot cos\left( \frac{b}{a} \cdot t \right) - b \cdot cos\left(t - \frac{b}{a}\cdot t \right) \)
  \(y = (a - b) \cdot sin\left( \frac{b}{a} \cdot t \right) - b \cdot sin\left(t - \frac{b}{a}\cdot t \right) \)
  \( t \in \left[ 0, 2\pi \right] \) .
  For this plot, \(a = 0.1, \; b = 0.3\) .
 */

double xHipocicloid(double t, double a, double b) {
    return (a - b) * cos(b / a * t) - b * cos(t - b / a * t);
}

double yHipocicloid(double t, double a, double b) {
    return (a - b) * sin(b / a * t) - b * sin(t - b / a * t);
}

void Display7() {
    double a = 0.1, b = 0.3;

    
    double yMin = 1000, yMax = 0;
    double xMin = 1000, xMax = 0;

    for (double t = 0; t <= tau; t += step) {
        double x1, y1;

        x1 = xHipocicloid(t, a, b);
        y1 = yHipocicloid(t, a, b);

        xMax = (xMax < x1) ? x1 : xMax;
        xMin = (xMin > x1) ? x1 : xMin;

        yMax = (yMax < y1) ? y1 : yMax;
        yMin = (yMin > y1) ? y1 : yMin;
    }

    yMax = (fabs(yMax) > fabs(yMin)) ? fabs(yMax) : fabs(yMin);
    xMax = (fabs(xMax) > fabs(xMin)) ? fabs(xMax) : fabs(xMin);

    /*
    glColor3f(1.0f, 0.08f, 0.58f);
    glBegin(GL_LINE_STRIP);
    for (double t = 0; t <= tau; t += step) {
        double x1, y1;

        x1 = xHipocicloid(t, a, b) / xMax;
        y1 = yHipocicloid(t, a, b) / yMax;

        glVertex2d(x1 * 0.95, y1 * 0.95);
    }
    glEnd();
    */

    plot(xHipocicloid, yHipocicloid, a, b, 0, tau, step, xMax, yMax, GL_LINE_STRIP);
}

/*
 4) Logarithmic spiral (in polar coordinates):
 \( r = a \cdot e^{1+t}, \; t \in (0, \infty) \) .
 For this plot, \(a = 0.02\) .
*/

double xPolarLogarithmic(double t, double a, double undefine = 0.0) {
    double r = a * exp(1 + t);

    return r * cos(t - 3 * pi / 4);
}

double yPolarLogarithmic(double t, double a, double undefine = 0.0) {
    double r = a * exp(1 + t);

    return r * sin( t - 3 * pi / 4);
}

void Display8() {
    double a = 0.02;
    double tMin = 0;
    double tMax = 2 * pi; 

    double xMax = 0, yMax = 0;

    for (double t = 0; t <= tau; t += step) {
        double x1, y1;

        x1 = xPolarLogarithmic(t, a, 0.0);
        y1 = yPolarLogarithmic(t, a, 0.0);

        xMax = (xMax < fabs(x1)) ? fabs(x1) : xMax;
        yMax = (yMax < fabs(y1)) ? fabs(y1) : yMax;
    }

    xMax = (fabs(xMax) > fabs(yMax)) ? fabs(xMax) : fabs(yMax);


    plot(xPolarLogarithmic, yPolarLogarithmic, a, 0.0, tMin, tMax + step, step, xMax, xMax, GL_LINE_STRIP);
}

/*
  4) Sine polar plot flower:
  \( r = sin(a \cdot t), \; t \in (0, \infty)  \) .
  For this plot, \(a = 10\), and the number 'petals' is \( 2 \cdot a \). Think about why.
*/
double xPolarFlower(double t, double a, double undefine = 0.0) {
    double r = sin(a * t);

    return r * cos(t);
}

double yPolarFlower(double t, double a, double undefine = 0.0) {
    double r = sin(a*t);

    return r * sin(t);
}

void Display9() {

    double a = 10.0;

    plot(xPolarFlower, yPolarFlower, a, 0.0, 0, tau + 0.01, 0.01, 0.95, 0.95, GL_LINE_LOOP);
}

/*
5) Longchamps' Trisectrix:
\(
x = \frac{a}{4 \cdot cos^2(t) - 3}, \;
y = \frac{a \cdot tg(t)}{4 \cdot cos^2(t) - 3}, \;
t \in (-\pi/2, \pi/2) \setminus \{ -\pi/6, \pi/6 \} \) .
For this plot, \(a = 0.2\) .
 */
double xTrefoil(double t, double a) {
    double numitor = a;
    double numarator = 4 * cos(t) * cos(t) - 3;
    return numitor / numarator;
}

double yTrefoil(double t, double a) {
    double numitor = a * tan(t);
    double numarator = 4 * cos(t) * cos(t) - 3;
    return numitor / numarator;
}


void Display10() {
    double step = 0.018;
    double a = 0.2;

    // punctul de unde incep triunghiurile in stanga
    double xStartingPoint = -0.97;
    double yStartingPoint = 0.95;

 
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    glVertex2f(xStartingPoint, yStartingPoint);

    //parcurgem intervalul (-pi/2,pi/2)
    for (double t = -pi / 2 + step; t < pi / 2 - step; t += step) {
        double x1, y1;

        //verificam ca t-ul curent sa nu apartine multimii {-pi/6,pi/6}
        if (fabs(t) - pi / 6 > epsilon) {
            x1 = xTrefoil(t, a) ;
            y1 = yTrefoil(t, a) ;

            // fortam conturul sa fie in cadranul 1
            if ( x1> -1 && x1 < 0 && y1 > 0 && y1 < 1) {
                if (y1 > 0 && y1 < 0.19) {
                    glColor3f(1, 0, 0);
                }
                glVertex2f(x1, y1);
            }
        }
    }
    glVertex2f(xStartingPoint, yStartingPoint);
    glEnd();

   
    step = 0.036;
    glColor3f(0.1, 0.1, 0.1);
    for (double t = -pi / 2 ; t < pi / 2 - step / 2; t += step) {
        if (fabs(t) - pi / 6 > epsilon) {
            double x1 = xTrefoil(t, a);
            double y1 = yTrefoil(t, a);
            if (x1>-1 && x1 < 0 && y1 > 0 && y1 < 1) {
                double y2 = yTrefoil(t + step/2, a);
                double x2 = xTrefoil(t + step/2, a);
                glBegin(GL_TRIANGLES);

                if (y1 > 0 && y1 < 0.18) {
                    glColor3f(1, 0.1, 0.1);
                }
                glVertex2f(xStartingPoint, yStartingPoint);
                glVertex2f(x1, y1);
                glVertex2f(x2, y2);

                glEnd();
            }
        }
    }

    glBegin(GL_TRIANGLES); {
        glColor3f(0.9, 0.1, 0.1);
        glVertex2f(xStartingPoint, yStartingPoint);
        glVertex2f(-1, yStartingPoint + a / 10);
        glVertex2f(-1, yStartingPoint - a / 10 );
    }
    glEnd();

  
}

void init(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glLineWidth(2);
    glPointSize(1);
    //glPolygonMode(GL_FRONT, GL_LINE);
    //Enabling blending and smoothing
    glEnable(GL_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_NICEST, GL_POINT_SMOOTH_HINT);
    glHint(GL_NICEST, GL_LINE_SMOOTH_HINT);
    glHint(GL_NICEST, GL_POLYGON_SMOOTH_HINT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
}

void Display(void) {
    std::cout << ("Call Display") << std::endl;
    // Clear the buffer. See init();
    glClear(GL_COLOR_BUFFER_BIT);

    switch (prevKey) {
    case '1':
        Display1();
        break;
    case '2':
        Display2();
        break;
    case '3':
        Display3();
        break;
    case '4':
        Display4();
        break;
    case '5':
        Display5();
        break;
    case '6':
        Display6();
        break;
    case '7':
        Display7();
        break;
    case '8':
        Display8();
        break;
    case '9':
        Display9();
        break;
    case '0':
        Display10();
        break;
    default:
        break;
    }
    glFlush();
}

void Reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}
void KeyboardFunc(unsigned char key, int x, int y) {
    prevKey = key;
    if (key == 27) // escape
        exit(0);
    //The proper way to ask glut to redraw the window.
    glutPostRedisplay();
}

/*
  Callback upon mouse press or release.
  The button can be:
  GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
  (and further for mousewheel and other mouse buttons)
  The state can be either GLUT_DOWN or  GLUT_UP, for
  a pressed or released button.
  (x, y) are the coordinates of the mouse.
*/
void MouseFunc(int button, int state, int x, int y) {
    std::cout << "Mouse button ";
    std::cout << ((button == GLUT_LEFT_BUTTON) ? "left" : ((button == GLUT_RIGHT_BUTTON) ? "right" : "middle")) << " ";
    std::cout << ((state == GLUT_DOWN) ? "pressed" : "released");
    std::cout << " at coordinates: " << x << " x " << y << std::endl;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(defaultW, defaultH);
    glutInitWindowPosition(-1, -1);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutCreateWindow(argv[0]);
    init();
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(KeyboardFunc);
    glutMouseFunc(MouseFunc);
    glutDisplayFunc(Display);
    //glutIdleFunc(Display);
    glutMainLoop();

    return 0;
}
