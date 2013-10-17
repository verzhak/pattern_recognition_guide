
#include "all.hpp"

void noise(unsigned index);
void dnoise(unsigned index);
void faces(unsigned index);

int main(const int argc, const char * argv[])
{
	int index, ret = 0;

	try
	{
		throw_if(argc != 3);
		
		index = atoi(argv[2]) - 1;

		throw_if(index < 0 || index > 2);

		if(! strcmp(argv[1], "noise"))
			noise(index);
		else if(! strcmp(argv[1], "delete_noise"))
			dnoise(index);
		else if(! strcmp(argv[1], "faces"))
			faces(index);
		else
			throw_;

	}
	catch(...)
	{
		ret = -1;
	}

	return ret;
}

vector<function<Mat (Size &)>> noise_gen =
{
	[] (Size & size)
	{
		Mat dst = Mat::zeros(size, CV_8U);

		circle(dst, Point(size.width / 2, size.height / 2), 100, Scalar::all(1), 1);

		return dst;
	},
	[] (Size & size)
	{
		Mat dst = Mat::zeros(size, CV_8U);

		circle(dst, Point(size.width / 2, size.height / 2), 150, Scalar::all(1), 1);

		return dst;
	},
	[] (Size & size)
	{
		Mat dst = Mat::zeros(size, CV_8U);

		circle(dst, Point(size.width / 2, size.height / 2), 150, Scalar::all(1), 1);

		return dst;
	}
};

vector<function<Mat (Size &)>> bv_filter =
{
	[] (Size & size)
	{
		Mat dst = Mat::zeros(size, CV_8U);

		circle(dst, Point(size.width / 2, size.height / 2), 100, Scalar::all(1), 2);
		threshold(dst, dst, 0, 1, THRESH_BINARY_INV);

		return dst;
	},
	[] (Size & size)
	{
		Mat dst = Mat::zeros(size, CV_8U);

		circle(dst, Point(size.width / 2, size.height / 2), 150, Scalar::all(1), 2);
		threshold(dst, dst, 0, 1, THRESH_BINARY_INV);

		return dst;
	},
	[] (Size & size)
	{
		Mat dst = Mat::zeros(size, CV_8U);

		circle(dst, Point(size.width / 2, size.height / 2), 150, Scalar::all(1), 2);
		threshold(dst, dst, 0, 1, THRESH_BINARY_INV);

		return dst;
	}
};

void noise(unsigned index)
{
	int v, u, t;
	stringstream fname;
	Mat frame, noise_gn;
	Size size;
	VideoCapture movie;
	VideoWriter movie_noise;

	try
	{
		namedWindow("frame", CV_WINDOW_NORMAL);
		namedWindow("magn_0", CV_WINDOW_NORMAL);
		namedWindow("magn_1", CV_WINDOW_NORMAL);
		namedWindow("magn_2", CV_WINDOW_NORMAL);
		namedWindow("frame_noise", CV_WINDOW_NORMAL);

		fname << DATA_DIR << "/faces/" << index + 1 << ".mkv";
		throw_if(! movie.open(fname.str()));

		throw_if(! movie.read(frame));
		size = frame.size();
		noise_gn = noise_gen[1](size);

		fname.str("");
		fname << DATA_DIR << "/faces/" << index + 1 << "_noise.mkv";
		throw_if(! movie_noise.open(fname.str(), CV_FOURCC('M', 'P', '4', 'V'), 25, size));

		do
		{
			double max[2], min[2];
			Mat temp, ttemp, frame_noise, frame_float = Mat_<Vec3f>(frame);
			vector<Mat> ch, ch_dft(3), ch_idft(3), magn(3);

			split(frame_float, ch);

			for(t = 0; t < 3; t++)
			{
				vector<Mat> temp_ch, temp = { ch[t], Mat::zeros(size, CV_32F) };

				merge(temp, ttemp);
				dft(ttemp, ch_dft[t], DFT_COMPLEX_OUTPUT);

				split(ch_dft[t], temp_ch);

				minMaxLoc(temp_ch[0], & min[0], & max[0]);
				minMaxLoc(temp_ch[1], & min[1], & max[1]);

				ch_dft[t].setTo(Scalar(max[0] / 100, max[1] / 100), noise_gn);

				split(ch_dft[t], temp_ch);
			 	// normalize(temp_ch[0], temp_ch[0], 0, 1, NORM_MINMAX);
				// normalize(temp_ch[1], temp_ch[1], 0, 1, NORM_MINMAX);
				magnitude(temp_ch[0], temp_ch[1], magn[t]);
				// normalize(magn[t], magn[t], 0, 1, NORM_MINMAX);
				magn[t] /= 5000;
				
				idft(ch_dft[t], ch_idft[t], DFT_SCALE);
				split(ch_idft[t], temp_ch);
				ch_idft[t] = temp_ch[0].clone();
			}

			merge(ch_idft, temp);
			frame_noise = Mat_<Vec3b>(temp);

			movie_noise.write(frame_noise);

			imshow("frame", frame);
			imshow("magn_0", magn[0]);
			imshow("magn_1", magn[1]);
			imshow("magn_2", magn[2]);
			imshow("frame_noise", frame_noise);

			waitKey(40); // 25 fps, задержка в миллисекундах = 1000 / 25 = 40
		}
		while(movie.read(frame));

		destroyAllWindows();
	}
	catch(...)
	{
		throw_;
	}
}

