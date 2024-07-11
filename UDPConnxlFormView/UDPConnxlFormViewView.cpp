#include "stdafx.h"
#include "UDPConnxlFormView.h"
#include "UDPConnxlFormViewView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CUDPConnxlFormViewView, CFormView)

CUDPConnxlFormViewView::CUDPConnxlFormViewView() noexcept
	: CFormView(IDD_UDPCONNXLFORMVIEW_FORM),
	m_hIcon(nullptr),
	m_currentIndex(0),
	m_timer(0),
	m_socket(INVALID_SOCKET),
	m_event(NULL)
{
}

CUDPConnxlFormViewView::~CUDPConnxlFormViewView()
{
	if (m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		WSACleanup();
	}

	if (m_event != NULL)
	{
		WSACloseEvent(m_event);
	}
}

void CUDPConnxlFormViewView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENT, m_listClient);
	DDX_Control(pDX, IDC_LIST_SERVER, m_listServer);
}

BEGIN_MESSAGE_MAP(CUDPConnxlFormViewView, CFormView)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CUDPConnxlFormViewView::OnBnClickedButtonSend)
	ON_MESSAGE(WM_SOCKET, &CUDPConnxlFormViewView::OnReceiveData)
END_MESSAGE_MAP()

void CUDPConnxlFormViewView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	// Set the icon for this dialog
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetIcon(m_hIcon, TRUE);         // Set big icon
	SetIcon(m_hIcon, FALSE);        // Set small icon

									// Initialize current index
	m_currentIndex = 0;

	// Load Excel data (update the file path as needed)
	bool excelDataLoaded = LoadExcelData(_T("C:\\Users\\user\\OneDrive\\Desktop\\points.csv"));
	if (!excelDataLoaded)
	{
		AfxMessageBox(_T("Failed to load Excel data."));
		GetParent()->SendMessage(WM_CLOSE);  // Optionally handle failure to load data
		return;
	}

	CString message;
	message.Format(_T("%d rows loaded from Excel."), m_excelData.size());
	AfxMessageBox(message);  // Show in a message box

							 // Set up the timer (100ms interval)
	m_timer = SetTimer(1, 100, NULL);

	// Initialize Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		AfxMessageBox(_T("Winsock initialization failed"));
		GetParent()->SendMessage(WM_CLOSE);
		return;
	}

	// Create socket
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_socket == INVALID_SOCKET)
	{
		AfxMessageBox(_T("Socket creation failed"));
		GetParent()->SendMessage(WM_CLOSE);
		return;
	}

	// Bind socket
	SOCKADDR_IN addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1234);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(m_socket, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("Bind failed"));
		closesocket(m_socket);
		WSACleanup();
		GetParent()->SendMessage(WM_CLOSE);
		return;
	}

	// Setup server address (assuming sending to localhost)
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_port = htons(1234);
	InetPton(AF_INET, _T("127.0.0.1"), &m_serverAddr.sin_addr);

	// Create an event for socket notifications
	m_event = WSACreateEvent();
	if (m_event == WSA_INVALID_EVENT)
	{
		AfxMessageBox(_T("WSACreateEvent failed"));
		closesocket(m_socket);
		WSACleanup();
		GetParent()->SendMessage(WM_CLOSE);
		return;
	}

	// Register socket for event-based notifications
	if (WSAEventSelect(m_socket, m_event, FD_READ) == SOCKET_ERROR)
	{
		AfxMessageBox(_T("WSAEventSelect failed"));
		closesocket(m_socket);
		WSACleanup();
		GetParent()->SendMessage(WM_CLOSE);
		return;
	}

	// Create a thread to handle socket events
	AfxBeginThread([](LPVOID pDlg) -> UINT {
		CUDPConnxlFormViewView* pThis = (CUDPConnxlFormViewView*)pDlg;
		WSANETWORKEVENTS events;

		while (true)
		{
			if (WSAWaitForMultipleEvents(1, &pThis->m_event, FALSE, WSA_INFINITE, FALSE) == WSA_WAIT_FAILED)
			{
				AfxMessageBox(_T("WSAWaitForMultipleEvents failed"));
				break;
			}

			if (WSAEnumNetworkEvents(pThis->m_socket, pThis->m_event, &events) == SOCKET_ERROR)
			{
				AfxMessageBox(_T("WSAEnumNetworkEvents failed"));
				break;
			}

			if (events.lNetworkEvents & FD_READ)
			{
				pThis->SendMessage(WM_SOCKET, 0, FD_READ);
			}
		}

		return 0;
	}, this);
}

void CUDPConnxlFormViewView::OnBnClickedButtonSend()
{
	SendDataAndUpdateListBox();
}

