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
#include <vector>
#include <algorithm>

//Namespaces
using namespace std;
using namespace cimg_library;

//Definiciones globales estáticas
const int nCasos = 2;
const int nTiposMonedas = 8;
const double tamanioMoneda[] = { 34.2, 37.8, 43.0, 39.6, 44.1, 47.7, 45.9, 50.2 };
const char* nombreMoneda[] = { "1 Centavo", "2 Centavos", "5 Centavos", "10 Centavos", "20 Centavos", "50 Centavos", "1 Euro", "2 Euros"};



int main (int argc, char *argv[]) {

	//Configuración por parámetros variables
 	const double _umbral = cimg_option("-u", 0.5f, "Umbral");
 	const int _margen = cimg_option("-m", 60, "Margen");
 	const double _blur = cimg_option("-b", 2.0, "Blur");
	
	for (int i = 1; i <= nCasos; i++) {
		std::string archivo = "images/" + IntToStr(i) + ".jpg";
		CImg<double> imagen(archivo.c_str());
		unsigned int H = imagen.height();
		unsigned int W = imagen.width();
		// LoopRecortarAnalizarCanales(imagen);

		CImg<double> gris = (imagen.get_channel(0) + imagen.get_channel(1) + imagen.get_channel(2)).normalize(0.0, 1.0);

		CImg<bool> mascara(imagen.width(), imagen.height(), 1, 1, true);

		cimg_forXY(gris, x, y) {
			if (gris(x,y) > 0.99f)
				mascara(x,y) = false;
		}
		mascara.blur_median(8);

		LoopCrecimiento(mascara, _umbral);

		CImg<int> etiquetado = label_cc(mascara);


		std::map<int, unsigned int> mapa_pixeles;
		std::map<int, Pixel > mapa_centros_promedio;


		cimg_forXY(etiquetado,x,y) {
			if (mapa_pixeles.find(etiquetado(x,y)) == mapa_pixeles.end() ) {
				mapa_pixeles[etiquetado(x,y)] = 0;
				//Acumulo para luego calcular centro promedio
				mapa_centros_promedio[etiquetado(x,y)].x = x;
				mapa_centros_promedio[etiquetado(x,y)].y = y;
			} else {
				mapa_pixeles[etiquetado(x,y)]++;
				//Acumulo para luego calcular centro promedio
				mapa_centros_promedio[etiquetado(x,y)].x += x;
				mapa_centros_promedio[etiquetado(x,y)].y += y;
			}
		}
		std::map<int, unsigned int>::iterator it = mapa_pixeles.begin();
		std::map<int, Pixel>::iterator it2 = mapa_centros_promedio.begin();
		// std::vector<unsigned int> v;
		while (it != mapa_pixeles.end()) {

			//Condiciones para que sea considerada una region valida, sino la borramos
			//Regiones expureas <--------> fondo
			if (it->second > 3000 && it->second < 20000) {
				//Terminamos de calcular el centro promedio y lo dibujamos
				mapa_centros_promedio[it->first].x /= it->second;
				mapa_centros_promedio[it->first].y /= it->second;
				imagen.draw_circle(mapa_centros_promedio[it->first].x, mapa_centros_promedio[it->first].y, 5, Verde);
				std::cout<<"Region "<<it->first<<" Cantidad de pixeles "<<it->second<<"\n";
				
			} else {
				//Borramos
				mapa_pixeles.erase(it->first);
				mapa_centros_promedio.erase(it->first);
				std::cout<<"Se elimina region "<<it->first<<" por tener "<<it->second<<"\n";
			}
			// v.push_back(it->second);
			it++; it2++;
		}



	 	//std::vector<unsigned int>::iterator pminimo = min_element(v.begin(), v.end());
	 	//std::vector<unsigned int>::iterator pmaximo = max_element(v.begin(), v.end());
		// std::cout<<"Minima cantidad de pixeles de una region "<<*pminimo<<" Maximo "<<*pmaximo<<"\n";
		std::cout<<"Se han detectado "<<mapa_pixeles.size() - 1<<" monedas\n";

		(imagen, gris, mascara).display();
		std::cout<<"** Termino de procesar "<<archivo.c_str()<<"\n";

	}//** fin for i
	
	return 0;
 }//** fin main

//Para compilar:
//	rm monedas.cpp.bin | g++ -o monedas.cpp.bin monedas.cpp -O0 -lm -lpthread -lfftw3 -lX11 -g && ./monedas.cpp.bin
