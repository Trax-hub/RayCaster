#include <Gamebuino-Meta.h>
#include <math.h>
#include <arduino.h>
#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define degInRad 0.0174533

//Map
const uint8_t mapWidth = 8;
const uint8_t mapHeight = 8;
const uint8_t mapSize = 64;
const uint8_t blockSize = 8;
const uint8_t margin = 8;
bool mapEnabled = false;

//Matrice de la map
const uint8_t mapData[] =
{
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 1, 0, 0, 1,
  1, 1, 1, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
};


//Caractéristiques joueur
float pX = 32.0f;
float pY = 32.0f;
float cameraAngle = PI / 2;
float pDeltaX = cos(cameraAngle);
float pDeltaY = sin(cameraAngle);



//          =============DESSINE LA MAP================
void drawMap() {

  //on colore le fond en blanc
  gb.display.fill(WHITE);

  int x, y;

  //boucle pour parcourir les colonnes de la map
  for (y = 0; y < mapHeight; y++) {

    //boucle pour parcourir les lignes de la map
    for (x = 0; x < mapWidth; x++) {

      //verification si mur ou pas
      if (mapData[y * mapWidth + x] == 1) {
        gb.display.setColor(BLACK);                 //Sélection du noir pour les murs
      } else {
        gb.display.setColor(96, 96, 96);            //Colorie les espaces vides en gris
      }
      gb.display.fillRect(x * blockSize + margin, y * blockSize, blockSize, blockSize);      //Dessin de murs
    }
  }
  }


//         =============DESSINE LE JOUEUR=============
void drawPlayer() {
  int px, py;
  px = pX;
  py = pY;

  //dessin l'angle de la camera
  gb.display.setColor(BLUE);
  gb.display.drawLine(px + margin, py, px + margin + pDeltaX  * blockSize, py + pDeltaY * blockSize);

  //dessine le joueur
  gb.display.drawPixel(px + margin, py, YELLOW);
}



//        =================LANCE LES RAYONS==================

/*
   rx : position X du rayon
   ry : position y du rayon
   ra : angle du rayon
   xo : offset en x du rayon
   yo : offset en y du rayon

   distanceF : distance définitive du rayon
   distanceH : distance provisoire entre le joueur et le rayon horizontal rencontrant un mur 
   distanceF : distance provisoire entre le joueur et le rayon vertical rencontrant un mur

   r : numéro du rayon
   mx : position x sur la map
   my : position y sur la map
   mp : indice correspondant à mx et my dans le tableau MapData
   exit : permet de limiter les calculs interminables
*/