void CUDPConnxlFormViewView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_timer)
	{
		// Check if there is data left to send
		if (m_currentIndex < m_excelData.size())
		{
			SendDataAndUpdateListBox(); // Trigger sending of next data row
		}
		else
		{
			KillTimer(m_timer); // Stop the timer if all data has been sent
		}
	}

	CFormView::OnTimer(nIDEvent);
}

LRESULT CUDPConnxlFormViewView::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	char buffer[1024] = { 0 };
	SOCKADDR_IN fromAddr;
	int fromLen = sizeof(fromAddr);
	int recvLen = recvfrom(m_socket, buffer, sizeof(buffer) - 1, 0, (SOCKADDR*)&fromAddr, &fromLen);
	if (recvLen != SOCKET_ERROR)
	{
		buffer[recvLen] = '\0';
		CString message(buffer);

		// Display in server list box
		int index = m_listServer.InsertString(0, message); // Insert at the beginning
		m_listServer.SetCurSel(index);  // Select the newly inserted item
		m_listServer.UpdateWindow();    // Ensure list box updates immediately
	}
	else
	{
		int error = WSAGetLastError();
		CString errMsg;
		errMsg.Format(_T("Receive failed with error: %d"), error);
		AfxMessageBox(errMsg);
	}

	return 0;
}

BOOL CUDPConnxlFormViewView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFormView::PreCreateWindow(cs);
}

#ifdef _DEBUG
void CUDPConnxlFormViewView::AssertValid() const
{
	CFormView::AssertValid();
}

void CUDPConnxlFormViewView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CUDPConnxlFormViewDoc* CUDPConnxlFormViewView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUDPConnxlFormViewDoc)));
	return (CUDPConnxlFormViewDoc*)m_pDocument;
}
#endif //_DEBUG

BOOL CUDPConnxlFormViewView::LoadExcelData(const CString& filePath)
{
	// Clear existing data if needed
	m_excelData.clear();

	CStdioFile file;
	CFileException fileException;
	if (!file.Open(filePath, CFile::modeRead | CFile::typeText, &fileException))
	{
		// Show detailed error message
		TCHAR errorMessage[1024];
		fileException.GetErrorMessage(errorMessage, 1024);
		AfxMessageBox(CString(_T("Failed to open Excel file: ")) + errorMessage);
		return false;
	}

	CString line;
	while (file.ReadString(line))
	{
		// Parse CSV line into array of strings (Time, X, Y, Z)
		CString token;
		std::vector<CString> rowData;
		int pos = 0;
		while (AfxExtractSubString(token, line, pos++, ','))
		{
			rowData.push_back(token);
		}

		if (rowData.size() == 4) // Ensure each row has exactly 4 elements
		{
			m_excelData.push_back(rowData);
		}
		else
		{
			AfxMessageBox(_T("Invalid row format: ") + line); // Debug message for invalid rows
		}
	}

	file.Close();

	CString message;
	message.Format(_T("%d rows loaded from Excel."), m_excelData.size());
	//AfxMessageBox(message); // Show total number of rows loaded

	if (m_excelData.empty())
	{
		AfxMessageBox(_T("No data loaded from Excel file."));
		return false;
	}

	return true; // Return true if data was loaded successfully
}

void CUDPConnxlFormViewView::SendDataAndUpdateListBox()
{
	// Check if there's any data left to send
	if (m_currentIndex >= m_excelData.size())
	{
		AfxMessageBox(_T("No more data to send."));
		return;
	}

	const auto& rowData = m_excelData[m_currentIndex];
	CString message;
	message.Format(_T("Time: %s, X: %s, Y: %s, Z: %s"),
		rowData[0], rowData[1], rowData[2], rowData[3]);

	// Convert CString to const char*
	CT2A convertedMessage(message);
	const char* sendMessage = convertedMessage;

	int len = strlen(sendMessage);
	int result = sendto(m_socket, sendMessage, len, 0, (SOCKADDR*)&m_serverAddr, sizeof(m_serverAddr));
	if (result == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		CString errMsg;
		errMsg.Format(_T("Send failed with error: %d"), error);
		AfxMessageBox(errMsg);
	}
	else if (result != len)
	{
		AfxMessageBox(_T("Incomplete data sent."));
	}
	else
	{
		// Display in client list box
		int index = m_listClient.InsertString(0, message); // Insert at the beginning
		m_listClient.SetCurSel(index);  // Select the newly inserted item
		m_listClient.UpdateWindow();    // Ensure list box updates immediately
	}

	// Increment index to move to the next row
	m_currentIndex++;

	// Check if all data has been sent
	if (m_currentIndex >= m_excelData.size())
	{
		AfxMessageBox(_T("All data sent."));
		KillTimer(m_timer); // Stop the timer if all data has been sent
	}
}
