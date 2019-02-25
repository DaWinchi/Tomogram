
// TomogramDlg.cpp: файл реализации
//

#include "stdafx.h"
#include "Tomogram.h"
#include "TomogramDlg.h"
#include "afxdialogex.h"
#include <numeric>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CTomogramDlg



CTomogramDlg::CTomogramDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TOMOGRAM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTomogramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE, imageDrawer);
}

BEGIN_MESSAGE_MAP(CTomogramDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD, &CTomogramDlg::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_TOMOGRAM, &CTomogramDlg::OnBnClickedTomogram)
END_MESSAGE_MAP()


// Обработчики сообщений CTomogramDlg

BOOL CTomogramDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию
	imageDrawer._image = &_image;

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CTomogramDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CTomogramDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTomogramDlg::OnBnClickedLoad()
{
	LoadPicture();
	IncreaseSizeImage();
	imageDrawer._image = &_imageIncreased;
	imageDrawer.Invalidate();
}

void CTomogramDlg::LoadPicture()
{
	CFileDialog fd(true, NULL, NULL, OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY |
		OFN_LONGNAMES | OFN_PATHMUSTEXIST, _T("Image Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"), NULL, 0, TRUE);

	if (fd.DoModal() != IDOK)
	{
		MessageBox(L"ERROR!!!", L"Error opening picture file.", MB_ICONERROR);
	};

	CString pathBMP = fd.GetPathName();
	Bitmap bmp(pathBMP);
	int width = bmp.GetWidth();
	int height = bmp.GetHeight();
	_image.clear();

	for (size_t i = 0; i < height; i++)
	{
		std::vector<float> bufPix;
		for (size_t j = 0; j < width; j++)
		{
			double value;
			Color color;
			bmp.GetPixel(j, height - i - 1, &color);
			value = 0.299*color.GetRed() + 0.587*color.GetGreen() + 0.114*color.GetBlue();
			bufPix.push_back(value);
		}
		_image.push_back(bufPix);
	}
}

//void CTomogramDlg::RotateImage(double angle, const imageType & dataIn, imageType & dataOut)
//{
//	const int size = dataIn.size();
//	dataOut.clear();
//	std::vector<float> row(size);
//	dataOut.resize(size, row);
//
//	Matrix matrix;
//	Point pointNew;
//	const Point center(size / 2, size / 2);
//	matrix.Translate(center.X, center.Y);
//	matrix.Rotate(angle);
//
//	Rect rect(0, 0, size, size);
////#pragma omp parallel for
//	for (int y = 0; y < size; y++)
//	{
//		for (int x = 0; x < size; x++)
//		{
//			pointNew.X = x - center.X;
//			pointNew.Y = y - center.Y;
//			matrix.TransformPoints(&pointNew);
//			if (!rect.Contains(pointNew)) continue;
//			dataOut[y][x] = dataIn[pointNew.Y][pointNew.X];
//		}
//	}
//}

void CTomogramDlg::RotateImage(double angle, const imageType & dataIn,
	imageType & dataOut, const std::vector<size_t> &indexes)
{
	const int size = dataIn.size();
	dataOut.clear();
	std::vector<float> row(size);
	dataOut.resize(indexes.size(), row);

	Matrix matrix;
	Point pointNew;
	const Point center(size / 2, size / 2);
	matrix.Translate(center.X, center.Y);
	matrix.Rotate(angle);

	Rect rect(0, 0, size, size);
	//#pragma omp parallel for

	for (size_t y = 0; y < indexes.size(); y++)
	{
		for (int x = 0; x < size; x++)
		{
			pointNew.X = x - center.X;
			pointNew.Y = indexes[y] - center.Y;
			matrix.TransformPoints(&pointNew);
			if (!rect.Contains(pointNew)) continue;
			dataOut[y][x] = dataIn[pointNew.Y][pointNew.X];
		}
	}
}

void CTomogramDlg::IncreaseSizeImage()
{
	const int sizeNew = (int)sqrt(_image.size()*_image.size() + _image[0].size()*_image[0].size());
	const int widthOld = _image[0].size(),
		heightOld = _image.size();
	const int sizeNewHalf = sizeNew / 2;

	_imageIncreased.clear();
	std::vector<float> row(sizeNew);
	_imageIncreased.resize(sizeNew, row);

#pragma omp parallel for
	for (int y = 0; y < heightOld; y++)
	{
		for (int x = 0; x < widthOld; x++)
		{
			_imageIncreased[y + sizeNewHalf - heightOld / 2][x + sizeNewHalf - widthOld / 2] = _image[y][x];
		}
	}
}



std::vector<float> CTomogramDlg::CreateTomogramRow(double angle, const std::vector<size_t> &indexes)
{
	std::vector<float> outRow(indexes.size());
	imageType rotatedCols;

	//Rotate columns by indexes
	RotateImage(angle, _imageIncreased, rotatedCols, indexes);

	//Get point of Radon converting
	for (size_t i = 0; i < indexes.size(); i++)
	{
		outRow[i] = std::accumulate(rotatedCols[i].begin(), rotatedCols[i].end(), 0);
	}

	return outRow;
}


void CTomogramDlg::OnBnClickedTomogram()
{
	// TODO: добавьте свой код обработчика уведомлений
}
