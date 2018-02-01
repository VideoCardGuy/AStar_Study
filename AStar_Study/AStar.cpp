#include "stdafx.h"
#include "AStar.h"

#define _SELF L"AStar.cpp"

int CAStar::Dirs[4][2] = { { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 } };//����
int CAStar::MoreDirs[8][2] = { { -1, -1 }, { 0, -1 }, { 1, -1 }, { -1, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } };

CAStar::CAStar()
{

}

CAStar::~CAStar()
{
}

VOID CAStar::InitMapArray(LPCWSTR pwszPath)
{
	ULONG_PTR m_pGdiToken;
	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_pGdiToken, &m_gdiplusStartupInput, NULL);
	Gdiplus::Bitmap * pImage = new Gdiplus::Bitmap(pwszPath);

	int nWidth = pImage->GetWidth();
	int nHeight = pImage->GetHeight();

	for (int i = 0; i < nWidth; ++i)
	{
		for (int j = 0; j < nHeight; ++j)
		{
			Gdiplus::Color cor;
			pImage->GetPixel(i, j, &cor);
			if (cor.GetR() == 0xFF && cor.GetG() == 0xFF && cor.GetB() == 0xFF)
			{
				visited[i][j] = COLOR_WHITE;
			}
			else
			{
				visited[i][j] = COLOR_BLACK;
				static Point poit;
				poit.X = (float)i;
				poit.Y = (float)j;
				MainLineList.push_back(poit);
			}
		}
	}
	
	delete pImage;
	pImage = NULL;
	Gdiplus::GdiplusShutdown(m_pGdiToken);
	m_pGdiToken = NULL;
}

DWORD CAStar::GetDistance(const Point* p1, const Point* p2)
{
	return (DWORD)sqrt((float)((p1->X - p2->X) * (p1->X - p2->X)) + (float)((p1->Y - p2->Y) * (p1->Y - p2->Y)));
}

BOOL CAStar::IsExistence(int x, int y)
{
	if (x < 0 || y < 0 || x >= MAX_PIC_X || y >= MAX_PIC_Y)
	{
		return false;
	}
	return visited[x][y] != NULL;
}

int CAStar::Heuristic(const Chess temp)
{
	//2�ַ�ʽ�ļ���,A*�Ĺؼ�������H��ֵ
	return (int)sqrt((float)(abs(temp.x - EndPoint.X) + abs(temp.y - EndPoint.Y))) ;
	//return  (abs(temp.x - Finish_X) + abs(temp.y - Finish_Y)) * 1;
	//return (int)sqrt((float)((temp.x - EndPoint.X) * (temp.x - EndPoint.X)) + (float)((temp.y - StartPoint.X) * (temp.y - EndPoint.Y)));
}

BOOL CAStar::GetRecentlyPoint(IN Chess* pChess, OUT UINT & uRetIndex)
{
	DWORD dwMinDis = MAXINT;
	UINT uIndex = -1;
	_Point StartPoint;

	StartPoint.X = (float)pChess->x;
	StartPoint.Y = (float)pChess->y;

	for (UINT i = 0; i < MainLineList.size(); ++i)
	{
		static _Point pt;
		pt = MainLineList.at(i);
		DWORD dwDis = GetDistance(&StartPoint, &pt);
		if (dwDis < dwMinDis)
		{
			dwMinDis = dwDis;
			uIndex = i;
		}
	}

	uRetIndex = uIndex;
	return uIndex != -1 ? TRUE : FALSE;
}

double CAStar::mult(Point& a, Point& b, Point& c)
{
	return (a.X - c.X)*(b.Y - c.Y) - (b.X - c.X)*(a.Y - c.Y);
}

bool CAStar::intersect(Point& aa, Point& bb, Point& cc, Point& dd)
{
	if (max(aa.X, bb.X) < min(cc.X, dd.X))
	{
		return false;
	}
	if (max(aa.Y, bb.Y) < min(cc.Y, dd.Y))
	{
		return false;
	}
	if (max(cc.X, dd.X) < min(aa.X, bb.X))
	{
		return false;
	}
	if (max(cc.Y, dd.Y) < min(aa.Y, bb.Y))
	{
		return false;
	}
	if (mult(cc, bb, aa)*mult(bb, dd, aa) < 0)
	{
		return false;
	}
	if (mult(aa, dd, cc)*mult(dd, bb, cc) < 0)
	{
		return false;
	}
	return true;
}

