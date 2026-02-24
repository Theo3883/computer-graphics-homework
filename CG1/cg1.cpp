/*
  This program plots different 2D functions.
*/

#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <iostream>
#include <string>
#include <complex>
using namespace std::complex_literals;

//#include "glut.h" //MSVC local library install
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h> //system-wide install (or compiler default path)
#endif

double circle = atan(1) * 8; 
double halfCircle = atan(1) * 4;
double tau = circle; // 2 * PI = TAU
double pi = halfCircle; // TAU / 2 = PI

int g_w = 1000, g_h = 1000;

unsigned char g_prevKey;

int g_recursionMax = 8, g_recursionCurrent = 2;
double g_jfa = -0.82, g_jfb = -0.17; //Julia-Fatou a and b values.

//----------------Utility functions----------------------

void bitmapString(void* font, const char* str) {
  //Draw a string, character-by-character.
  char cp;
  for(const char* c = str; *c != 0; ++c) {
    cp = *c; //to respect const
    glutBitmapCharacter(font, cp);
  }
}

void drawBitmapString(const char* str, float x = -2, float y = -2) {
  //Draw a string, optionally setting raster position.
  /*
    We define the convetion that both values -2 mean 'do not change
    raster position'.
  */
  if((-2 != x) || (-2 != y)) {
    glRasterPos2f(x, y);
  }
  //freeglut, not old glut: glutBitmapString(GLUT_BITMAP_8_BY_13, str);
  bitmapString(GLUT_BITMAP_8_BY_13, str);
}

template <typename Numtype>
void drawBitmapNumber(Numtype number, float x = -2, float y = -2) {
  //Convert a number to a string, then draw it.
  //We need the template so we don't display '2' as '2.000000'.
  if((-2 != x) || (-2 != y)) {
    glRasterPos2f(x, y);
  }
  bitmapString(GLUT_BITMAP_8_BY_13, std::to_string(number).c_str());
}

void drawRecursionLevel() {
  //Simple utility function.
  drawBitmapString("Recursion Level: ", -0.98, -0.98);
  /*if we don't set explicit raster positions,
    drawing characters increments the paster position appropriately
  */
  drawBitmapNumber(g_recursionCurrent); 
}

void drawJfConstants() {
  drawBitmapString("Julia-Fatou constants: a = ", -0.98, -0.98);
  drawBitmapNumber(g_jfa);
  drawBitmapString(", b = ");
  drawBitmapNumber(g_jfb);
}
//^^^^^^^^^^^^^^^^^Utility functions^^^^^^^^^^^^^^^^^^


class Turtle {
/*
  Turtle Graphics:
  draw using points, directions and distances.
  (Radial coordinates.)
*/
protected:
  double m_x, m_y;
  double m_angle;

public:
  Turtle(double x = 0, double y = 0):
    m_x(x),
    m_y(y),
    m_angle(0) { }

  void rotate(double angle) {
    m_angle += angle;
  }

  void move(double distance) {
    //Move the Turtle without drawing.
    /*
      We convert from Radial coordinates
      to Cartesian coordinates.
     */
    m_x += distance * cos(m_angle);
    m_y += distance * sin(m_angle);
  }

  void draw(double distance) {
    //Move the Turtle and draw its path.
    glBegin(GL_LINES); {
      glVertex2d(m_x, m_y);
      move(distance);
      glVertex2d(m_x, m_y);
    }
    glEnd();
  }

  void resetPos() {m_x = 0; m_y = 0;}
  void resetRotation() {m_angle = 0;}
};

void drawCircle(double cx, double cy, double radius, int segments) {
  //How to draw a circle with Turtle graphics.
  Turtle t;
  //Arrive at the Cartesian coordinates of the centre.
  t.move(cx);
  t.rotate(pi/2);
  t.move(cy);
  //Reset rotation.
  t.rotate(-pi/2);

  //Arrive on the circle, at angle 0.
  t.move(radius);
  /*
    Up, the tangent on the circle
    (in the trigonometric direction).
  */
  t.rotate(pi/2);
  //2 * pi / segments
  double angle = tau / double(segments);
  //2 * pi * radius
  double segmentLength = (tau * radius) / segments;
  /*
    In order to properly fit segments one in the
    continuation of the other, we draw them with
    the angle of the middle of the circle surface
    they replace (not the start).
    This is how we get the least approximation error.
    Try setting this to zero, see what happens.
  */
  double midAngle = tau * double(0.5) / double(segments);
  t.rotate(midAngle);
  //<= so we make a loop, by overlapping the first and last segments.
  for(int ii = 0; ii <= segments; ++ii) {
    t.draw(segmentLength);
    t.rotate(angle);
  }
}

