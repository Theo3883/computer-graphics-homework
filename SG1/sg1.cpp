/*
  Acest program deseneaza diferite functii si curbe 2D parametrice
  folosind biblioteca OpenGL (prin intermediul GLUT).
*/

#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <iostream>

// Includerea corecta a bibliotecii GLUT in functie de sistemul de operare
#if defined(__APPLE__)
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Constante matematice utile
double circle = atan(1) * 8;     // Echivalent cu 2 * PI
double halfCircle = atan(1) * 4; // Echivalent cu PI
double tau = circle;             // 2 * PI = TAU
double pi = halfCircle;          // TAU / 2 = PI

// 'step' defineste rezolutia curbei. Un pas mai mic = o curba mai lina (mai multe puncte calculate).
double step = 0.05;

// Dimensiunile initiale ale ferestrei
int defaultW = 1000, defaultH = 1000;
int viewportPadding = 24;

// Variabila pentru a retine ultima tasta apasata, utila pentru a sti ce curba sa redesenam
unsigned char prevKey;

/*
   1) Concoida lui Nicomede
*/
void Display1()
{
  double xmax, ymax, xmin, ymin;

  // Parametrii 'a' si 'b' definesc forma exacta a concoidei
  double a = 1, b = 2;
  double step = 0.05;

  // Pasul 1: Calculam extinderea maxima a functiei pe axele X si Y
  // pentru a sti cum sa o scalam incat sa incapa in fereastra OpenGL [-1, 1]
  xmax = a - b - 1;
  xmin = a + b + 1;
  ymax = ymin = 0;

  for (double t = -pi / 2 + step; t < pi / 2; t += step)
  {
    double x1, y1, x2, y2;
    // Ramura 1
    x1 = a + b * cos(t);
    xmax = (xmax < x1) ? x1 : xmax;
    xmin = (xmin > x1) ? x1 : xmin;

    // Ramura 2
    x2 = a - b * cos(t);
    xmax = (xmax < x2) ? x2 : xmax;
    xmin = (xmin > x2) ? x2 : xmin;

    // Y pentru ambele ramuri
    y1 = a * tan(t) + b * sin(t);
    ymax = (ymax < y1) ? y1 : ymax;
    ymin = (ymin > y1) ? y1 : ymin;

    y2 = a * tan(t) - b * sin(t);
    ymax = (ymax < y2) ? y2 : ymax;
    ymin = (ymin > y2) ? y2 : ymin;
  }

  // Ne intereseaza valoarea absoluta maxima pentru a face o scalare uniforma fata de origine (0,0)
  xmax = (fabs(xmax) > fabs(xmin)) ? fabs(xmax) : fabs(xmin);
  ymax = (fabs(ymax) > fabs(ymin)) ? fabs(ymax) : fabs(ymin);

  // Setam culoarea desenului (Rosu) - valorile sunt R, G, B in intervalul [0, 1]
  glColor3f(1, 0.1, 0.1);

  // Pasul 2: Desenam prima ramura
  glBegin(GL_LINE_STRIP); // GL_LINE_STRIP uneste punctele consecutive cu linii
  for (double t = -pi / 2 + step; t < pi / 2; t += step)
  {
    // Impartim la xmax si ymax pentru a "tasa" curba in intervalul [-1, 1] al ecranului
    double x1 = (a + b * cos(t)) / xmax;
    double y1 = (a * tan(t) + b * sin(t)) / ymax;
    glVertex2d(x1, y1); // Adaugam punctul in lista de desenare
  }
  glEnd();

  // Pasul 3: Desenam a doua ramura
  glBegin(GL_LINE_STRIP);
  for (double t = -pi / 2 + step; t < pi / 2; t += step)
  {
    double x2 = (a - b * cos(t)) / xmax;
    double y2 = (a * tan(t) - b * sin(t)) / ymax;
    glVertex2d(x2, y2);
  }
  glEnd();
}

// 2) Graficul functiei modul de sin(x) inmultit cu e^(-sin(x))
void Display2()
{
  double xmax = 8 * pi;
  // ymax este setat la exp(1.1) pentru a oferi putin spatiu deasupra curbei
  // (valoarea maxima reala e mai mica decat e^1, dar asa ne asiguram ca nu atinge marginea de sus).
  double ymax = exp(1.1);

  glColor3f(1, 0.1, 0.1);
  glBegin(GL_LINE_STRIP);
  for (double x = 0; x < xmax; x += step)
  {
    double x1 = x / xmax;                             // Scalare pe X (va desena doar pe jumatatea dreapta a ecranului [0, 1])
    double y1 = (fabs(sin(x)) * exp(-sin(x))) / ymax; // Scalare pe Y
    glVertex2d(x1, y1);
  }
  glEnd();
}

