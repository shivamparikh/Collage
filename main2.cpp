//  Author: Shivam Parikh, Period 4, Due 1/8/15

#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <ctime>
#include "Color.h"

using namespace std;


//function definitions
void write_bmp_header_file(ofstream& output_file, int px, int pz);
int string_to_int(string s);
string makeName(int num);
int biToInt(ifstream& stream, int offset);
//end function definitions

int main(int argc, char * argv[]){
    int i, j;
    const int n = string_to_int(argv[3]);
    string inName = argv[1];
    ifstream istrm_1;
    istrm_1.open(inName.c_str(), ios::in | ios::binary);
    int fSize = biToInt(istrm_1, 2);
    int start = biToInt(istrm_1, 10);
    int width = biToInt(istrm_1, 18);
    int height = biToInt(istrm_1, 22);
    cout << width << " is the width of the file in pixels\n";
    cout << height << " is the height of the file in pixels\n";
    cout << n << " is the number of subdivisions in the image\n";
    int a = n;
    int b = 0;
    Color** arr = new Color* [n];
    for(i = 0; i < n; i++){
        arr[i] = new Color[n];
    }
    
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
    
    unsigned char** inBuffer = new unsigned char* [height];
    for (i = 0; i< height; i++){
        inBuffer[i]= new unsigned char [width*3];
    }

    int tBlue = 0;
    int tGreen = 0;
    int tRed = 0;
    int tPixel = 0;
    Color blank;
    istrm_1.seekg(start);
    for(a = n; a > 0; a--){
        for(b = 0; b < n; b++){
            for(i = (((height-1)*a)/n); i > (((height-1)*(a-1))/n); i--){
                for(j = ((width*b)/n); j < ((width*(b+1))/n)*3; j+=3){
                    int blue,green,red;
                    blue = istrm_1.get();
                    green = istrm_1.get();
                    red = istrm_1.get();
                    //cout << blue << " " << green << " " << red << "\n";
                    tBlue += blue;
                    tGreen += green;
                    tRed += red;
                    //cout << tBlue << " " << tGreen << " " << tRed << "\n";
                    inBuffer[i][j] = blue;
                    inBuffer[i][j+1] = green;
                    inBuffer[i][j+2] = red;
                    
                }
                istrm_1.seekg(padding, ios::cur);
            }
            tPixel = (height*width)/(n*n);
            //cout << "Pre Division: " << tPixel << " " << tBlue << " " << tGreen << " " << tRed << "\n";
            tBlue = tBlue/tPixel;
            tGreen = tGreen/tPixel;
            tRed = tRed/tPixel;
            //cout << "Post Division: " << tPixel << " " << tBlue << " " << tGreen << " " << tRed << "\n\n";
            blank.setVal(tRed, tGreen, tBlue);
            arr[a-1][b] = blank;
        }
        //automatically manipulated a by the for loop
    }
    //Compute Averages
    
    //Print Averages Hopefully
    for(int k = 0; k < n; k++){
        for(int l = 0; l < n; l++){
            cout << "Red: " << arr[k][l].getR() << " -- Green: " << arr[k][l].getG() << " -- Blue: " << arr[k][l].getB() << "   |   ";
        }
        cout << "\n";
    }
    delete [] arr;
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
    //int padding;
    if(rem==0)padding=0;
    else (padding=4-rem);
    write_bmp_header_file(ostrm_1, px, pz);
    unsigned char * * buffer = new unsigned char * [pz];
    for (i=0; i< pz; i++){
        buffer[i]= new unsigned char [px*3];
    }
    //initialize the buffer into the old one from the background
    for(i=0;i<pz;i++){
        for(j=0;j<3*px;j=j+3){
            buffer[i][j]=inBuffer[i][j];
            buffer[i][j+1]=inBuffer[i][j+1];
            buffer[i][j+2]=inBuffer[i][j+2];
            
        }
    }

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