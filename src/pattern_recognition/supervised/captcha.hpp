
#ifndef SUPERVISED_HPP
#define SUPERVISED_HPP

#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

extern const unsigned lwh;
extern const unsigned nic;
extern const unsigned alphabet_size;

class CCaptcha
{
	char * captcha;
	Mat img_captcha, img_num;

	protected:

		char * alp;
		int font_face, thickness;
		double font_scale;

	public:

		CCaptcha();
		~CCaptcha();

		Mat operator()();
		Mat operator()(unsigned num);

		void show();
		unsigned check(char * captcha);
		char alphabet(unsigned ind);
};

class CCaptchaBrigade1 : public CCaptcha
{
	public:
		
		CCaptchaBrigade1();
};

class CCaptchaBrigade2 : public CCaptcha
{
	public:
		
		CCaptchaBrigade2();
};

class CCaptchaBrigade3 : public CCaptcha
{
	public:
		
		CCaptchaBrigade3();
};

#endif