void drawSquare(Turtle t, float distance) {
  /*
    We assume the lower-left point of the square as the starting point,
    and the distance as the side length.
    (so: draw by moving forward and turning left)
  */
  t.draw(distance);

  t.rotate(pi/2);
  t.draw(distance);

  t.rotate(pi/2);
  t.draw(distance);

  t.rotate(pi/2);
  t.draw(distance);
}

void fractalRecursiveSquare(Turtle t, float distance, int recursionsLeft = 1) {
  if(recursionsLeft <= 0) {
    return;
  }

  float childDistance = distance / 3.0f;

  // Draw the center square for the current region.
  Turtle center = t;
  center.move(childDistance);
  center.rotate(pi/2);
  center.move(childDistance);
  center.rotate(-pi/2);
  drawSquare(center, childDistance);

  if(recursionsLeft == 1) {
    return;
  }

  // Recurse into the 8 surrounding subregions.
  for(int row = 0; row < 3; ++row) {
    for(int col = 0; col < 3; ++col) {
      if((1 == row) && (1 == col)) {
        continue;
      }

      Turtle child = t;
      child.move(col * childDistance);
      child.rotate(pi/2);
      child.move(row * childDistance);
      child.rotate(-pi/2);
      fractalRecursiveSquare(child, childDistance, recursionsLeft - 1);
    }
  }
}

void fractalKochCurve(Turtle t, float distance, int recursionsLeft = 1) {
  if(recursionsLeft > 0) {
    --recursionsLeft;
    distance /=3;
    
    //Draw straight forwards: '_'
    fractalKochCurve(t, distance, recursionsLeft); 
    t.move(distance);

    //Turn left: '_/'
    t.rotate(pi/3);
    fractalKochCurve(t, distance, recursionsLeft);
    t.move(distance);

    //Turn right: '_/\'
    t.rotate(- 2 * pi/3);
    fractalKochCurve(t, distance, recursionsLeft);
    t.move(distance);

    //Turn left: '_/\_'
    t.rotate(pi/3);
    fractalKochCurve(t, distance, recursionsLeft);
    //t.move(distance);
    /*
      ^ No need to move the equivalent distance,
      since no more segments are left.
    */
  } else {
    t.draw(distance);
  }
}

/*
  Display1 - Fractalul Koch (Fulg de Nea)
  ----------------------------------------
  Fractatul Koch se construieste pornind de la un triunghi echilateral.
  Fiecare latura a triunghiului este inlocuita recursiv cu o curba Koch:
    - se imparte segmentul in 3 parti egale
    - partea de mijloc este inlocuita cu un triunghi echilateral fara baza
    - rezulta 4 segmente de lungime 1/3 din original, dispuse in forma de 'cort'

  Calcule geometrice:
    - raza cercului circumscris triunghiului initial = radius = 0.95
    - cele 3 varfuri sunt pozitionate la unghiuri 0, 120, 240 grade fata de centru
    - lungimea laturii unui triunghi echilateral cu raza cercului circumscris R = R*sqrt(3)
    - la fiecare nivel de recursie, lungimea segmentului se imparte la 3
    - unghiurile de rotire ale curbei: +60 grade la urcare, -120 grade la coborare, +60 grade revenire

  Cu fiecare nivel de recursie, numarul de segmente se inmulteste cu 4,
  iar lungimea fiecaruia scade la 1/3, deci lungimea totala creste cu factorul 4/3.
  La infinit, perimetrul devine infinit, dar aria ramane finita.
*/
void Display1() {
  glColor3f(1, 0, 0);
  drawRecursionLevel();
  //Size of the fractal - radius of the circle circumscribing the starting triangle.
  double radius = 0.95;
  //Draw the circle containing the whole fractal.
  drawCircle(0, 0, radius, 36);
  //Start from the centre.
  Turtle t0(0, 0);
  Turtle t1 = t0;
  Turtle t2 = t0;
  //Rotate towards the 3 vertices of an equilateral triangle. (assuming we start from the centre).
  t0.rotate(0 * pi / 3);
  t1.rotate(2 * pi / 3);
  t2.rotate(4 * pi / 3);

  //Move onto the verices.
  t0.move(radius);
  t1.move(radius);
  t2.move(radius);
  
  //Rotate in the correct direction to draw edges from the vertices.
  t0.rotate(-pi/3 - pi/2);
  t1.rotate(-pi/3 - pi/2);
  t2.rotate(-pi/3 - pi/2);

  // De ce sqrt(3)?
  // Latura unui triunghi echilateral inscris intr-un cerc de raza R este a = R * sqrt(3).
  // Demonstratie:
  //   - inaltimea unui triunghi echilateral cu latura a: h = (sqrt(3)/2) * a
  //   - circumcentrul imparte mediana in raport 2:1, deci R = (2/3) * h = (sqrt(3)/3) * a
  //   - rezulta: a = R * sqrt(3)
  fractalKochCurve(t0, sqrt(3) * radius, g_recursionCurrent);
  fractalKochCurve(t1, sqrt(3) * radius, g_recursionCurrent);
  fractalKochCurve(t2, sqrt(3) * radius, g_recursionCurrent);
}

