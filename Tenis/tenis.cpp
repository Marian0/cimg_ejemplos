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
const int nCasos = 3;
const double tamanioMoneda[] = { 34.2, 37.8, 43.0, 39.6, 44.1, 47.7, 45.9, 50.2 };
const char* nombreMoneda[] = { "1 Centavo", "2 Centavos", "5 Centavos", "10 Centavos", "20 Centavos", "50 Centavos", "1 Euro", "2 Euros"};



int main (int argc, char *argv[]) {

	//Configuración por parámetros variables
 	const double _umbral = cimg_option("-u", 0.75f, "Umbral");
 	const double _margen = cimg_option("-m", 0.8f , "Margen Superior");
 	const double _blur = cimg_option("-b", 2.0, "Blur");
 	const int nTiposMonedas = cimg_option("-m", 10, "Blur");
	
	for (int i = 1; i <= nCasos; i++) {
		std::string archivo = "images/" + IntToStr(i) + ".jpg";
		CImg<double> imagen(archivo.c_str());
		unsigned int H = imagen.height();
		unsigned int W = imagen.width();
		// LoopRecortarAnalizarCanales(imagen);

		CImg<double> gris = (imagen.get_channel(0) + imagen.get_channel(1) + imagen.get_channel(2)).normalize(0.0, 1.0);

		CImg<double> cropeada = gris.get_crop(0, H * _margen, W, H);
		CImg<double> cropeada_color = imagen.get_crop(0, H * _margen, W, H);
		CImg<bool> lineas = cropeada.get_threshold(_umbral);

		CImg<double> hough = TransformadaHough(lineas);

		CImg<double> tempy = hough;

		double angulo = 0;
		for (int i = 0; i < nTiposMonedas; ++i)
		{
			Pixel maximo = Maximo(tempy);
			tempy(maximo.x,maximo.y) = 0;
			angulo += maximo.x;
			std::cout<<maximo.y<<"\n";
		}
		angulo /= nTiposMonedas;
		angulo -= (double)(hough.width()/2);
		angulo *= 90.0 / (double)(hough.width()/2);

		cout<<"Angulo de giro: "<<angulo<<endl;




		CImg<double> ihough = InversaHough(hough, nTiposMonedas);

		cimg_forXY(ihough, x, y) {
			if (ihough(x,y) > 100) {
				cropeada_color(x,y,0,0) = 255.f;				
				cropeada_color(x,y,0,1) = 0.0;				
				cropeada_color(x,y,0,2) = 0.0;				
			}
		}


		CImg<double> rotada = cropeada.get_rotate(angulo);		

		(cropeada_color, lineas, rotada).display();
		std::cout<<"** Termino de procesar "<<archivo.c_str()<<"\n";

	}//** fin for i
	
	return 0;
 }//** fin main

//Para compilar:
//	rm tenis.cpp.bin | g++ -o tenis.cpp.bin tenis.cpp -O0 -lm -lpthread -lfftw3 -lX11 -g && ./tenis.cpp.bin
