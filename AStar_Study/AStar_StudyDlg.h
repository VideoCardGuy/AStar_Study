
// AStar_StudyDlg.h : ͷ�ļ�
//

#pragma once


// CAStar_StudyDlg �Ի���
class CAStar_StudyDlg : public CDialogEx
{
// ����
public:
	CAStar_StudyDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ASTAR_STUDY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnStnClickedStaticMap();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
