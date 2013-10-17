
#include "all.hpp"
#include "captcha.hpp"
#include "classifier.hpp"

CClassifier * train(EClassifierType clt, CCaptcha * captcha, unsigned train_vec = 10000, unsigned test_num = 1000);
unsigned test(CClassifier * cl, CCaptcha * captcha);

int main(const int argc, const char * argv[])
{
	int ret = 0;
	unsigned u, v, tv, t, res, yes, yes_captcha, test_num = 1000;
	CCaptcha * captcha = NULL;
	CClassifier * cl = NULL;

	try
	{
		throw_null((captcha = new CCaptchaBrigade1));
		throw_null((cl = train(CLT_BAYES, captcha, 10000, 1000)));

		for(u = 0, yes = 0, yes_captcha = 0; u < test_num; u++)
		{
			yes += (res = test(cl, captcha));

			if(res == nic)
				yes_captcha++;
		}

		printf("Right numbers: %u from %u (%lf %%)\nRight captcha: %u from %u (%lf %%)\n\n",
				yes, test_num * nic, yes * 100 / (double) (test_num * nic), yes_captcha, test_num, yes_captcha * 100 / (double) test_num);
	}
	catch(...)
	{
		ret = -1;

		fprintf(stderr, "\n-------> Error <-------\n\n");
	}

	if(captcha != NULL)
		delete captcha;

	if(cl != NULL)
		delete cl;

	return ret;
}

CClassifier * train(EClassifierType clt, CCaptcha * captcha, unsigned train_vec, unsigned test_num)
{
	CClassifier * cl = NULL;

	try
	{
		throw_null((cl = new CClassifier(clt)));

		unsigned u, v, tv, t, num, yes;
		Mat train_data(train_vec, dim, CV_32FC1);
		Mat responses(train_vec, 1, CV_32SC1);

		for(u = 0; u < train_vec; u++)
		{
			num = rand() % 10;

			Mat img = (* captcha)(num);
			Mat vc = vec(img);

			for(t = 0; t < dim; t++)
				train_data.at<float>(u, t) = vc.at<float>(0, t);

			responses.at<int>(u, 0) = num;
		}

		cl->train(train_data, responses);

		for(u = 0, yes = 0; u < test_num; u++)
		{
			num = rand() % 10;

			Mat img = (* captcha)(num);
			Mat vc = vec(img);

			if(num == cl->predict(vc))
				yes++;
		}

		printf("Правильно классифицированных векторов: %u из %u (%f %%)\n", yes, test_num, yes * 100 / (double) test_num);
	}
	catch(...)
	{
		if(cl != NULL)
			delete cl;

		cl = NULL;
	}

	return cl;
}

unsigned test(CClassifier * cl, CCaptcha * captcha)
{
	char captcha_predict[nic];
	unsigned u, v, max_area_ind;
	double area, max_area;
	Mat img = (* captcha)();
	Size size = img.size();
	Mat kernel(3, 3, CV_64FC1);
	Mat blr, median, filter, lpl, sum, temp, nimg, thr;
	vector<Mat> ch;
	vector<vector<Point> > cnt;

	kernel.at<double>(0, 0) = -0.1;
	kernel.at<double>(0, 1) = -0.1;
	kernel.at<double>(0, 2) = -0.1;
	kernel.at<double>(1, 0) = -0.1;
	kernel.at<double>(1, 1) = 2;
	kernel.at<double>(1, 2) = -0.1;
	kernel.at<double>(2, 0) = -0.1;
	kernel.at<double>(2, 1) = -0.1;
	kernel.at<double>(2, 2) = -0.1;

	medianBlur(img, median, 5);
	filter2D(median, filter, -1, kernel);
	Laplacian(filter, lpl, CV_32F, 5);
	threshold(lpl, thr, 150, 255, THRESH_BINARY);

	split(thr, ch);
	add(ch[0], ch[1], temp, noArray());
	add(temp, ch[2], sum, noArray(), CV_8U);

	for(u = 0; u < nic; u++)
	{
		try
		{
			Mat nimg = sum(Range(0, size.height), Range(u * size.width / nic, (u + 1) * size.width / nic)).clone();
			Mat tnimg = nimg.clone();
			temp = nimg.clone();
			Mat vc = vec(nimg);

			captcha_predict[u] = captcha->alphabet(cl->predict(vc));

			printf("%c\n", captcha_predict[u]);

			Mat cnt_img(size.height, size.width / nic, CV_8UC1);
			findContours(temp, cnt, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);

			for(v = 0, max_area = 0; v < cnt.size(); v++)
			{
				area = contourArea(cnt[v]);

				if(area > max_area)
				{
					max_area = area;
					max_area_ind = v;
				}
			}

			vector<vector<Point> > approx_cnt;
			approx_cnt.push_back(vector<Point>());
			approxPolyDP(cnt[max_area_ind], approx_cnt[0], 2, true);

			rectangle(cnt_img, Point(0, 0), Point(size.width, size.height), Scalar::all(0), CV_FILLED);
			drawContours(cnt_img, approx_cnt, -1, Scalar::all(255));

			namedWindow("img", CV_WINDOW_NORMAL);
			namedWindow("nimg", CV_WINDOW_NORMAL);
			namedWindow("median", CV_WINDOW_NORMAL);
			namedWindow("filter", CV_WINDOW_NORMAL);
			namedWindow("laplacian", CV_WINDOW_NORMAL);
			namedWindow("thres", CV_WINDOW_NORMAL);
			namedWindow("sum", CV_WINDOW_NORMAL);
			namedWindow("cnt_img", CV_WINDOW_NORMAL);

			imshow("img", img);
			imshow("nimg", tnimg);
			imshow("median", median);
			imshow("filter", filter);
			imshow("laplacian", lpl);
			imshow("thres", thr);
			imshow("sum", sum);
			imshow("cnt_img", cnt_img);

			waitKey();

			destroyAllWindows();
		}
		catch(...)
		{
			;
		}
	}

	return captcha->check(captcha_predict);
}

