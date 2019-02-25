
// TomogramDlg.h: файл заголовка
//

#pragma once

#include "Draw2D.h"


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

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoad();
	void LoadPicture();
	typedef std::vector<std::vector<float>> imageType;
	void RotateImage(double angle, const imageType & dataIn, imageType & dataOut, const std::vector<size_t> &indexes);
	void IncreaseSizeImage();
	std::vector<float> CreateTomogramRow(double angle, const std::vector<size_t> &indexes);
	void NormalizeAmplitude(imageType &data);
	Draw2D imageDrawer;
	Draw2D drawerTomogram;
	imageType _image;
	imageType _imageIncreased;
	imageType _imageTomogram;

	std::vector<std::vector<float>> _imageRotated;
	afx_msg void OnBnClickedTomogram();
	CStatic _resolutionText;
	int _step_d;
	double _step_a;
	CStatic _resolutionTomText;
	double _angle_max;
};
