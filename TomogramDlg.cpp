
// TomogramDlg.cpp: файл реализации
//

#include "stdafx.h"
#include "Tomogram.h"
#include "TomogramDlg.h"
#include "afxdialogex.h"
#include <numeric>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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

void CTomogramDlg::RotateFullImage(double angle, const imageType & dataIn, imageType & dataOut)
{
	const int size = dataIn.size();
	dataOut.clear();
	std::vector<float> row(size);
	dataOut.resize(size, row);

	Matrix matrix;
	Point pointNew;
	const Point center(size / 2, size / 2);
	matrix.Translate(center.X, center.Y);
	matrix.Rotate(angle);

	Rect rect(0, 0, size, size);
	for (size_t y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			pointNew.X = x - center.X;
			pointNew.Y = y - center.Y;
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

	int widthText = _imageIncreased[0].size(),
		heightText = _imageIncreased.size();
	CString str, arg1, arg2;
	str = "Resolution: ";
	arg1.Format(_T("%d"), widthText);
	arg2.Format(_T("%d"), heightText);
	str += arg1 + "x" + arg2;
	_resolutionText.SetWindowTextW(str);
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
	UpdateData(TRUE);
	const size_t step = _step_d;
	const double angleStep = _step_a;
	std::vector<size_t> indexes;
	_imageTomogram.clear();

	//Create indexes of columns for radon converting
	for (size_t i = 0; i < _imageIncreased.size(); i += step)
	{
		indexes.push_back(i);
	}

	for (double angle = 0; angle < _angle_max; angle += angleStep)
	{
		auto row = CreateTomogramRow(angle, indexes);
		_imageTomogram.push_back(row);
	}

	NormalizeAmplitude(_imageTomogram);
	drawerTomogram.Invalidate();
	int widthText = _imageTomogram[0].size(),
		heightText = _imageTomogram.size();
	CString str, arg1, arg2;
	str = "Resolution: ";
	arg1.Format(_T("%d"), widthText);
	arg2.Format(_T("%d"), heightText);
	str += arg1 + "x" + arg2;
	str += " (n_projection x n_angle)";
	_resolutionTomText.SetWindowTextW(str);
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
			data[i][j] = data[i][j] / max * 255.0;
		}
	}

}

void CTomogramDlg::BackProjection(imageType & dataOut)
{
	const double angleStep = _step_a;
	const size_t step = _step_d;

	const int size = _imageIncreased.size();

	dataOut.clear();
	std::vector<float> row(size);
	dataOut.resize(size, row);

	std::vector<size_t> indexes;
	imageType dataIn;
	dataIn.resize(size, row);

	for (size_t i = 0; i < _imageIncreased.size(); i += step)
	{
		indexes.push_back(i);
	}

	size_t numProjection = 0;
	for (double angle = 0; angle < _angle_max; angle += angleStep)
	{
		RotateFullImage(angle, dataIn, dataOut);

		//
#pragma omp parallel for
		for (int row = 0; row < indexes.size(); row++)
		{
			double value = _imageTomogram[numProjection][row] / indexes.size();
			for (int col = 0; col < dataOut[0].size(); col++)
			{
				dataOut[indexes[row]][col] += value;
			}
		}

		RotateFullImage(-angle, dataOut, dataIn);
		numProjection++;
	}

	//std::swap(dataIn, dataOut);
}


void CTomogramDlg::OnBnClickedRestore()
{
	UpdateData(TRUE);
	int width = 0;
	int height = 0;

	BackProjection(_imageRestored);
	imageType bufferImage;
	RotateFullImage(-90, _imageRestored, bufferImage);
	swap(_imageRestored, bufferImage);
	ReduceImage(_imageRestored, _imageRestoredReduced);
	int newWidth = _imageRestoredReduced[0].size();
	int newHeight = _imageRestoredReduced.size();

	InterpolateImage(_imageRestoredReduced, width, height);
	FourierTransform(_imageRestoredReduced, -1);
	AddFilter();
	FourierTransform(_imageRestoredReduced, 1);
	InterpolateImage(_imageRestoredReduced, newWidth, newHeight);
	NormalizeAmplitude(_imageRestoredReduced);
	_drawerRestored._image = &_imageRestoredReduced;
	_drawerRestored.RedrawWindow();
}

