#include "myCImg.h"

CImg<double> AlfaRecortado(CImg<double>& img, int mx, int my, int d) {
	int dx = (mx-1)/2;
	int dy = (my-1)/2;
	CImg<double> resultado( img.width(), img.height(), 1, 1, 0.0 );
	int upperLimit, lowerLimit, leftLimit, rightLimit;
	vector<double> vecino(mx*my);
	cimg_forXY( img, i, j ) {
		leftLimit  = max( 0, i - dx );
		rightLimit = min( img.width()-1, i + dx );
		upperLimit = max( 0, j - dy );
		lowerLimit = min( img.height()-1, j + dy );
		vecino.clear();
		for(int x=leftLimit; x<=rightLimit; x++) {
			for(int y=upperLimit; y<=lowerLimit; y++) {
				vecino.push_back( img(x,y) );
			}
		}
		sort( vecino.begin(), vecino.end() );
		int vSize = vecino.size();
		if( vSize > 2*d ) {
			for( int k=d; k<vSize-d; k++ )
				resultado(i,j) += vecino[k];
			resultado(i,j) /= double(vSize)-double(2*d);
		}
		else
			resultado(i,j) = img(i,j);
	}
	return resultado;
}

CImg<double> MediaGeometrica(const CImg<double> &img, int ancho_filtro, int alto_filtro) {
	CImg<double> res(img);
	double aux, d = 1.0/(ancho_filtro * alto_filtro);
	int h = alto_filtro/2, w = ancho_filtro/2;
	cimg_forXY(img, x, y){
		CImg<double> vecinos = img.get_crop(x-w, y-h, x+w, y+h, true);
		aux = 1.0;
		cimg_forXY(vecinos, i, j){
			aux *= vecinos(i, j);
		}
		res(x, y) = pow(aux, d);
	}
	return res;
}

CImg<double> ContraArmonica(const CImg<double> &img, int Q, int ancho_filtro, int alto_filtro) {
	CImg<double> res(img);
	double aux1, aux2;
	int h = alto_filtro/2, w = ancho_filtro/2;
	cimg_forXY(img, x, y){
		CImg<double> vecinos = img.get_crop(x-w, y-h, x+w, y+h, true);
		aux1 = aux2 = 0.0;
		cimg_forXY(vecinos, i, j){
			aux1 += pow(vecinos(i, j), Q+1.0);
			aux2 += pow(vecinos(i, j), Q);
		}
		res(x, y) = aux1/aux2;
	}
	return res;
}


/**
* \param img Imagen a procesar.
* \param MG Media global de la imagen.
* \param DG Desvío estándar global de la imagen.
* \param k0 Constante positiva menor que 1.
* \param k1 Constante, debe cumplir k1 < k2.
* \param k2 Constante, mayor que 1 si se desea realzar áreas claras, menor que 1 si se desea realzar áreas oscuras.
* \param E Constante multiplicativa.
* \param size Tamaño de la vecindad (size x size). Defecto 3.
* \return La imagen procesada.
*/
CImg<double> RealceLocal(CImg<double> &img, double MG, double DG, double k0, double k1, double k2, double E, int size) {
	CImg<double> res(img), medias(img), stds(img);
	int h = size/2, w = size/2;
	double m, d;
	cimg_forXY(img, x, y){
		CImg<double> vecinos = img.get_crop(x-w, y-h, x+w, y+h, true);
		d = sqrt(vecinos.variance_mean(0, m));
		if (m <= k0*MG && k1*DG <= d && d <= k2*DG)
			res(x, y) = E * img(x, y);
	}
	return res;
}