/* 3)
   Functie definita pe ramuri, utilizeaza distanta pana la cel mai apropiat intreg.
 */
void Display3()
{
  double xmax = 20.0;
  double ymax = 1.0;
  double step = 0.01;

  glColor3f(1, 0.1, 0.1);
  glBegin(GL_LINE_STRIP);

  // Punctul de start definit manual pentru a evita impartirea la 0
  glVertex2d(0.0, 1.0);

  for (double x = step; x <= xmax; x += step)
  {
    // Calculam distanta pana la cel mai apropiat intreg
    double frac = x - floor(x);
    double d = (frac <= 0.5) ? frac : (1.0 - frac);
    double y = d / x;

    // x / xmax scaleaza graficul de la [0, 20] la [0, 1]
    glVertex2d(x / xmax, y / ymax);
  }
  glEnd();
}

/*
  Functie utilitara creata de tine pentru a nu repeta codul de scalare si desenare
  pentru curbele parametrice care folosesc 2 functii (fx, fy).
*/
void plot(double (*fx)(double, double, double), double (*fy)(double, double, double),
          double a, double b, double intervalStart, double intervalEnd,
          double step = 0.01, double scaleX = 1, double scaleY = 1,
          GLint primitive = GL_LINE_STRIP)
{
  // Daca factorii de scalare sunt 0, ii calculam automat cautand maximele
  if (scaleX == 0 || scaleY == 0)
  {
    double xmax = 0, ymax = 0;
    for (double t = intervalStart; t <= intervalEnd; t += step)
    {
      double xv = fx(a, b, t);
      double yv = fy(a, b, t);
      if (fabs(xv) > xmax)
        xmax = fabs(xv);
      if (fabs(yv) > ymax)
        ymax = fabs(yv);
    }
    if (scaleX == 0)
      scaleX = (xmax > 0) ? xmax : 1;
    if (scaleY == 0)
      scaleY = (ymax > 0) ? ymax : 1;
  }

  glColor3f(1, 0.1, 0.1);
  glBegin(primitive);
  for (double t = intervalStart; t <= intervalEnd; t += step)
  {
    // Generam punctele, le scalam direct si le trimitem catre OpenGL
    double xv = fx(a, b, t) / scaleX;
    double yv = fy(a, b, t) / scaleY;
    glVertex2d(xv, yv);
  }
  glEnd();
}

// --- Definitiile matematice ale curbelor parametrice ---

// Melcul lui Pascal (Limacon)
double limacon_x(double a, double b, double t) { return 2 * (a * cos(t) + b) * cos(t); }
double limacon_y(double a, double b, double t) { return 2 * (a * cos(t) + b) * sin(t); }

// Cicloida
double cycloid_x(double a, double b, double t) { return a * t - b * sin(t); }
double cycloid_y(double a, double b, double t) { return a - b * cos(t); }

// Epicicloida
double epicycloid_x(double a, double b, double t)
{
  double ratio = b / a;
  return (a + b) * cos(ratio * t) - b * cos(t + ratio * t);
}
double epicycloid_y(double a, double b, double t)
{
  double ratio = b / a;
  return (a + b) * sin(ratio * t) - b * sin(t + ratio * t);
}

// Hipocicloida
double hypocycloid_x(double a, double b, double t)
{
  double ratio = b / a;
  return (a - b) * cos(ratio * t) - b * cos(t - ratio * t);
}
double hypocycloid_y(double a, double b, double t)
{
  double ratio = b / a;
  return (a - b) * sin(ratio * t) - b * sin(t - ratio * t);
}

// Spirala logaritmica (convertita din coordonate polare in carteziene: x = r*cos(t), y = r*sin(t))
double logspiral_x(double a, double b, double t)
{
  double r = a * exp(1 + t);
  return r * cos(t);
}
double logspiral_y(double a, double b, double t)
{
  double r = a * exp(1 + t);
  return r * sin(t);
}

// Floare in coordonate polare (Sine polar flower)
double sineflower_x(double a, double b, double t)
{
  double r = sin(a * t);
  return r * cos(t);
}
double sineflower_y(double a, double b, double t)
{
  double r = sin(a * t);
  return r * sin(t);
}

