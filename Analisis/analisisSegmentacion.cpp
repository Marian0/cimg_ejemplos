//Mi librería
#include "../lib/myCImg.h"
#include "../lib/Varios.cpp"
#include "../lib/Ruido.cpp"
#include "../lib/SegmentacionLineas.cpp"
#include "../lib/SegmentacionRegiones.cpp"
#include "../lib/SegmentacionUmbral.cpp"
#include "../lib/Hough.cpp"
#include "../lib/Fourier.cpp"
#include "../lib/Analisis.cpp"

//Librería de cátedra
#include "../lib/PDI_functions.h"

#include <string>

//Namespaces
using namespace std;
using namespace cimg_library;

//Definiciones globales estáticas
const int nCasos = 10;
const int nTiposMonedas = 8;
const double tamanioMoneda[] = { 34.2, 37.8, 43.0, 39.6, 44.1, 47.7, 45.9, 50.2 };
const char* nombreMoneda[] = { "1 Centavo", "2 Centavos", "5 Centavos", "10 Centavos", "20 Centavos", "50 Centavos", "1 Euro", "2 Euros"};



int main (int argc, char *argv[]) {

	//Configuración por parámetros variables
 	const double _umbral = cimg_option("-u", 0.5f, "Umbral");
 	const int _x_semilla = cimg_option("-x", 10, "Margen");
 	const int _y_semilla = cimg_option("-y", 10, "Margen");
 	const double _blur = cimg_option("-b", 2.0, "Blur");
 	const char* _input = cimg_option("-i", "../Billetes_Girados/2.jpg", "Entrada");
	
	CImg<double> imagen(_input);
		unsigned int H = imagen.height();
		unsigned int W = imagen.width();
		
	 	CImg<double> gris(W,H,1,1,0.0);
 	
		if (imagen.spectrum() == 3) {
			std::cout<<"Pasando a grises \n";
			gris = (imagen.get_channel(0) + imagen.get_channel(1) + imagen.get_channel(2)).normalize(0.0, 1.0);
		} else {
			gris = imagen;
		}

		
		CImg<bool> retval(imagen.width(), imagen.height(), 1, 1, false);
		CImg<bool> retval2(imagen.width(), imagen.height(), 1, 1, false);
		CImg<bool> retval3(imagen.width(), imagen.height(), 1, 1, false);
		CImg<bool> retval4(imagen.width(), imagen.height(), 1, 1, false);
		CImg<bool> retval5(imagen.width(), imagen.height(), 1, 1, false);
		CImg<bool> retval6(imagen.width(), imagen.height(), 1, 1, false);

		retval = MaximosLocales(gris,_umbral);
		// retval.display("Maximos locales");


		retval2 = Inundar(gris,_umbral);
		// retval2.display("Inundar");

		retval3 = InundarInverso(gris,_umbral);
		// retval3.display("Inundar Inverso");

		retval4 = Crecimiento(gris, _umbral);
		// retval4.display("Crecimiento");

		retval5 = CrecimientoInverso(gris, _umbral);
		// retval5.display("Crecimiento Inverso");

		retval6 = SegmentarColorRegion(gris, _umbral, _x_semilla, _y_semilla);
		// retval6.display("Segmentacion Color");

		Pixel maximo = Maximo(gris);
		Pixel minimo = Minimo(gris);

		imagen.draw_circle(minimo.x, minimo.y, 5, Verde);
		imagen.draw_circle(maximo.x, maximo.y, 5, Rojo);

		std::string titulo = "Max locales, Inundar, Inv Inundar, Crec, Inv Crec, Seg Color (UMBRAL " + to_string( _umbral ) + ")"; 


		(imagen, retval, retval2, retval3, retval4, retval5, retval6).display(titulo.c_str(), false);
	
	return 0;
 }//** fin main

//Para compilar:
//	rm analisisSegmentacion.cpp.bin | g++ -o analisisSegmentacion.cpp.bin analisisSegmentacion.cpp -O0 -lm -lpthread -lfftw3 -lX11 -g && ./analisisSegmentacion.cpp.bin
