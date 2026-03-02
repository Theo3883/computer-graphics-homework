/*
  This program plots different 2D functions.
*/

#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <iostream>
// #include "glut.h" //MSVC local library install
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h> //system-wide install (or compiler default path)
#endif
double circle = atan(1) * 8;
double halfCircle = atan(1) * 4;
double tau = circle;    // 2 * PI = TAU
double pi = halfCircle; // TAU / 2 = PI

// How often should the drawing algorithm sample the function.
double step = 0.05;

int defaultW = 1000, defaultH = 1000;

unsigned char prevKey;

/*
   Nicomedes' Conchoid
   $x = a + b \cdot cos(t), y = a \cdot tg(t) + b \cdot sin(t)$. or
   $x = a - b \cdot cos(t), y = a \cdot tg(t) - b \cdot sin(t)$. where
   $t \in (-\pi / 2, \pi / 2)$
*/
void Display1()
{
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
  for (double t = -pi / 2 + step; t < pi / 2; t += step)
  {
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
  // We care about the maximal extent on each axis (from the origin).
  xmax = (fabs(xmax) > fabs(xmin)) ? fabs(xmax) : fabs(xmin);
  ymax = (fabs(ymax) > fabs(ymin)) ? fabs(ymax) : fabs(ymin);

  /*
    Since we have, for x and for y, the maximal absolute values,
    dividing the coordinates of the points by these values will
    ensure we draw the whole function inside [-1, 1]^2, the default
    OpenGL screen.
   */

  glColor3f(1, 0.1, 0.1);
  glBegin(GL_LINE_STRIP);
  for (double t = -pi / 2 + step; t < pi / 2; t += step)
  {
    double x1, y1, x2, y2; // You might get some warnings in your IDE. Why?
    x1 = (a + b * cos(t)) / xmax;
    x2 = (a - b * cos(t)) / xmax;
    y1 = (a * tan(t) + b * sin(t)) / ymax;
    y2 = (a * tan(t) - b * sin(t)) / ymax;

    glVertex2d(x1, y1);
  }
  glEnd();

  glBegin(GL_LINE_STRIP);
  for (double t = -pi / 2 + step; t < pi / 2; t += step)
  {
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
void Display2()
{
  /*
    We can determine how far the function extends
    (and thus the needed scaling factors)
    by looking at the function and doing a bit of Calculus.
  */
  double xmax = 8 * pi;
  double ymax = exp(1.1); // Why 1.1?

  glColor3f(1, 0.1, 0.1);
  glBegin(GL_LINE_STRIP);
  for (double x = 0; x < xmax; x += step)
  {
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
   where d(x) = distance between x and the closest integer, x in [0, 100].
 */
void Display3()
{
  double xmax = 20.0;
  double ymax = 1.0; // f(x) max is 1 (on [0, 0.5])
  double step = 0.01;

  glColor3f(1, 0.1, 0.1);
  glBegin(GL_LINE_STRIP);

  // f(0) = 1 by definition; continuous since lim x->0+ d(x)/x = 1
  glVertex2d(0.0, 1.0);

  for (double x = step; x <= xmax; x += step)
  {
    double frac = x - floor(x);
    double d = (frac <= 0.5) ? frac : (1.0 - frac);
    double y = d / x;
    glVertex2d(x / xmax, y / ymax);
  }
  glEnd();
}

// 3) Generic plotting function for parametric curves f(a, b, t).
void plot(double (*fx)(double, double, double), double (*fy)(double, double, double),
         double a, double b, double intervalStart, double intervalEnd,
         double step = 0.01, double scaleX = 1, double scaleY = 1,
         GLint primitive = GL_LINE_STRIP)
{
  // If scale factors are 0, compute them automatically from bounds
  if (scaleX == 0 || scaleY == 0) {
    double xmax = 0, ymax = 0;
    for (double t = intervalStart; t <= intervalEnd; t += step) {
      double xv = fx(a, b, t);
      double yv = fy(a, b, t);
      if (fabs(xv) > xmax) xmax = fabs(xv);
      if (fabs(yv) > ymax) ymax = fabs(yv);
    }
    if (scaleX == 0) scaleX = (xmax > 0) ? xmax : 1;
    if (scaleY == 0) scaleY = (ymax > 0) ? ymax : 1;
  }

  glColor3f(1, 0.1, 0.1);
  glBegin(primitive);
  for (double t = intervalStart; t <= intervalEnd; t += step) {
    double xv = fx(a, b, t) / scaleX;
    double yv = fy(a, b, t) / scaleY;
    glVertex2d(xv, yv);
  }
  glEnd();
}

// --- Parametric function definitions for use with plot() ---

// Limaçon
double limacon_x(double a, double b, double t) { return 2 * (a * cos(t) + b) * cos(t); }
double limacon_y(double a, double b, double t) { return 2 * (a * cos(t) + b) * sin(t); }

// Cycloid
double cycloid_x(double a, double b, double t) { return a * t - b * sin(t); }
double cycloid_y(double a, double b, double t) { return a - b * cos(t); }

// Epicycloid
double epicycloid_x(double a, double b, double t) {
  double ratio = b / a;
  return (a + b) * cos(ratio * t) - b * cos(t + ratio * t);
}
double epicycloid_y(double a, double b, double t) {
  double ratio = b / a;
  return (a + b) * sin(ratio * t) - b * sin(t + ratio * t);
}

// Hypocycloid
double hypocycloid_x(double a, double b, double t) {
  double ratio = b / a;
  return (a - b) * cos(ratio * t) - b * cos(t - ratio * t);
}
double hypocycloid_y(double a, double b, double t) {
  double ratio = b / a;
  return (a - b) * sin(ratio * t) - b * sin(t - ratio * t);
}

// Logarithmic spiral (polar -> cartesian)
double logspiral_x(double a, double b, double t) {
  double r = a * exp(1 + t);
  return r * cos(t);
}
double logspiral_y(double a, double b, double t) {
  double r = a * exp(1 + t);
  return r * sin(t);
}

// Sine polar flower (polar -> cartesian)
double sineflower_x(double a, double b, double t) {
  double r = sin(a * t);
  return r * cos(t);
}
double sineflower_y(double a, double b, double t) {
  double r = sin(a * t);
  return r * sin(t);
}

/*
  2) Circle Concoid (Limaçon, Pascal's Snail):
  \(x = 2 \cdot (a \cdot cos(t) + b) \cdot cos(t), \; y = 2 \cdot (a \cdot cos(t) + b) \cdot sin(t), \; t \in (-\pi, \pi)\) .
  For this plot, \(a = 0.3, \; b = 0.2\) .
*/
void Display4()
{
  plot(limacon_x, limacon_y, 0.3, 0.2, -pi, pi, 0.01, 0, 0);
}

/*
  2) Cicloid:
  \(x = a \cdot t - b \cdot sin(t), \; y = a - b \cdot cos(t), \; t \in \mathbb{R} \) .
  For this plot, \(a = 0.1, \; b = 0.2\) .
*/
void Display5()
{
  plot(cycloid_x, cycloid_y, 0.1, 0.2, -10, 10, 0.01, 0, 0);
}

/*
  2) Epicicloid:
  \(x = (a + b) \cdot cos\left( \frac{b}{a} \cdot t \right) - b \cdot cos\left(t + \frac{b}{a}\cdot t \right) \)
  \(y = (a + b) \cdot sin\left( \frac{b}{a} \cdot t \right) - b \cdot sin\left(t + \frac{b}{a}\cdot t \right) \)
  \( t \in \left[ 0, 2\pi \right] \) .
  For this plot, \(a = 0.1, \; b = 0.3\) .
*/
void Display6()
{
  plot(epicycloid_x, epicycloid_y, 0.1, 0.3, 0, tau, 0.01, 0, 0);
}

/*
  2) Hipocicloid:
  \(x = (a - b) \cdot cos\left( \frac{b}{a} \cdot t \right) - b \cdot cos\left(t - \frac{b}{a}\cdot t \right) \)
  \(y = (a - b) \cdot sin\left( \frac{b}{a} \cdot t \right) - b \cdot sin\left(t - \frac{b}{a}\cdot t \right) \)
  \( t \in \left[ 0, 2\pi \right] \) .
  For this plot, \(a = 0.1, \; b = 0.3\) .
 */
void Display7()
{
  plot(hypocycloid_x, hypocycloid_y, 0.1, 0.3, 0, tau, 0.01, 0, 0);
}

/*
 4) Logarithmic spiral (in polar coordinates):
 \( r = a \cdot e^{1+t}, \; t \in (0, \infty) \) .
 For this plot, \(a = 0.02\) .
*/
void Display8()
{
  plot(logspiral_x, logspiral_y, 0.02, 0, 0, 2 * pi, 0.01, 0, 0);
}

/*
  4) Sine polar plot flower:
  \( r = sin(a \cdot t), \; t \in (0, \infty)  \) .
  For this plot, \(a = 10\), and the number 'petals' is \( 2 \cdot a \). Think about why.
  Number of petals = 2*a because a is even: sin(a*t) completes a full
  cycle through positive and negative values, each producing a petal.
*/
void Display9()
{
  // r = sin(a*t), need t in [0, 2*pi] for full flower when a is integer
  plot(sineflower_x, sineflower_y, 10, 0, 0, 2 * pi, 0.001, 0, 0);
}

/*
5) Longchamps' Trisectrix (Equilateral Trefoil):
\(
x = \frac{a}{4 \cdot cos^2(t) - 3}, \;
y = \frac{a \cdot tg(t)}{4 \cdot cos^2(t) - 3}, \;
t \in (-\pi/2, \pi/2) \setminus \{ -\pi/6, \pi/6 \} \) .
For this plot, \(a = 0.2\) .

Manipulation: The denominator 4cos^2(t)-3 has asymptotes at t = ±pi/6.
The curve has 3 regions in (-pi/2, pi/2): (-pi/2,-pi/6), (-pi/6,pi/6), (pi/6,pi/2).
The middle region forms a closed loop (the leaf). The outer two regions
go to infinity. To get the trefoil, we observe: the closed loop from
t in (-pi/6, pi/6) gives one leaf. For the full equilateral trefoil,
we rotate the parametric curve by 2pi/3 and 4pi/3 to get 3 leaves.
 */
void Display10()
{
  
}

void init(void)
{
  glClearColor(1.0, 1.0, 1.0, 1.0);
  glLineWidth(2);
  glPointSize(1);
  // glPolygonMode(GL_FRONT, GL_LINE);
  // Enabling blending and smoothing
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

void Display(void)
{
  std::cout << ("Call Display") << std::endl;
  // Clear the buffer. See init();
  glClear(GL_COLOR_BUFFER_BIT);

  switch (prevKey)
  {
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

void Reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}
void KeyboardFunc(unsigned char key, int x, int y)
{
  prevKey = key;
  if (key == 27) // escape
    exit(0);
  // The proper way to ask glut to redraw the window.
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
void MouseFunc(int button, int state, int x, int y)
{
  std::cout << "Mouse button ";
  std::cout << ((button == GLUT_LEFT_BUTTON) ? "left" : ((button == GLUT_RIGHT_BUTTON) ? "right" : "middle")) << " ";
  std::cout << ((state == GLUT_DOWN) ? "pressed" : "released");
  std::cout << " at coordinates: " << x << " x " << y << std::endl;
}

int main(int argc, char **argv)
{
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
  // glutIdleFunc(Display);
  glutMainLoop();

  return 0;
}