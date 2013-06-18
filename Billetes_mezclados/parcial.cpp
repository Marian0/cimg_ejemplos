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
const int nCasos = 1;
const int nSemillas = 100;
const double Rmin = 36;
const double Rmax = 38;

const double Gmin = 52;
const double Gmax = 54;

const double Bmin = 25;
const double Bmax = 27;

const int pixeles_fondo = 828421;
const int pixeles_billete = 160000;
const int nLineas = 1;


const double tamanioMoneda[] = { 34.2, 37.8, 43.0, 39.6, 44.1, 47.7, 45.9, 50.2 };
const char* nombreMoneda[] = { "1 Centavo", "2 Centavos", "5 Centavos", "10 Centavos", "20 Centavos", "50 Centavos", "1 Euro", "2 Euros"};

struct Zona {
	int id_zona;
	unsigned int cantidad_pixeles;
	Pixel centro;
	Pixel norte;
	Pixel sur;
	Pixel este;
	Pixel oeste;
};



int rombos_billete[] = { -1, 100, 50, 20, 10, 5, 2 };

int main (int argc, char *argv[]) {

	//Configuración por parámetros variables
 	const double _umbral = cimg_option("-u", 0.1f, "Umbral");





 	const double _umbral2 = cimg_option("-t", 0.6f, "Umbral");

 	const int _margen = cimg_option("-m", 60, "Margen");
 	const double _blur = cimg_option("-b", 2.0, "Blur");
	
	for (int i = 1; i <= nCasos; i++) {
		//Lectura del Archivo
		std::string archivo = "images/" + IntToStr(i) + ".jpg";
		CImg<double> imagen(archivo.c_str());

		//Sus medidas
		unsigned int H = imagen.height();
		unsigned int W = imagen.width();
		
		//Transformo a escala de gris
		CImg<double> gris = (imagen.get_channel(0) + imagen.get_channel(1) + imagen.get_channel(2)).normalize(0.0, 1.0);
		
		//Tiro semillas por la imagen hasta caer en el fondo (COLOR FIJO)
		unsigned int isemillas, seed_x, seed_y;
		bool es_fondo = false;
		for (isemillas = 1; i <= nSemillas; i++) {
			seed_x = rand()%W;
			seed_y = rand()%H;

			//Compruebo si caemos en el fondo
			if ( imagen(seed_x,seed_y,0,0) > Rmin && imagen(seed_x,seed_y,0,0) < Rmax &&
				  imagen(seed_x,seed_y,0,1) > Gmin && imagen(seed_x,seed_y,0,1) < Gmax &&
				  imagen(seed_x,seed_y,0,2) > Bmin && imagen(seed_x,seed_y,0,2) < Bmax
			   ) {
			   	imagen.draw_circle(seed_x,seed_y,5, Rojo);
			    es_fondo = true;
				break;
			}

		}
		//Me aseguro de caer en el fondo
		if (es_fondo) {
			std::cout<<"* Se cae en el fondo tras arrojar "<<isemillas<<" semillas.\n";
		} else {
			std::cout<<"* No se pudo encotrar el fondo, incremente el numero de semillas.\n";
			return 0;
		}


		//Segmento el fondo y los billetes
		CImg<bool> fondo = SegmentarColorRegion(gris, _umbral, seed_x, seed_y);
		CImg<bool> billetes = NegarMascara(fondo);

		//Aplico Etiquetado de regiones para saber y diferenciar cada uno de los billetes
		CImg<int> etiquetado =  label_cc(billetes);

		etiquetado.display("etiqu", false);
		std::map<int, CImg<bool> > mapa_mascaras;
		std::map<int, unsigned int > mapa_pixeles;

		cimg_forXY(etiquetado, x, y) {
			if (mapa_mascaras.find( etiquetado(x,y)) == mapa_mascaras.end() ) {
				CImg<bool> tempy(W,H,1,1,false);
				tempy(x,y) = true;
				mapa_mascaras[etiquetado(x,y)] = tempy;
				mapa_pixeles[etiquetado(x,y)] = 0;
			} else {
				mapa_mascaras[etiquetado(x,y)](x,y) = true;
				mapa_pixeles[etiquetado(x,y)]++;
			}
			
		}

		std::map<int, CImg<bool> >::iterator it = mapa_mascaras.begin();
		std::map<int, unsigned int >::iterator itp = mapa_pixeles.begin();

		unsigned int dinero = 0;

		while(it != mapa_mascaras.end()) {
			// if (itp->second > pixeles_fondo || itp->second < pixeles_billete) {
				//borramos basura (fondo y cosas menores)
				// mapa_pixeles.erase(it->first);
				// mapa_mascaras.erase(it->first);
				// std::cout<<"Se borro por ser basura (fondo o cosas chicas) \n";

			// } else {	
				std::string titulo = "Zona " + to_string(it->first);
				// it->second.display(titulo.c_str(), false );
			// }
				it++; 
				itp++;
				std::cout<<"Cantidad PIXELES "<<itp->second<<"\n";
				if (itp->second > 170000 || itp->second < 120000)
					continue;
				// it->second.display();

				Pixel norte = GetNorte(it->second);
				Pixel sur = GetSur(it->second);
				Pixel este = GetEste(it->second);
				Pixel oeste = GetOeste(it->second);

				CImg<double> billete_recortado = imagen.get_crop(este.x, norte.y, oeste.x, sur.y);
				CImg<double> mascara_recortada = it->second.get_crop(este.x, norte.y, oeste.x, sur.y);
				

				// (billete_recortado, mascara_recortada).display();
				// // billete_recortado.display();

				// // (billete_recortado, mascara_recortada).display();

				// //Paso a escala de gris
				CImg<double> billete_gris = ( billete_recortado.get_channel(0) + billete_recortado.get_channel(1) + billete_recortado.get_channel(2) ).normalize(0.0, 1.0);

				CImg<double> temporal = billete_recortado.get_mul(mascara_recortada);

				// (billete_recortado, mascara_recortada, temporal).display();

				Pixel inicio = Maximo(temporal);


				// //Crecimiento de regiones para segmentar el billete
				CImg<bool> crecimiento = SegmentarColorRegion(billete_gris, _umbral2, inicio.x, inicio.y );


				// //Filtro para elimiar cierto ruido
				crecimiento.blur_median(10);

				// //Escalo para recortar en base a un factor
				CImg<bool> crecimiento_resize = crecimiento.get_resize(billete_gris.width() * 0.98, billete_gris.height() * 0.98);

				// //Copio la mascara en una más grande para luego operar
				CImg<bool> bordes_billete = AgrandarMascara(crecimiento_resize, crecimiento.width(), crecimiento.height(), false );

				// //Opero centrado
				bordes_billete.shift( (crecimiento.width() - crecimiento_resize.width())/2 , (crecimiento.height() - crecimiento_resize.height())/2);

				// //Resto la grande a la chica y me quedo con pseudobordes
				bordes_billete = crecimiento - bordes_billete;

				// (billete_gris, crecimiento, bordes_billete).display();
				// //Transofrmo en hough
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
				
				int cx = billete_recortado.width()/2;
				int cy = billete_recortado.height()/2;

				//FIX PARA EL CASO DE 10 PESOS (NO FUNCIONA)
				// if (billete_recortado.width() < billete_recortado.height()) {
				// 	CImg<double> temporal2(billete_recortado.width(), billete_recortado.width(), billete_recortado.depth(), billete_recortado.spectrum());
				// 	cimg_forXY(temporal2, x, y) {
				// 		cimg_forC(temporal2,c) {
				// 			temporal2(x,y,0,c) = 0.0;
				// 		}
				// 	}
					
				// 	cimg_forXY(billete_recortado, x, y) {
				// 		cimg_forC(billete_recortado,c) {
				// 			temporal2(x,y,0,c) = billete_recortado(x,y,0,c);
				// 		}
						
				// 	}
				// 	(temporal2, billete_recortado).display();
				// }

				CImg<double> final = billete_recortado.get_rotate(angulo, cx, cy, 1, 0);
				// final.display();

				//Tambien rotamos los bordes billete para calcular los limites del billete
				CImg<bool> bordes_derecho = bordes_billete;


				//Esto es un fix........ 
				for (int i = 0; i < bordes_derecho.width(); i++) {
					bordes_derecho(i,0) = false;
					bordes_derecho(i,bordes_derecho.height()-1) = false;

				}
				for (int i = 0; i < bordes_derecho.height(); i++) {
					bordes_derecho(0,i) = false;
					bordes_derecho(bordes_derecho.width()-1, i) = false;

				}

				bordes_derecho.rotate(angulo, cx, cy, 1, 0);
				norte = GetNorte(bordes_derecho);
				sur = GetSur(bordes_derecho);
				este = GetEste(bordes_derecho);
				oeste = GetOeste(bordes_derecho);

				CImg<double> recortada_final = final.get_crop(este.x, norte.y, oeste.x, sur.y);
				//Dibujo un circulo en cada punto cardinal
				final.draw_circle(norte.x, norte.y, 5, Rojo);
				final.draw_circle(sur.x, sur.y, 5, Verde);
				final.draw_circle(este.x, este.y, 5, Amarillo);
				final.draw_circle(oeste.x, oeste.y, 5, Azul);



				(billete_recortado, mascara_recortada, final, recortada_final ).display("Debug 1", false);


				//----------------------------------------

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

				std::map<int, unsigned int> mapa_rombos;
				cimg_forXY(cc_regiones, x, y) {
				if (mapa_rombos.find( cc_regiones(x,y) ) == mapa_rombos.end() ) {
				mapa_rombos[cc_regiones(x,y)] = 0;
				} else {
				mapa_rombos[cc_regiones(x,y)]++;
				}
				}

				 int rombos = mapa_rombos.size()-1;

				std::cout<<"Cantidad de Rombos "<<mapa_rombos.size()-1<<"\n";
				(mitad_derecha, mitad_izquierda, recortada_final, rombos_color, rombos_mask ).display("Debug", false);

				if (rombos == 1) {
					dinero += 100;
				} else if(rombos == 2) {
					dinero += 50;
				} else if(rombos == 6) {
					dinero += 2;
				} else if(rombos == 5) {
					dinero += 5;
				} else if(rombos == 4) {
					dinero += 10;
				} else if(rombos == 3) {
					dinero += 20;
				} else {
					std::cout<<" *** BILLETE NO RECONOCIDO \n";
				}


		}
		std::cout<<"Cantidad de dinero: "<<dinero<<"\n";
		// (imagen, gris, fondo, billetes, etiquetado).display("Ejercicio", false);
		std::cout<<"** Termino de procesar "<<archivo.c_str()<<"\n";

	}//** fin for i
	
	return 0;
 }//** fin main

//Para compilar:
//	rm parcial.cpp.bin | g++ -o parcial.cpp.bin parcial.cpp -O0 -lm -lpthread -lfftw3 -lX11 -g && ./parcial.cpp.bin
