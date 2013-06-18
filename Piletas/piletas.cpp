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

#include <map>

//Namespaces
using namespace std;
using namespace cimg_library;

//Definiciones globlales estáticas
const int nCasos = 8;
const int nTiposMonedas = 8;
const double tamanioMoneda[] = { 34.2, 37.8, 43.0, 39.6, 44.1, 47.7, 45.9, 50.2 };
const char* nombreMoneda[] = { "1 Centavo", "2 Centavos", "5 Centavos", "10 Centavos", "20 Centavos", "50 Centavos", "1 Euro", "2 Euros"};


int minR = 70;
int maxR = 124;
int minG = 89;
int maxG = 173;
int minB = 108;
int maxB = 206;

int minH = 177;
int maxH = 215;
double minS = 0.17;
double maxS = 0.45;


int main (int argc, char *argv[]) {

	//Configuración por parámetros variables
 	const double _umbral = cimg_option("-u", 0.5f, "Umbral");
 	const int _margen = cimg_option("-m", 60, "Margen");
 	const double _blur = cimg_option("-b", 2.0, "Blur");
	
	for (int i = 1; i < nCasos; i++) {
		std::string archivo = "images/google" + IntToStr(i) + ".png";
		CImg<double> imagen(archivo.c_str());
		unsigned int H = imagen.height();
		unsigned int W = imagen.width();
		
		CImg<double> imagenHSI = imagen.get_RGBtoHSI();
		LoopRecortarAnalizarCanales(imagenHSI, true);
		LoopRecortarAnalizarCanales(imagen, false);

		// imagenHSI.blur(_umbral);

		CImg<bool> segmentada(imagen.width(), imagen.height(), 1, 1, false);

		cimg_forXY(imagen, x, y) {
			//COMPROBSAMOS HSI
			if (
				imagenHSI(x,y,0,0) > minH && imagenHSI(x,y,0,0) < maxH &&
				imagenHSI(x,y,0,1) > minS && imagenHSI(x,y,0,1) < maxS &&
				imagen(x,y,0,2) > 75
				)
				segmentada(x,y) = true;
		}

		segmentada.blur_median(10);

		//Vamos a mejorar la definición de las regiones



		CImg<int> etiquetado = label_cc(segmentada);

		std::map<int, unsigned int> mapa;

		cimg_forXY(etiquetado, x, y) {
			if ( mapa.find(etiquetado(x,y)) == mapa.end() ) {
				//No está
				mapa[etiquetado(x,y)] = 0;
			} else {
				mapa[etiquetado(x,y)]++;
			}
		}

		std::map<int,unsigned int>::iterator it = mapa.begin();
		std::map<int, bool> zonas_a_borrar;
		while( it != mapa.end() )  {
			//Recordar que la posición 0 es el fondo
			cout<<"Pixeles zona "<<it->first<<": "<<it->second<<"\n";
			it++;

			if (it->second < 100) {
				zonas_a_borrar[it->first] = true;
			}
		}
		std::map<int,bool>::iterator it2 = zonas_a_borrar.begin();
		while( it2 != zonas_a_borrar.end() )  {
			cimg_forXY(etiquetado, x, y) {
				if (etiquetado(x,y) == it2->first && it2->second)
					segmentada(x,y) = false;
			}
			
			it2++;
		}


		std::cout<<"** Se encontraron "<<mapa.size() - 1 - zonas_a_borrar.size()<<" piletas \n";
		std::cout<<"** Termino de procesar "<<archivo.c_str()<<"\n";

		(imagen, segmentada, imagen.get_mul(segmentada) ).display();
		// loopRecortarAnalizarCanales(imagen, false);


	}//** fin for i
	
	return 0;
 }//** fin main

//Para compilar:
//	rm piletas.cpp.bin | g++ -o piletas.cpp.bin piletas.cpp -O0 -lm -lpthread -lfftw3 -lX11 -g && ./piletas.cpp.bin