void CTomogramDlg::Fourie1D(std::vector<cmplx> *data, int n, int is)
{
	int i, j, istep;
	int m, mmax;
	float r, r1, theta, w_r, w_i, temp_r, temp_i;
	float pi = 3.1415926f;

	r = pi * is;
	j = 0;
	for (i = 0; i < n; i++)
	{
		if (i < j)
		{
			temp_r = (*data)[j].real;
			temp_i = (*data)[j].image;
			(*data)[j].real = (*data)[i].real;
			(*data)[j].image = (*data)[i].image;
			(*data)[i].real = temp_r;
			(*data)[i].image = temp_i;
		}
		m = n >> 1;
		while (j >= m) { j -= m; m = (m + 1) / 2; }
		j += m;
	}
	mmax = 1;
	while (mmax < n)
	{
		istep = mmax << 1;
		r1 = r / (float)mmax;
		for (m = 0; m < mmax; m++)
		{
			theta = r1 * m;
			w_r = (float)cos((double)theta);
			w_i = (float)sin((double)theta);
			for (i = m; i < n; i += istep)
			{
				j = i + mmax;
				temp_r = w_r * (*data)[j].real - w_i * (*data)[j].image;
				temp_i = w_r * (*data)[j].image + w_i * (*data)[j].real;
				(*data)[j].real = (*data)[i].real - temp_r;
				(*data)[j].image = (*data)[i].image - temp_i;
				(*data)[i].real += temp_r;
				(*data)[i].image += temp_i;
			}
		}
		mmax = istep;
	}
	if (is > 0)
		for (i = 0; i < n; i++)
		{
			(*data)[i].real /= (float)n;
			(*data)[i].image /= (float)n;
		}


}

void CTomogramDlg::Fourie2D(std::vector<std::vector<cmplx>> &data, int is)
{
	int width = data[0].size();
	int height = data.size();
#pragma omp parallel for
	for (int i = 0; i < height; i++)
	{
		Fourie1D(&data[i], width, is);
	}

	std::vector<std::vector<cmplx>> bufRes;
	for (int i = 0; i < width; i++)
	{
		std::vector<cmplx> buffer;
		for (int j = 0; j < height; j++)
		{
			buffer.push_back(data[j][i]);
		}
		bufRes.push_back(buffer);
	}

#pragma omp parallel for
	for (int i = 0; i < width; i++)
	{
		Fourie1D(&bufRes[i], height, is);
	}

	data.clear();
	data = bufRes;
}

void CTomogramDlg::InterpolateImage(imageType &image, int &newWidth, int &newHeight)
{
	int width = image[0].size(),
		height = image.size();
	if ((newWidth == 0) && (newHeight == 0))
	{
		CheckBin(width, newWidth);
		CheckBin(height, newHeight);

	}

	float stepX = (float)width / (newWidth + 1.0);
	float stepY = (float)height / (newHeight + 1.0);
	imageType tempData;

	for (int h = 0; h < height; h++)
	{
		std::vector<float> bufLine;
		for (int i = 0; i < newWidth; i++)
		{
			float x = i * stepX;
			for (int w = 1; w < width; w++)
			{
				if ((x >= w - 1) && (x < w))
				{
					float value = (float)(image[h][w] - image[h][w - 1])*x +
						(float)(image[h][w - 1] - (image[h][w] - image[h][w - 1])*((float)w - 1.f));
					bufLine.emplace_back(value);
				}
			}
		}
		tempData.emplace_back(bufLine);
	}

	imageType transponData;
	for (int i = 0; i < newWidth; i++)
	{
		std::vector<float> bufCol;
		for (int j = 0; j < height; j++)
		{
			bufCol.emplace_back(tempData[j][i]);
		}
		transponData.emplace_back(bufCol);
	}

	tempData.clear();

	width = transponData[0].size();
	height = transponData.size();

	newWidth = newHeight;
	for (int h = 0; h < height; h++)
	{
		std::vector<float> bufLine;
		for (int i = 0; i < newWidth; i++)
		{
			double x = i * stepY;
			for (int w = 1; w < width; w++)
			{
				if ((x >= w - 1) && (x < w))
				{
					float value = (float)(transponData[h][w] - transponData[h][w - 1])*x +
						(float)(transponData[h][w - 1] - (transponData[h][w] - transponData[h][w - 1])*((float)w - 1.f));
					bufLine.emplace_back(value);
				}
			}
		}
		tempData.emplace_back(bufLine);
	}

	transponData.clear();
	for (int i = 0; i < newWidth; i++)
	{
		std::vector<float> bufCol;
		for (int j = 0; j < newHeight; j++)
		{
			bufCol.emplace_back(tempData[j][i]);
		}
		transponData.emplace_back(bufCol);
	}

	image = transponData;
}

bool CTomogramDlg::CheckBin(int value, int &newvalue)
{
	bool result;
	int tempVal = value;
	if (tempVal <= 0)
		return false;
	while ((tempVal % 2) == 0)
	{
		if ((tempVal /= 2) == 1)
		{
			newvalue = tempVal;
			return true;
		}
	}

	int initValue = 1;
	while (initValue <= value)
	{
		initValue *= 2;
	}
	newvalue = initValue;

	return false;
}

