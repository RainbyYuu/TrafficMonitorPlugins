#include "pch.h"
#include "QuickNoteDlg.h"
#include "AddNoteDlg.h"
#include "OptionsDlg.h"
#include <sstream>

IMPLEMENT_DYNAMIC(CQuickNoteDlg, CDialog)

CQuickNoteDlg::CQuickNoteDlg(CWnd* pParent)
	: CDialog(IDD_INI_DIALOG, pParent), m_searchTimerId(0)
{
}

CQuickNoteDlg::~CQuickNoteDlg()
{
	if (m_searchTimerId != 0) {
		KillTimer(m_searchTimerId);
	}
}

void CQuickNoteDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CATEGORY_LIST, m_categoryList);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_editSearch);
	DDX_Control(pDX, IDC_LIST_SEARCH_RESULT, m_listSearchResult);
}

BEGIN_MESSAGE_MAP(CQuickNoteDlg, CDialog)
	//ON_BN_CLICKED(IDC_BTN_ADD_CATEGORY, &CQuickNoteDlg::OnBnClickedBtnAddCategory)
	//ON_BN_CLICKED(IDC_BTN_SEARCH, &CQuickNoteDlg::OnBnClickedBtnSearch)
	ON_EN_CHANGE(IDC_EDIT_SEARCH, &CQuickNoteDlg::OnEnChangeEditSearch)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CATEGORY_LIST, &CQuickNoteDlg::OnLvnItemchangedCategoryList)
	ON_LBN_SELCHANGE(IDC_LIST_SEARCH_RESULT, &CQuickNoteDlg::OnLbnSelchangeListSearchResult)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CATEGORY_LIST, &CQuickNoteDlg::OnLvnEndlabeleditListctrl)
	ON_BN_CLICKED(IDC_BTN_OPEN, &CQuickNoteDlg::OnBnClickedBtnOpen)
	ON_BN_CLICKED(IDC_BTN_DELETE, &CQuickNoteDlg::OnBnClickedBtnDelete)
END_MESSAGE_MAP()


void CQuickNoteDlg::UpdateCategoriesList()
{
	m_categoryList.DeleteAllItems();

	int index = 0;
	for (const auto& category : g_data.m_setting_data.category)
	{
		CString name = category.second.c_str(); // ��������
		CString count;
		count.Format(L"%d", g_data.searchByCategory(category.first)); // �ʼ���

		int item = m_categoryList.InsertItem(index, name); // �����0��
		m_categoryList.SetItemText(item, 1, count);        // ���õ�1�У��ʼ�����
		index++;
	}
}

void CQuickNoteDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == SEARCH_TIMER_ID) {
		KillTimer(m_searchTimerId);
		m_searchTimerId = 0;

		// ִ������
		PerformSearch();
	}

	CDialog::OnTimer(nIDEvent);
}

void CQuickNoteDlg::LoadCategories()
{
	auto categories = g_data.m_setting_data.category;
}

void CQuickNoteDlg::PerformSearch()
{
	CString searchText;
	m_editSearch.GetWindowText(searchText);

	if (searchText.IsEmpty()) {
		ClearSearchResults();
		return;
	}

	// ִ������
	searchResults = g_data.SearchNotes(searchText.GetString());

	// �������������ʾ
	UpdateSearchResults(searchResults);
}

void CQuickNoteDlg::UpdateSearchResults(const std::vector<NoteData>& notes)
{
	m_listSearchResult.ResetContent();

	if (notes.empty()) {
		// û���������������ListBox
		ShowSearchResults(false);
		return;
	}

	// ����������
	for (const auto& note : notes) {
		std::wstring displayText = note.summary;
		if (displayText.length() > 50) {
			displayText = displayText.substr(0, 50) + L"...";
		}
		displayText += L" (" + note.updateTime + L")";

		int index = m_listSearchResult.AddString(displayText.c_str());
		m_listSearchResult.SetItemData(index, note.id);
	}

	// ����ListBox��С
	AdjustListBoxSize(static_cast<int>(notes.size()));
}

