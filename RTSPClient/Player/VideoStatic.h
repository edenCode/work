#if !defined(AFX_VIDEOSTATIC_H__D8A10BC9_8252_4C21_940C_8D667E1BA02D__INCLUDED_)
#define AFX_VIDEOSTATIC_H__D8A10BC9_8252_4C21_940C_8D667E1BA02D__INCLUDED_


/////////////////////////////////////////////////////////////////////////////
// VideoStatic window
class CVideoStatic : public CStatic
{
// Construction
public:
	CVideoStatic();
	virtual ~CVideoStatic();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VideoStatic)
	//}}AFX_VIRTUAL
	virtual BOOL Create(CWnd *pParentWnd);
	virtual int	OpenChannel(const char* url);
protected:
	//{{AFX_MSG(VideoStatic)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	long	m_hPlayer;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOSTATIC_H__D8A10BC9_8252_4C21_940C_8D667E1BA02D__INCLUDED_)
