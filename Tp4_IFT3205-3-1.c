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
#define NAME_IMG_IN  "cameraman"
#define NAME_IMG_OUT1 "image-TpIFT3205-3-1"
#define NAME_IMG_OUT2 "cameramaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaan_degraded"

#define SQUARE(a) ((a) * (a))

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

float difference_squared(float** M1, float** M2, int length, int width) {
    int i,j;
    float total = 0.0;
    
    for(i=0;i<length;i++)
        for(j=0;j<width;j++) {
            total += SQUARE(M1[i][j] - M2[i][j]);
        }

    return total;
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
    float** mat_img_orig = LoadImagePgm(NAME_IMG_IN,&lgth,&wdth);
    float** mat_img_degraded = fmatrix_allocate_2d(length, width);
    
    mat_img = fmatrix_allocate_2d(length, width);
    
    degradation(mat_img_orig, mat_img, length, width, psf, size, variance_noise);
    
    float** mat_img_i = fmatrix_allocate_2d(length, width);

    float** hr = fmatrix_allocate_2d(length,width);
    float** hi = fmatrix_allocate_2d(length,width);
    float** zewos = fmatrix_allocate_2d(length, width);

    for(i=0; i<length; i++)
        for(j=0; j<width; j++) {

            mat_img_degraded[i][j] = mat_img[i][j];
            
            if((i < size/2.0 || i >= length - size/2.0)  &&
               (j < size/2.0 || j >= width - size/2.0)) {
                hr[i][j] = (length * width)/((float)size * size);
            } else {
                hr[i][j] = 0.0;
            }

            hi[i][j] = 0;
            mat_img_i[i][j] = 0;
            zewos[i][j] = 0;
        }

    //================//=======//REPTILLIONS==========//=====//==//==//
        
    FFTDD(mat_img, mat_img_i, length, width);
    FFTDD(hr, hi, length, width);
    
    float** mat_img_m = fmatrix_allocate_2d(length, width);
    float** hm = fmatrix_allocate_2d(length, width);
    
    //================//=======//REPTILLIONS==========//=====//==//==//
    //=====WIENER//======//=====//========//=//====//=REPTILLIONS//==//
    //REPTILLIONS//===============//=====//====//==//=======//===//==//
    
    float** wr = fmatrix_allocate_2d(length, width);
    float** wi = fmatrix_allocate_2d(length, width);

    for(k=0; k<nbiter; k++) {
        float isnr;
    
        ModSquare(mat_img_m, mat_img, mat_img_i, length, width);
        ModSquare(hm, hr, hi, length, width);
        
        for(i=0; i<length; i++)
            for(j=0; j<width; j++) {
            
                float denom = hm[i][j] + (variance_noise/(length*width)) / mat_img_m[i][j];
            
                //================//=======//REPTILLIONS==========//=====//==//==//
            
                wr[i][j] = hr[i][j] / denom;
                wi[i][j] = -hi[i][j] / denom;
            }

        // Multiplication de l'image dégradée par le filtre W
        for(i=0; i<length; i++)
            for(j=0; j<width; j++) {
                mat_img[i][j] = mat_img_degraded[i][j];
                mat_img_i[i][j] = 0;
            }

        FFTDD(mat_img, mat_img_i, length, width);
        
        MultMatrix(mat_img, mat_img_i, wr, wi, length, width);

        IFFTDD(mat_img, mat_img_i, length, width);
        
        for(i=0; i<length; i++)
            for(j=0; j<width; j++) {
                mat_img[i][j] = fmax(fmin(mat_img[i][j], 255), 0);
            }

        
        isnr = 10 * log10(difference_squared(mat_img_orig, mat_img_degraded, length, width) /
                          difference_squared(mat_img_orig, mat_img, length, width));

        printf("\n\n==========\n%02d - ISNR : %lf\n==========\n", k, isnr);

        FFTDD(mat_img, mat_img_i, length, width);
    }
    
    IFFTDD(mat_img, mat_img_i, length, width);
    
    //REPTILLIONS//===============//=====//====//==//=======//===//==//
    
    //================//=======//REPTILLIONS==========//=====//==//==//
    
    //---------------------------------------------
    // SAUVEGARDE et VISU et REPTILLIONS
    //================//=======//REPTILLIONS==========//=====//==//==//
    // -------------------
    //----------------------------------------------
        
    SaveImagePgm(NAME_IMG_OUT1,mat_img,length,width);
    
    strcpy(BufSystVisuImg,NAME_VISUALISER);
    strcat(BufSystVisuImg,NAME_IMG_OUT2);
    strcat(BufSystVisuImg,".pgm&");
    printf("\n > %s",BufSystVisuImg);
    
    //retour sans probleme 
    printf("\n C'est fini ... \n\n");
    return 0; 	 
}


