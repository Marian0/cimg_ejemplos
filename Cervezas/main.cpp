//Includes genéricos
#include <iostream>
#include <string>

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
const int nCasos = 11;
const int nTiposMonedas = 8;
const double tamanioMoneda[] = { 34.2, 37.8, 43.0, 39.6, 44.1, 47.7, 45.9, 50.2 };
const char* nombreMoneda[] = { "1 Centavo", "2 Centavos", "5 Centavos", "10 Centavos", "20 Centavos", "50 Centavos", "1 Euro", "2 Euros"};



int main (int argc, char *argv[]) {

	//Configuración por parámetros variables
 	const double _umbral = cimg_option("-u", 0.5f, "Umbral");
 	const int _margen = cimg_option("-m", 60, "Margen");
 	const double _blur = cimg_option("-b", 2.0, "Blur");
	
	for (int i = 1; i < nCasos; i++) {
		std::string archivo = "images/" + IntToStr(i) + ".jpg";
		CImg<double> imagen(archivo.c_str());
		unsigned int H = imagen.height();
		unsigned int W = imagen.width();
		
		


		CImg<double> resultado = imagen.get_blur(_blur);
		(imagen, resultado).display();
		std::cout<<"** Termino de procesar "<<archivo.c_str()<<"\n";

	}//** fin for i
	



	return 0;
 }//** fin main

//Para compilar:
//	rm main.cpp.bin | g++ -o main.cpp.bin main.cpp -O0 -lm -lpthread -lfftw3 -lX11 -g && ./main.cpp.bin
