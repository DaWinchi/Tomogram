
// TomogramDlg.h: файл заголовка
//

#pragma once

#include "Draw2D.h"
#include <complex>

struct Projection
{
	std::vector<float> vecSpecificProjection;

	std::vector<std::vector<std::vector<int>>> vecCoeffs;
};


// Диалоговое окно CTomogramDlg
class CTomogramDlg : public CDialogEx
{
// Создание
public:
	CTomogramDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TOMOGRAM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	struct cmplx { float real; float image; };

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoad();
	void LoadPicture();
	typedef std::vector<std::vector<float>> imageType;
	void IncreaseSizeImage();
	void NormalizeAmplitude(imageType &data);
	Draw2D imageDrawer;
	Draw2D drawerTomogram;
	imageType _image;
	imageType _imageIncreased;
	imageType _imageTomogram;
	imageType _imageRestored;
	imageType _imageRestoredReduced;

	std::vector<std::vector<cmplx>> spectre_cmplx;

	std::vector<std::vector<float>> _imageRotated;
	afx_msg void OnBnClickedTomogram();
	CStatic _resolutionText;
	int _step_d;
	double _step_a;
	CStatic _resolutionTomText;
	double _angle_max;
	Draw2D _drawerRestored;
	afx_msg void OnBnClickedRestore();

	void NormilizeAmplitude(imageType &pData, int indentX, int indentY);
	
	float _R;
	size_t originalW, originalH, offsetX, offsetY;


	std::vector<Projection> getRadonTransform(const std::vector<std::vector<float>> & vecImage, int anglesNumber);
	Projection getSpecificProjection(const std::vector<std::vector<float>> & vecImage, double angle);
	void coordTransform(double s, double t, int&x, int&y, double theta);
	void linearizeEquationsSystem(const std::vector<Projection> &p,
		std::vector<std::vector<int>> & a, std::vector<float> & y);
	void SVD(std::vector<int> &matrixA, std::vector<std::vector<float>> &x, std::vector<float> & b, int nn, int ny);
	std::vector<Projection> _vecRadonTransform;
	std::vector<std::vector<float>> _vecProjectionsSVD;
	std::size_t _d;
};
