#include "myCImg.h"

CImg<double> TransformadaHough( CImg<bool>& img ) {
	const int M = img.width(),N = img.height();
	CImg<double> iHough(M, N, 1, 1, 0.0);
	double maxRho = sqrt(float(pow(N-1,2)+pow(M-1,2))), stepRho = 2.*maxRho/(N-1), stepTheta = M_PI/(M-1), rho, theta;
	int r;
	cimg_forXY(img,y,x) {
		if ( img(y,x) ) {
			for (int t=0; t<M; t++) {
				theta = t*stepTheta - M_PI/2;
				rho = x*cos(theta) + y*sin(theta);
				r = floor((rho+maxRho)/stepRho+.5);
				iHough(t,r)+= 1;
			}
		}
	} 
	return iHough; 
}
/*

	Entra la transformada de hough y grafica nLineas correspondientes a los nMaximos
*/
CImg<double> InversaHough(CImg<double> img, int nLineas) {
	const unsigned M = img.width(),N = img.height();
	CImg<double> iHough(M, N, 1, 1, 0.0);
	double maxRho = sqrt(float(pow(N-1,2)+pow(M-1,2))), stepRho = 2.*maxRho/(N-1), stepTheta = M_PI/(M-1), rho, theta;
	const double blanco[1] = {255.f};
	float x0, x1, y0, y1;
	for( int nl=0; nl < nLineas; nl++ ) {
		Pixel p = Maximo( img );
		int t = p.x, r = p.y;
		theta=t*stepTheta-M_PI/2;
		rho=r*stepRho-maxRho;
		if (theta>-M_PI/2 && theta<M_PI/2){
			y0=0; y1=M-1;
			x0=rho/cos(theta);
			x1=rho/cos(theta)-(M-1)*tan(theta);
		}
		else {
			x0=0; x1=N-1;
			y0=rho/sin(theta);
			y1=rho/sin(theta)-(N-1)/tan(theta);
		}
		iHough.draw_line(y0,x0,y1,x1,blanco);
		img(t,r) = 0;
	}
	return iHough; 
}

CImg<double> TransformadaHoughCirculos(CImg<bool> &img, int r) {
	int M = img.width(), N = img.height();
	CImg<double> iHough(M, N, 1, 1, 0.0);
	cimg_forXY(img, i, j){
		if (img(i, j)) {
			for (int a=0; a<M; ++a){
				double det = r*r-(i-a)*(i-a);
				if (det>0){
					double b=round(j-sqrt(det));
					if (b>0 && b<N)
						iHough(a,b)++;
				}
			}
		}
	}
	return iHough;
}
