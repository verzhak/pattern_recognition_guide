
#include "all.hpp"
#include "unsupervised.hpp"

int eval(unsigned ab)
{
	int ret = 0;

	try
	{
		char buf[200];
		int v, u, t, from_to[] = {0, 0, 0, 1, 0, 2};
		unsigned tv, tu, v_1_lwh, u_lwh, u_1_lwh, ind, step, vec_per_class, class_num = 70, real_class_num, height_width = height * width;
		bool is_class_null;
		double d, hu[7], thres[7] = {0.003, 0.003, 0.004, 0.005, 0.004, 0.003, 0.005};
		vector<unsigned> center_ind;
		Mat src, res(height * mult_lwh, width * mult_lwh, CV_8UC3),
			samples(height_width, 7, CV_32FC1), labels(height_width, 1, CV_32SC1), centers(class_num, 7, CV_32FC1);
		Vec3b pixel;
		
		sprintf(buf, "%s/text/%u.png", DATA_DIR, ab + 1);
		src = imread(buf, 0);
		throw_null(src.data);

		// ############################################################################ 
		// Составление пространства

		for(v = 0; v < height; v++)
			for(u = 0; u < width; u++)
			{
				vector<vector<Point> > cnt;
				Mat win = src(Range(v * mult_lwh, (v + 1) * mult_lwh), Range(u * mult_lwh, (u + 1) * mult_lwh)).clone();

				findContours(win, cnt, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
				Moments m = moments(cnt[0]);
				HuMoments(m, hu);

				for(t = 0; t < 7; t++)
					samples.at<float>(v * width + u, t) = hu[t];
			}

		// ############################################################################ 
		// Классификация

		kmeans(samples, class_num, labels, TermCriteria(CV_TERMCRIT_EPS, 0, 0), 100, KMEANS_PP_CENTERS, centers);

		// ############################################################################ 
		// Пост - обработка классов

		for(v = 0; v < class_num; v++)
			center_ind.push_back(v);

		// Удаление всех пустых классов
		for(v = center_ind.size() - 1; v >= 0; v--)
		{
			for(u = 0, is_class_null = true; u < height_width && is_class_null; u++)
				if(labels.at<int>(u, 0) == center_ind[v])
					is_class_null = false;

			if(is_class_null)
				center_ind.erase(center_ind.begin() + v);
		}

		// Объединение классов, Эвклидово расстояние между центрами которых меньше заданного порога
		for(v = 0; v < center_ind.size(); v++)
			for(u = center_ind.size() - 1; u > v; u--)
			{
				for(t = 0, d = 0; t < 7; t++)
					d += (centers.at<float>(center_ind[v], t) - centers.at<float>(center_ind[u], t))
						* (centers.at<float>(center_ind[v], t) - centers.at<float>(center_ind[u], t));

				if(sqrt(d) < thres[ab])
				{
					for(t = 0; t < height_width; t++)
						if(labels.at<int>(t, 0) == center_ind[u])
							labels.at<int>(t, 0) = center_ind[v];

					center_ind.erase(center_ind.begin() + u);
				}
			}

		// Подсчет количества векторов в каждом классе
		for(v = 0, real_class_num = 0; v < center_ind.size(); v++)
		{
			for(u = 0, vec_per_class = 0; u < height_width; u++)
				if(labels.at<int>(u, 0) == center_ind[v])
					vec_per_class++;

			printf("Класс %u: %u\n", v, vec_per_class);

			if(vec_per_class > 7)
				real_class_num ++;
		}

		// ############################################################################ 

		printf("\nВсего различных символов: %u (правильный ответ: %u)\n\n", real_class_num, ab_num[ab]);

		// ############################################################################ 

		step = 0xAAAAAA / (center_ind.size() - 1);
		mixChannels(& src, 1, & res, 1, from_to, 3);

		for(v = 0; v < height; v++)
			for(u = 0; u < width; u++)
			{
				ind = 0x555555 + labels.at<int>(v * width + u, 0) * step;

				pixel[0] = (ind & 0xFF0000) >> 16;
				pixel[1] = (ind & 0xFF00) >> 8;
				pixel[2] = ind & 0xFF;

				for(tv = v * mult_lwh, v_1_lwh = (v + 1) * mult_lwh, u_lwh = u * mult_lwh, u_1_lwh = (u + 1) * mult_lwh; tv < v_1_lwh; tv++)
					for(tu = u_lwh; tu < u_1_lwh; tu++)
						if(src.at<unsigned char>(tv, tu) > 20)
							res.at<Vec3b>(tv, tu) = pixel;
			}

		namedWindow("result", CV_WINDOW_NORMAL);
		imshow("result", res);
		while((char) waitKey() != 'q') ;
		destroyAllWindows();
	}
	catch(...)
	{
		ret = -1;
	}

	return ret;
}