void CQuickNoteDlg::ClearSearchResults()
{
	m_listSearchResult.ResetContent();
	ShowSearchResults(false); // ����ListBox
}

void CQuickNoteDlg::InitializeLayout()
{
	// ����ԭʼ�Ի����С
	GetWindowRect(&m_originalDialogRect);

	// ����ԭʼListBoxλ�úʹ�С
	m_listSearchResult.GetWindowRect(&m_originalListRect);
	ScreenToClient(&m_originalListRect);

	// ��ȡListBox����߶�
	m_itemHeight = m_listSearchResult.GetItemHeight(0);
	if (m_itemHeight <= 0) {
		m_itemHeight = 16; // Ĭ�ϸ߶�
	}

	// �������ɼ��������ɸ�����Ҫ������
	m_maxVisibleItems = 8;

	// ��ʼ״̬������ListBox
	m_listSearchResult.ShowWindow(SW_HIDE);
}

void CQuickNoteDlg::ShowSearchResults(bool show)
{
	if (m_isListBoxVisible == show) {
		return; // ״̬û�иı䣬����Ҫ����
	}

	m_isListBoxVisible = show;

	if (show) {
		// ��ʾListBox
		m_listSearchResult.ShowWindow(SW_SHOW);
	}
	else {
		// ����ListBox�������Ի����С
		m_listSearchResult.ShowWindow(SW_HIDE);
	}
}

void CQuickNoteDlg::OnLvnEndlabeleditListctrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	if (pDispInfo->item.pszText != nullptr)
	{
		CString newText = pDispInfo->item.pszText;

		// �������ı�
		m_categoryList.SetItemText(pDispInfo->item.iItem, 0, newText);

		g_data.AddCategory(newText.GetString());

		int index = m_categoryList.InsertItem(m_categoryList.GetItemCount(), _T("˫����ӷ���"));
		m_categoryList.EditLabel(index); // ��������༭״̬
		LoadCategories();
	}

	*pResult = 0; // 1 ��ʾ�����û��޸ģ�0 ��ʾȡ��
}


void CQuickNoteDlg::AdjustListBoxSize(int itemCount)
{
	if (itemCount <= 0) {
		ShowSearchResults(false);
		return;
	}

	// ��ʾListBox
	ShowSearchResults(true);

	// ������Ѹ߶�
	int optimalHeight = CalculateOptimalListHeight(itemCount);

	// ����ListBox��С
	CRect newListRect = m_originalListRect;
	newListRect.bottom = newListRect.top + optimalHeight;

	m_listSearchResult.MoveWindow(&newListRect);
}

int CQuickNoteDlg::CalculateOptimalListHeight(int itemCount)
{
	// ������ʾ������
	int visibleItems = min(itemCount, m_maxVisibleItems);

	// ����߶ȣ����� * ��߶� + �߿�
	int height = visibleItems * m_itemHeight + 4; // +4 Ϊ�߿�߶�

	// ��С�߶�
	const int minHeight = m_itemHeight + 4;
	height = max(height, minHeight);

	return height;
}

BOOL CQuickNoteDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_categoryList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_categoryList.InsertColumn(0, L"����", LVCFMT_LEFT, 150);
	m_categoryList.InsertColumn(1, L"�ʼ���", LVCFMT_LEFT, 150);
	m_categoryList.ModifyStyle(0, LVS_REPORT | LVS_EDITLABELS);


	// ��ʼ���Ի���
	UpdateCategoriesList();

	int index = m_categoryList.InsertItem(m_categoryList.GetItemCount(), _T("˫����ӷ���"));
	m_categoryList.EditLabel(index); // ��������༭״̬

	InitializeLayout();

	return TRUE;
}

void CQuickNoteDlg::OnOK()
{
	CDialog::OnOK();
}

