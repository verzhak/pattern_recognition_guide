
#include "classifier.hpp"

const unsigned real_dim = 31;
const unsigned dim = real_dim; // 7;
fmll_pca * pca = NULL;

Mat old_good_vec(Mat & img)
{
	unsigned u, t, v, max_area_ind;
	double area, max_area, hu[7];
	vector<vector<Point> > cnt, approx_cnt;
	Size size = img.size();
	Mat vec(1, real_dim, CV_32FC1);
	Moments mm;

	findContours(img, cnt, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);

	if(cnt.size() > 0)
	{
		for(v = 1, max_area = contourArea(cnt[0]), max_area_ind = 0; v < cnt.size(); v++)
		{
			area = contourArea(cnt[v]);

			if(area > max_area)
			{
				max_area = area;
				max_area_ind = v;
			}
		}

		approx_cnt.push_back(vector<Point>());
		approxPolyDP(cnt[max_area_ind], approx_cnt[0], 2, true);

		mm = moments(approx_cnt[0]);
		HuMoments(mm, hu);
	}
	else
	{
		for(t = 0; t < 7; t++)
			hu[t] = 0;
	}

	for(t = 0; t < 7; t++)
		vec.at<float>(0, t) = hu[t];

	vec.at<float>(0, 7) = mm.m00;
	vec.at<float>(0, 8) = mm.m10;
	vec.at<float>(0, 9) = mm.m01;
	vec.at<float>(0, 10) = mm.m20;
	vec.at<float>(0, 11) = mm.m11;
	vec.at<float>(0, 12) = mm.m02;
	vec.at<float>(0, 13) = mm.m30;
	vec.at<float>(0, 14) = mm.m21;
	vec.at<float>(0, 15) = mm.m12;
	vec.at<float>(0, 16) = mm.m03;

	vec.at<float>(0, 17) = mm.mu20;
	vec.at<float>(0, 18) = mm.mu11;
	vec.at<float>(0, 19) = mm.mu02;
	vec.at<float>(0, 20) = mm.mu30;
	vec.at<float>(0, 21) = mm.mu21;
	vec.at<float>(0, 22) = mm.mu12;
	vec.at<float>(0, 23) = mm.mu03;

	vec.at<float>(0, 24) = mm.nu20;
	vec.at<float>(0, 25) = mm.nu11;
	vec.at<float>(0, 26) = mm.nu02;
	vec.at<float>(0, 27) = mm.nu30;
	vec.at<float>(0, 28) = mm.nu21;
	vec.at<float>(0, 29) = mm.nu12;
	vec.at<float>(0, 30) = mm.nu03;

	return vec;
}

Mat new_vec(Mat & img)
{
	unsigned v, u;
	Mat vec(1, dim, CV_32FC1), full_vec = old_good_vec(img);
	double sum, max[real_dim];

	max[0] = 3.594771;
	max[1] = 8.899912;
	max[2] = 95.616770;
	max[3] = 62.832524;
	max[4] = 3830.425599;
	max[5] = 55.443609;
	max[6] = 3007.716647;
	max[7] = 4329.500000;
	max[8] = 255919.833333;
	max[9] = 260634.666667;
	max[10] = 16263018.083333;
	max[11] = 15737807.208333;
	max[12] = 18073730.250000;
	max[13] = 1095781003.400000;
	max[14] = 1028988082.833333;
	max[15] = 1131853547.600000;
	max[16] = 1414921572.750000;
	max[17] = 1135414.181414;
	max[18] = 517927.343967;
	max[19] = 2442095.507717;
	max[20] = 4970215.911468;
	max[21] = 3169988.104632;
	max[22] = 5602262.578337;
	max[23] = 18826514.621770;
	max[24] = 1.439417;
	max[25] = 1.169975;
	max[26] = 3.015064;
	max[27] = 1.163781;
	max[28] = 0.562169;
	max[29] = 1.314042;
	max[30] = 7.990945;

	double low = 0;
	double high = 1;

	for(v = 0; v < dim; v++)
		full_vec.at<float>(0, v) = (high - low) * full_vec.at<float>(0, v) / max[v] + low;

	if(pca == NULL)
		throw_null(pca = fmll_pca_load("/home/amv/RGRTU/evm/ns/fmll/build/captcha_3"));

	for(v = 0; v < dim; v++)
	{
		vec.at<float>(0, v) = 0;

		for(u = 0; u < real_dim; u++)
			vec.at<float>(0, v) += full_vec.at<float>(0, u) * pca->w[v][u];
	}

	return vec;
}

Mat vec(Mat & img)
{
	return old_good_vec(img);
	// return new_vec(img);
}

