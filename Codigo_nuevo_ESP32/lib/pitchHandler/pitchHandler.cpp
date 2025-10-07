

#include "pitchHandler.h"

void PitchHandler::begin(){
    for (int i=0; i<MAX_COLS; i++){
        for (int j=0; j<MAX_ROWS; j++){
            pitches[i][j] = 0;
        }
    }
    setDefaultMap();
}

byte PitchHandler::getPitch(float x, float y){
    int xpos = getPositionInGrid(x, cols);
    int ypos = getPositionInGrid(y, rows);

    // vemos los valores limites
    if (xpos < 0 || xpos >= cols || ypos < 0 || ypos >= rows){
        Serial.println("Error: Fuera de rango");
        return 0;
    }
    byte pitch = pitches[xpos][ypos] + transpositions[xpos] + base_transposition;
    return pitch;
}

byte PitchHandler::getPreviousPitch(float x, float y){
    int xpos = getPositionInGrid(x, cols);
    int ypos = getPositionInGrid(y, rows);

    ypos -= 1;
    if (ypos < 0) ypos = 0;
    byte pitch = pitches[xpos][ypos] + transpositions[xpos] + base_transposition;
    return pitch;
}

byte PitchHandler::getNextPitch(float x, float y){
    int xpos = getPositionInGrid(x, cols);
    int ypos = getPositionInGrid(y, rows);

    ypos += 1;
    if (ypos >= rows) ypos = rows - 1;
    byte pitch = pitches[xpos][ypos] + transpositions[xpos] + base_transposition;
    return pitch;
}

int PitchHandler::getPositionInGrid(float value, int steps){
    return floor(steps*value); // funcion de cmath
}

float PitchHandler::getLowerBound(float value, int steps){
    return (1/steps)*getPositionInGrid(value, steps);
}

float PitchHandler::getUpperBound(float value, int steps){
    return (1/steps)*(getPositionInGrid(value, steps) + 1);
}

void PitchHandler::set3zonesMap(){
    rows = 3;
    cols = 1;

    pitches[0][0] = 60;
    pitches[0][1] = 72;
    pitches[0][2] = 84;
}



void PitchHandler::setDefaultMap() {

  rows = 12;
  cols = 2;

  pitches[0][0] = 60;
  pitches[0][1] = 61;
  pitches[0][2] = 62;
  pitches[0][3] = 63;
  pitches[0][4] = 64;
  pitches[0][5] = 65;
  pitches[0][6] = 66;
  pitches[0][7] = 67;
  pitches[0][8] = 68;  
  pitches[0][9] = 69;
  pitches[0][10] = 70;
  pitches[0][11] = 71;  
 
  pitches[1][0] = 72; 
  pitches[1][1] = 73;
  pitches[1][2] = 74;
  pitches[1][3] = 75;
  pitches[1][4] = 76;
  pitches[1][5] = 77;
  pitches[1][6] = 78;
  pitches[1][7] = 79;
  pitches[1][8] = 80;  
  pitches[1][9] = 81;
  pitches[1][10] = 82;
  pitches[1][11] = 83;
  
  transpositions[0] = 0;
  transpositions[1] = 0;
  transpositions[2] = 0;
  transpositions[3] = 0;
   
}

void PitchHandler::setMPEMap() {
    rows = 3;
    cols = 1;

    pitches[0][0] = 60;
    pitches[0][1] = 72;
    pitches[0][2] = 84;
    
    transpositions[0] = 0;
    transpositions[1] = 0; 
    transpositions[2] = 0;
    transpositions[3] = 0;
}

void PitchHandler::set1ColBluesMap() {

  rows = 17;
  cols = 1;

  pitches[0][0] = 46;
  pitches[0][1] = 46;
  pitches[0][2] = 46;
  pitches[0][3] = 49;
  pitches[0][4] = 51;
  pitches[0][5] = 52;
  pitches[0][6] = 53;
  pitches[0][7] = 56;
  pitches[0][8] = 58;
  pitches[0][9] = 61; 
  pitches[0][10] = 63;
  pitches[0][11] = 64;
  pitches[0][12] = 65;
  pitches[0][13] = 68;  
  pitches[0][14] = 70; 
  pitches[0][15] = 70; 
  pitches[0][16] = 70; 

  transpositions[0] = 0;
  transpositions[1] = 0;
  transpositions[2] = 0;
  transpositions[3] = 0;

}

