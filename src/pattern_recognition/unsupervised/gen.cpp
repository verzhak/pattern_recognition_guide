
#include "all.hpp"
#include "unsupervised.hpp"

const unsigned lwh = 40, mult_lwh = 200, height = 30, width = 20, ab_num[7] = {31, 25, 32, 28, 36, 24, 31};
const string ab_name[7] = {"futurama", "hylain", "quenya", "sarati", "sindarin", "uruk", "cirth"};

int gen(unsigned ab)
{
	int ret = 0;

	try
	{
		char fname[200];
		unsigned v, u, y, x, ty, tx, from_x;
		Mat rotate_scale, dst(height * mult_lwh, width * mult_lwh, CV_8UC1);

		rotate_scale = getRotationMatrix2D(Point2f(0, 0), 0, 5);

		srand48(time(NULL));

		for(v = 0; v < height; v++)
			for(u = 0; u < width; u++)
			{
				Mat src, rotate, inv_src(lwh, lwh, CV_8UC1), big_src(mult_lwh, mult_lwh, CV_8UC1), warp_src(mult_lwh, mult_lwh, CV_8UC1);

				sprintf(fname, "%s/alphabet/%s/%ld.png", DATA_DIR, ab_name[ab].c_str(), lrand48() % ab_num[ab] + 1);
				src = imread(fname, 0);
				throw_null(src.data);

				// ############################################################################ 
				// Инверсия цветов

				threshold(src, inv_src, 150, 255, THRESH_BINARY_INV);

				// ############################################################################ 
				// Увеличить в mult раз

				warpAffine(inv_src, big_src, rotate_scale, big_src.size(), INTER_LINEAR, BORDER_CONSTANT, 0);

				// ############################################################################ 
				// Случайный поворот

				rotate = getRotationMatrix2D(Point2f(mult_lwh / 2, mult_lwh / 2), -20.0 + drand48() * 40.0, 1);
				warpAffine(big_src, warp_src, rotate, warp_src.size(), INTER_LINEAR, BORDER_CONSTANT, 0);

				// ############################################################################ 
				// Копирование в результирующее изображение

				for(y = v * mult_lwh, from_x = u * mult_lwh, ty = 0; ty < mult_lwh; y++, ty++)
					for(x = from_x, tx = 0; tx < mult_lwh; x++, tx++)
						dst.at<unsigned char>(y, x) = warp_src.at<unsigned char>(ty, tx);

				// ############################################################################ 
			}

		sprintf(fname, "%s/text/%u.png", DATA_DIR, ab + 1);
		imwrite(fname, dst);
	}
	catch(...)
	{
		ret = -1;
	}

	return ret;
}

