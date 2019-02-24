
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
	Draw2D imageDrawer;
	std::vector<std::vector<float>> _image;

};