void draw2DRays() {
  float rx, ry, ra, xo, yo, distanceF;
  int r, mx, my, mp, exit;

  //Coloration du ciel
  gb.display.setColor(WHITE);
  gb.display.fillRect(0,0,80,32);
  
  //Coloration de sol
  gb.display.setColor(200, 200, 200);
  gb.display.fillRect(0,32,80,64);
  

  ra = cameraAngle - degInRad * 40;

  for (r = 0; r < 80 ; r++) {
    if (ra < 0) {
    ra += 2 * PI;
    }
    if (ra > 2 * PI) {
      ra -= 2 * PI;
    }

    // == Lignes Horizontales ==
    exit = 0;
    float distanceH = 99999;          //Valeur grande à l'initialisation pour etre sur qu'elle soit modifiée
    float hx = pX;
    float hy = pY;
    float aTan = -1 / tan(ra);

    // == Vers le Haut ==
    if (ra > PI) {
      ry = (((int)pY / 8) * 8) - 0.001;
      rx = (pY - ry) * aTan + pX;
      yo = -blockSize;
      xo = -yo * aTan;
    }

    // == Vers le Bas ==
    if (ra < PI) {
      SerialUSB.print("\n===VERS LE BAS===");
      ry = (((int)pY / 8) * 8) + blockSize;
      rx = (pY - ry) * aTan + pX;
      yo = blockSize;
      xo = -yo * aTan;
    }

    // == Horizontal parfait ==
    if (ra == 0 || ra == PI) {
      rx = pX;
      ry = pY;
      exit = mapHeight;
    }

    while (exit < mapHeight) {
      mx = (int) (rx) / 8;
      my = (int) (ry) / 8;
      mp = my * mapWidth + mx;

      if (mp > 0 && mp < mapWidth * mapHeight && mapData[mp] == 1) { //Si le rayon rencontre un mur
        hx = rx;
        hy = ry;
        distanceH = distance(pX, pY, hx, hy, ra);
        exit = mapHeight;
      } else {
        rx += xo;
        ry += yo;
        exit += 1;
      }
    }


    //----Lignes Verticales----
    exit = 0;
    float distanceV = 99999;
    float vx = pX;
    float vy = pY;
    float nTan = -tan(ra);

    // == Vers la Droite ==
    if (ra > P2 && ra < P3) {
      rx = (((int)pX / 8) * 8) - 0.01;
      ry = (pX - rx) * nTan + pY;
      xo = -blockSize;
      yo = -xo * nTan;
    }

    // == Vers a Gauche ==
    if (ra < P2 || ra > P3) {
      rx = (((int)pX / 8) * 8) + 8;
      ry = (pX - rx) * nTan + pY;
      xo = blockSize;
      yo = -xo * nTan;
    }
    if (ra == 0 || ra == PI) {
      rx = pX;
      ry = pY;
      exit = mapWidth;
    }

    while (exit < 8) {
      mx = (int) (rx) / 8;
      my = (int) (ry) / 8;
      mp = my * mapWidth + mx;
      if (mp > 0 && mp < mapWidth * mapHeight && mapData[mp] == 1) { //Si le rayon rencontre un mur
        vx = rx;
        vy = ry;
        distanceV = distance(pX, pY, vx, vy, ra);
        exit = mapWidth;
      } else {
        rx += xo;
        ry += yo;
        exit += 1;
      }
    }


    // == Choix du rayon ==

    if (distanceV < distanceH) {
      rx = vx;
      ry = vy;
      distanceF = distanceV;
      gb.display.setColor(0, 0, 250);
    }

    if (distanceV > distanceH) {
      rx = hx;
      ry = hy;
      distanceF = distanceH;
      gb.display.setColor(0, 0, 200);
    }

    //on passe au rayon suivant
    ra += degInRad;

    //======MURS======
    float ca = cameraAngle - ra;

    if (ca < 0) {
      ca += 2 * PI;
    }

    if (ca > 2 * PI) {
      ca -= 2 * PI;
    }

    //Annulation de l'effet "Fish Eye"
    distanceF = distanceF * cos(ca);


    float lineH = mapSize * 8 / distanceF;

    if (lineH > 64) {
      lineH = 64;
    }

    //permet d'aligner le mur au milieu de l'écran
    float lineO = 32 - lineH / 2;

    //dessine la ligne du mur
    gb.display.drawLine(r , lineO , r, lineH + lineO);
  }
}

float distance(float ax, float ay, float bx, float by, float ang) {
  return (sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay))); //Pythagore
}

void setup() {
  gb.begin();
}

void loop() {

  while (!gb.update()) {
    gb.display.clear();

    if (!mapEnabled) {
      draw2DRays();
    } else {
      drawMap();
      drawPlayer();
    }

    //si le jouer tourne sa camera à gauche
    if (gb.buttons.repeat(BUTTON_LEFT, 1)) {
      cameraAngle -= 0.1;
      if (cameraAngle < 0) {
        cameraAngle += 2 * PI;
      }
      pDeltaX = cos(cameraAngle);
      pDeltaY = sin(cameraAngle);
    }

    //si le joueur tourne sa camera à droite
    if (gb.buttons.repeat(BUTTON_RIGHT, 1)) {
      cameraAngle += 0.1;
      if (cameraAngle > 2 * PI) {
        cameraAngle -= 2 * PI;
      }
      pDeltaX = cos(cameraAngle);
      pDeltaY = sin(cameraAngle);
    }

    //si le joueur va en avant
    if (gb.buttons.repeat(BUTTON_UP, 1)) {
      float oldX = pX;
      float oldY = pY;
      pX += pDeltaX;
      pY += pDeltaY;
      int mx = (int) (pX) >> 3;
      int my = (int) (pY) >> 3;
      int mp = my * mapWidth + mx;
      if (mapData[mp] == 1) {
        pX = oldX;
        pY = oldY;
      }
    }

    //si le joeur va en arrière
    if (gb.buttons.repeat(BUTTON_DOWN, 1)) {
      float oldX = pX;
      float oldY = pY;
      pX -= pDeltaX;
      pY -= pDeltaY;
      int mx = (int) (pX) >> 3;
      int my = (int) (pY) >> 3;
      int mp = my * mapWidth + mx;
      if (mapData[mp] == 1) {
        pX = oldX;
        pY = oldY;
      }
    }

    if (gb.buttons.released(BUTTON_A)) {
      mapEnabled = true;
    }

    if (gb.buttons.released(BUTTON_B)) {
      mapEnabled = false;
    }
  }




}
