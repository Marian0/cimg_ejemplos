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

//Definiciones globales estáticas
const int nCasos = 9;
const int nTiposMonedas = 8;
const double tamanioMoneda[] = { 34.2, 37.8, 43.0, 39.6, 44.1, 47.7, 45.9, 50.2 };
const char* nombreMoneda[] = { "1 Centavo", "2 Centavos", "5 Centavos", "10 Centavos", "20 Centavos", "50 Centavos", "1 Euro", "2 Euros"};



int main (int argc, char *argv[]) {

	//Configuración por parámetros variables
 	const double _umbral = cimg_option("-u", 0.959f, "Umbral");
 	const double _umbral2 = cimg_option("-s", 0.4f, "Umbral");
 	const int _margen = cimg_option("-m", 60, "Margen");
 	const double _blur = cimg_option("-b", 2.0, "Blur");
	
	for (int i = 1; i < nCasos; i++) {
		std::string archivo =  IntToStr(i) + ".jpg";
		CImg<double> imagen(archivo.c_str());
		unsigned int H = imagen.height();
		unsigned int W = imagen.width();
		

		CImg<double> gris = (imagen.get_channel(0) + imagen.get_channel(1) + imagen.get_channel(2)).normalize(0.0,1.0);
		Pixel minimo = Minimo(gris);

		imagen.draw_circle(minimo.x, minimo.y, 5, Rojo);

		CImg<bool> mascara =  Crecimiento(gris, _umbral);
		mascara.blur_median(10);

		CImg<bool> mascara_chica = mascara.get_resize(mascara.width() * 0.98, mascara.height() * 0.98);
		CImg<bool> recortada = AgrandarMascara(mascara_chica, W, H, false);
		recortada.shift(  (W- mascara_chica.width())/2 , (H - mascara_chica.height()) / 2  );
		recortada = mascara - recortada;

		CImg<double> hough = TransformadaHough(recortada);

		Pixel maximo = Maximo(hough);
		double angulo = maximo.x - (double)(hough.width()/2);
		angulo *= 90.0 / (double)(hough.width()/2);

		cout<<"Angulo de giro: "<<angulo<<endl;
		
		int cx = imagen.width()/2;
		int cy = imagen.height()/2;
		CImg<double> final = imagen.get_rotate(angulo, cx, cy, 1, 1, 2);

	//Esto es un fix........ (No funciona porque las imagenes son truchas)
		for (int i = 0; i < recortada.width(); i++) {
			recortada(i,0) = false;
			recortada(i,recortada.height()-1) = false;

		}
		for (int i = 0; i < recortada.height(); i++) {
			recortada(0,i) = false;
			recortada(recortada.width()-1, i) = false;

		}

		recortada.rotate(angulo, cx, cy, 1, 1, 0);
		Pixel norte = GetNorte(recortada);
		Pixel sur = GetSur(recortada);
		Pixel este = GetEste(recortada);
		Pixel oeste = GetOeste(recortada);

		CImg<double> recortada_final = final.get_crop(oeste.x + 5, norte.y + 5 , este.x - 5 , sur.y - 5);
		//Dibujo un circulo en cada punto cardinal
		final.draw_circle(norte.x, norte.y, 5, Rojo);
		final.draw_circle(sur.x, sur.y, 5, Verde);
		final.draw_circle(este.x, este.y, 5, Amarillo);
		final.draw_circle(oeste.x, oeste.y, 5, Azul);

		(imagen, mascara, recortada, final, recortada_final ).display();

		//Voy a ver si está al derecho o al revez
		CImg<double> mitad_derecha = recortada_final.get_crop(0,0, recortada_final.width() * 0.375f, recortada_final.height() );
		CImg<double> mitad_izquierda = recortada_final.get_crop(recortada_final.width() * 0.625f,0, recortada_final.width(), recortada_final.height() );
		if (mitad_derecha.mean() < mitad_izquierda.mean()) {
			recortada_final.rotate(180, recortada_final.width()/2, recortada_final.height()/2, 1, 0);
		}

		CImg<double> rombos_color = recortada_final.get_crop(recortada_final.width() * 0.191f, recortada_final.height() * 0.04f, recortada_final.width()*0.282f, recortada_final.height() * 0.55f );
		

		CImg<double> rombos_gris = (rombos_color.get_channel(0)+rombos_color.get_channel(1)+rombos_color.get_channel(2)).normalize(0.0,1.0);

		CImg<double> rombos_mask = SegmentarColorRegion(rombos_gris.get_blur(_blur), _umbral2, 0, 0);

		rombos_mask = NegarMascara(rombos_mask);


		CImg<int> cc_regiones = label_cc(rombos_mask);

		std::map<int, unsigned int> mapa;
		cimg_forXY(cc_regiones, x, y) {
			if (mapa.find( cc_regiones(x,y) ) == mapa.end() ) {
				mapa[cc_regiones(x,y)] = 0;
			} else {
				mapa[cc_regiones(x,y)]++;
			}
		}

		std::cout<<"Cantidad de Rombos "<<mapa.size()-1<<"\n";


		(mitad_derecha, mitad_izquierda, recortada_final, rombos_color, rombos_mask ).display();
		std::cout<<"** Termino de procesar "<<archivo.c_str()<<"\n";

	}//** fin for i
	
	return 0;
 }//** fin main

//Para compilar:
//	rm billetes_genericos.cpp.bin | g++ -o billetes_genericos.cpp.bin billetes_genericos.cpp -O0 -lm -lpthread -lfftw3 -lX11 -g && ./billetes_genericos.cpp.bin
