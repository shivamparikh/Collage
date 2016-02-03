//  Author: Shivam Parikh, Period 4, Due 1/8/15
//argv[]:  0 = filename; 1 = inputFile Name; 2 = output file name (no extension); 3 = number of divisions in each axis; 4 = number of colors; 5 = filter type (0 = none, 1 = ho, 2 =ve, 12 = v/h)
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include "Color.h"

using namespace std;


//function definitions
void inputFile(unsigned char** buffer, ifstream& istream, int width, int height, int start, int padding);
void transferBuffer(unsigned char** in, unsigned char** out, int width, int height);
Color calcAverage(unsigned char** buffer, int wStart, int hStart, int wFin, int hFin);
void drawAverage(unsigned char** buffer, int wStart, int hStart, int wFin, int hFin);
void drawAverageTotal(unsigned char** buffer, int width, int height, int n);
void tint(Color color, int px1, int px2, int pz1, int pz2, unsigned char** inBuffer);
double smallest(Color color);
void horizontalFilter(unsigned char** buffer, Color* colr, int width, int height, int div);
void verticalFilter(unsigned char** buffer, Color* colr, int width, int height, int div);
int biToInt(ifstream& stream, int offset);
void write_bmp_header_file(ofstream& output_file, int px, int pz);
int string_to_int(string s);
string makeName(int num);
//end function definitions

int main(int argc, char * argv[]){
    int i, j;
    srand(time(0));
    string inName = argv[1];
    ifstream istrm_1;
    istrm_1.open(inName.c_str(), ios::in | ios::binary);
    int fSize = biToInt(istrm_1, 2);
    int start = biToInt(istrm_1, 10);
    int width = biToInt(istrm_1, 18);
    int height = biToInt(istrm_1, 22);
    cout << width << " is the width of the file in pixels\n";
    cout << height << " is the height of the file in pixels\n";
    int subSize = string_to_int(argv[3]);
    int numSplit = string_to_int(argv[4]);
    int filterType = string_to_int(argv[5]);
    cout << "\nThe split count is " << numSplit << "\n";
    //Scan Lines
    int lineSize = width*3;
    int padding = 0;
    if (lineSize % 4 != 0){
        padding = 4 - lineSize % 4;
    }
    if (fSize != start + (lineSize + padding) * height){
        cout << "That's how you know you messed up" << endl;
        return 1;
    }
    //Initialize the Input Buffer
    unsigned char** inBuffer = new unsigned char* [height];
    for (i = 0; i< height; i++){
        inBuffer[i]= new unsigned char [width*3];
    }
    //Input the file itself, transfer it to the buffer we will be using.
    inputFile(inBuffer, istrm_1, width, height, start, padding);
    //For Outputting the File At the END
    int px = width;
    int pz = height;
    string nom = argv[2];
    nom = nom + ".bmp";
    ofstream ostrm_1;
    ostrm_1.open(nom.c_str(), ios::out | ios::binary);
    if(ostrm_1.fail()){
        cout << "Error.  Can't open output file " << nom << "." << endl;
        return 0;
    }
    cout << "Opening output file " << nom <<"." << endl;
    //end make names of files

    string arg;
    if(px<=0){
        ostrm_1.close();
        return -1;
    }
    if(pz<=0){
        ostrm_1.close();
        return -1;
    }
    int rem;
    rem=3*px%4;
    if(rem==0)padding=0;
    else (padding=4-rem);
    write_bmp_header_file(ostrm_1, px, pz);
    unsigned char * * buffer = new unsigned char * [pz];
    for (i=0; i< pz; i++){
        buffer[i]= new unsigned char [px*3];
    }
    transferBuffer(inBuffer, buffer, width, height);
    Color* bl = new Color[numSplit];
    Color blue(80, 80, 175);
    Color red(175, 80, 80);
    Color white(254, 254, 254);
    Color bk[] = {blue, white, red};
    for(int i = 0; i < numSplit; i++){
        Color rand(80, 200);
        cout << rand.getR() << " " << rand.getG() << " " << rand.getB() <<"\n";
        bl[i] = rand;
    }
    switch(filterType){
        case 0: break;
        case 1: horizontalFilter(buffer, bl, width, height, numSplit);
            break;
        case 2: verticalFilter(buffer, bl, width, height, numSplit);
            break;
        case 12: horizontalFilter(buffer, bl, width, height, numSplit);
                verticalFilter(buffer, bl, width, height, numSplit);
                break;
        default: break;
    }
    if(subSize > 0){
        drawAverageTotal(buffer, width, height, subSize);
    }

    delete[] bl;
    unsigned char p_buffer[4];
    p_buffer[0]=0;
    p_buffer[1]=0;
    p_buffer[2]=0;
    p_buffer[3]=0;

    for(i=pz-1;i>=0;i--){
        ostrm_1.write ((char*)buffer[i], px*3*sizeof (unsigned char));
        ostrm_1.write ((char*)p_buffer, padding*sizeof (unsigned char));
    }
    //Erase the memory of the buffer after printing it, so that we can start over.
    for(int i = 0; i<pz;i++){
        delete[] buffer[i];
        buffer[i] = NULL;
    }
    delete[] buffer;
    ostrm_1.close();
    return 0;
}


