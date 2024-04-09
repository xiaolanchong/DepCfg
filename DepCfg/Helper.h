#ifndef _HELPER_H_
#define _HELPER_H_

#include  <string>

//! TRACE на произвольное количество аргументов
//! \param szFormat строка, выводимая в trace
//void TRACE(LPCTSTR szFormat, ...);

//! Вывод информации об ошибке
//! \param hRes название полученой ошибки
void AtlTraceRelease(HRESULT hRes);

//! форматирование строки с произвольным количеством аргументов
//! \param szFormat строка для форматирования
std::wstring Formats(LPCTSTR szFormat, ...);

#endif