void fractalBinaryTree(Turtle t, float distance, int recursionsLeft = 1) {
    if(recursionsLeft > 0) {
      --recursionsLeft;
      t.draw(distance);
      Turtle tLeft = t, tRight = t;
      tRight.rotate(-pi/4);
      tLeft.rotate(pi/4);
      fractalBinaryTree(tRight, distance/2, recursionsLeft);
      fractalBinaryTree(tLeft, distance/2, recursionsLeft);
    } else {
      t.draw(distance);
    }
}

/*
  Display2 - Arborele Binar Fractal
  ----------------------------------
  Un arbore binar fractal se construieste recursiv astfel:
    - se deseneaza un segment (trunchiul) in directia curenta
    - la capatul segmentului se creeaza 2 ramuri noi, rotite cu +45 si -45 grade
    - fiecare ramura are lungimea jumatate fata de cea parinte: distance/2
    - recursiunea se opreste cand nu mai sunt niveluri ramase

  Calcule geometrice:
    - punctul de start este (0, -0.95) - baza jos-centru a ferestrei
    - directia initiala este pi/2 (sus, adica 90 grade)
    - la fiecare nivel i, lungimea ramurii = 0.95 / 2^i
    - numarul de ramuri la nivelul i = 2^i
    - unghiul ramurilor stangi fata de parinte: +pi/4 (+45 grade)
    - unghiul ramurilor drepte fata de parinte: -pi/4 (-45 grade)

  Structura rezultata seamana cu un copac sau o antena.
  Cu cat nivelul de recursie este mai mare, cu atat arborele are mai multe ramuri subtiri.
*/
void Display2() {
  glColor3f(1, 0, 0);
  drawRecursionLevel();
  Turtle t(0, -0.95);
  t.rotate(pi/2); //up
  fractalBinaryTree(t, 0.95, g_recursionCurrent);
}

/*
  Display3 - Fractatul Patratelor Recursive (Covorul lui Sierpinski patrat)
  --------------------------------------------------------------------------
  Pornind de la un patrat mare, acesta se imparte intr-o grila 3x3 de 9 celule egale.
  Celula din mijloc (rand=1, col=1) se deseneaza si ramane goala (nu recurseza mai departe).
  Celelalte 8 celule din jur sunt recurse la randul lor cu acelasi algoritm.

  Calcule geometrice:
    - dimensiunea initiala drawSize = 1.9 (aproape toata fereastra)
    - dimensiunea copilului = drawSize / 3 la fiecare nivel
    - pozitionarea copilului (row, col) fata de parintele din colt stanga-jos:
        * muta turtle cu col * childDistance in directia curenta (orizontal)
        * roteste cu +pi/2 (90 grade stanga)
        * muta cu row * childDistance (vertical)
        * roteste inapoi cu -pi/2 pentru a readuce orientarea originala
    - centrul (row=1, col=1) este sarit - acolo se afla patratul desenat

  La nivelul N de recursie:
    - exista 8^N patrate mici
    - dimensiunea fiecaruia este (1/3)^N din dimensiunea initiala
    - structura este un fractal cu dimensiunea Hausdorff = log(8)/log(3) ≈ 1.893
*/
void Display3() {
  //Draw the recursive-square fractal here.
  glColor3f(1, 0, 0);
  drawRecursionLevel();

  float drawSize = 1.9f;
  Turtle frame(-drawSize / 2.0f, -drawSize / 2.0f);

  // Outer frame.
  drawSquare(frame, drawSize);
  // Recursive center-squares in surrounding subregions.
  fractalRecursiveSquare(frame, drawSize, g_recursionCurrent);
}


