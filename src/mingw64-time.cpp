#include <windows.h>

#include <string>
#include <math.h>

double toSeconds(const FILETIME &f)
{
    double x=f.dwLowDateTime+ldexp(f.dwHighDateTime,32);
    return x*1e-7;
}

int main(int argc, char *argv[])
{
    if(argc<2){
        printf("Usage : mingw64-time program [arg0 arg1 ...]");
        return 1;
    }

    std::string commandLine=argv[1];
    for(int i=2; i<argc; i++){
        commandLine+=" ";
        commandLine+=argv[i];
    }

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    BOOL ret=CreateProcess(
        /*lpApplicationName*/ NULL,
        const_cast<char*>(commandLine.c_str()),
        /*lpProcessAttributes*/ NULL,
        /*lpThreadAttributes*/ NULL,
        /*bInheritHandles*/ TRUE,
        /*dwCreationFlags*/ 0,
        /*lpEnvironment*/ 0,
        /*lpCurrentDirectory*/ NULL,
        /*lpStartupInfo*/ &si,
        /*lpProcessInformation*/ &pi
      );
    if(!ret){
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return 1;
    }

    HANDLE hProcess=pi.hProcess;

    WaitForSingleObject( hProcess, INFINITE );

    FILETIME CreationTime;
    FILETIME ExitTime;
    FILETIME KernelTime;
    FILETIME UserTime;

    ret=GetProcessTimes(
        hProcess,
        &CreationTime,
        &ExitTime,
        &KernelTime,
        &UserTime
    );
    if(!ret){
        printf( "GetProcessTimes failed (%d).\n", GetLastError() );
        return 1;
    }

    double user=toSeconds(UserTime);
    double kernel=toSeconds(KernelTime);
    double real=toSeconds(ExitTime)-toSeconds(CreationTime);

    fprintf(stderr, "real     %.3lf\n", real);
    fprintf(stderr, "user     %.3lf\n", user);
    fprintf(stderr, "sys      %.3lf\n", kernel);

    CloseHandle(hProcess);

    return 0;
}