void inputFile(unsigned char** buffer, ifstream& istream, int width, int height, int start, int padding){
    istream.seekg(start);
    for(int i = height-1; i > 0; i--){
        for(int j = 0; j < width*3; j+=3){
            buffer[i][j] = istream.get();
            buffer[i][j+1] = istream.get();
            buffer[i][j+2] = istream.get();
        }
        istream.seekg(padding, ios::cur);
    }
}
Color calcAverage(unsigned char** buffer, int wStart, int hStart, int wFin, int hFin){
    Color avg;
    int red = 0;
    int green = 0;
    int blue = 0;
    for(int i = hStart; i < hFin; i++){
        for(int j = wStart*3; j < wFin*3; j+=3){
            blue += buffer[i][j];
            green += buffer[i][j+1];
            red += buffer[i][j+2];
        }
    }
    int size = (wFin - wStart+1)*(hFin-hStart+1);
    avg.setB(blue/size);
    avg.setG(green/size);
    avg.setR(red/size);
    //cout << red/size << " " << green/size << " " << blue/size << "\n";
    return avg;
}
void drawAverage(unsigned char** buffer, int wStart, int hStart, int wFin, int hFin){
    Color avg;
    int red = 0;
    int green = 0;
    int blue = 0;
    for(int i = hStart; i < hFin; i++){
        for(int j = wStart*3; j < wFin*3; j+=3){
            blue += buffer[i][j];
            green += buffer[i][j+1];
            red += buffer[i][j+2];
        }
    }
    int size = (wFin - wStart+1)*(hFin-hStart+1);
    red/=size;
    green/=size;
    blue/=size;
    //cout << red << " " <<  green << " " << blue << "\n";
    for(int i = hStart; i < hFin; i++){
        for(int j = (wStart*3); j < wFin*3; j+=3){
            buffer[i][j] = blue;
            buffer[i][j+1] = green;
            buffer[i][j+2] = red;
        }
    }
}
void drawAverageTotal(unsigned char** buffer, int width, int height, int n){
    int x = width/n;
    int y = height/n;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            drawAverage(buffer, (x*i), (y*j), (x*(i+1)), (y*(j+1)));
        }
    }
}