/*
  hexLeaf - Deseneaza forma de baza a fractalului (frunza hexagonala).

  Parametri:
    px, py      - coordonatele punctului de origine al nodului curent
    vAngle      - directia vectorului 'v' in acest nod (radiani)
    length (L)  - lungimea de referinta pentru segmente
    angleOffset - rotatie suplimentara acumulata prin recursie (radiani)

  Calcul puncte:
    Directia combinata: ang = vAngle + angleOffset
    Pornind din origine (px, py), cele 4 puncte se calculeaza cu cos/sin:
      a = (px + cos(ang - pi/3)   * L/2,  py + sin(ang - pi/3)   * L/2)   <- stanga-sus
      b = (px + cos(ang + pi/3)   * L/2,  py + sin(ang + pi/3)   * L/2)   <- dreapta-sus
      c = (px + cos(ang + 2*pi/3) * L,    py + sin(ang + 2*pi/3) * L)     <- varf sus
      d = (px + cos(ang + 4*pi/3) * L,    py + sin(ang + 4*pi/3) * L)     <- varf jos

  Forma desenata:
    Se deseneaza lantul de 3 segmente: d -> a -> b -> c
    Aceasta formeaza un arc deschis de hexagon (3 laturi din 6).

  Tehnica Turtle (polyline trick):
    - Turtle porneste plasata la pozitia 'd'.
    - Pentru fiecare segment urmator, se calculeaza unghiul sau cu atan2(dy, dx).
    - Turtle se roteste cu diferenta dintre unghiul nou si cel curent,
      apoi deseneaza segmentul cu t.draw(lungime).
    - Astfel nu este nevoie sa stim pozitia absoluta - doar directia relativa.
*/
void hexLeaf(double px, double py, double vAngle, double length, double angleOffset) {
  double ang = vAngle + angleOffset;
  double L   = length;

  double ax = px + cos(ang - pi/3)    * L/2;
  double ay = py + sin(ang - pi/3)    * L/2;
  double bx = px + cos(ang + pi/3)    * L/2;
  double by = py + sin(ang + pi/3)    * L/2;
  double cx = px + cos(ang + 2*pi/3)  * L;
  double cy = py + sin(ang + 2*pi/3)  * L;
  double dx = px + cos(ang + 4*pi/3)  * L;
  double dy = py + sin(ang + 4*pi/3)  * L;

  // Draw strip d→a→b→c using turtle polyline trick (rotate+draw per segment).
  double segs[4][2] = {{dx,dy},{ax,ay},{bx,by},{cx,cy}};
  Turtle t(dx, dy);
  double curAngle = 0.0;
  for(int ii = 1; ii < 4; ++ii) {
    double ddx = segs[ii][0] - segs[ii-1][0];
    double ddy = segs[ii][1] - segs[ii-1][1];
    double segAngle = atan2(ddy, ddx);
    t.rotate(segAngle - curAngle);
    t.draw(hypot(ddx, ddy));
    curAngle = segAngle;
  }
}

