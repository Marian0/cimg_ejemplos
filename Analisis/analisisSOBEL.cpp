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
	const char* _input = cimg_option("-i", "1.png", "Entrada");
 	const double _blur = cimg_option("-b", 0.0, "Blur");

 	const double _umbral = cimg_option("-u", 0.5f, "Umbral");
 	const int _margen = cimg_option("-m", 60, "Margen");
	
 	CImg<double> imagen(_input);
	unsigned int H = imagen.height();
	unsigned int W = imagen.width();
 	std::cout<<"Leyendo "<<_input<<" Medidas "<<W<<"x"<<H<<"\n";
 	
 	CImg<double> gris(W,H,1,1,0.0);
 	
 	if (imagen.spectrum() == 3) {
 		std::cout<<"Pasando a grises \n";
 		gris = (imagen.get_channel(0) + imagen.get_channel(1) + imagen.get_channel(2)).normalize(0.0, 1.0);
 	} else {
 		gris = imagen;
 	}

	std::cout<<"Normalizando 0.0 a 1.0 \n";
 	gris.normalize(0.0,1.0);

	std::cout<<"Aplicando un blur de "<<_blur<<" \n";
	CImg<double> blur = gris.get_blur(_blur);

	CImgList<double> operador_sobel = OperadorSobel();
	std::cout<<"Aplicando SOBEL \n";
	CImg<double> sobel_gris = AplicarOperador(gris, operador_sobel);
	CImg<double> sobel_blur = AplicarOperador(blur, operador_sobel);

	std::cout<<"Umbralizacion \n";
	CImg<bool> umbral_blur = sobel_blur.get_threshold(_umbral);
	CImg<bool> umbral_gris = sobel_gris.get_threshold(_umbral);

	(gris, sobel_gris, umbral_gris).display("Grises, Sobel de grises, Umbralizado", false);
	(gris, sobel_blur, umbral_blur).display("Grises, Sobel de grises, Umbralizado", false);
	
	return 0;
 }//** fin main

//Para compilar:
//	rm analisisSOBEL.cpp.bin | g++ -o analisisSOBEL.cpp.bin analisisSOBEL.cpp -O0 -lm -lpthread -lfftw3 -lX11 -g && ./analisisSOBEL.cpp.bin
