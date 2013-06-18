#include "myCImg.h"

CImg<double> LaplacianoGausiano(const CImg<double> &img, double sigma) {
	int size = sigma*8, ini = -(size-1)/2, fin = (size-1)/2;
	size -= size%2;
	double r2;
	CImg<double> mask(size, size, 1, 1, 0.0);
	for (int x=ini; x<fin+1; ++x){
		for (int y=ini; y<fin+1; ++y){
			r2 = x*x + y*y;
			mask(x-ini, y-ini) = -((r2 - sigma)/(sigma*sigma)) * exp( -r2 / (2*sigma) );
		}
	}
	mask -= (double(mask.sum())/double(size*size));
	return img.get_convolve(mask);
}

CImg<bool> CrucesCero(const CImg<double> &img, double lim, int size) {
	size /= 2;
	bool positivo, negativo;
	CImg<bool> res( img.width(), img.height() );
	cimg_forXY(img, x, y) {
		positivo = false;
		negativo = false;
		for(int dx=-size; dx<=size; dx++) {
			for(int dy=-size; dy<=size; dy++) {
				if( x+dx < 0 || x+dx >= img.width() || y+dy < 0 || y+dy >= img.height() ) 
					continue;
				positivo = positivo || (img(x+dx, y+dy) > lim);
				negativo = negativo || (img(x+dx, y+dy) < -lim);
			}
		}
		res(x,y) = positivo && negativo;
	}
	return res;
}

CImg<double> MagnitudGradiente(CImg<double> &img) {
	CImgList<double> g = img.get_gradient();
	g[0].pow(2); 
	g[1].pow(2);
	return (g[0]+g[1]).sqrt();
}

CImg<double> FaseGradiente(CImg<double> &img) {
	CImgList<double> g = img.get_gradient();
	return g[1].atan2(g[0]);
}


CImg<double> LineaHorizontal(CImg<double> &img) {
	static double data[] = {-1, -1, -1, 2, 2, 2, -1, -1, -1};
	static CImg<double> m(data, 3, 3, 1, 1, true);
	return img.get_convolve(m);
}

CImg<double> LineaVertical(CImg<double> &img) {
	static double data[] = {-1, 2, -1, -1, 2, -1, -1, 2, -1};
	static CImg<double> m(data, 3, 3, 1, 1, true);
	return img.get_convolve(m);
}


CImg<double> LineaDiagonal1(CImg<double> &img) {
	static double data[] = {-1, -1, 2, -1, 2, -1, 2, -1, -1};
	static CImg<double> m(data, 3, 3, 1, 1, true);
	return img.get_convolve(m);
}


CImg<double> LineaDiagonal2(CImg<double> &img) {
	static double data[] = {2, -1, -1, -1, 2, -1, -1, -1, 2};
	static CImg<double> m(data, 3, 3, 1, 1, true);
	return img.get_convolve(m);
}






//@ Devuelve el Operador de Roberts para el gradiente en X [0] e Y [1]
CImgList<double> OperadorRoberts(){
    CImgList<double> ret_val(2,2,2,1,1,0);
    //Gradiente en X
    ret_val[0](0,0) = -1;
    ret_val[0](1,1) = 1;

    //Gradiente en Y
    ret_val[1](0,1) = 1;
    ret_val[1](1,0) = -1;
    return ret_val;
}

//@ Devuelve el Operador de Prewitt para el gradiente en las 4 direcciones
CImgList<double> OperadorPrewitt(){
    CImgList<double> ret_val(4,3,3,1,1,0);
    //Gradiente en X
    ret_val[0](0,0) = -1;
    ret_val[0](1,0) = -1;
    ret_val[0](2,0) = -1;
    ret_val[0](0,2) = 1;
    ret_val[0](1,2) = 1;
    ret_val[0](2,2) = 1;

    ret_val[1](0,0) = -1;
    ret_val[1](0,1) = -1;
    ret_val[1](0,2) = -1;
    ret_val[1](2,0) = 1;
    ret_val[1](2,1) = 1;
    ret_val[1](2,2) = 1;

    ret_val[2](0,1) = -1;
    ret_val[2](0,2) = -1;
    ret_val[2](1,2) = -1;
    ret_val[2](1,0) = 1;
    ret_val[2](2,0) = 1;
    ret_val[2](2,1) = 1;

    ret_val[3](0,0) = -1;
    ret_val[3](0,1) = -1;
    ret_val[3](1,0) = -1;
    ret_val[3](2,2) = 1;
    ret_val[3](1,2) = 1;
    ret_val[3](2,1) = 1;

    return ret_val;
}

//@ Devuelve el Operador de Sobel para el gradiente en las 4 direcciones
CImgList<double> OperadorSobel(){
    CImgList<double> ret_val(4,3,3,1,1,0);
    //Gradiente en X
    ret_val[0](0,0) = -1;
    ret_val[0](1,0) = -2;
    ret_val[0](2,0) = -1;
    ret_val[0](0,2) = 1;
    ret_val[0](1,2) = 2;
    ret_val[0](2,2) = 1;

    ret_val[1](0,0) = -1;
    ret_val[1](0,1) = -2;
    ret_val[1](0,2) = -1;
    ret_val[1](2,0) = 1;
    ret_val[1](2,1) = 2;
    ret_val[1](2,2) = 1;

    ret_val[2](0,1) = -1;
    ret_val[2](0,2) = -2;
    ret_val[2](1,2) = -1;
    ret_val[2](1,0) = 1;
    ret_val[2](2,0) = 2;
    ret_val[2](2,1) = 1;

    ret_val[3](0,1) = -1;
    ret_val[3](0,0) = -2;
    ret_val[3](1,0) = -1;
    ret_val[3](1,2) = 1;
    ret_val[3](2,2) = 2;
    ret_val[3](2,1) = 1;

    return ret_val;
}


//@ Devuelve una mascara llena de -1 excepto en el centro donde tiene un 8
CImgList<double> OperadorLaplaciano8() {
    CImgList<double> ret_val(1,3,3,1,1,-1);
    ret_val[0](1,1) = 8; 
    return ret_val; 
}

//@ Devuelve una mascara de 3x3 con un 4 en el medio y cuatro -1 en los extremos
CImgList<double> OperadorLaplaciano4() {
    CImgList<double> ret_val(1,3,3,1,1,0);
    ret_val[0](1,1) = 4; 

    ret_val[0](0,1) = -1; 
    ret_val[0](1,0) = -1; 
    ret_val[0](1,2) = -1; 
    ret_val[0](2,1) = -1; 
    
    return ret_val; 
}

//@ Devuelve la mascara de 5x5 para el LoG
CImgList<double> OperadorLoG() {
    CImgList<double> ret_val(1,5,5,1,1,0);
    ret_val[0](2,0) = -1; 
    ret_val[0](1,1) = -1; 
    ret_val[0](3,1) = -1; 
    ret_val[0](4,2) = -1; 
    ret_val[0](3,3) = -1; 
    ret_val[0](2,4) = -1; 
    ret_val[0](1,3) = -1; 
    ret_val[0](0,2) = -1; 
    
    ret_val[0](1,2) = -2; 
    ret_val[0](2,1) = -2; 
    ret_val[0](3,2) = -2; 
    ret_val[0](2,3) = -2;
    
    ret_val[0](2,2) = 16;

    return ret_val;
}