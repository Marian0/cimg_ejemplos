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
 	const char* _input = cimg_option("-i", "1.png", "Fichero de entrada");
 	const double _umbral = cimg_option("-u", 0.5f, "Umbral");

 	CImg<double> imagen(_input);

 	if (imagen.spectrum() == 3) {
 		CImg<double> gris = (imagen.get_channel(0) + imagen.get_channel(1) + imagen.get_channel(2)).normalize(0.0, 1.0);
 		imagen = gris;
 	}

	LoopCrecimiento(imagen, _umbral);
	
	return 0;
 }//** fin main

//Para compilar:
//	rm analisisCrecimiento.cpp.bin | g++ -o analisisCrecimiento.cpp.bin analisisCrecimiento.cpp -O0 -lm -lpthread -lfftw3 -lX11 -g && ./analisisCrecimiento.cpp.bin