void CQuickNoteDlg::OnEnChangeEditSearch()
{
	// ������ж�ʱ�������У���ȡ����
	if (m_searchTimerId != 0) {
		KillTimer(m_searchTimerId);
		m_searchTimerId = 0;
	}

	// ��ȡ�����ı�
	CString searchText;
	m_editSearch.GetWindowText(searchText);

	// ���������Ϊ�գ�����������
	if (searchText.IsEmpty()) {
		ClearSearchResults();
		return;
	}

	// �����ӳ�������ʱ������������
	m_searchTimerId = SetTimer(SEARCH_TIMER_ID, SEARCH_DELAY, nullptr);
	if (m_searchTimerId != 0) {
		KillTimer(m_searchTimerId);
		m_searchTimerId = 0;
	}

	m_editSearch.GetWindowText(searchText);

	if (searchText.IsEmpty()) {
		ClearSearchResults();
		return;
	}

	m_searchTimerId = SetTimer(SEARCH_TIMER_ID, SEARCH_DELAY, nullptr);
}

void CQuickNoteDlg::OnLvnItemchangedCategoryList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

void CQuickNoteDlg::OnLbnSelchangeListSearchResult()
{
	int selectedIndex = m_listSearchResult.GetCurSel();
	if (selectedIndex != LB_ERR) {
		// ��ȡѡ�бʼǵ�ID
		int noteId = searchResults[selectedIndex].id;
		CEditNoteDlg dlg(this);

		dlg.m_strNote = searchResults[selectedIndex].text.c_str(); // ����ԭʼ�ʼ�����

		vector<std::pair<int, std::wstring>>ca = g_data.m_setting_data.category;
		int cid = searchResults[selectedIndex].categoryId;
		for (int i = 0; i < ca.size(); i++)
		{
			if (ca[i].first == cid)
			{
				dlg.m_strCategory = ca[i].second.c_str();
				break;
			}
		}

		if (dlg.DoModal() == IDOK)
		{
			// �û������ȷ�����������ݿ�
			g_data.UpdateNoteTextById(noteId, dlg.m_strNote.GetString(), searchResults[selectedIndex].categoryId);
		}
	}
}

void CQuickNoteDlg::OnBnClickedBtnOpen()
{
	int selectedIndex = m_categoryList.GetNextItem(-1, LVNI_SELECTED);
	if (selectedIndex == -1)
	{
		AfxMessageBox(_T("����ѡ��һ������"));
		return;
	}

	// ��ȡ����ID�����豣����ItemData�У�
	int categoryId = g_data.m_setting_data.category[selectedIndex].first;

	// ���¶Ի��򲢴��� categoryId
	COptionsDlg dlg(this, categoryId);
	EndDialog(IDOK);
	dlg.DoModal();
}

void CQuickNoteDlg::OnBnClickedBtnDelete()
{
	int selectedIndex = m_categoryList.GetNextItem(-1, LVNI_SELECTED);
	if (selectedIndex == -1)
	{
		AfxMessageBox(_T("����ѡ��һ������"));
		return;
	}
	else if (selectedIndex >= g_data.m_setting_data.category.size())
	{
		AfxMessageBox(_T("ѡ�������Ч"));
		return;
	}
	else
	{
		int result = AfxMessageBox(_T("ȷ��ɾ���÷��༰�������ʼ���"), MB_YESNO | MB_ICONQUESTION);

		if (result == IDYES)
		{
			int categoryId = g_data.m_setting_data.category[selectedIndex].first;
			g_data.DeleteByCategoryId(categoryId);
			auto& categories = g_data.m_setting_data.category; // ����category��һ��std::vector<std::pair<int, std::wstring>>
			for (auto it = categories.begin(); it != categories.end(); ++it)
			{
				if (categoryId == it->first) // ����Ƿ��ҵ���ƥ���categoryId
				{
					categories.erase(it); // ʹ�õ�������ɾ��Ԫ��
					break; // �ҵ���ɾ�����˳�ѭ��
				}
			}
			UpdateCategoriesList();
		}
		else return;
	}
	
	int index = m_categoryList.InsertItem(m_categoryList.GetItemCount(), _T("˫����ӷ���"));
	m_categoryList.EditLabel(index); // ��������༭״̬

}
