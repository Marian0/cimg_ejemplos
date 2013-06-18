#include "myCImg.h"

CImg<double> MedialAxis( CImg<double> img ) {
	bool hayCambios=true;
	CImg<double> copia = img;
	while( hayCambios ) {
		hayCambios = false;
		cimg_forXY(img,i,j) {
			if(img(i,j) == 0) {
				double max = 0;
				for(int dx=-1; dx<=1; dx++) {
					for(int dy=-1; dy<=1; dy++) {
						if( abs(dx)+abs(dy)>1 ) continue;
						if(img(i+dx,j+dy) > max)
							max = img(i+dx,j+dy);
					}
				}
				if( max != 0 ) {
					hayCambios = true;
					copia(i,j) = max+1;
				}
			}
		}
		img = copia;
	}
	return img;
}

/*
Entra una imagen RGB
La recorre para encontrar los muntos NSEO que sea menor que el limite 
	(Detecta que es fondo (negro) y que es imagen)
Recorta la imagen en esas posiciones
*/
void EnfocarImagen( CImg<double> &img, double lim ) {
	int xmin=img.width(), xmax=0, ymin=img.height(), ymax=0;
	for( int j=0; j<img.height(); j++ ) { 
		for( int i=0; i<img.width(); i++ ) {
			if( img(i,j,0,0)+img(i,j,0,1)+img(i,j,0,2) < lim*3 ) {
				if( xmin > i ) xmin=i;
				if( xmax < i ) xmax=i;
				if( ymin > j ) ymin=j;
				if( ymax < j ) ymax=j;
			}
		}
	}
	img.crop( xmin,ymin,xmax,ymax );
}

/*
Grafica una imagen fila y la devuelve
*/
CImg<double> Graficar( CImg<double> &img, int h ) {
	static const byte tc[] = { 255 };
	CImg<double> grafico( img.width(), h );
	grafico.fill(0);
	grafico.draw_graph( img, tc );
	return grafico;
}

CImg<double> PerfilY( CImg<double>& img ) {
	CImg<double> perfil( img.height(), 1 );
	perfil.fill(0);
	cimg_forXY(img, i, j) {
		perfil(j) += img(i,j);
	}
	return perfil;
}

CImg<double> PerfilX( CImg<double>& img ) {
	CImg<double> perfil = CImg<double> ( img.width(), 1 );
	perfil.fill(0);
	cimg_forXY(img, i, j) {
		perfil(i) += img(i,j);
	}
	return perfil;
}

Pixel Minimo( CImg<double>& img ) {
	int xmin=0, ymin=0;
	double minimo = img(0,0);
	cimg_forXY(img, i, j) {
		if( img(i,j) < minimo ) {
			minimo = img(i,j);
			xmin = i;
			ymin = j;
		}
	}
	return Pixel(xmin, ymin, &img);
}

Pixel Maximo( CImg<double>& img ) {
	int xmax=0, ymax=0;
	double maximo = img(0,0);
	cimg_forXY(img, i, j) {
		if( img(i,j) > maximo ) {
			maximo = img(i,j);
			xmax = i;
			ymax = j;
		}
	}
	return Pixel(xmax, ymax, &img);
}

/*
Dada img, separa en img1 img2 img3 por referencia correspondiendo a cada canal
*/
void SepararCanales( CImg<double>& img, CImg<double>& img1, CImg<double>& img2, CImg<double>& img3 ) {
	img1 = CImg<double>( img.width(), img.height() );
	img2 = CImg<double>( img.width(), img.height() );
	img3 = CImg<double>( img.width(), img.height() );
	cimg_forXY(img, i, j) {
		img1(i,j) = img(i,j,0,0);
		img2(i,j) = img(i,j,0,1);
		img3(i,j) = img(i,j,0,2);
	}
}


std::string IntToStr(int value) {
	std::string s;
	std::stringstream out;

	out<<value;
	return out.str();
}

/*
Para umbralizar
*/

CImg<double> UmbralizarMayorGris(CImg<double> img, double umbral) {
	cimg_forXY(img, x, y) {
		if (img(x,y) < umbral)
			img(x,y) = 0;
	}
	return img;
}
CImg<double> UmbralizarMenorGris(CImg<double> img, double umbral) {
	cimg_forXY(img, x, y) {
		if (img(x,y) > umbral)
			img(x,y) = 0;
	}
}


/*
Dada una máscara, la resizea al tamaño que se necesita rellenando con 0 el fondo
*/
CImg<bool> AgrandarMascara(CImg<bool> & mascara, unsigned int W, unsigned int H, bool fondo) {
	assert(mascara.width() < W && mascara.height() < H);
	CImg<bool> retval(W,H,1,1,fondo);

	cimg_forXY(mascara, x, y) {
		retval(x,y) = mascara(x,y);
	}

	return retval;
}
// Negacion de una mascara
CImg<bool> NegarMascara(CImg<bool> mascara ) {

	cimg_forXY(mascara, x, y) {
		mascara(x,y) = !mascara(x,y);
	}	
	return mascara;
}



/*
Aplica el operador deseado por ejemplo OperadorSobel
*/
CImg<double> AplicarOperador(CImg<double> & img, CImgList<double> &operador) {
	CImgList<double> resultados;
    
    CImg<double> retval(img.width(), img.height(), 1, 1, 0);
    unsigned int cantidad = operador.size();
    for (unsigned int i = 0; i < cantidad; i++) {
        CImg<double> tempy = img.get_convolve(operador[i]);
        retval += tempy;
    }
     
    return retval;
}

/* Agarra una mascara y la transforma a double para poder usar algunas funciones*/
CImg<double> BoolADouble(CImg<bool> &img) {
	CImg<double> retval(img.width(), img.height(), 1, 1, 0.0);
	cimg_forXY(img, x, y) {
		if (img(x,y))
			retval(x,y) = 1.0;
	}
	return retval;
}


template <typename T>
std::string to_string(T const& value) {
    stringstream sstr;
    sstr << value;
    return sstr.str();
}