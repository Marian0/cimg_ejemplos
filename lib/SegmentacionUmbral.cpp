#include "myCImg.h"


CImg<bool> SegmentacionColor(const CImg<double> &img, double RoH, double GoS, double BoI, double tol) {
	CImg<bool> res(img.width(), img.height(), 1, 1, false);
	tol *= tol;
	cimg_forXY(img, x, y) {
		res(x, y) = ((pow(img(x, y, 0, 1) - RoH, 2) + pow(img(x, y, 0, 1) - GoS, 2) + pow(img(x, y, 0, 2) - BoI, 2)) < tol); 
	}
	return res;
}