/*
  hexLineFractal - Fractal hex-linie recursiv.

  Parametri:
    px, py      - punctul de origine al nodului curent in spatiul 2D
    vAngle      - directia de propagare a nodului (radiani; 0 = dreapta, pi/2 = sus)
    length      - lungimea de referinta curenta (se injumatateste la fiecare nivel)
    level       - nivelul de recursivitate ramas (0 = deseneaza frunza)
    angleOffset - offset acumulat de rotatie pentru orientarea formei frunzei

  Algoritm:
    Cazul de baza (level == 0):
      Se apeleaza hexLeaf() pentru a desena forma deschisa de hexagon.

    Cazul recursiv (level > 0):
      1. newLen = length / 2  (lungimea copilului este jumatate)
      2. Se calculeaza 3 directii de propagare:
           v1 = vAngle           (inainte)
           v2 = vAngle + 2*pi/3  (stanga, +120 grade)
           v3 = vAngle + 4*pi/3  (dreapta, +240 grade)
      3. Fiecare copil este plasat la distanta newLen in directia sa:
           p_fiu = (px + cos(v_fiu) * newLen,  py + sin(v_fiu) * newLen)
      4. Se recurse cu level-1 si cu angleOffset actualizat:
           Fiu 1 (inainte):  angleOffset nemodificat
           Fiu 2 (+120 grade): angleOffset += rotation * 2*pi/3
           Fiu 3 (+240 grade): angleOffset -= rotation * 2*pi/3

  Rotatia alternanta:
    rotation = (level % 2 == 0) ? -1 : +1
    La nivelurile pare rotatia este negativa, la cele impare pozitiva.
    Aceasta inverseaza sensul de rasucire al formei la fiecare nivel,
    producand efectul de 'S' sau 'Z' vizibil la nivelul 2.

  Structura rezultata:
    La nivel 1: 3 frunze aranjate radial la 120 grade una fata de alta.
    La nivel N: 3^N frunze in tiparul unui triunghi Sierpinski cu 3 ramuri.
*/
void hexLineFractal(double px, double py, double vAngle, double length, int level, double angleOffset) {
  double rotation = (level % 2 == 0) ? -1.0 : 1.0;

  if(level == 0) {
    hexLeaf(px, py, vAngle, length, angleOffset);
    return;
  }

  double newLen = length / 2.0;

  // Child 1: straight ahead in v
  double p1x = px + cos(vAngle) * newLen;
  double p1y = py + sin(vAngle) * newLen;
  hexLineFractal(p1x, p1y, vAngle, newLen, level-1, angleOffset);

  // Child 2: v rotated +120°
  double v2 = vAngle + 2*pi/3;
  double p2x = px + cos(v2) * newLen;
  double p2y = py + sin(v2) * newLen;
  hexLineFractal(p2x, p2y, v2, newLen, level-1, angleOffset + rotation * 2*pi/3);

  // Child 3: v rotated +240°
  double v3 = vAngle + 4*pi/3;
  double p3x = px + cos(v3) * newLen;
  double p3y = py + sin(v3) * newLen;
  hexLineFractal(p3x, p3y, v3, newLen, level-1, angleOffset - rotation * 2*pi/3);
}

/*
  Display4 - Fractalul de Linii Hexagonale
  ---------------------------------------------------------------------------
  Acest fractal are o structura radiala cu 3 ramuri simetrice la 120 grade.
  La fiecare nod, recursiunea produce 3 copii plasati in directiile:
    - inainte (vAngle + 0)
    - stanga (vAngle + 120 grade = +2*pi/3)
    - dreapta (vAngle + 240 grade = +4*pi/3)
  Fiecare copil are lungimea jumatate fata de parinte: newLen = length / 2.

  Rotatie alternanta:
    - la nivelurile pare, rotation = -1
    - la nivelurile impare, rotation = +1
    - unghiul de offset se acumuleaza: angleOffset +/- rotation * 120 grade
    - aceasta produce efectul de rasucire S/Z vizibil in fractal

  Frunza (nivel 0) - forma desenata:
    Pornind din punctul (px, py) cu directia vAngle + angleOffset, se calculeaza
    4 puncte (a, b, c, d) folosind rotatii de -60, +60, +120, +240 grade:
      a = p + (ang - 60°) * L/2
      b = p + (ang + 60°) * L/2
      c = p + (ang + 120°) * L
      d = p + (ang + 240°) * L
    Se deseneaza poliinia d -> a -> b -> c (3 segmente conectate).

  Pozitionarea pe ecran:
    - punctul de start: (0, 0), directia initiala: pi/2 (sus)
    - fractalul este centrat in origine, translat usor in jos cu glTranslated
    - nivelul de recursie transmis = g_recursionCurrent - 1
      (deoarece nivelul 0 din main.cpp corespunde unui singur element vizibil)
*/
void Display4() {
  glColor3f(1, 0, 0);
  drawRecursionLevel();
  glPushMatrix();
  glLoadIdentity();
  glTranslated(0.0, -0.4, 0.0);
  glScaled(1, 1, 1.0);
  hexLineFractal(0.0, 0.0, pi/2, 1.0, g_recursionCurrent - 1, 0.0);
  glPopMatrix();
}

template <typename FloatType>
class JF {
protected:
  //The x and y mathematical bounds of the fractal slice we're displaying.
  FloatType m_xmin, m_xmax, m_ymin, m_ymax;
  //The constant we're biasing the JF fractal with.
  std::complex<FloatType> m_c;
  //The radius around the origin we're using to detect divergence.
  FloatType m_maxRadius;
  //How many iterations we'll do to allow the number sequence to
  //exceed the limit.
  int m_maxIteration;

