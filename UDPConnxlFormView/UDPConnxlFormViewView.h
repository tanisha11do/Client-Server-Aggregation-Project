#pragma once

#include <afxwin.h>
#include <vector>
#include <string>
#include <afxsock.h> // Include Winsock headers if using UDP sockets
#include "UDPConnxlFormViewDoc.h"  
#define WM_SOCKET (WM_USER + 1)

class CUDPConnxlFormViewView : public CFormView
{
protected: // create from serialization only
	CUDPConnxlFormViewView() noexcept;
	DECLARE_DYNCREATE(CUDPConnxlFormViewView)

public:
	// Standard constructor
	virtual ~CUDPConnxlFormViewView();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UDPCONNXLFORMVIEW_FORM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void AssertValid() const;
	void Dump(CDumpContext & dc) const;
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnReceiveData(WPARAM wParam, LPARAM lParam);

protected:
	HICON m_hIcon;
	CListBox m_listClient;
	CListBox m_listServer;

	int m_currentIndex;
	UINT_PTR m_timer;
	SOCKET m_socket;
	SOCKADDR_IN m_serverAddr;
	WSAEVENT m_event;

	std::vector<std::vector<CString>> m_excelData;

protected:
	CUDPConnxlFormViewDoc * GetDocument() const;
	BOOL LoadExcelData(const CString& filePath);
	void SendDataAndUpdateListBox();

public:
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
};
