// TomogramDlg.cpp: файл реализации
//

#define _USE_MATH_DEFINES
#include "stdafx.h"
#include "Tomogram.h"
#include "TomogramDlg.h"
#include "afxdialogex.h"
#include <numeric>
#include <algorithm>
#include <math.h>
#include <fstream>


#include "Eigen/Dense"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define M_PI           3.14159265358979323846  /* pi */
// Диалоговое окно CTomogramDlg



CTomogramDlg::CTomogramDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TOMOGRAM_DIALOG, pParent)
	, _step_d(5)
	, _step_a(5)
	, _angle_max(180)
	, _R(10)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTomogramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE, imageDrawer);
	DDX_Control(pDX, IDC_IMAGE2, drawerTomogram);
	DDX_Control(pDX, IDC_RESOLUTION, _resolutionText);
	DDX_Text(pDX, IDC_STEP_D, _step_d);
	DDX_Text(pDX, IDC_STEP_A, _step_a);
	DDX_Control(pDX, IDC_RESOLUTION2, _resolutionTomText);
	DDX_Text(pDX, IDC_MAX_ANGLE, _angle_max);
	DDX_Control(pDX, IDC_IMAGE3, _drawerRestored);
	DDX_Text(pDX, IDC_R, _R);
}

BEGIN_MESSAGE_MAP(CTomogramDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOAD, &CTomogramDlg::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_TOMOGRAM, &CTomogramDlg::OnBnClickedTomogram)
	ON_BN_CLICKED(IDC_RESTORE, &CTomogramDlg::OnBnClickedRestore)
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
	drawerTomogram._image = &_imageTomogram;

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
	size_t width = bmp.GetWidth();
	size_t height = bmp.GetHeight();
	_image.clear();

	for (int i = 0; i < height; i++)
	{
		std::vector<float> bufPix;
		for (int j = 0; j < width; j++)
		{
			float value;
			Color color;
			bmp.GetPixel(j, (int)height - i - 1, &color);
			value = 0.299F*color.GetRed() + 0.587F*color.GetGreen() + 0.114F*color.GetBlue();
			bufPix.push_back(value);
		}
		_image.push_back(bufPix);
	}
}

void CTomogramDlg::IncreaseSizeImage()
{
	const int sizeNew = (int)sqrt(_image.size()*_image.size() + _image[0].size()*_image[0].size());
	const size_t widthOld = _image[0].size(),
		heightOld = _image.size();
	originalW = widthOld;
	originalH = heightOld;
	const int sizeNewHalf = sizeNew / 2;

	offsetY = sizeNewHalf - heightOld / 2;
	offsetX = sizeNewHalf - widthOld / 2;
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

	size_t widthText = _imageIncreased[0].size(),
		heightText = _imageIncreased.size();
	CString str, arg1, arg2;
	str = "Resolution: ";
	arg1.Format(_T("%d"), widthText);
	arg2.Format(_T("%d"), heightText);
	str += arg1 + "x" + arg2;
	_resolutionText.SetWindowTextW(str);
}

void CTomogramDlg::OnBnClickedTomogram()
{
	UpdateData(TRUE);
	const size_t step = _step_d;
	const double angleStep = _step_a;
	std::vector<size_t> indexes;
	_vecRadonTransform.clear();

	int anglesCount = static_cast<int>(_angle_max / angleStep);

	_vecRadonTransform = getRadonTransform(_image, anglesCount);

	_vecProjectionsSVD.clear();
	_vecProjectionsSVD.resize(_vecRadonTransform.size());

	for (std::size_t idxEl{ 0U }; idxEl < _vecProjectionsSVD.size(); ++idxEl)
	{
		_vecProjectionsSVD[idxEl] = _vecRadonTransform[idxEl].vecSpecificProjection;
	}

	NormalizeAmplitude(_vecProjectionsSVD);	

	drawerTomogram._image = &_vecProjectionsSVD;
	drawerTomogram.Invalidate();
	int widthText = _vecProjectionsSVD[0].size(),
		heightText = _vecProjectionsSVD.size();
	CString str, arg1, arg2;
	str = "Resolution: ";
	arg1.Format(_T("%d"), widthText);
	arg2.Format(_T("%d"), heightText);
	str += arg1 + "x" + arg2;
	str += " (n_projection x n_angle)";
	_resolutionTomText.SetWindowTextW(str);
}

