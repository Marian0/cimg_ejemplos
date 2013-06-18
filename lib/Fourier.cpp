#include "myCImg.h"

void Cartesianas2Polares(CImgList<double>& imagen) {
	double ang, mag;
	cimg_forXY( imagen[0], x, y) {
		ang = arg(complex<double>(imagen[0](x,y), imagen[1](x,y)));
		mag = sqrt(pow(imagen[0](x,y),2)+pow(imagen[1](x,y),2));
		imagen[0](x,y)=mag;
		imagen[1](x,y)=ang;
	}
}

void Polares2Cartesianas(CImgList<double>& imagen) {
	double rea, ima;
	cimg_forXY( imagen[0], x, y) {
			rea = imagen[0](x,y) * cos( imagen[1](x,y) );
			ima = imagen[0](x,y) * sin( imagen[1](x,y) );
			imagen[0](x,y)=rea;
			imagen[1](x,y)=ima;
	}
}

CImg<double> BonitaTF(CImgList<double> fft) {
	Cartesianas2Polares( fft );
	CImg<double> mag = fft[0];
	mag.shift(mag.width()/2, mag.height()/2, 0, 0, 2);
	mag += numeric_limits<double>::epsilon();
	mag.log();
	mag.normalize(0, 255);
	return mag;
}

void VerTF(const CImg<double> &img, const char *title) {
	CImgList<double> fft = img.get_FFT();
	BonitaTF( fft ).display(title, false);
}