void dnoise(unsigned index)
{
	int t;
	stringstream fname;
	Size size;
	Mat frame, flt;
	vector<Mat> temp_ch, ch_dft_mask(3), ch_mean(3), magn(3), ch_mean_magn(3), ch_and_magn(3);
	VideoCapture movie;
	VideoWriter movie_recover;

	try
	{
		namedWindow("frame", CV_WINDOW_NORMAL);
		namedWindow("frame_recover", CV_WINDOW_NORMAL);
		namedWindow("magn_0", CV_WINDOW_NORMAL);
		namedWindow("magn_1", CV_WINDOW_NORMAL);
		namedWindow("magn_2", CV_WINDOW_NORMAL);

		fname << DATA_DIR << "/faces/" << index + 1 << "_noise.mkv";
		throw_if(! movie.open(fname.str()));

		frame = movie.read(frame);
		size = frame.size();
		flt = Mat_<float>(bv_filter[index](size));

		fname.str("");
		fname << DATA_DIR << "/faces/" << index + 1 << "_recover.mkv";
		throw_if(! movie_recover.open(fname.str(), CV_FOURCC('M', 'P', '4', 'V'), 25, size));

		do
		{
			Mat temp, ttemp, frame_recover, frame_float = Mat_<Vec3f>(frame);
			vector<Mat> ch, temp_ch, ch_dft(3), ch_idft(3), magn(3);

			split(frame_float, ch);

			for(t = 0; t < 3; t++)
			{
				vector<Mat> temp = { ch[t], Mat::zeros(size, CV_32F) };

				merge(temp, ttemp);
				dft(ttemp, ch_dft[t], DFT_COMPLEX_OUTPUT);

				split(ch_dft[t], temp_ch);
				temp_ch[0] = temp_ch[0].mul(flt);
				temp_ch[1] = temp_ch[1].mul(flt);
				merge(temp_ch, ch_dft[t]);

				idft(ch_dft[t], ch_idft[t], DFT_SCALE);

				split(ch_idft[t], temp_ch);
				ch_idft[t] = temp_ch[0].clone();
				// magnitude(temp_ch[0], temp_ch[1], ch_idft[t]);

				split(ch_dft[t], temp_ch);
				normalize(temp_ch[0], temp_ch[0]);
				normalize(temp_ch[1], temp_ch[1]);
				magnitude(temp_ch[0], temp_ch[1], magn[t]);
				magn[t] *= 5000;
			}

			merge(ch_idft, temp);
			frame_recover = Mat_<Vec3b>(temp);

			movie_recover.write(frame_recover);

			imshow("frame", frame);
			imshow("frame_recover", frame_recover);
			imshow("magn_0", magn[0]);
			imshow("magn_1", magn[1]);
			imshow("magn_2", magn[2]);

			waitKey(40); // 25 fps, задержка в миллисекундах = 1000 / 25 = 40
		}
		while(movie.read(frame));

		destroyAllWindows();
	}
	catch(...)
	{
		throw_;
	}
}

void faces(unsigned index)
{
	stringstream fname;
	Mat frame, frame_faces;
	VideoCapture movie;
	CascadeClassifier cascade;
	vector<Rect> faces;

	try
	{
		namedWindow("frame", CV_WINDOW_NORMAL);
		namedWindow("frame_faces", CV_WINDOW_NORMAL);

		fname << DATA_DIR << "/faces/" << index + 1 << "_recover.mkv";
		throw_if(! movie.open(fname.str()));

		fname.str("");
		fname << DATA_DIR << "/classifiers/haarcascade_frontalface_alt.xml";
		throw_if(! cascade.load(fname.str()));

		while(movie.read(frame))
		{
			cascade.detectMultiScale(frame, faces, 1.1, 3,
					0
					// | CV_HAAR_FIND_BIGGEST_OBJECT
					// | CV_HAAR_DO_ROUGH_SEARCH
					// | CV_HAAR_DO_CANNY_PRUNING
					| CV_HAAR_SCALE_IMAGE
					, Size(30, 30));

			frame_faces = frame.clone();

			for(auto iter = faces.begin(); iter != faces.end(); iter ++)
				rectangle(frame_faces, Point(iter->x, iter->y), Point(iter->x + iter->width, iter->y + iter->height), CV_RGB(255, 255, 255), 3);

			imshow("frame", frame);
			imshow("frame_faces", frame_faces);

			waitKey(1); // 25 fps, задержка в миллисекундах = 1000 / 25 = 40
		}

		destroyAllWindows();
	}
	catch(...)
	{
		throw_;
	}
}

