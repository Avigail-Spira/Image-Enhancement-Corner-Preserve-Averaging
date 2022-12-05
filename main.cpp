#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

class imageProcessing {
public:
  int numRows;
  int numCols;
  int minVal;
  int maxVal;
  int **frameAry;
  int **outAry;
  int **thrAry;
  int thrVal;
  int mask[8][5][5];

  imageProcessing(ifstream &inFile, int threshold) {
    std::string line;
    getline(inFile, line);
    stringstream ssin(line);
    ssin >> numRows >> numCols >> minVal >> maxVal;

    thrVal = threshold;

    frameAry = new int *[numRows + 4];
    outAry = new int *[numRows + 4];
    thrAry = new int *[numRows + 4];
    for (int i = 0; i < numRows + 4; i++) {
      frameAry[i] = new int[numCols + 4];
      outAry[i] = new int[numCols + 4];
      thrAry[i] = new int[numCols + 4];
    }
  } 

  ~imageProcessing() {
    for (int i = 0; i < numRows + 4; i++) {
      delete[] frameAry[i];
      delete[] outAry[i];
      delete[] thrAry[i];
    }
    delete[] frameAry;
    delete[] outAry;
    delete[] thrAry;
  }

  void loadImage(ifstream &inFile) {
    int row = 2;
    std::string line;
    while (std::getline(inFile, line)) {
      stringstream ssin(line);
      int number;
      for (int i = 2; i < numCols+2; i++) {
        ssin >> number;
        frameAry[row][i] = number;
      }
      row++;
    }
  }

  void mirrorFraming() {
    int rows = numRows + 4;
    int cols = numCols + 4;
    
    for (int i = 0; i < rows; i++) {
        frameAry[0][i] = frameAry[3][i];
        frameAry[1][i] = frameAry[2][i];
        frameAry[rows-2][i] = frameAry[rows-3][i];
        frameAry[rows-1][i] = frameAry[rows-4][i];
    }

    for (int j = 0; j < cols; j++) {
        frameAry[j][0] = frameAry[j][3];
        frameAry[j][1] = frameAry[j][2];
        frameAry[j][cols-2] = frameAry[j][cols-3];
        frameAry[j][cols-1] = frameAry[j][cols-4];
    }
    
  }

  void loadMask() {
    int array[8][5][5] = {
      { {0,0,0,0,0}, {0,0,0,0,0}, {0,0,1,0,0}, {0,1,1,1,0}, {1,1,1,1,1} },
      { {1,0,0,0,0}, {1,1,0,0,0}, {1,1,1,0,0}, {1,1,0,0,0}, {1,0,0,0,0} },
      { {1,1,1,1,1}, {0,1,1,1,0}, {0,0,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
      { {0,0,0,0,1}, {0,0,0,1,1}, {0,0,1,1,1}, {0,0,0,1,1}, {0,0,0,0,1} },
      { {1,1,1,0,0}, {1,1,1,0,0}, {1,1,1,0,0}, {0,0,0,0,0}, {0,0,0,0,0} },
      { {0,0,1,1,1}, {0,0,1,1,1}, {0,0,1,1,1}, {0,0,0,0,0}, {0,0,0,0,0} },
      { {0,0,0,0,0}, {0,0,0,0,0}, {0,0,1,1,1}, {0,0,1,1,1}, {0,0,1,1,1} },
      { {0,0,0,0,0}, {0,0,0,0,0}, {1,1,1,0,0}, {1,1,1,0,0}, {1,1,1,0,0} }
    };

    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 5; j++) {
        for (int k = 0; k < 5; k++) {
          mask[i][j][k] = array[i][j][k];
        }
      }
    }
    
  }

  int convolution5x5(int row, int col, int mask2D[5][5]) {
    int sum = 0, maskRow = 0;

    for (int i = row-2; i <= row+2; i++) {
      int maskCol = 0;
      for (int j = col-2; j <= col+2; j++) {
        sum += frameAry[i][j] * mask2D[maskRow][maskCol];
        maskCol++;
      }
      maskRow++;
    }

    return sum;
  }

  void cornerPreserveAvg() {
    for (int r=2; r < numRows+2; r++) {
      for (int c=2; c < numCols+2; c++) {
        int maskIndex = 0;
        int minAvg = frameAry[r][c];
        int minDiff = 9999;

        while(maskIndex < 8) {
          int result = convolution5x5(r,c,mask[maskIndex])/9;
          int diff = abs(result - frameAry[r][c]);

          if (diff < minDiff) {
            minDiff = diff;
            minAvg = result;
          }
          maskIndex++;
        }
        outAry[r][c] = minAvg;
      }
    }
  }

  void threshold(int **array) {
    for (int i=0; i<numRows+4; i++) {
      for (int j=0; j< numCols+4; j++) {
        if (array[i][j] >= thrVal)
          thrAry[i][j] = 1;
        else
          thrAry[i][j] = 0;
      }
    }
  }

  void imgReformat(int **inAry, int newMin, int newMax, ofstream &outFile) {
    string str = to_string(newMax);
    int width = str.length();
    int r = 0;

    while (r < numRows+4) {
      int c = 0;
      while (c < numCols+4) {
        outFile << inAry[r][c];
        str = to_string(inAry[r][c]);
        int ww = str.length();

        while (ww < width) {
          outFile << " ";
          ww++;
        }
        c++;
      }
      outFile << endl;
      r++;
    }
  }
};

int main(int argc, char *argv[]) {
  ifstream inFile(argv[1]);
  ofstream outFile1(argv[3]);
  ofstream outFile2(argv[4]);

  imageProcessing ip(inFile, std::atoi(argv[2]));

  ip.loadImage(inFile);                        
  ip.mirrorFraming();
  ip.loadMask();

  outFile1 << ip.numRows << " " << ip.numCols << " " << ip.minVal << " " << ip.maxVal << endl;
  
  ip.imgReformat(ip.frameAry, ip.minVal, ip.maxVal, outFile1);
  
  ip.threshold(ip.frameAry);
  ip.imgReformat(ip.thrAry, 0, 1, outFile1);
  
  ip.cornerPreserveAvg();
  ip.imgReformat(ip.outAry, ip.minVal, ip.maxVal, outFile1);

  ip.threshold(ip.outAry);
  ip.imgReformat(ip.thrAry, 0, 1, outFile1);

  outFile2 << ip.numRows << " " << ip.numCols << " 0 1" << endl;
  
  for (int i = 2; i < ip.numRows+2; i++) {
    for (int j = 2; j < ip.numCols+2; j++) {
      outFile2 << ip.thrAry[i][j] << " ";
    }
    outFile2 << endl;
  }

  inFile.close();
  outFile1.close();
  outFile2.close();
}
