
#include "all.hpp"
#include "captcha.hpp"

const unsigned lwh = 120;
const unsigned nic = 5;
const unsigned alphabet_size = 10;

CCaptcha::CCaptcha() : img_captcha(lwh, lwh * nic, CV_8UC3), img_num(lwh, lwh, CV_8UC1)
{
	alp = NULL;
	captcha = NULL;

	srand(time(NULL));
	throw_null((alp = new char[alphabet_size]));
	throw_null((captcha = new char[nic]));
}

CCaptcha::~CCaptcha()
{
	if(alp != NULL)
		delete alp;

	if(captcha != NULL)
		delete captcha;
}

Mat CCaptcha::operator()()
{
	char str[2] = {'\0', '\0'};
	unsigned u, tu, v, t, n, num;
	double bg_color[3];
	Vec3b color;

	for(t = 0; t < 3; t++)
		bg_color[t] = 255 / ((double) lwh * nic);

	for(u = 0; u < lwh * nic; u++)
	{
		for(t = 0; t < 3; t++)
			color[t] = bg_color[t] * u;

		for(v = 0; v < lwh; v++)
			img_captcha.at<Vec3b>(v, u) = color;
	}

	for(n = 0; n < nic; n++)
	{
		num = rand() % alphabet_size;
		captcha[n] = alp[num];

		this->operator()(num);

		for(t = 0; t < 3; t++)
			color[t] = rand() % 256;

		for(v = 0; v < lwh; v++)
			for(u = 0, tu = lwh * n; u < lwh; u++, tu++)
				if(img_num.at<unsigned char>(v, u))
					img_captcha.at<Vec3b>(v, tu) = color;
	}

	for(t = 0; t < 3; t++)
		color[t] = rand() % 256;

	for(t = 0; t < 1000; t++)
		img_captcha.at<Vec3b>(rand() % lwh, rand() % (lwh * nic)) = color;

	return img_captcha;
}

Mat CCaptcha::operator()(unsigned num)
{
	throw_if(num > 9);

	char str[2] = {alp[num], '\0'};
	int base_line;
	Mat timg = img_num.clone(), rotate = getRotationMatrix2D(Point2f(lwh / 2, lwh / 2), -20.0 + (rand() / (double) RAND_MAX) * 40.0, 1);
	Size size = getTextSize(str, font_face, font_scale, thickness, & base_line);

	rectangle(timg, Point(0, 0), Point(lwh - 1, lwh - 1), Scalar::all(0), CV_FILLED);
	putText(timg, str, Point((lwh - size.width) / 2, (lwh + size.height) / 2), font_face, font_scale, Scalar::all(255), thickness);
	warpAffine(timg, img_num, rotate, img_num.size(), INTER_LINEAR, BORDER_CONSTANT, 0);

	return img_num;
}

void CCaptcha::show()
{
	namedWindow("Captcha", CV_WINDOW_NORMAL);
	namedWindow("Number", CV_WINDOW_NORMAL);

	imshow("Captcha", img_captcha);
	imshow("Number", img_num);

	waitKey();

	destroyAllWindows();
}

unsigned CCaptcha::check(char * captcha)
{
	unsigned yes, u;

	for(u = 0, yes = 0; u < nic; u++)
		if(this->captcha[u] == captcha[u])
			yes++;

	return yes;
}

char CCaptcha::alphabet(unsigned ind)
{
	if(ind < alphabet_size)
		return alp[ind];

	return -1;
}

CCaptchaBrigade1::CCaptchaBrigade1()
{
	alp[0] = '0';
	alp[1] = '1';
	alp[2] = '2';
	alp[3] = '3';
	alp[4] = '4';
	alp[5] = '5';
	alp[6] = '6';
	alp[7] = '7';
	alp[8] = '8';
	alp[9] = '9';

	font_face = FONT_HERSHEY_SCRIPT_SIMPLEX;
	thickness = 2;
	font_scale = 4;
}

CCaptchaBrigade2::CCaptchaBrigade2()
{
	alp[0] = 'A';
	alp[1] = '?';
	alp[2] = 'P';
	alp[3] = 'D';
	alp[4] = 'E';
	alp[5] = 'Z';
	alp[6] = 'W';
	alp[7] = '2';
	alp[8] = 'X';
	alp[9] = 'I';

	font_face = FONT_HERSHEY_PLAIN;
	thickness = 3;
	font_scale = 8;
}

CCaptchaBrigade3::CCaptchaBrigade3()
{
	alp[0] = '~';
	alp[1] = 'P';
	alp[2] = '4';
	alp[3] = '{';
	alp[4] = '}';
	alp[5] = '^';
	alp[6] = '+';
	alp[7] = '#';
	alp[8] = '?';
	alp[9] = '@';

	font_face = FONT_HERSHEY_PLAIN;
	thickness = 3;
	font_scale = 8;
}

