
// Tomogram.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CTomogramApp:
// Сведения о реализации этого класса: Tomogram.cpp
//

class CTomogramApp : public CWinApp
{
public:
	CTomogramApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CTomogramApp theApp;