void PitchHandler::set1ColBluesMapLong() {

  rows = 23;
  cols = 1;

  pitches[0][0] = 39;
  pitches[0][1] = 39;
  pitches[0][2] = 39;
  pitches[0][3] = 40;
  pitches[0][4] = 41;
  pitches[0][5] = 44;
  pitches[0][6] = 46;
  pitches[0][7] = 49;
  pitches[0][8] = 51;
  pitches[0][9] = 52; 
  pitches[0][10] = 53;
  pitches[0][11] = 56;
  pitches[0][12] = 58;
  pitches[0][13] = 61;  
  pitches[0][14] = 63; 
  pitches[0][15] = 64;
  pitches[0][16] = 65;
  pitches[0][17] = 68;  
  pitches[0][18] = 70; 
  pitches[0][19] = 73; 
  pitches[0][20] = 75; 
  pitches[0][21] = 75;  
  pitches[0][22] = 75; 

  transpositions[0] = 0;
  transpositions[1] = 0;
  transpositions[2] = 0;
  transpositions[3] = 0;

}


void PitchHandler::set2ColBluesMap() {

  rows = 17;
  cols = 2;

  pitches[0][0] = 39;
  pitches[0][1] = 39;
  pitches[0][2] = 39;
  pitches[0][3] = 40;
  pitches[0][4] = 41;
  pitches[0][5] = 44;
  pitches[0][6] = 46;
  pitches[0][7] = 49;
  pitches[0][8] = 51;
  pitches[0][9] = 52; 
  pitches[0][10] = 53;
  pitches[0][11] = 56;
  pitches[0][12] = 58;
  pitches[0][13] = 61;  
  pitches[0][14] = 63; 
  pitches[0][15] = 63; 
  pitches[0][16] = 63; 
  
  pitches[1][0] = 46;
  pitches[1][1] = 46;
  pitches[1][2] = 46;
  pitches[1][3] = 49;
  pitches[1][4] = 51;
  pitches[1][5] = 52;
  pitches[1][6] = 53;
  pitches[1][7] = 56;
  pitches[1][8] = 58;
  pitches[1][9] = 61; 
  pitches[1][10] = 63;
  pitches[1][11] = 64;
  pitches[1][12] = 65;
  pitches[1][13] = 68;  
  pitches[1][14] = 70; 
  pitches[1][15] = 70; 
  pitches[1][16] = 70; 

  transpositions[0] = 0;
  transpositions[1] = 0;
  transpositions[2] = 0;
  transpositions[3] = 0;

}

void PitchHandler::set3ColBluesMap() {

  rows = 17;
  cols = 3;

  pitches[0][0] = 39;
  pitches[0][1] = 39;
  pitches[0][2] = 39;
  pitches[0][3] = 40;
  pitches[0][4] = 41;
  pitches[0][5] = 44;
  pitches[0][6] = 46;
  pitches[0][7] = 49;
  pitches[0][8] = 51;
  pitches[0][9] = 52; 
  pitches[0][10] = 53;
  pitches[0][11] = 56;
  pitches[0][12] = 58;
  pitches[0][13] = 61;  
  pitches[0][14] = 63; 
  pitches[0][15] = 63; 
  pitches[0][16] = 63; 

  pitches[1][0] = 41;
  pitches[1][1] = 41;
  pitches[1][2] = 41;
  pitches[1][3] = 44;
  pitches[1][4] = 46;
  pitches[1][5] = 49;
  pitches[1][6] = 51;
  pitches[1][7] = 52;
  pitches[1][8] = 53;
  pitches[1][9] = 56; 
  pitches[1][10] = 58;
  pitches[1][11] = 61;
  pitches[1][12] = 63;
  pitches[1][13] = 64;  
  pitches[1][14] = 65; 
  pitches[1][15] = 65; 
  pitches[1][16] = 65; 

  pitches[2][0] = 51;
  pitches[2][1] = 51;
  pitches[2][2] = 51;
  pitches[2][3] = 52;
  pitches[2][4] = 53;
  pitches[2][5] = 56;
  pitches[2][6] = 58;
  pitches[2][7] = 61;
  pitches[2][8] = 63;
  pitches[2][9] = 64; 
  pitches[2][10] = 65;
  pitches[2][11] = 68;
  pitches[2][12] = 70;
  pitches[2][13] = 73;  
  pitches[2][14] = 75; 
  pitches[2][15] = 75; 
  pitches[2][16] = 75; 

  transpositions[0] = 0;
  transpositions[1] = 0;
  transpositions[2] = 0;
  transpositions[3] = 0;

}

