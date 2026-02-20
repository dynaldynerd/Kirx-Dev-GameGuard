#pragma once

#include <afxcmn.h>
#include <afxwin.h>

class CMapData;

class CMapView : public CDialog
{
public:
  explicit CMapView(CWnd *pParent = nullptr);
  virtual ~CMapView() = default;

  void RefreshMapTree();
  bool LoadSelectedMap();

protected:
  BOOL OnInitDialog() override;

  afx_msg void OnBnClickedLoad();
  afx_msg void OnTvnSelchangedMapTree(NMHDR *pNMHDR, LRESULT *pResult);

  DECLARE_MESSAGE_MAP()

private:
  CMapData *GetSelectedMap() const;
  bool ActivateMap(CMapData *map);

  CTreeCtrl m_treeMap;
};

