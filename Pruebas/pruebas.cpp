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
const int nCasos = 4;
const int nTiposMonedas = 8;



int main (int argc, char *argv[]) {

	//Configuración por parámetros variables
 	const double _umbral = cimg_option("-u", 0.5f, "Umbral");
 	const int _opcion = cimg_option("-o", 1, "Margen");
 	const double _blur = cimg_option("-b", 2.0, "Blur");
	
	for (int i = 1; i < nCasos; i++) {
		std::string archivo =  IntToStr(i) + ".jpg";
		CImg<double> imagen(archivo.c_str());
		unsigned int H = imagen.height();
		unsigned int W = imagen.width();
		
		CImg<double> gris = (imagen.get_channel(0) + imagen.get_channel(1) + imagen.get_channel(2)).normalize(0.0, 1.0);
		
		CImg<bool> retval(imagen.width(), imagen.height(), 1, 1, false);
		CImg<bool> retval2(imagen.width(), imagen.height(), 1, 1, false);
		CImg<bool> retval3(imagen.width(), imagen.height(), 1, 1, false);
		CImg<bool> retval4(imagen.width(), imagen.height(), 1, 1, false);
		CImg<bool> retval5(imagen.width(), imagen.height(), 1, 1, false);
		CImg<bool> retval6(imagen.width(), imagen.height(), 1, 1, false);

		retval = MaximosLocales(gris,_umbral);
		retval.display("Maximos locales");


		retval2 = Inundar(gris,_umbral);
		retval2.display("Inundar");

		retval3 = InundarInverso(gris,_umbral);
		retval3.display("Inundar Inverso");

		retval4 = Crecimiento(gris, _umbral);
		retval4.display("Crecimiento");

		retval5 = CrecimientoInverso(gris, _umbral);
		retval5.display("Inverso");

		retval6 = SegmentarColorRegion(gris, _umbral, 10, 10);
		retval6.display("Segmentacion Color");

		Pixel maximo = Maximo(gris);
		Pixel minimo = Minimo(gris);

		imagen.draw_circle(minimo.x, minimo.y, 5, Verde);
		imagen.draw_circle(maximo.x, maximo.y, 5, Rojo);


		(imagen, retval, retval2, retval3, retval4, retval5, retval6).display();
		std::cout<<"** Termino de procesar "<<archivo.c_str()<<"\n";		

	}//** fin for i
	



	return 0;
 }//** fin main

//Para compilar:
//	rm pruebas.cpp.bin | g++ -o pruebas.cpp.bin pruebas.cpp -O0 -lm -lpthread -lfftw3 -lX11 -g && ./pruebas.cpp.bin