BOOL CAStar::IsCut(IN Chess* pStart, IN Chess* pEnd)
{
	Point PointA = pStart->GetPoint();					//���
	Point PointB = pEnd->GetPoint();					//�յ�

	int nLineAB = GetDistance(&PointA, &PointB);

	if (nLineAB >= MAX_SEARCHPATH_DIS)
		return TRUE;


	UINT uStartIndex = 0;
	if (!GetRecentlyPoint(pStart, uStartIndex))
		return FALSE;

	UINT uEndIndex = 0;
	if (!GetRecentlyPoint(pEnd, uEndIndex))
		return FALSE;

	Point PointC = MainLineList.at(uStartIndex);		//A����ڵ�����ĵ�
	Point PointD = MainLineList.at(uEndIndex);			//B����ڵ�����ĵ�


	int nLineAC = GetDistance(&PointA, &PointC);
	int nLineBC = GetDistance(&PointB, &PointC);

	int nLineBD = GetDistance(&PointB, &PointD);
	int nLineAD = GetDistance(&PointA, &PointD);

	int nLineCD = GetDistance(&PointC, &PointD);

	//�жϡ�ACB�Ƿ���� �жϡ�ADB�Ƿ����
	//�ж�AC^2 + AB^2 > BC^ ���Ƕ۽� =��ֱ��  <�����
	if (nLineAC * nLineAC + nLineBC * nLineBC > nLineAB * nLineAB &&
		nLineBD * nLineBD + nLineAD * nLineAD > nLineAB * nLineAB)
	{
		if (intersect(PointA, PointB, PointC, PointD))
		{
			return TRUE;
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CAStar::ConvertPoint(IN Point & MapPoint OUT, Point & GamePoint)
{
	float nDValue_X = (StartPoint_Convert.X - MapPoint.X) * -1;
	float nDValue_Y = StartPoint_Convert.Y - MapPoint.Y;

	GamePoint.X = ((float)GamePoint_Convert.X + nDValue_X * 5.6f);
	GamePoint.Y = ((float)GamePoint_Convert.Y + nDValue_Y * 5.6f);

	return TRUE;
}

VOID CAStar::WritePointFile(std::vector<Point> & vlst)
{
	CStdioFile CSF;
	static CString str;
	static _Point pt;

	if (CSF.Open(L"Point.inf", CFile::modeCreate | CFile::modeReadWrite))
	{
		CSF.WriteString(L"");
		INT i = -1;
		for (i = vlst.size() - 1; i >= 0; i -= 9)
		{
			ConvertPoint(vlst.at(i), pt);
			str.Format(L"%d,%d\r\n", pt.X, pt.Y);
			CSF.WriteString(str);
		}

		//д���յ�����
		if (i != -9)
		{
			ConvertPoint(vlst.at(0), pt);
			str.Format(L"%d,%d\r\n", pt.X, pt.Y);
			CSF.WriteString(str);
		}

		CSF.Close();
	}
}

BOOL CAStar::BeginAStar(Chess* pStart, Chess* pEnd)
{
	std::vector<_Point> vlst;
	_Point lstPoint;

	if (visited[pStart->x][pStart->y] == COLOR_WHITE)//����ǰ׵��ʱ��
	{
		if (visited[pEnd->x][pEnd->y] == COLOR_WHITE)//���յ�Ҳ�ǰ׵�
		{
			if (IsCut(pStart, pEnd))//�ж��Ƿ���Ҫ������ɫ����·
			{
				UINT uStartIndex = 0;
				if (!GetRecentlyPoint(pStart, uStartIndex))
					return FALSE;

				UINT uEndIndex = 0;
				if (!GetRecentlyPoint(pEnd, uEndIndex))
					return FALSE;

				static Chess StartChess;
				StartChess.x = (int)MainLineList.at(uStartIndex).X;
				StartChess.y = (int)MainLineList.at(uStartIndex).Y;
				StartChess.h = Heuristic(StartChess);
				StartChess.g = 0;
				StartChess.f = StartChess.h + StartChess.g;
				StartChess.Root = NULL;

				//�ȵ��ڵ�
				Chess* pNode1 = AStar(pStart, &StartChess, COLOR_OTHER);//��->��
				while (!IsBadCodePtr(FARPROC(pNode1)) && pNode1 != NULL && pNode1->Root != NULL)
				{
					vlst.push_back(pNode1->GetPoint());
					//dlg->GetDC()->SetPixel(rect.left + pNode1->x, rect.top + pNode1->y, RGB(255, 0, 0));
					pNode1 = pNode1->Root;
				}

				StartChess.x = (int)MainLineList.at(uStartIndex).X;
				StartChess.y = (int)MainLineList.at(uStartIndex).Y;
				StartChess.h = Heuristic(StartChess);
				StartChess.g = 0;
				StartChess.f = StartChess.h + StartChess.g;
				StartChess.Root = NULL;

				static Chess EndChess;
				EndChess.x = (int)MainLineList.at(uEndIndex).X;
				EndChess.y = (int)MainLineList.at(uEndIndex).Y;
				EndChess.h = Heuristic(EndChess);
				EndChess.g = 0;
				EndChess.f = EndChess.h + EndChess.g;
				EndChess.Root = NULL;

				Chess* pNode2 = AStar(&StartChess, &EndChess, COLOR_WHITE);//��->��
				while (!IsBadCodePtr(FARPROC(pNode2)) && pNode2 != NULL && pNode2->Root != NULL)
				{
					vlst.push_back(pNode2->GetPoint());
					//dlg->GetDC()->SetPixel(rect.left + pNode2->x, rect.top + pNode2->y, RGB(255, 0, 0));
					pNode2 = pNode2->Root;
				}

				//�ٵ��׵�
				EndChess.x = (int)MainLineList.at(uEndIndex).X;
				EndChess.y = (int)MainLineList.at(uEndIndex).Y;
				EndChess.h = Heuristic(EndChess);
				EndChess.g = 0;
				EndChess.f = EndChess.h + EndChess.g;
				EndChess.Root = NULL;

				Chess* pNode3 = AStar(&EndChess, pEnd, COLOR_OTHER);//��->��
				while (!IsBadCodePtr(FARPROC(pNode3)) && pNode3 != NULL && pNode3->Root != NULL)
				{
					vlst.push_back(pNode3->GetPoint());
					//dlg->GetDC()->SetPixel(rect.left + pNode3->x, rect.top + pNode3->y, RGB(255, 0, 0));
					pNode3 = pNode3->Root;
				}

			}
			else//ֱ�Ӱ׵㵽�׵�A*
			{
				Chess* pNode = AStar(pStart, pEnd, COLOR_BLACK);
				while (!IsBadCodePtr(FARPROC(pNode)) && pNode != NULL && pNode->Root != NULL)
				{
					//dlg->GetDC()->SetPixel(rect.left + pNode->x, rect.top + pNode->y, RGB(255, 0, 0));
					pNode = pNode->Root;
				}
			}
		}
		else if (visited[pEnd->x][pEnd->y] == COLOR_BLACK)//���յ��Ǻ�ɫ��ʱ��
		{
			UINT uIndex = 0;
			if (!GetRecentlyPoint(pStart, uIndex))
				return FALSE;

			static Chess StartChess;
			StartChess.x = (int)MainLineList.at(uIndex).X;
			StartChess.y = (int)MainLineList.at(uIndex).Y;
			StartChess.h = Heuristic(StartChess);
			StartChess.g = 0;
			StartChess.f = StartChess.h + StartChess.g;
			StartChess.Root = NULL;
			Chess* pNode1 = AStar(pStart, &StartChess, COLOR_OTHER);

			while (!IsBadCodePtr(FARPROC(pNode1)) && pNode1 != NULL && pNode1->Root != NULL)
			{
				//dlg->GetDC()->SetPixel(rect.left + pNode1->x, rect.top + pNode1->y, RGB(255, 0, 0));
				pNode1 = pNode1->Root;
			}

			StartChess.x = (int)MainLineList.at(uIndex).X;
			StartChess.y = (int)MainLineList.at(uIndex).Y;
			StartChess.h = Heuristic(StartChess);
			StartChess.g = 0;
			StartChess.f = StartChess.h + StartChess.g;
			StartChess.Root = NULL;
			Chess* pNode2 = AStar(&StartChess, pEnd, COLOR_WHITE);
			while (!IsBadCodePtr(FARPROC(pNode2)) && pNode2 != NULL && pNode2->Root != NULL)
			{
				//dlg->GetDC()->SetPixel(rect.left + pNode2->x, rect.top + pNode2->y, RGB(255, 0, 0));
				pNode2 = pNode2->Root;
			}
		}
	}
	else if (visited[pStart->x][pStart->y] == COLOR_BLACK)//����Ǻڵ��ʱ��
	{
		if (visited[pEnd->x][pEnd->y] == COLOR_WHITE)//���յ��ǰ׵�
		{
			UINT uIndex = 0;
			if (!GetRecentlyPoint(pEnd, uIndex))
				return FALSE;

			static Chess StartChess;
			StartChess.x = (int)MainLineList.at(uIndex).X;
			StartChess.y = (int)MainLineList.at(uIndex).Y;
			StartChess.h = Heuristic(StartChess);
			StartChess.g = 0;
			StartChess.f = StartChess.h + StartChess.g;
			StartChess.Root = NULL;
			Chess* pNode1 = AStar(pStart, &StartChess, COLOR_WHITE);


			while (!IsBadCodePtr(FARPROC(pNode1)) && pNode1 != NULL && pNode1->Root != NULL)
			{
				//dlg->GetDC()->SetPixel(rect.left + pNode1->x, rect.top + pNode1->y, RGB(255, 0, 0));
				pNode1 = pNode1->Root;
			}

			StartChess.x = (int)MainLineList.at(uIndex).X;
			StartChess.y = (int)MainLineList.at(uIndex).Y;
			StartChess.h = Heuristic(StartChess);
			StartChess.g = 0;
			StartChess.f = StartChess.h + StartChess.g;
			StartChess.Root = NULL;
			Chess* pNode2 = AStar(&StartChess, pEnd, COLOR_OTHER);
			while (!IsBadCodePtr(FARPROC(pNode2)) && pNode2 != NULL && pNode2->Root != NULL)
			{
				//dlg->GetDC()->SetPixel(rect.left + pNode2->x, rect.top + pNode2->y, RGB(255, 0, 0));
				pNode2 = pNode2->Root;
			}
		}
		else if (visited[pEnd->x][pEnd->y] == COLOR_BLACK)//���յ�Ҳ�Ǻ�ɫ��ʱ��
		{
			Chess* pNode = AStar(pStart, pEnd, COLOR_WHITE);
			while (!IsBadCodePtr(FARPROC(pNode)) && pNode != NULL && pNode->Root != NULL)
			{
				vlst.push_back(pNode->GetPoint());
				//dlg->GetDC()->SetPixel(rect.left + pNode->x, rect.top + pNode->y, RGB(255, 0, 0));
				pNode = pNode->Root;
			}
		}
	}

	WritePointFile(vlst);
	return TRUE;
}

Chess* CAStar::AStar(Chess* pStart, Chess* pEnd, DWORD dwDisorderColor)
{
	//Color color(255,255,0,0);
	DWORD dwStartTick = ::GetTickCount();
	static DWORD BackupVistied[MAX_PIC_X][MAX_PIC_Y] = { 0 };
	static std::priority_queue<Chess> que;
	static Chess RetNode;

	memcpy(BackupVistied, visited, sizeof(BackupVistied));
	memset(&RetNode, 0, sizeof(RetNode));

	que.push(*pStart);

	BOOL bInit = FALSE;
	while (!que.empty())//�����в�Ϊ��
	{
		Chess * temp1 = new Chess;
		deletelist.push_back(temp1);
		*temp1 = que.top();//��ȡf���Ľڵ�
		que.pop();//��ջ
		if (visited[temp1->x][temp1->y] == dwDisorderColor && bInit)
		{
			continue;
		}

		visited[temp1->x][temp1->y] = dwDisorderColor;//����·������Ϊ��ͨ.�����൱�ڼ��뵽�ر��б�
		bInit = TRUE;
		if (temp1->x == pEnd->x && temp1->y == pEnd->y)//�Ƿ񵽴��յ���
		{
			que.push(*temp1);
			RetNode = *temp1;
			goto lb_Exit;
		}
		for (int i = 0; i < 4; i++)
		{
			//4������
			Chess * temp2 = new Chess;
			deletelist.push_back(temp2);
			temp2->x = temp1->x + Dirs[i][0];
			temp2->y = temp1->y + Dirs[i][1];
			//��ȡf���Ľڵ������һ������.�жϾ������ⷽ����Ƿ�������̣�����·��ͨ��
			if (IsExistence(temp2->x, temp2->y) && visited[temp2->x][temp2->y] != dwDisorderColor /*&& !TraverseVlist(temp2)*/)
			{
				temp2->g = temp1->g + 23;
				temp2->h = Heuristic(*temp2);
				temp2->f = temp2->g + temp2->h;
				temp2->Root = temp1;//��¼���ڵ���������������·��
				que.push(*temp2);
			}
		}
	}

lb_Exit:;
	//��ն���
	while (!que.empty())
		que.pop();

	memcpy(visited, BackupVistied, sizeof(visited));

	WCHAR wszText[1024] = { 0 };
	wsprintfW(wszText, L"���ѵ�ʱ����:%dms", ::GetTickCount() - dwStartTick);
	::MessageBoxW(NULL, wszText, L"Prompt", NULL);
	return &RetNode;
}