#include "myCImg.h"

CImg<bool> MaximosLocales( CImg<double>& img, double limite ) {
	CImg<bool> maximo(img.width(), img.height(),1,1,true);
	cimg_forXY(img,i,j) {
		for(int dx=-1; dx<=1 && maximo(i,j); dx++) {
			for(int dy=-1; dy<=1 && maximo(i,j); dy++) {
				if(dx==0 && dy==0) continue;
				if( i+dx < 0 || i+dx >= img.width() || j+dy < 0 || j+dy >= img.height() ) continue;
				maximo(i,j) = maximo(i,j) && ( img(i,j) >= img(i+dx,j+dy) ) && img(i,j) > limite; 
			}
		}
	}
	return maximo;
}


CImg<bool> Inundar( CImg<double> img, double limite ) {
	double acumulador = 0;
	img.normalize(0.0, 1.0);
	limite *= ((double)img.width()*(double)img.height()) - img.sum();
	CImg<bool> mask(img.width(), img.height(), 1, 1, false );
	
	set<Pixel> cola;
	cola.insert( Minimo( img ) );
	
	while( acumulador < limite ) {
		//agrego el pixel de menor valor (el primero de la cola)
		Pixel p = (*cola.begin()); cola.erase( cola.begin() );
		acumulador += 1.0 - p.value();
		mask(p.x,p.y) = true;
		//Agrego los vecinos a la cola
		Pixel pix = p;
		pix.x++;
		if( pix.AdentroImagen() && !mask(pix.x, pix.y) ) cola.insert( pix );
		pix.x -= 2;
		if( pix.AdentroImagen() && !mask(pix.x, pix.y) ) cola.insert( pix );
		pix.x++; pix.y++;
		if( pix.AdentroImagen() && !mask(pix.x, pix.y) ) cola.insert( pix );
		pix.y -= 2;
		if( pix.AdentroImagen() && !mask(pix.x, pix.y) ) cola.insert( pix );
	}
	return mask;
}

CImg<bool> InundarInverso( CImg<double> img, double limite) {
	return Inundar( img.max() - img, limite);
}


// Va preguntando si cada valor es menor que el limite, si es menor lo agrega a la mascara 
// y agrega a procesar su vecindadrio
// Si no se le pasa una semilla, siembra en el minimo de la funcion.
// No corre en diagonal solo en +

CImg<bool> Crecimiento( CImg<double> img, double limite, int seed_x , int seed_y ) {
	img.normalize(0.0, 1.0);
	CImg<bool> mask(img.width(), img.height(), 1, 1, false );
	CImg<bool> procesado(img.width(), img.height(), 1, 1, false );
	deque<Pixel> cola;

	Pixel inicio;
	//No se envía ninguna semilla
	if (seed_y == -1 || seed_x == -1) {
		inicio = Minimo(img);
	} else {
		inicio.x = seed_x;
		inicio.y = seed_y;
		inicio.img = &img;
	}
	cola.push_back(inicio);

	procesado(cola.front().x, cola.front().y) = true;
	
	while( !cola.empty() ) {
		Pixel p = cola.front(); cola.pop_front();
		if( p.value() < limite ) {
			mask(p.x,p.y) = true;
			//agrego a los vecinos para procesarlos
			Pixel pix = p;
			pix.x++;
			if( pix.AdentroImagen() && !procesado(pix.x, pix.y) ) {
				cola.push_back( pix );
				procesado(pix.x, pix.y) = true;
			}
			pix.x -= 2;
			if( pix.AdentroImagen() && !procesado(pix.x, pix.y) ) {
				cola.push_back( pix );
				procesado(pix.x, pix.y) = true;
			}
			pix.x++; pix.y++;
			if( pix.AdentroImagen() && !procesado(pix.x, pix.y) ) {
				cola.push_back( pix );
				procesado(pix.x, pix.y) = true;
			}
			pix.y -= 2;
			if( pix.AdentroImagen() && !procesado(pix.x, pix.y) ) {
				cola.push_back( pix );
				procesado(pix.x, pix.y) = true;
			}
		}
	}
	return mask;
}

CImg<bool> CrecimientoInverso( CImg<double> img, double limite, int seed_x , int seed_y  ) {
	return Crecimiento( img.max() - img, limite);
}


//No esta bien probada ni terminada
// jajajaja!!!!! mentira, tiene la posta!
CImg<bool> SegmentarColorRegion( CImg<double> img, double limite, int seed_x , int seed_y ) {
	img.normalize(0.0, 1.0);
	CImg<bool> mask(img.width(), img.height(), 1, 1, false );
	CImg<bool> procesado(img.width(), img.height(), 1, 1, false );
	deque<Pixel> cola;

	Pixel inicio;
	inicio.x = seed_x;
	inicio.y = seed_y;
	inicio.img = &img;

	cola.push_back(inicio);

	procesado(cola.front().x, cola.front().y) = true;
	
	while( !cola.empty() ) {
		Pixel p = cola.front(); cola.pop_front();
		if( fabs( p.value()-inicio.value() ) < limite ) {
			mask(p.x,p.y) = true;
			//agrego a los vecinos para procesarlos
			Pixel pix = p;
			pix.x++;
			if( pix.AdentroImagen() && !procesado(pix.x, pix.y) ) {
				cola.push_back( pix );
				procesado(pix.x, pix.y) = true;
			}
			pix.x -= 2;
			if( pix.AdentroImagen() && !procesado(pix.x, pix.y) ) {
				cola.push_back( pix );
				procesado(pix.x, pix.y) = true;
			}
			pix.x++; pix.y++;
			if( pix.AdentroImagen() && !procesado(pix.x, pix.y) ) {
				cola.push_back( pix );
				procesado(pix.x, pix.y) = true;
			}
			pix.y -= 2;
			if( pix.AdentroImagen() && !procesado(pix.x, pix.y) ) {
				cola.push_back( pix );
				procesado(pix.x, pix.y) = true;
			}
		}
	}
	return mask;
}