/** Вычислить преобразование Радона.
* @param vecImage - изображение для вычисления преобразования.
* @param numberAngles - число углов.
*/
std::vector<Projection> CTomogramDlg::getRadonTransform(const std::vector<std::vector<float>> & vecImage,
	int anglesNumber)
{
	std::vector<Projection> vecProjections;
	vecProjections.resize(anglesNumber + 1);
	double stepAngle = M_PI / (anglesNumber + 1);

	for (int idxAngle{ 0 }; idxAngle <= anglesNumber; ++idxAngle)
	{
		vecProjections[idxAngle] = getSpecificProjection(vecImage, idxAngle * stepAngle);
	}

	return vecProjections;
}

/** Вычислить проекцию для конкретного угла.
* @param vecImage - изображение.
* @param angle - конкретный угол.
*/
Projection CTomogramDlg::getSpecificProjection(const std::vector<std::vector<float>> & vecImage, double angle)
{
	int height = static_cast<int>(vecImage.size());
	int width = static_cast<int>(vecImage[0].size());

	int d = static_cast<int>((std::sqrt(width / 2 * width / 2 + height / 2 * height / 2)));
	// image size
	_d = 2 * static_cast<std::size_t>(d);

	Projection projection;
	int x, y;
	std::vector<std::vector<int>> vecCoeffs;
	vecCoeffs.resize(2 * d, std::vector<int>(2 * d, 0));

	// per s
	for (int s = -d; s < d; s += _step_d)
	{
		for (int idxEl{ 0 }; idxEl < vecCoeffs.size(); ++idxEl)
		{
			std::fill(vecCoeffs[idxEl].begin(), vecCoeffs[idxEl].end(), 0);
		}

		double sum = 0.0;
		// per t
		for (int t = -d; t < d; ++t)
		{
			coordTransform(s, t, x, y, angle);

			int idxHeight = height / 2 + y;
			int idxWidth = width / 2 + x;

			if ((idxWidth >= 0) && (idxWidth < width) && (idxHeight >= 0) && (idxHeight < height))
			{
				sum += vecImage[idxHeight][idxWidth];
			}
			if ((d + x >= 0) && (d + x < 2 * d) && (d + y >= 0) && (d + y < 2 * d))
			{
				vecCoeffs[d + y][d + x] = 1;
			}
		}

		projection.vecSpecificProjection.emplace_back(sum);
		projection.vecCoeffs.emplace_back(vecCoeffs);
	}

	return projection;
}

/** Преобразовать координаты s, t в x, y.
* @param s - координата по s.
* @param t - координата по t.
* @param x - координата по Ох.
* @param y - координата по Оу.
* @param theta - угол поворота.
*/
void CTomogramDlg::coordTransform(double s, double t, int&x, int&y, double theta)
{
	x = (int)(s * cos(theta) - t * sin(theta));
	y = (int)(s * sin(theta) + t * cos(theta));
}


void CTomogramDlg::NormalizeAmplitude(imageType &data)
{
	float max = 0;
	for (size_t i = 0; i < data.size(); i++)
	{
		float local_max = *std::max_element(data[i].begin(), data[i].end());
		max = local_max > max ? local_max : max;
	}

#pragma omp parallel for
	for (int i = 0; i < data.size(); i++)
	{
		for (int j = 0; j < data[i].size(); j++)
		{
			data[i][j] = data[i][j] / max * 255.0F;
		}
	}

}

/** Линеаризовать систему уравнений.
* @param p - вектор проекций.
* @param a - матрица коэффициентов, которая будет линеразиована.
* @param y - вектор значений проекций, который будет линеаризован.
*/
void CTomogramDlg::linearizeEquationsSystem(const std::vector<Projection> &p,
	std::vector<std::vector<int>> & a, std::vector<float> & y)
{
	a.resize(p.size() * p[0].vecSpecificProjection.size());
	for (std::size_t idxHeight{ 0U }; idxHeight < p.size(); ++idxHeight)
	{
		for (std::size_t idxWidth{ 0U }; idxWidth < p[idxHeight].vecSpecificProjection.size(); ++idxWidth)
		{
			y.emplace_back(p[idxHeight].vecSpecificProjection[idxWidth]);
			for (std::size_t k1{ 0U }; k1 < p[idxHeight].vecCoeffs[idxWidth].size(); ++k1)
			{
				for (std::size_t k2{ 0U }; k2 < p[idxHeight].vecCoeffs[idxWidth][k1].size(); ++k2)
				{
					a[idxHeight * p[idxHeight].vecSpecificProjection.size() + idxWidth].emplace_back(p[idxHeight].vecCoeffs[idxWidth][k1][k2]);
				}
			}
		}
	}
}

