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
#define NAME_IMG_OUT1 "cameraman_restored"
#define NAME_IMG_OUT2 "cameraman_degraded"

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
float** mat_psf;

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
    mat_psf=fmatrix_allocate_2d(length,width);

 
    //=========================================================
    //== PROG =================================================
    //=========================================================

    //>Lecture Image
    mat_img=LoadImagePgm(NAME_IMG_IN,&lgth,&wdth);

    //--------------------------------------------------------
    //>Degradation
    //--------------------------------------------------------
    // Cette fonction ajoute un flou créé par une psf uniforme 
    // (fonction porte) de taille sizexsize puis ajoute sur
    // cette image rendue floue, un bruit Gaussien de variance
    // variance_noise
    //
    // Entrée : mat_img :: image non dégradée
    // Sortie : mat     :: image dégradée
    //--------------------------------------------------------
    degradation(mat_img,mat,length,width,psf,size,variance_noise);



    //============
    //WIENER
    //============
    // .....
  

    //---------------------------------------------
    // SAUVEGARDE et VISU
    // -------------------
    // Le resultat de la restoration > mat_rest
    // L'image dégradée              > mat
    // L'image non dégradée          > mat_img
    //----------------------------------------------
    SaveImagePgm(NAME_IMG_OUT1,mat_rest,length,width);
    SaveImagePgm(NAME_IMG_OUT2,mat,length,width);
  
    strcpy(BufSystVisuImg,NAME_VISUALISER);
    strcat(BufSystVisuImg,NAME_IMG_OUT2);
    strcat(BufSystVisuImg,".pgm&");
    printf("\n > %s",BufSystVisuImg);
    system(BufSystVisuImg);
    strcpy(BufSystVisuImg,NAME_VISUALISER);
    strcat(BufSystVisuImg,NAME_IMG_IN);
    strcat(BufSystVisuImg,".pgm&");
    printf("\n > %s",BufSystVisuImg);
    system(BufSystVisuImg);
    strcpy(BufSystVisuImg,NAME_VISUALISER);
    strcat(BufSystVisuImg,NAME_IMG_OUT1);
    strcat(BufSystVisuImg,".pgm&");
    printf("\n > %s",BufSystVisuImg);
    system(BufSystVisuImg);

    //retour sans probleme 
    printf("\n C'est fini ... \n\n");
    return 0; 	 
}


