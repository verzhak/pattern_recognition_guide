
#ifndef CLASSIFIER_HPP
#define CLASSIFIER_HPP

#include "all.hpp"
#include "captcha.hpp"

extern const unsigned dim;

Mat vec(Mat & img);

enum EClassifierType
{
	CLT_SVM, CLT_BAYES, CLT_MLP, CLT_RTREE,
	CLT_MY_MLP
};

class CClassifier
{
	CvSVM * svm;
	CvNormalBayesClassifier * bayes;
	CvANN_MLP * mlp;
	CvRTrees * rtree;
	fmll_random * my_rnd;
	fmll_mlp ** my_mlp;

	EClassifierType clt;

	public:

		CClassifier(EClassifierType clt);
		~CClassifier();

		void train(Mat & train_data, Mat & responses);
		unsigned predict(Mat & vec);
};

#endif

