#pragma once
#include "afxwin.h"
#include <gdiplus.h>
#include <vector>
#include <iostream>
using namespace Gdiplus;
using namespace std;



class Draw2D : public CStatic
{

public:
	double xmin, xmax, ymin, ymax;
	int *_R;
	std::vector<std::vector<float>>* _image;
	bool _isSpectr, _ellipseDrawing;
	Bitmap *_bmpSpect;
	Draw2D();
	~Draw2D();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	REAL X(LPDRAWITEMSTRUCT RECT, float x);
	REAL Y(LPDRAWITEMSTRUCT RECT, float y);
	REAL Width(LPDRAWITEMSTRUCT RECT, float width);
	REAL Height(LPDRAWITEMSTRUCT RECT, float height);

};

