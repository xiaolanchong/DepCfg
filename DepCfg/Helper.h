#ifndef _HELPER_H_
#define _HELPER_H_

#include  <string>

//! TRACE �� ������������ ���������� ����������
//! \param szFormat ������, ��������� � trace
//void TRACE(LPCTSTR szFormat, ...);

//! ����� ���������� �� ������
//! \param hRes �������� ��������� ������
void AtlTraceRelease(HRESULT hRes);

//! �������������� ������ � ������������ ����������� ����������
//! \param szFormat ������ ��� ��������������
std::wstring Formats(LPCTSTR szFormat, ...);

#endif