// Apelurile propriu-zise folosind functia utilitara 'plot'
void Display4() { plot(limacon_x, limacon_y, 0.3, 0.2, -pi, pi, 0.001, 0, 0, GL_LINE_LOOP); }
void Display5()
{
  double a = 0.1;
  double b = 0.2;
  double tStart = -pi;
  double tEnd = 5 * pi;
  double localStep = 0.01;

  double xmin = DBL_MAX, xmax = -DBL_MAX;
  double ymin = DBL_MAX, ymax = -DBL_MAX;
  for (double t = tStart; t <= tEnd; t += localStep)
  {
    double x = cycloid_x(a, b, t);
    double y = cycloid_y(a, b, t);
    if (x < xmin)
      xmin = x;
    if (x > xmax)
      xmax = x;
    if (y < ymin)
      ymin = y;
    if (y > ymax)
      ymax = y;
  }

  double xCenter = (xmin + xmax) * 0.5;
  double yCenter = (ymin + ymax) * 0.5;
  double xRange = xmax - xmin;
  double yRange = ymax - ymin;
  if (xRange == 0)
    xRange = 1;
  if (yRange == 0)
    yRange = 1;

  double targetWidth = 1.8;
  double targetHeight = 0.42;
  double yOffset = -0.02;

  glColor3f(1.0f, 0.1f, 0.1f);
  glBegin(GL_LINE_STRIP);
  for (double t = tStart; t <= tEnd; t += localStep)
  {
    double x = cycloid_x(a, b, t);
    double y = cycloid_y(a, b, t);

    double xn = (x - xCenter) * (targetWidth / xRange);
    double yn = (y - yCenter) * (targetHeight / yRange) + yOffset;
    glVertex2d(xn, yn);
  }
  glEnd();
}
void Display6() { plot(epicycloid_x, epicycloid_y, 0.1, 0.3, 0, tau, 0.01, 0, 0); }
void Display7() { plot(hypocycloid_x, hypocycloid_y, 0.1, 0.3, 0, tau, 0.01, 0, 0); }
void Display8()
{
  double a = 0.02;
  double tStart = 4.1;
  double tEnd = -0.3;
  double localStep = 0.002;

  double xmin = DBL_MAX, xmax = -DBL_MAX;
  double ymin = DBL_MAX, ymax = -DBL_MAX;
  for (double t = tEnd; t <= tStart; t += localStep)
  {
    double x = logspiral_x(a, 0, t);
    double y = logspiral_y(a, 0, t);
    if (x < xmin)
      xmin = x;
    if (x > xmax)
      xmax = x;
    if (y < ymin)
      ymin = y;
    if (y > ymax)
      ymax = y;
  }

  double xCenter = (xmin + xmax) * 0.5;
  double yCenter = (ymin + ymax) * 0.5;
  double xRange = xmax - xmin;
  double yRange = ymax - ymin;
  if (xRange == 0)
    xRange = 1;
  if (yRange == 0)
    yRange = 1;

  double targetWidth = 0.95;
  double targetHeight = 1.1;
  double xOffset = -0.45;
  double yOffset = -0.2;

  glColor3f(1.0f, 0.1f, 0.1f);
  glBegin(GL_LINE_STRIP);
  for (double t = tStart; t >= tEnd; t -= localStep)
  {
    double x = logspiral_x(a, 0, t);
    double y = logspiral_y(a, 0, t);

    double xn = (x - xCenter) * (targetWidth / xRange) + xOffset;
    double yn = (y - yCenter) * (targetHeight / yRange) + yOffset;
    glVertex2d(xn, yn);
  }
  glEnd();
}
void Display9() { plot(sineflower_x, sineflower_y, 10, 0, 0, 2 * pi, 0.001, 0, 0); }

/*
  10) Trisectoarea lui Longchamps (Trefla Echilaterala):
  Am implementat aceasta functie bazandu-ma pe descrierea ta matematica.
*/
void Display10()
{
  double a = 0.2;
  double step = 0.005;

  // Conform comentariului tau, bucla centrala (o frunza) se formeaza pentru t in (-pi/6, pi/6)
  // Folosesc un "epsilon" (0.01) pentru a evita impartirea la zero fix in asimptote.
  double t_start = -pi / 6 + 0.01;
  double t_end = pi / 6 - 0.01;

  // Calculam factorul de scalare o singura data pentru o frunza
  double xmax = 0, ymax = 0;
  for (double t = t_start; t <= t_end; t += step)
  {
    double numitor = 4 * cos(t) * cos(t) - 3;
    double x = a / numitor;
    double y = (a * tan(t)) / numitor;
    if (fabs(x) > xmax)
      xmax = fabs(x);
    if (fabs(y) > ymax)
      ymax = fabs(y);
  }
  double scaleFactor = (xmax > ymax) ? xmax : ymax;
  // Oprim impartirea la zero in caz de eroare matematica
  if (scaleFactor == 0)
    scaleFactor = 1.0;

  // Desenam de 3 ori (pentru a crea cele 3 frunze ale treflei) rotind sistemul de coordonate
  for (int i = 0; i < 3; ++i)
  {
    glPushMatrix(); // Salvam sistemul de coordonate curent

    // Rotim matricea curenta cu 0, 120 si respectiv 240 de grade (OpenGL foloseste grade, nu radiani aici!)
    // Rotatia se face in jurul axei Z (0, 0, 1) pentru un desen 2D
    glRotatef(i * 120.0, 0.0, 0.0, 1.0);

    glColor3f(1.0, 0.1, 0.1); // Culoare rosie
    glBegin(GL_LINE_STRIP);
    for (double t = t_start; t <= t_end; t += step)
    {
      double numitor = 4 * cos(t) * cos(t) - 3;
      double x = (a / numitor) / scaleFactor;
      double y = ((a * tan(t)) / numitor) / scaleFactor;
      glVertex2d(x, y);
    }
    glEnd();

    glPopMatrix(); // Restauram sistemul de coordonate initial pentru urmatoarea frunza
  }
}