void transferBuffer(unsigned char** in, unsigned char** out, int width, int height){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width*3; j+=3){
            out[i][j] = in[i][j];
            out[i][j+1] = in[i][j+1];
            out[i][j+2] = in[i][j+2];
        }
    }
}
void tint(Color color, int px1, int px2, int pz1, int pz2, unsigned char** inBuffer){
    double small = smallest(color);
    double tAlpha=(255-small)/255;
    double myB = (color.getB()-small)/tAlpha;
    double myG = (color.getG()-small)/tAlpha;
    double myR = (color.getR()-small)/tAlpha;
    for(int i=pz1;i<pz2;i++){
        for(int j=3*px1;j<3*px2;j=j+3){
            inBuffer[i][j] += (myB - inBuffer[i][j])*tAlpha;
            inBuffer[i][j+1] += (myG - inBuffer[i][j+1])*tAlpha;
            inBuffer[i][j+2] += (myR - inBuffer[i][j+2])*tAlpha;
            
        }
    }
}
void horizontalFilter(unsigned char** buffer, Color* colr, int width, int height, int div){
    int skip = height/div;
    for(int i = 0; i < div; i++){
        tint(colr[i], 0, width, i*skip, ((i+1)*skip), buffer);
    }
}
void verticalFilter(unsigned char** buffer, Color* colr, int width, int height, int div){
    int wSkip = width/div;
    for(int i = 0; i < div; i++){
        tint(colr[i], i*wSkip, (i+1)*wSkip, 0, height, buffer);
    }
}
double smallest(Color color){
    double num[3] = {color.getR(), color.getG(), color.getB()};
    double smallest = num[0];
    for(int i = 1; i<3; i++){
        if(num[i]<smallest){
            smallest = num[i];
        }
    }
    return smallest;
}
int biToInt(ifstream& stream, int offset){
    stream.seekg(offset);
    int result = 0;
    int base = 1;
    for (int i = 0; i < 4; i++){
        result = result + stream.get() * base;
        base = base * 256;
    }
    return result;
}

void write_bmp_header_file(ofstream& output_file, int px, int pz){
    unsigned short int bfType;
    bfType = 0x4D42;
    output_file.write ((char*)&bfType, sizeof (short int));
    unsigned int bfSize;
    int rem;
    rem=3*px%4;
    int padding;
    if(rem==0){
        padding=0;
    }
    else{
        padding=4-rem;
    }
    
    bfSize = 14 + 40 + (3 * px+padding) * pz;
    //	bfSize = 14 + 40 + (3 * px+padding) * pz + 2;
    output_file.write ((char*)&bfSize, sizeof (int));
    
    unsigned short int bfReserved1;
    bfReserved1 = 0;
    output_file.write ((char*)&bfReserved1, sizeof (short int));
    
    unsigned short int bfReserved2;
    bfReserved2 = 0;
    output_file .write ((char*)&bfReserved2, sizeof (short int));
    
    unsigned int bfOffsetBits;
    bfOffsetBits = 14 + 40;
    output_file.write ((char*)&bfOffsetBits, sizeof (int));
    
    unsigned int biSize;
    biSize=40;
    output_file.write ((char*)&biSize, sizeof (int));
    
    int biWidth;
    biWidth=px;
    output_file.write ((char*)&biWidth, sizeof (int));
    
    int biHeight;
    biHeight=pz;
    output_file.write ((char*)&biHeight, sizeof (int));
    
    unsigned short int biPlanes;
    biPlanes=1;
    output_file.write ((char*)&biPlanes, sizeof (short int));
    
    unsigned short int biBitCount;
    biBitCount=24;
    output_file.write ((char*)&biBitCount, sizeof (short int));
    
    unsigned int biCompression;
    // #define BI_RGB 0
    unsigned int bi_rgb = 0;
    //	biCompression=BI_RGB;
    biCompression=bi_rgb;
    output_file.write ((char*)&biCompression, sizeof (int));
    
    unsigned int biSizeImage;
    biSizeImage=0;
    output_file.write ((char*)&biSizeImage, sizeof (int));
    
    unsigned int biXPelsPerMeter;
    biXPelsPerMeter=0;
    output_file.write ((char*)&biXPelsPerMeter, sizeof (int));
    
    unsigned int biYPelsPerMeter;
    biYPelsPerMeter=0;
    output_file.write ((char*)&biYPelsPerMeter, sizeof (int));
    
    unsigned int biClrUsed;
    biClrUsed = 0;
    output_file.write ((char*)&biClrUsed, sizeof (int));   
    
    unsigned int biClrImportant;
    biClrImportant = 0;
    output_file.write ((char*)&biClrImportant, sizeof (int));   
}
int string_to_int(string s){
    istringstream strm;
    strm.str(s);
    int n=0;
    strm >> n;
    return n;
}
string makeName(int num){
    ostringstream strm;
    strm << setfill('0') << setw(4) << num;
    return strm.str();
}