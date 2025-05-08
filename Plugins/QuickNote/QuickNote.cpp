#include "pch.h"
#include "QuickNote.h"
#include "DataManager.h"
#include "OptionsDlg.h"
#pragma comment(lib, "ole32.lib")

CQuickNote CQuickNote::m_instance;

CQuickNote::CQuickNote()
{
    // ��ʼ����Ա����
    //m_data_manager = std::make_unique<CDataManager>();
    //g_data.LoadConfig();  // ��������

}

CQuickNote::~CQuickNote()
{
    //g_data.SaveConfig();  // ��������
}

CQuickNote& CQuickNote::Instance()
{
    return m_instance;
}

IPluginItem* CQuickNote::GetItem(int index)
{
    switch (index)
    {
    case 0:
        return &m_item;
    default:
        break;
    }
    return nullptr;
}

const wchar_t* CQuickNote::GetTooltipInfo()
{
    return m_tooltip_info.c_str();
}

void CQuickNote::DataRequired()
{

}

const wchar_t* CQuickNote::GetInfo(PluginInfoIndex index)
{
    static CString str;
    switch (index)
    {
    case TMI_NAME:
        return L"��ݱʼ�";
    case TMI_DESCRIPTION:
        return L"This is a tool for notes";
    case TMI_AUTHOR:
        return L"rain";
    case TMI_COPYRIGHT:
        return L"Copyright (C) by Yu Xiaoyu 2025";
    case ITMPlugin::TMI_URL:
        return L"NULL";
        break;
    case TMI_VERSION:
        return L"1.00";
    default:
        break;
    }
    return L"";
}

void CQuickNote::OnExtenedInfo(ExtendedInfoIndex index, const wchar_t* data)
{
    switch (index)
    {
    case ITMPlugin::EI_CONFIG_DIR:
        //�������ļ���ȡ����
        g_data.LoadConfig();
        //����һ����ʱ��
        /*SetTimer(NULL, 1265, 50, [](HWND, UINT, UINT_PTR, DWORD) {
            m_instance.DoDataAcquire();
            });*/

        break;
    default:
        break;
    }
}

void* CQuickNote::GetPluginIcon()
{
    return g_data.GetIcon(IDI_ICON1);
}

int CQuickNote::GetCommandCount()
{
    return 3;  // �޸�Ϊʹ��ö��ֵ��������������
}

const wchar_t* CQuickNote::GetCommandName(int command_index)
{
    CommandIndex index = static_cast<CommandIndex>(command_index);
    switch (index)
    {
    case CQuickNote::CMD_NEW:
        return g_data.StringRes(IDI_ICON6);
    case CQuickNote::CMD_SAVE:
        return g_data.StringRes(IDI_ICON7);
    case CQuickNote::CMD_CLEAR:
        return g_data.StringRes(IDI_ICON2);
    case CQuickNote::CMD_CMD_MODE:
        return g_data.StringRes(IDI_ICON3);
    case CQuickNote::CMD_HIDE:
        return g_data.StringRes(IDI_ICON4);
    case CQuickNote::CMD_MAX:
        return g_data.StringRes(IDI_ICON5);
    case CQuickNote::CMD_AUTO_PAGE:
        return g_data.StringRes(IDI_ICON1);
    }
    return nullptr;
}

void* CQuickNote::GetCommandIcon(int command_index)
{
    CommandIndex index = static_cast<CommandIndex>(command_index);
    switch (index)
    {
    case CQuickNote::CMD_NEW:
        return g_data.GetIcon(IDI_ICON6);
    case CQuickNote::CMD_SAVE:
        return g_data.GetIcon(IDI_ICON7);
    case CQuickNote::CMD_CLEAR:
        return g_data.GetIcon(IDI_ICON2);
    case CQuickNote::CMD_CMD_MODE:
        return g_data.GetIcon(IDI_ICON3);
    case CQuickNote::CMD_HIDE:
        return g_data.GetIcon(IDI_ICON4);
    case CQuickNote::CMD_MAX:
        return g_data.GetIcon(IDI_ICON5);
    case CQuickNote::CMD_AUTO_PAGE:
        return g_data.GetIcon(IDI_ICON1);
    }
    return nullptr;
}

void CQuickNote::OnPluginCommand(int command_index, void* hWnd, void* para)
{
    CommandIndex index = static_cast<CommandIndex>(command_index);
    switch (index)
    {
    case CMD_NEW:
        g_data.AddNote(L"New Note");
        break;
    case CMD_SAVE:
        g_data.SaveConfig();
        break;
    case CMD_CLEAR:
        g_data.ClearNotes();
        break;
    case CMD_CMD_MODE:
        m_item.ProcessCommand(L"/cmd");
        break;
    case CMD_HIDE:
        // ������������
        break;
    case CMD_MAX:
        // �����������
        break;
    case CMD_AUTO_PAGE:
        // �����Զ���ҳ����
        break;
    default:
        break;
    }
}

//int CQuickNote::IsCommandChecked(int command_index)
//{
//    CommandIndex index = static_cast<CommandIndex>(command_index);
//    switch (index)
//    {
//    case CMD_AUTO_PAGE:
//        return g_data.m_setting_data.auto_read;
//    case CMD_HIDE:
//        return g_data.m_boss_key_pressed;
//    }
//    return 0;
//}

ITMPlugin::OptionReturn CQuickNote::ShowOptionsDialog(void* hParent)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    CWnd* pParent = CWnd::FromHandle((HWND)hParent);
    COptionsDlg dlg(pParent);
    dlg.m_data = g_data.m_setting_data;
    
    if (dlg.DoModal() == IDOK)
    {
        //g_data.m_setting_data = dlg.m_data;
        g_data.SaveConfig();
        return ITMPlugin::OR_OPTION_CHANGED;
    }
    return ITMPlugin::OR_OPTION_UNCHANGED;
    /*AFX_MANAGE_STATE(AfxGetStaticModuleState());
    COptionsDlg dlg(m_data_manager.get());
    if (dlg.DoModal() == IDOK)
    {
        return OR_OPTION_CHANGED;
    }
    return OR_OPTION_UNCHANGED;*/
}

ITMPlugin* TMPluginGetInstance()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return &CQuickNote::Instance();
}
