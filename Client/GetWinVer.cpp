#include "Client_PCH.h"

BOOL GetWinVersion(char *szVersion)
{
#ifdef PLATFORM_WINDOWS
   // Windows 구현부 - 단순화된 버전
   OSVERSIONINFOEX osvi;
   BOOL bOsVersionInfoEx;

   // OSVERSIONINFOEX 구조체를 사용하여 GetVersionEx 호출을 시도한다.
   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
   {
      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
         return FALSE;
   }

   char szTemp[512];

   switch (osvi.dwPlatformId)
   {
      // Windows NT 제품군인지 확인한다.
      case VER_PLATFORM_WIN32_NT:
         if ( osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0 )
            strcat (szVersion, "Windows 10/11");
         else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3 )
            strcat (szVersion, "Windows 8.1");
         else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 )
            strcat (szVersion, "Windows 8");
         else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 )
            strcat (szVersion, "Windows 7");
         else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
            strcat (szVersion, "Windows Vista");
         else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
            strcat (szVersion, "Windows XP");
         else
            sprintf(szVersion, "Windows NT %d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
         break;

      // Windows 95 제품군인지 확인한다.
      case VER_PLATFORM_WIN32_WINDOWS:
         if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
             strcpy(szVersion, "Windows 95");
         else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
             strcpy(szVersion, "Windows 98");
         else if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
             strcpy(szVersion, "Windows ME");
         else
             strcpy(szVersion, "Windows 9x");
         break;

      default:
         strcpy(szVersion, "Unknown Windows");
         break;
   }

   // 빌드 번호가 있으면 추가한다.
   if (osvi.dwBuildNumber > 0)
   {
      sprintf(szTemp, " (Build %d)", osvi.dwBuildNumber & 0xFFFF);
      strcat(szVersion, szTemp);
   }

   return TRUE;

#else
   // Windows가 아닌 플랫폼
   strcpy(szVersion, "Non-Windows Platform");
   return TRUE;
#endif
}