// Functia de initializare a contextului OpenGL
void init(void)
{
  glClearColor(1.0, 1.0, 1.0, 1.0); // Fundal alb
  glLineWidth(2);                   // Grosimea liniei
  glPointSize(1);                   // Dimensiunea punctului

  // Setari pentru netezirea liniilor (Antialiasing)
  glEnable(GL_SMOOTH);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glHint(GL_NICEST, GL_POINT_SMOOTH_HINT);
  glHint(GL_NICEST, GL_LINE_SMOOTH_HINT);
  glHint(GL_NICEST, GL_POLYGON_SMOOTH_HINT);

  // Activam blending-ul (necesar pentru antialiasing)
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Functia principala de randare care este apelata la fiecare "cadru"
void Display(void)
{
  std::cout << ("Call Display") << std::endl;

  // Curatam ecranul cu culoarea de fundal setata in init()
  glClear(GL_COLOR_BUFFER_BIT);

  // In functie de ultima tasta apasata, apelam functia de desenare corespunzatoare
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
    break; // Nu deseneaza nimic daca e alta tasta
  }

  // Fortam executia comenzilor OpenGL in placa video
  glFlush();
}

// Se apeleaza cand fereastra este redimensionata de catre utilizator
void Reshape(int w, int h)
{
  // Aplicam padding real pe toate laturile prin restrangerea viewport-ului
  int viewW = w - 2 * viewportPadding;
  int viewH = h - 2 * viewportPadding;

  if (viewW < 1)
    viewW = 1;
  if (viewH < 1)
    viewH = 1;

  glViewport(viewportPadding, viewportPadding, (GLsizei)viewW, (GLsizei)viewH);
}

// Gestioneaza intrarile de la tastatura
void KeyboardFunc(unsigned char key, int x, int y)
{
  prevKey = key;
  if (key == 27) // Codul ASCII pentru tasta ESC
    exit(0);

  // Spune sistemului GLUT "Hei, variabilele mele s-au schimbat, redeseneaza fereastra!"
  // Fara aceasta linie, apasarea unei taste nu s-ar reflecta vizual imediat.
  glutPostRedisplay();
}

// Gestioneaza click-urile de mouse (doar afiseaza in consola deocamdata)
void MouseFunc(int button, int state, int x, int y)
{
  std::cout << "Mouse button ";
  std::cout << ((button == GLUT_LEFT_BUTTON) ? "left" : ((button == GLUT_RIGHT_BUTTON) ? "right" : "middle")) << " ";
  std::cout << ((state == GLUT_DOWN) ? "pressed" : "released");
  std::cout << " at coordinates: " << x << " x " << y << std::endl;
}

int main(int argc, char **argv)
{
  // Initializam biblioteca GLUT
  glutInit(&argc, argv);
  glutInitWindowSize(defaultW, defaultH);
  glutInitWindowPosition(-1, -1);
  // Folosim un singur buffer de culoare (SINGLE) si spatiu de culoare RGBA
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
  glutCreateWindow(argv[0]); // Titlul ferestrei (numele executabilului)

  init(); // Apelam functia noastra de setari vizuale

  // Inregistram functiile "callback" - ii spunem lui GLUT ce functii sa apeleze cand apar anumite evenimente
  glutReshapeFunc(Reshape);
  glutKeyboardFunc(KeyboardFunc);
  glutMouseFunc(MouseFunc);
  glutDisplayFunc(Display);

  // Intram in bucla infinita care asculta dupa evenimente si redeseneaza ecranul
  glutMainLoop();

  return 0;
}