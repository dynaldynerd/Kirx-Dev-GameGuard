#pragma once

#include <afxcmn.h>
#include <afxdlgs.h>
#include <afxwin.h>

class CMapTab : public CPropertyPage
{
public:
  CMapTab();
  ~CMapTab() override;

  void UpdateTab();

protected:
  void DoDataExchange(CDataExchange *pDX) override;
  BOOL OnSetActive() override;

  afx_msg void OnButtonMapchange();

  DECLARE_MESSAGE_MAP()

public:
  HTREEITEM m_hMap[60];
  CTreeCtrl m_trMap;
};

class CServerTab : public CPropertyPage
{
public:
  CServerTab();
  ~CServerTab() override;

  void UpdateServerTab();

protected:
  void DoDataExchange(CDataExchange *pDX) override;
  BOOL OnInitDialog() override;
  BOOL OnSetActive() override;

  afx_msg void OnButtonUpdateserver();

  DECLARE_MESSAGE_MAP()

public:
  CTreeCtrl m_trServer;
};

class CIPXTab : public CPropertyPage
{
public:
  CIPXTab();
  ~CIPXTab() override;

  void UpdateTab();

protected:
  void DoDataExchange(CDataExchange *pDX) override;
  BOOL OnSetActive() override;
  BOOL OnKillActive() override;

  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnButtonUpdatelistsize();

  DECLARE_MESSAGE_MAP()

public:
  bool m_bUpdate;
  int m_edAccretiaNum;
  int m_edBellatoNum;
  int m_edCoraNum;
};

class CTCPTab : public CPropertyPage
{
public:
  CTCPTab();
  ~CTCPTab() override;

  void UpdateTab();

protected:
  void DoDataExchange(CDataExchange *pDX) override;
  BOOL OnSetActive() override;
  BOOL OnKillActive() override;

  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnBUTTONMainThreadControlApply();
  afx_msg void OnButtonUpdatelistsize();

  DECLARE_MESSAGE_MAP()

public:
  bool m_bUpdate;
  CButton m_btReLoadFile;
  unsigned int m_edConnSocketNum;
  unsigned int m_edOddMsg;
  int m_edConnectUserNum;
  int m_edTotalUser;
  int m_edMainThreadControlSleepIgnore;
  int m_edMainThreadControlSleepTerm;
  int m_edMainThreadControlSleepValue;
  CString m_edChkLoopTime;
  CString m_edChkPercent;
  CString m_edNetLoopTime;
  CString m_edNetPercent;
  CString m_edObjLoopTime;
  CString m_edObjPercent;
  int m_edSaveFail;
};

class CObjectTab : public CPropertyPage
{
public:
  CObjectTab();
  ~CObjectTab() override;

  void UpdateTab();

protected:
  void DoDataExchange(CDataExchange *pDX) override;
  BOOL OnSetActive() override;
  BOOL OnKillActive() override;

  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnButtonUpdate();
  afx_msg void OnButtonDestory();
  afx_msg void OnButtonSearch();
  afx_msg void OnButtonAuto();

  DECLARE_MESSAGE_MAP()

public:
  CButton m_btAuto;
  CTreeCtrl m_trObject;
  bool m_bAutoUpdate;
};