void CTomogramDlg::FourierTransform(imageType &image, int flag)
{
	if (flag == -1)
	{
		std::vector<std::vector<cmplx>> image_cmplx;
		int width = image[0].size();
		int height = image.size();

		for (int i = 0; i < height; i++)
		{
			std::vector<cmplx> buffer;
			for (int j = 0; j < width; j++)
			{
				cmplx value;
				value.image = 0;
				value.real = image[i][j];
				buffer.emplace_back(value);
			}
			image_cmplx.emplace_back(buffer);
		}
		Fourie2D(image_cmplx, flag);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				double val = sqrt(image_cmplx[j][i].image*image_cmplx[j][i].image + image_cmplx[j][i].real*image_cmplx[j][i].real);
				if (val < 0) val = 0;
				image[i][j] = val;
			}
		}
		std::swap(image_cmplx, spectre_cmplx);
	}
	else
	{
		std::vector<std::vector<cmplx>> image_cmplx;
		int width = image[0].size();
		int height = image.size();

		for (int i = 0; i < height; i++)
		{
			std::vector<cmplx> buffer;
			for (int j = 0; j < width; j++)
			{
				cmplx value;
				value.image = spectre_cmplx[i][j].image;
				value.real = spectre_cmplx[i][j].real;
				buffer.emplace_back(value);
			}
			image_cmplx.emplace_back(buffer);
		}
		Fourie2D(image_cmplx, flag);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				double val = image_cmplx[j][i].real;
				if (val < 0) val = 0;
				image[i][j] = val;
			}
		}
	}
}

void CTomogramDlg::NormilizeAmplitude(imageType &pData, int indentX, int indentY)
{
	double max = 0;

	double height = pData.size(),
		width = pData[0].size();

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

void CTomogramDlg::AddFilter()
{
	int height = spectre_cmplx.size(),
		width = spectre_cmplx[0].size();

	float R = _R;

//#pragma omp parallel for
//	for (int i = 0; i <= height / 2; i++)
//	{
//		for (int j = 0; j <= width / 2; j++)
//		{
//			if (sqrt(i*i + j * j) <= R)
//			{
//				spectre_cmplx[i][j].image *= (float)sqrt(i*i + j * j);
//				spectre_cmplx[i][j].real *= (float)sqrt(i*i + j * j);
//			}
//		}
//	}
//#pragma omp parallel for
//	for (int i = 0; i <= height / 2; i++)
//	{
//		for (int j = width - 1; j >= width / 2; j--)
//		{
//			if (sqrt(i*i + (width - 1 - j) *(width - 1 - j)) <= R)
//			{
//				spectre_cmplx[i][j].image *= (float)(sqrt(i*i + (width - 1 - j)*(width - 1 - j)));
//				spectre_cmplx[i][j].real *= (float)(sqrt(i*i + (width - 1 - j)*(width - 1 - j)));
//			}
//		}
//	}
//#pragma omp parallel  for
//	for (int i = height - 1; i >= height / 2; i--)
//	{
//		for (int j = 0; j <= width / 2; j++)
//		{
//			if (sqrt((height - 1 - i)*(height - 1 - i) + j * j) <= R)
//			{
//				spectre_cmplx[i][j].image *= (float)(sqrt((height - 1 - i)*(height - 1 - i) + j * j));
//				spectre_cmplx[i][j].real *= (float)(sqrt((height - 1 - i)*(height - 1 - i) + j * j));
//			}
//		}
//	}
//
//#pragma omp parallel  for
//	for (int i = height - 1; i >= height / 2; i--)
//	{
//		for (int j = width - 1; j >= width / 2; j--)
//		{
//			if (sqrt((height - 1 - i)*(height - 1 - i) + (width - 1 - j) * (width - 1 - j)) <= R)
//			{
//				spectre_cmplx[i][j].image *= (float)(sqrt((height - 1 - i)*(height - 1 - i) + (width - 1 - j) * (width - 1 - j)));
//				spectre_cmplx[i][j].real *= (float)(sqrt((height - 1 - i)*(height - 1 - i) + (width - 1 - j) * (width - 1 - j)));
//			}
//		}
//	}	

#pragma omp parallel for
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
				spectre_cmplx[i][j].image *= (float)sqrt(i*i + j * j);
				spectre_cmplx[i][j].real *= (float)sqrt(i*i + j * j);
		}
	}
}

void CTomogramDlg::ReduceImage(const imageType &dataIn, imageType &dataOut)
{

	std::vector<float> row(originalW);
	dataOut.resize(originalH, row);

//#pragma omp parallel for
	for (int y = 0; y < originalH; y++)
	{
		for (int x = 0; x < originalW; x++)
		{
			dataOut[y][x] = dataIn[y+offsetY][x+offsetX];
		}
	}
}