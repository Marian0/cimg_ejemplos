//Includes genéricos
#include <iostream>
#include <string>
#include <CImg.h>


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
const int nLineas = 40;
const int nCasos = 5;


int main (int argc, char *argv[]) {

	//Configuración por parámetros variables
 	const double _umbral = cimg_option("-u", 0.95f, "Umbral");
 	const int _margen = cimg_option("-m", 60, "Margen");
 	const double _blur = cimg_option("-b", 2.0, "Blur");
	
	for (int i = 1; i < nCasos; i++) {
		std::string archivo = IntToStr(i) + ".jpg";
		CImg<double> imagen(archivo.c_str());
		unsigned int H = imagen.height();
		unsigned int W = imagen.width();

		//Paso a escala de gris
		CImg<double> gris = ( imagen.get_channel(0) + imagen.get_channel(1) + imagen.get_channel(2) ).normalize(0.0, 1.0);

		//Crecimiento de regiones para segmentar el billete
		CImg<bool> crecimiento = Crecimiento(gris, _umbral);

		//Filtro para elimiar cierto ruido
		crecimiento.blur_median(10);

		//Escalo para recortar en base a un factor
		CImg<bool> crecimiento_resize = crecimiento.get_resize(W * 0.95, H * 0.95);

		//Copio la mascara en una más grande para luego operar
		CImg<bool> bordes_billete = AgrandarMascara(crecimiento_resize, crecimiento.width(), crecimiento.height(), false );

		//Opero centrado
		bordes_billete.shift( (crecimiento.width() - crecimiento_resize.width())/2 , (crecimiento.height() - crecimiento_resize.height())/2);

		//Resto la grande a la chica y me quedo con pseudobordes
		bordes_billete = crecimiento - bordes_billete;

		//Transofrmo en hough
		CImg<double> hough = TransformadaHough(bordes_billete);

		// Para calcular el angulo de giro de la imagen tomo los "nLineas" maximos de la imagen y los promedio 
		double angulo = 0;
		for(int i=0; i<nLineas; i++) {
			Pixel p = Maximo( hough );
			angulo += p.x;
			hough( p.x, p.y ) = 0;
		}
		angulo /= (double)nLineas;
		angulo -= (double)(hough.width()/2);
		angulo *= 90.0 / (double)(hough.width()/2);
		cout<<"Angulo de giro: "<<angulo<<endl;
		
		int cx = imagen.width()/2;
		int cy = imagen.height()/2;
		CImg<double> final = imagen.get_rotate(angulo, cx, cy, 1, 1, 2);

		//Tambien rotamos los bordes billete para calcular los limites del billete
		CImg<bool> bordes_derecho = bordes_billete;


		//Esto es un fix........ (No funciona porque las imagenes son truchas)
		for (int i = 0; i < bordes_derecho.width(); i++) {
			bordes_derecho(i,0) = false;
			bordes_derecho(i,bordes_derecho.height()-1) = false;

		}
		for (int i = 0; i < bordes_derecho.height(); i++) {
			bordes_derecho(0,i) = false;
			bordes_derecho(bordes_derecho.width()-1, i) = false;

		}

		bordes_derecho.rotate(angulo, cx, cy, 1, 1, 0);
		Pixel norte = GetNorte(bordes_derecho);
		Pixel sur = GetSur(bordes_derecho);
		Pixel este = GetEste(bordes_derecho);
		Pixel oeste = GetOeste(bordes_derecho);

		CImg<double> recortada_final = final.get_crop(este.x, norte.y, oeste.x, sur.y);
		//Dibujo un circulo en cada punto cardinal
		final.draw_circle(norte.x, norte.y, 5, Rojo);
		final.draw_circle(sur.x, sur.y, 5, Verde);
		final.draw_circle(este.x, este.y, 5, Amarillo);
		final.draw_circle(oeste.x, oeste.y, 5, Azul);
		

		(imagen, bordes_billete, bordes_derecho, final, recortada_final ).display();
		std::cout<<"** Termino de procesar "<<archivo.c_str()<<"\n";


	}//** fin for i
	



	return 0;
 }//** fin main

//Para compilar:
//	rm main.cpp.bin | g++ -o main.cpp.bin main.cpp -O0 -lm -lpthread -lfftw3 -lX11 -g && ./main.cpp.bin
