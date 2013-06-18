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

int main (int argc, char *argv[]) {

	//Configuración por parámetros variables
 	const char* _input = cimg_option("-i", "../Piletas/images/google2.png", "Fichero de entrada");

 	CImg<double> imagen(_input);

 	CImg<double> imagenHSI = imagen.get_RGBtoHSI();

 	LoopRecortarAnalizarCanales(imagenHSI, true);
	
	return 0;
 }//** fin main

//Para compilar:
//	rm analisisHSI.cpp.bin | g++ -o analisisHSI.cpp.bin analisisHSI.cpp -O0 -lm -lpthread -lfftw3 -lX11 -g && ./analisisHSI.cpp.bin
