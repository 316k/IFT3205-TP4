  //------------------------------------------------------
// Prog    : Tp4_IFT3205                          
// Auteur  :                                            
// Date    :                                  
// version :                                             
// langage : C                                          
// labo    : DIRO                                       
//------------------------------------------------------

//------------------------------------------------
// FICHIERS INCLUS -------------------------------
//------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "FonctionDemo4.h"

//------------------------------------------------
// DEFINITIONS -----------------------------------   
//------------------------------------------------
#define NAME_VISUALISER "display "
#define NAME_IMG_IN  "cameraman_degraded"
#define NAME_IMG_OUT1 "image-TpIFT3205-2-2"
#define NAME_IMG_OUT2 "cameramaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaan_degraded"

//------------------------------------------------
// PROTOTYPE DE FONCTIONS  -----------------------   
//------------------------------------------------

//------------------------------------------------
// VARIABLES GLOBALES  ---------------------------   
//------------------------------------------------

float** mat;
float** mat_img;
float** mat_rest;
float** mat_rest_prec;
float** mat_rest_best;

//>Taille Image
int length=256;
int width=256;
int size_image=256;

//>Parametre de degradation
int size=9;
float variance_noise=0.5;
int psf=1;

//>Nb d'Iterations
int nbiter=20;

//>ImprovmentSNR
float isnr;

//------------------------------------------------
//------------------------------------------------
// FONCTIONS  ------------------------------------   
//------------------------------------------------
//------------------------------------------------
void PreFFT_Translation(float** Matrix, int length, int width) {
    int x,y;
    for(x=0; x<length; x++)
        for(y=0; y<width; y++)
            if((x + y) % 2 == 1)
                Matrix[x][y] = -Matrix[x][y];
}

//---------------------------------------------------------
//---------------------------------------------------------
// PROGRAMME PRINCIPAL   ----------------------------------                     
//---------------------------------------------------------
//---------------------------------------------------------
int main(int argc,char **argv)
{
    int i,j,k;
    int lgth,wdth;
    char BufSystVisuImg[100];

    //Allocation memoire matrice
    mat=fmatrix_allocate_2d(length,width);
    mat_rest=fmatrix_allocate_2d(length,width);
    mat_rest_prec=fmatrix_allocate_2d(length,width); 
    mat_rest_best=fmatrix_allocate_2d(length,width); 
 
    //=========================================================
    //== PROG =================================================
    //=========================================================

    //>Lecture Image
    mat_img = LoadImagePgm(NAME_IMG_IN,&lgth,&wdth);
    float** mat_img_i = fmatrix_allocate_2d(length, width);

    float** hr = fmatrix_allocate_2d(length,width);
    float** hi = fmatrix_allocate_2d(length,width);

    for(i=0; i<length; i++)
        for(j=0; j<width; j++) {

            if(i > length/2 - size/2 && i < length/2 + size/2 &&
               j > width/2 - size/2 && j < width/2 + size/2) {

                hr[i][j] = 1.0/(size*size) * length * width;
                hi[i][j] = 1.0/(size*size) * length * width;
                
            } else {
                hr[i][j] = 0.0;
            }

            mat_img_i[i][j] = 0;
        }
    //================//=======//REPTILLIONS==========//=====//==//==//

    IFFTDD(hr, hi, length, width);
    PreFFT_Translation(hr, length, width);
    PreFFT_Translation(hi, length, width);
    FFTDD(hr, hi, length, width);
    
    FFTDD(mat_img, mat_img_i, length, width);
    FFTDD(hr, hi, length, width);
    
    float** mat_img_m = fmatrix_allocate_2d(length, width);
    float** hm = fmatrix_allocate_2d(length, width);
    
    ModSquare(mat_img_m, mat_img, mat_img_i, length, width);
    ModSquare(hm, hr, hi, length, width);

    Recal(mat_img_m, length, width);
    Recal(hm, length, width);
      //================//=======//REPTILLIONS==========//=====//==//==//
     //=====WIENER//======//=====//========//=//====//=REPTILLIONS//==//
    //REPTILLIONS//===============//=====//====//==//=======//===//==//
    
    float** wr = fmatrix_allocate_2d(length, width);
    float** wi = fmatrix_allocate_2d(length, width);
    
    for(i=0; i<length; i++)
        for(j=0; j<width; j++) {
            
            float denom = hm[i][j] + variance_noise / (length * width * mat_img_m[i][j]);
            
            //================//=======//REPTILLIONS==========//=====//==//==//

            wr[i][j] = hr[i][j] / denom;
            wi[i][j] = -hi[i][j] / denom;
            
            mat_img[i][j] *= wr[i][j];
            mat_img_i[i][j] *= wi[i][j];
        }
    
    //REPTILLIONS//===============//=====//====//==//=======//===//==//

    FFTDD(mat_img, mat_img_i, length, width);
    
    //---------------------------------------------
    // SAUVEGARDE et VISU
    //================//=======//REPTILLIONS==========//=====//==//==//
    // -------------------
    //----------------------------------------------
    Recal(mat_img, length, width);
    SaveImagePgm(NAME_IMG_OUT1,mat_img,length,width);


    strcpy(BufSystVisuImg,NAME_VISUALISER);
    strcat(BufSystVisuImg,NAME_IMG_OUT2);
    strcat(BufSystVisuImg,".pgm&");
    printf("\n > %s",BufSystVisuImg);
    
    //retour sans probleme 
    printf("\n C'est fini ... \n\n");
    return 0; 	 
}


