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

double Gris1[] = { 50 , 50, 50};
double Gris2[] = { 150 , 150, 150};


//Rango donde es posible encontrar rombos
unsigned int minx = 120;
unsigned int miny = 10;
unsigned int maxx = 185;
unsigned int maxy = 90;

//Definición de los billetes
int rombos_billete[] = { -1, 100, 50, 20, 10, 5, 2 };
// rombos_billete[0] = -1; //Billete no reconocido
// rombos_billete[1] = 100; //Billete de 100
// rombos_billete[2] = 50; //Billete de 50
// rombos_billete[3] = 20; //Billete de 20
// rombos_billete[4] = 10; //Billete de 10
// rombos_billete[5] = -1; //Billete no reconocido
// rombos_billete[6] = 2; //Billete de 2



int main (int argc, char *argv[]) {
 	const double _umbral = cimg_option("-u", 0.5f, "Umbral");
 	const int _margen = cimg_option("-m", 60, "Umbral");
 	const double _blur = cimg_option("-b", 2.0, "Umbral");
	std::vector<int> tipos_billetes;

	tipos_billetes.push_back(2);
	tipos_billetes.push_back(5);
	tipos_billetes.push_back(10);
	tipos_billetes.push_back(20);
	tipos_billetes.push_back(50);
	tipos_billetes.push_back(100);

	int _cantidad = tipos_billetes.size();
	
	for (int j=0; j < _cantidad; j++) {
		for( int i=1; i< 5; i++ ) {
			std::string archivo = "images/" + IntToStr(tipos_billetes[j]) + "_" + IntToStr(i) + ".jpg";

			CImg<double> imagen( archivo.c_str() );
			CImg<double> original = imagen;
			imagen.normalize(0.0, 1.0);


			//Paso a gris
			CImg<double> gris = (imagen.get_channel(0) + imagen.get_channel(1) + imagen.get_channel(2)).normalize(0.0,1.0);


			// CImg<unsigned char> uchar( archivo.c_str() );
			// draw_3D_image(uchar);

			//Testeo para ver perfil
			// CImg<double> perfil = PerfilX(gris).normalize(0.0,1.0), visor = gris;
			// visor.fill(0);
			// (visor.draw_graph(perfil, Gris2, 1, 1, 0.0 , 1.0 , 0) ).display();



			// 1) Detectamos si está rotado promediando margenes izquierdo (mas claro) y derecho
			CImg<double> izquierda = gris.get_crop(0,0,_margen, gris.height() );
			CImg<double> derecha = gris.get_crop( gris.width() - _margen , 0 , gris.width() , gris.height() );

			double izq_mean = izquierda.mean();
			double der_mean = derecha.mean();

			if ( izq_mean > der_mean ) {
				std::cout<<"* Billete bien orientado "<<"\n";
			} else {
				std::cout<<"* Rotando Billete "<<"\n";
				imagen.rotate(180);
				gris.rotate(180);
			}

			// 2) Segmentamos los rombos, en base a una posición promedio de todos los billetes
				//Se hará con un blur + un umbral definido a prueba/error
			CImg<double> rombos_gris = gris;
			rombos_gris.crop( minx , miny, maxx , maxy );
			rombos_gris.blur(_blur);

			cimg_forXY(rombos_gris, x, y) {
				if (rombos_gris(x,y) > _umbral)
					rombos_gris(x,y) = 0; 
				else
					rombos_gris(x,y) = 1;
			}


			// 3) Conteo de Regiones
			CImg<int> regiones = label_cc(rombos_gris, 1);

			std::set<int> unicos;
			cimg_forXY(regiones, x, y) {
				unicos.insert(regiones(x,y));
			}
			int n_rombos = unicos.size() - 1;

			std::cout<<"Cantidad de Rombos reconocidos: "<<n_rombos<<"\n";

			if (n_rombos < 0 || n_rombos > 6 || rombos_billete[n_rombos] == -1) {
				std::cout<<"*** Error: El billete no pudo ser reconocido \n";
			} else {
				std::cout<<"*** Exito: El billete es de "<<rombos_billete[n_rombos]<<" Pesos.\n";
			}

			(original, imagen, rombos_gris, gris.get_crop(minx , miny, maxx , maxy) ).display();






		}
	}
	return 0;
}