  virtual inline int test(std::complex<FloatType> z, std::complex<FloatType> c, double maxRadius = 2, int maxIteration = 50) {
    /*
      Compute the Julia-Fatou set in a point in 4D (x, y, a, b). Return the iterations *left*
      upon radius breach. So, a return value of 0 means estimated-divergence, other values
      mean speed of estimated convergence.
    */
    //We create a number sequence, and estimate its limit.
    for(int ii = maxIteration; ii > 0; --ii) {
      z = z * z + c;
      if(abs(z) > maxRadius)
	return(ii);
    }
    return 0;
  }
  
public:
  JF(FloatType xmin, FloatType xmax, FloatType ymin, FloatType ymax, FloatType a = 0, FloatType b = 0, FloatType maxRadius = 20, int maxIteration = 150):
    m_xmin(xmin),
    m_xmax(xmax),
    m_ymin(ymin),
    m_ymax(ymax),
    m_c(a, b),
    m_maxRadius(maxRadius),
    m_maxIteration(maxIteration) {
  }

  void draw(FloatType l, FloatType r, FloatType b, FloatType t, int samplePointsHorizontal, int samplePointsVertical) {
    /*
      Draw the current slice of the JF set onto the screen.
      Left, right, bottom, top, and the steps for each axis.
    */
    glPointSize(1);
    FloatType stepx = (m_xmax - m_xmin) / FloatType(samplePointsHorizontal);
    FloatType stepy = (m_ymax - m_ymin) / FloatType(samplePointsVertical);
    FloatType steph = (r      - l)      / FloatType(samplePointsHorizontal);
    FloatType stepv = (t      - b)      / FloatType(samplePointsVertical);
    int iterations;
    std::complex<FloatType> z;
    glBegin(GL_POINTS);
    /*
      We need to move both on screen pixels and in the mathematical plane -
      at the same time.
    */
    for(FloatType jj = 0, y = m_ymin, v = b; jj < samplePointsVertical; jj += 1, y += stepy, v += stepv) {
      z.imag(y);
      for(FloatType ii = 0, x = m_xmin, h = l; ii < samplePointsHorizontal; ii += 1, x += stepx, h += steph) {
	z.real(x);
	iterations = test(z, m_c, m_maxRadius, m_maxIteration);
	if(0 == iterations) {
	  glColor3f(1, 0, 0);
	  glVertex2d(h, v);	  
	}
      }
    }
    glEnd();
  }
};

//Modify what you think necessary in the MB class to draw the Mandelbrot Fractal.
template <typename FloatType>
class MB: public JF<FloatType> {
public:
  MB(FloatType xmin, FloatType xmax, FloatType ymin, FloatType ymax, FloatType a = 0, FloatType b = 0, FloatType maxRadius = 20, int maxIteration = 150):
    JF<FloatType>(xmin, xmax, ymin, ymax, a, b, maxRadius, maxIteration) {}

  // Mandelbrot: z0=0, c=point (swap roles vs JF)
  virtual inline int test(std::complex<FloatType> z, std::complex<FloatType> c, double maxRadius = 2, int maxIteration = 50) override {
    std::complex<FloatType> zz(0, 0);
    for(int ii = maxIteration; ii > 0; --ii) {
      zz = zz * zz + z; // z is the screen point used as c
      if(std::abs(zz) > maxRadius) return ii;
    }
    return 0;
  }

  // 2-colour draw: inside=red, outside=not drawn (white background shows through)
  void drawTwoColor(FloatType l, FloatType rt, FloatType bot, FloatType top, int sph, int spv) {
    glPointSize(1);
    FloatType stepx = (this->m_xmax - this->m_xmin) / FloatType(sph);
    FloatType stepy = (this->m_ymax - this->m_ymin) / FloatType(spv);
    FloatType steph = (rt  - l)   / FloatType(sph);
    FloatType stepv = (top - bot) / FloatType(spv);
    std::complex<FloatType> z;
    glColor3f(1.0f, 0.1f, 0.1f); // inside: red
    glBegin(GL_POINTS);
    for(FloatType jj = 0, y = this->m_ymin, v = bot; jj < spv; jj += 1, y += stepy, v += stepv) {
      z.imag(y);
      for(FloatType ii = 0, x = this->m_xmin, h = l; ii < sph; ii += 1, x += stepx, h += steph) {
        z.real(x);
        int it = this->test(z, this->m_c, this->m_maxRadius, this->m_maxIteration);
        if(it == 0) {
          glVertex2d(h, v); // only draw inside points
        }
      }
    }
    glEnd();
  }