void CTomogramDlg::OnBnClickedRestore()
{
	UpdateData(TRUE);
	int width = 0;
	int height = 0;

	std::vector<std::vector<int>> vecCoeffs;
	std::vector<float> vecY;

	linearizeEquationsSystem(_vecRadonTransform, vecCoeffs, vecY);

	int nn = static_cast<int>(vecCoeffs[0].size());
	int ny = static_cast<int>(vecY.size());

	std::vector<double> vecX;
	vecX.resize(nn, 0.0);

	std::vector<int> vecA;
	for (std::size_t idxHeight = 0; idxHeight < vecCoeffs.size(); ++idxHeight)
	{
		for (std::size_t idxWidth = 0; idxWidth < vecCoeffs[idxHeight].size(); ++idxWidth)
		{
			vecA.emplace_back(vecCoeffs[idxHeight][idxWidth]);
		}
	}

	_imageRestored.clear();
	_imageRestored.resize(_d, std::vector<float>(_d, 0.0));

	SVD(vecA, _imageRestored, vecY, nn, ny);

	NormalizeAmplitude(_imageRestored);

	_drawerRestored._image = &_imageRestored;
	_drawerRestored.RedrawWindow();
}

void CTomogramDlg::NormilizeAmplitude(imageType &pData, int indentX, int indentY)
{
	float max = 0;

	double height = (double)pData.size(),
		width = (double)pData[0].size();

	//#pragma omp parallel for
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (((j < indentX) && (i < indentY))
				|| (((height - i) < indentY) && ((width - j) < indentX))
				|| (((height - i) < indentY) && (j < indentX))
				|| ((i < indentY) && ((width - j) < indentX)))
				continue;
			if (pData[i][j] > max) max = pData[i][j];
		}
	}
	//#pragma omp parallel for
	int pp = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (((j < indentX) && (i < indentY))
				|| (((height - i) < indentY) && ((width - j) < indentX))
				|| (((height - i) < indentY) && (j < indentX))
				|| ((i < indentY) && ((width - j) < indentX)))
			{
				pData[i][j] = 255.f;
				continue;
			}
			pData[i][j] = pData[i][j] / max * 255.f;
		}
	}
}

void CTomogramDlg::SVD(std::vector<int> &matrixA, std::vector<std::vector<float>> &x, std::vector<float> & b, int nn, int ny)
{
	//std::ofstream out;
	Eigen::MatrixXf As(ny, nn);
	Eigen::VectorXf bb(ny);
	for (int i = 0; i < ny; ++i)
	{
		for (int j = 0; j < nn; ++j)
		{
			As(i, j) = static_cast<float>(matrixA[i*nn + j]);
		}
		bb(i) = b[i];
	}

	/*out.open("A.txt");
	if (out.is_open()) 
	{
		for (int i = 0; i < ny; ++i)
		{
			for (int j = 0; j < nn; ++j)
			{
				out<<As(i, j)<<" ";
			}
			out << std::endl;
		}
	} 
	out.close();*/

	/*out.open("y.txt");
	if (out.is_open())
	{
		for (int i = 0; i < ny; ++i)
		{
			out << bb(i);
			out << std::endl;
		}
	}
	out.close();*/


	Eigen::VectorXf Xs =  As.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(bb);

	x.clear();
	std::vector<float> row(_d, 0.0);
	x.resize(_d, row);
	   	 
	/*out.open("x.txt");
	if (out.is_open()) {
		out << Xs << endl;
	} out.close();*/

	//out.open("x.txt");
	for (int i = 0; i < _d; ++i)
	{
		for (int j = 0; j < _d; ++j)
		{
			x[i][j] = Xs(i*_d+j);
			if (x[i][j] < 0) x[i][j] = 0;
			//out << x[i][j] << " ";
		}

		//out << std::endl;
	}
	//out.close();
}