void PitchHandler::set4ColBluesMap() {

  rows = 17;
  cols = 4;

  pitches[0][0] = 39;
  pitches[0][1] = 39;
  pitches[0][2] = 39;
  pitches[0][3] = 40;
  pitches[0][4] = 41;
  pitches[0][5] = 44;
  pitches[0][6] = 46;
  pitches[0][7] = 49;
  pitches[0][8] = 51;
  pitches[0][9] = 52; 
  pitches[0][10] = 53;
  pitches[0][11] = 56;
  pitches[0][12] = 58;
  pitches[0][13] = 61;  
  pitches[0][14] = 63; 
  pitches[0][15] = 63; 
  pitches[0][16] = 63; 

  pitches[1][0] = 41;
  pitches[1][1] = 41;
  pitches[1][2] = 41;
  pitches[1][3] = 44;
  pitches[1][4] = 46;
  pitches[1][5] = 49;
  pitches[1][6] = 51;
  pitches[1][7] = 52;
  pitches[1][8] = 53;
  pitches[1][9] = 56; 
  pitches[1][10] = 58;
  pitches[1][11] = 61;
  pitches[1][12] = 63;
  pitches[1][13] = 64;  
  pitches[1][14] = 65; 
  pitches[1][15] = 65; 
  pitches[1][16] = 65; 
  
  pitches[2][0] = 51;
  pitches[2][1] = 51;
  pitches[2][2] = 51;
  pitches[2][3] = 52;
  pitches[2][4] = 53;
  pitches[2][5] = 56;
  pitches[2][6] = 58;
  pitches[2][7] = 61;
  pitches[2][8] = 63;
  pitches[2][9] = 64; 
  pitches[2][10] = 65;
  pitches[2][11] = 68;
  pitches[2][12] = 70;
  pitches[2][13] = 73;  
  pitches[2][14] = 75; 
  pitches[2][15] = 75; 
  pitches[2][16] = 75; 


  pitches[3][0] = 53;
  pitches[3][1] = 53;
  pitches[3][2] = 53;
  pitches[3][3] = 56;
  pitches[3][4] = 58;
  pitches[3][5] = 61;
  pitches[3][6] = 63;
  pitches[3][7] = 64;
  pitches[3][8] = 65;
  pitches[3][9] = 68; 
  pitches[3][10] = 70;
  pitches[3][11] = 73;
  pitches[3][12] = 75;
  pitches[3][13] = 76;  
  pitches[3][14] = 77; 
  pitches[3][15] = 77; 
  pitches[3][16] = 77; 

  transpositions[0] = 0;
  transpositions[1] = 0;
  transpositions[2] = 0;
  transpositions[3] = 0;

}

void PitchHandler::setTibetanMap() {

  rows = 12;
  cols = 1;

  pitches[0][0] = 35;
  pitches[0][1] = 35;
  pitches[0][2] = 35;
  pitches[0][3] = 36;
  pitches[0][4] = 38;
  pitches[0][5] = 40;
  pitches[0][6] = 41;
  pitches[0][7] = 43;
  pitches[0][8] = 47;
  pitches[0][9] = 47; 
  pitches[0][10] = 47;
  pitches[0][11] = 47;

  
  
  transpositions[0] = 0;
  transpositions[1] = 0;
  transpositions[2] = 0;
  transpositions[3] = 0;

}

void PitchHandler::setArpMap() {

  rows = 8;
  cols = 2;

  pitches[0][0] = 60;
  pitches[0][1] = 62;
  pitches[0][2] = 64;
  pitches[0][3] = 65;
  pitches[0][4] = 67;
  pitches[0][5] = 69;
  pitches[0][6] = 71;
  pitches[0][7] = 72;
  
  
  pitches[1][0] = 60;
  pitches[1][1] = 62;
  pitches[1][2] = 64;
  pitches[1][3] = 65;
  pitches[1][4] = 67;
  pitches[1][5] = 69;
  pitches[1][6] = 71;
  pitches[1][7] = 72;
  
  transpositions[0] = 0;
  transpositions[1] = 12;
  transpositions[2] = 0;
  transpositions[3] = 0;

}

void PitchHandler::setContinousMap() {

  rows = 1;
  cols = 1;

  pitches[0][0] = 60;
  
  transpositions[0] = 0;
  transpositions[1] = 0;
  transpositions[2] = 0;
  transpositions[3] = 0;

}