  // HSV to RGB helper (S=1, V=1)
  void hsvToRgb(float hue, float &r, float &g, float &b) {
    hue = fmod(hue, 360.0f);
    if(hue < 0) hue += 360.0f;
    int sector = int(hue / 60.0f) % 6;
    float f = hue / 60.0f - float(sector);
    float q = 1.0f - f;
    switch(sector) {
      case 0: r=1;   g=f;   b=0;   break;
      case 1: r=q;   g=1;   b=0;   break;
      case 2: r=0;   g=1;   b=f;   break;
      case 3: r=0;   g=q;   b=1;   break;
      case 4: r=f;   g=0;   b=1;   break;
      default:r=1;   g=0;   b=q;   break;
    }
  }

  // Gradient draw: HSV rainbow — blue (far from set) → green → red (near boundary), inside=black
  void drawGradient(FloatType l, FloatType rt, FloatType bot, FloatType top, int sph, int spv) {
    glPointSize(1);
    FloatType stepx = (this->m_xmax - this->m_xmin) / FloatType(sph);
    FloatType stepy = (this->m_ymax - this->m_ymin) / FloatType(spv);
    FloatType steph = (rt  - l)   / FloatType(sph);
    FloatType stepv = (top - bot) / FloatType(spv);
    std::complex<FloatType> z;
    glBegin(GL_POINTS);
    for(FloatType jj = 0, y = this->m_ymin, v = bot; jj < spv; jj += 1, y += stepy, v += stepv) {
      z.imag(y);
      for(FloatType ii = 0, x = this->m_xmin, h = l; ii < sph; ii += 1, x += stepx, h += steph) {
        z.real(x);
        int it = this->test(z, this->m_c, this->m_maxRadius, this->m_maxIteration);
        if(it == 0) {
          // inside the set: black (skip drawing, black bg shows through)
        } else {
          // hue: 240=blue (fast escape/far) down to 0=red (slow escape/near boundary)
          float t = float(it) / float(this->m_maxIteration);
          float hue = 240.0f * t;
          float cr, cg, cb;
          hsvToRgb(hue, cr, cg, cb);
          glColor3f(cr, cg, cb);
          glVertex2d(h, v);
        }
      }
    }
    glEnd();
  }
};

/*
  Display5 - Multimea Mandelbrot in 2 culori
  -------------------------------------------
  Multimea Mandelbrot este un caz special al multimii Julia-Fatou:
    MB(x, y) = JF(x, y, x, y)  =>  z0 = x+yi,  c = x+yi  (ambele egale cu punctul de pe ecran)

  Algoritmul pentru fiecare pixel (x, y):
    1. Se construieste numarul complex z0 = x + y*i
    2. Se itereaza sirul: z_{n+1} = z_n^2 + z0  (c = z0, nu o constanta fixa)
    3. Daca |z_n| > raza_maxima inainte de maxIteration pasi => diverge => punct IN AFARA multimii
    4. Daca sirul nu a divergit dupa maxIteration pasi => converge => punct IN multime

  Colorare:
    - Punctele din interiorul multimii (converge): desenate cu rosu (1.0, 0.1, 0.1)
    - Punctele din exterior (diverge): NU se deseneaza => fundalul alb al ferestrei ramane vizibil

  Domeniu matematic: [-2, 2] x [-2, 2]
  Domeniu pe ecran:  [-1, 1] x [-1, 1]
*/
void Display5() {
  // 2-colour Mandelbrot: inside=red, outside=dark blue
  drawBitmapString("Mandelbrot 2-colour", -0.98, -0.98);
  MB<double> mb(-2, 2, -2, 2);
  mb.drawTwoColor(-1.0, 1.0, -1.0, 1.0, g_w, g_h);
}

