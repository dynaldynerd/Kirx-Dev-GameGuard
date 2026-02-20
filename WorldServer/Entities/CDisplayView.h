#pragma once

#include <afxwin.h>

class CDisplayView : public CDialog
{
public:
  explicit CDisplayView(CWnd *pParent = nullptr);
  virtual ~CDisplayView() = default;

  void GetDrawableRect(CRect *rcOut);
  void SetExtendMode(int bExtend);

protected:
  virtual BOOL OnInitDialog() override;

  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();
  afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
  afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

  DECLARE_MESSAGE_MAP()

public:
  CPoint m_ptLBtn;
  int m_nVerRange;
  int m_nHorzRange;
  int m_nVertPos;
  int m_nHorzPos;
  int m_nVerPage;
  int m_nHorzPage;
};