CClassifier::CClassifier(EClassifierType clt)
{
	this->clt = clt;

	switch(clt)
	{
		case CLT_SVM:
		{
			throw_null(svm = new CvSVM);

			break;
		}

		case CLT_BAYES:
		{
			throw_null((bayes = new CvNormalBayesClassifier));

			break;
		}

		case CLT_MLP:
		{
			Mat layer_size(1, 5, CV_32S);

			layer_size.at<int>(0, 0) = dim;
			layer_size.at<int>(0, 1) = 30;
			layer_size.at<int>(0, 2) = 20;
			layer_size.at<int>(0, 3) = 10;
			layer_size.at<int>(0, 4) = 10;

			throw_null((mlp = new CvANN_MLP));

			mlp->create(layer_size);

			break;
		}

		case CLT_RTREE:
		{
			throw_null((rtree = new CvRTrees));

			break;
		}

		case CLT_MY_MLP:
		{
			unsigned v, layers_num = 3;
			unsigned * N;
			double (* fun[layers_num])(double);
			double (* d_fun[layers_num])(double);

			throw_null(N = (unsigned *) fmll_alloc(sizeof(unsigned), 1, layers_num));

			N[0] = 10;
			N[1] = 10;
			N[2] = 1;

			for(v = 0; v < layers_num; v++)
			{
				fun[v] = & fmll_sigmoid;
				d_fun[v] = & fmll_d_sigmoid;
			}

			throw_null(my_rnd = fmll_random_init(FMLL_RANDOM_ALGORITHM_LCG, FMLL_RANDOM_DISTRIBUTION_UNIFORM, NULL, time(NULL)));
			throw_null(my_mlp = (fmll_mlp **) fmll_alloc(sizeof(fmll_mlp *), 1, alphabet_size));

			for(v = 0; v < alphabet_size; v++)
			{
				throw_null(my_mlp[v] = fmll_mlp_init(dim, layers_num, N, fun, d_fun));
				throw_if(fmll_mlp_weight_init_nguyen_widrow(my_mlp[v], my_rnd));
			}

			fmll_free(N);
		}
	};
}

CClassifier::~CClassifier()
{
	switch(clt)
	{
		case CLT_SVM:
		{
			delete svm;

			break;
		}

		case CLT_BAYES:
		{
			delete bayes;

			break;
		}

		case CLT_MLP:
		{
			delete mlp;

			break;
		}

		case CLT_RTREE:
		{
			delete rtree;

			break;
		}

		case CLT_MY_MLP:
		{
			fmll_free(my_rnd);
			fmll_free(my_mlp);

			break;
		}
	};
}

void CClassifier::train(Mat & train_data, Mat & responses)
{
	switch(clt)
	{
		case CLT_SVM:
		{
			CvSVMParams param;

			param.svm_type = CvSVM::C_SVC;
			param.C = 2;
			param.kernel_type = CvSVM::RBF;
			param.term_crit = TermCriteria(CV_TERMCRIT_EPS, (int) 1e7, 1e-6);

			svm->train(train_data, responses, Mat(), Mat(), param);

			break;
		}

		case CLT_BAYES:
		{
			bayes->train(train_data, responses);

			break;
		}

		case CLT_MLP:
		{
			unsigned u, v;
			Size size = responses.size();
			Mat tresp(size.height, 10, CV_32FC1);
			CvANN_MLP_TrainParams param;

			param.term_crit = cvTermCriteria(CV_TERMCRIT_EPS, 1000, 0.0000001);

			for(u = 0; u < size.height; u++)
				for(v = 0; v < 10; v++)
				{
					if(v == responses.at<int>(u, 0))
						tresp.at<float>(u, v) = 1;
					else
						tresp.at<float>(u, v) = 0;
				}

			mlp->train(train_data, tresp, Mat(), Mat(), param);

			break;
		}

		case CLT_RTREE:
		{
			rtree->train(train_data, CV_ROW_SAMPLE, responses);

			break;
		}

		case CLT_MY_MLP:
		{
			unsigned v, u, t;
			double low = 0, high = 1, ** vec = NULL, ** d = NULL;
			Size size = responses.size();

			throw_null(vec = (double **) fmll_alloc(sizeof(double), 2, size.height, dim));
			throw_null(d = (double **) fmll_alloc(sizeof(double), 2, size.height, 1));

			for(v = 0; v < size.height; v++)
				for(u = 0; u < dim; u++)
					vec[v][u] = train_data.at<float>(v, u);

			fmll_sigmoid_a = 0.1;

			for(t = 0; t < alphabet_size; t++)
			{
				for(v = 0; v < size.height; v++)
				{
					if(responses.at<int>(v, 0) == t)
						d[v][0] = high;
					else
						d[v][0] = low;
				}

				throw_if(fmll_mlp_teach_gradient_batch(my_mlp[t], vec, d, size.height, 0, & fmll_timing_next_beta_step_plus_0_01,
							0, 100000, 0.0006, 0.0000001));
	//			throw_if(fmll_mlp_teach_conjugate_gradient(my_mlp[t], vec, d, size.height, my_rnd, 100000, 0.01, 0, 0.000000001));
			}

			fmll_free(vec);
			fmll_free(d);

			break;
		}
	};
}

unsigned CClassifier::predict(Mat & vec)
{
	unsigned ret;

	switch(clt)
	{
		case CLT_SVM:
		{
			ret = svm->predict(vec);

			break;
		}

		case CLT_BAYES:
		{
			ret = bayes->predict(vec);

			break;
		}

		case CLT_MLP:
		{
			unsigned u;
			float max = 0;
			Mat output(1, 10, CV_32FC1);

			mlp->predict(vec, output);

			for(u = 0; u < 10; u++)
				if(output.at<float>(0, u) > max)
				{
					max = output.at<float>(0, u);
					ret = u;
				}

			break;
		};

		case CLT_RTREE:
		{
			ret = rtree->predict(vec);

			break;
		}

		case CLT_MY_MLP:
		{
			unsigned v;
			double d, max, tvec[dim];

			for(v = 0; v < dim; v++)
				tvec[v] = vec.at<float>(0, v);

			ret = 0;
			max = fmll_mlp_run(my_mlp[0], tvec)[0];

			for(v = 1; v < alphabet_size; v++)
			{
				d = fmll_mlp_run(my_mlp[v], tvec)[0];

				if(d > max)
				{
					max = d;
					ret = v;
				}
			}

			break;
		}
	};

	return ret;
}