/*
  Display6 - Multimea Mandelbrot cu gradient de culori (curcubeu HSV)
  ---------------------------------------------------------------------
  Foloseste acelasi algoritm ca Display5 pentru a calcula multimea Mandelbrot,
  dar punct exterior primeste o culoare in functie de VITEZA de divergenta.

  Algoritmul de colorare prin "escape-time":
    - test() returneaza numarul de iteratii RAMASE cand |z| > raza_maxima
    - escape_iter = maxIteration - it  =>  cat de repede a divergit punctul
      * escape_iter mic (1-2): diverge rapid => punct DEPARTE de multime
      * escape_iter mare (aproape maxIteration): diverge lent => punct APROAPE de granita

  Conversia escape_iter in culoare HSV:
    t = escape_iter / maxIteration  (valoare intre 0 si 1)
    hue = 240 * (1 - t)  (unghi de culoare in grade)
      * t≈0 (departe, diverge repede): hue=240 => albastru
      * t≈0.5 (distanta medie):        hue=120 => verde
      * t≈1 (aproape de granita):      hue=0   => rosu
    Culoarea finala RGB se calculeaza din HSV cu saturatie=1, luminozitate=1.

  Punctele DIN interiorul multimii nu se deseneaza => fundalul negru ramane vizibil.
  Fundalul este setat la negru cu glClearColor(0,0,0,1) la inceputul functiei.

  Domeniu matematic: [-2, 2] x [-2, 2]
  maxRadius = 2 (raza standard de escape pentru Mandelbrot)
  maxIteration = 150 (mai multe iteratii => mai multa detaliu la granita)
*/
void Display6() {
  // Gradient Mandelbrot: black background, HSV rainbow outside, black inside
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  drawBitmapString("Mandelbrot gradient", -0.98, -0.98);
  MB<double> mb(-2, 2, -2, 2);
  mb.drawGradient(-1.0, 1.0, -1.0, 1.0, g_w, g_h);
}

void Display7() {
}

void Display8() {
}

void Display9() {
}

void Display10() {
}

void init(void) {
  glColor3f(1, 0, 0); //Just a starting default drawing colour.
  glClearColor(1.0,1.0,1.0,1.0);
  glLineWidth(1);
  glPointSize(1);
  //glPolygonMode(GL_FRONT, GL_LINE);
  //As we want pixel-perfect display for JF fractals, don't enable point smoothing.
  glEnable(GL_SMOOTH);
  //glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glHint(GL_NICEST, GL_POINT_SMOOTH_HINT);
  glHint(GL_NICEST, GL_LINE_SMOOTH_HINT);
  glHint(GL_NICEST, GL_POLYGON_SMOOTH_HINT);
  glHint (GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

  //Alpha-blending
  glEnable(GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void Display(void) {
  // Clear the buffer. See init();
  glClear(GL_COLOR_BUFFER_BIT);

  switch(g_prevKey) {
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
  g_w = w;
  g_h = h;
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}
void KeyboardFunc(unsigned char key, int x, int y) {
  switch(key) {
  case 27: // escape
    exit(0);
    break;
  case '+':
    ++g_recursionCurrent;
    if(g_recursionCurrent > g_recursionMax)
      g_recursionCurrent = g_recursionMax;
    break;
  case '-':
    --g_recursionCurrent;
    if(g_recursionCurrent < 0)
      g_recursionCurrent = 0;
    break;
  case 'j':
    g_jfa -= 0.01;
    if(g_jfa < -2)
      g_jfa = -2;
    break;
  case 'l':
    g_jfa += 0.01;
    if(g_jfa > 2)
      g_jfa = 2;
    break;
  case 'k':
    g_jfb -= 0.01;
    if(g_jfb < -2)
      g_jfb = -2;
    break;
  case 'i':
    g_jfb += 0.01;
    if(g_jfb > 2)
      g_jfb = 2;
    break;
  default:
    //Only change the image if a 'special' key wasn't pressed.
    g_prevKey = key;
  }

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
  std::cout<< "Mouse button ";
  std::cout<<( (button == GLUT_LEFT_BUTTON) ? "left" : ((button == GLUT_RIGHT_BUTTON) ? "right": "middle") ) << " ";
  std::cout<< ( (state == GLUT_DOWN) ? "pressed" : "released" );
  std::cout<< " at coordinates: " << x <<" x " << y << std::endl;
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitWindowSize(g_w, g_h);
  glutInitWindowPosition(-1, -1);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA); 
  glutCreateWindow (argv[0]);
  init();
  glutReshapeFunc(Reshape);
  glutKeyboardFunc(KeyboardFunc);
  glutMouseFunc(MouseFunc);
  glutDisplayFunc(Display);
  //glutIdleFunc(Display);
  glutMainLoop();

  return 0;
}