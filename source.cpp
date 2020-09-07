#include <iostream>
#include <Windows.h>
#include <time.h>
#include <iomanip>
#include <sstream>

void UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
    LONGLONG ll;
    SYSTEMTIME st;
    ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD)ll;
    pft->dwHighDateTime = ll >> 32;
    FileTimeToSystemTime(pft, &st);

    std::ostringstream ossMessage;
    ossMessage << st.wYear << "-"
               << std::setw(2) << std::setfill('0') << st.wMonth << "-"
               << std::setw(2) << std::setfill('0') << st.wDay << " "
               << std::setw(2) << std::setfill('0') << st.wHour << ":"
               << std::setw(2) << std::setfill('0') << st.wMinute << ":"
               << std::setw(2) << std::setfill('0') << st.wSecond << "."
               << std::setw(3) << std::setfill('0') << st.wMilliseconds;
    std::string strMessage = ossMessage.str();
    std::cout << "Last 32-bit time = " << strMessage << std::endl;
}

int main()
{
    FILETIME ft;
    const time_t time = 0x7FFFFFFF;
    UnixTimeToFileTime(time, &ft);
    return 0;
}
