#include <string>
#include "CNativeFile.h"
#include <conio.h>

// name of the forbidden file
#define FILE_FORBIDDEN_NAME L"lpt3.txt"


// Nt API functions requires this prefix, see for example NtCreateFile
#define NATIVEAPI_PREFIX    L"\\??\\"


void WriteConsoleColor ( WCHAR* text,  bool bWarning = true )
{
	HANDLE hStdout = GetStdHandle ( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
	WORD wOldColorAttrs;

	// Save the current text colors.
	GetConsoleScreenBufferInfo ( hStdout, &csbiInfo );
	wOldColorAttrs = csbiInfo.wAttributes;

	// Set the text attributes to draw color text on black background.
	if ( bWarning == false )
		SetConsoleTextAttribute ( hStdout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	else
		SetConsoleTextAttribute ( hStdout, FOREGROUND_RED | FOREGROUND_INTENSITY );

	wprintf ( text );
	// Restore the original text colors.
	SetConsoleTextAttribute ( hStdout, wOldColorAttrs );

	

}

int main()
{
	printf ( "USBVaccine.exe (c) 2016 modoran.george@gmail.com  "
			 "All rights reserved\n\n"
			 "\tThis tool prevents creating any 'autorun' files on any fixed or removable media, \n"
			 "creating a folder named 'autorun.inf' and a file named 'lpt3.txt' which cannot be \ndeleted by most stupid viruses around here :)\n\n"
			 "\tIt is by no means an antivirus replacement and comes with NO WARRANTY, USE IT AT YOUR OWN RISK!\n\n" );

	WCHAR szBuf [ 4096 ];
	// get required buffer size from windows
	DWORD dwSize = GetLogicalDriveStringsW ( 0, NULL );

	// allocating memory and calling function again
	WCHAR* buf = new WCHAR[ dwSize ];
	GetLogicalDriveStringsW ( dwSize, buf );


	WCHAR* szSingleDrive = buf;

	while ( *szSingleDrive )
	{
		UINT driveType = GetDriveTypeW ( szSingleDrive );

		if ( driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE )
		{
			std::wstring bufString;
			bufString += szSingleDrive;
			bufString += L"autorun.inf";
			DWORD dwAttr = GetFileAttributesW ( bufString.c_str() );

			if ( INVALID_FILE_ATTRIBUTES == dwAttr )
			{
				// all seems good, no such file or directory
				// attempting to create 'autorun.inf' directory
				if ( CreateDirectoryW ( bufString.c_str(), NULL ) )
				{
					// creating a forbidden name file (from win32 at least)
					CNativeFile nFile;
					bufString = ( NATIVEAPI_PREFIX ) + bufString;
					bufString += L"\\";
					bufString += FILE_FORBIDDEN_NAME;

					if ( nFile.Create ( bufString.c_str() ) )
					{
						wprintf ( L"Drive '%s' vaccinated successfully, created new file\n", szSingleDrive );
					}
					else
					{
						wsprintfW ( szBuf, L"Drive '%s' cannot be vaccinated, file not created\n", szSingleDrive );
						WriteConsoleColor ( szBuf, true );
					}

				}
				else
				{
					wsprintfW ( szBuf, L"Drive '%s' cannot be vaccinated, failed to create directory\n", szSingleDrive );
					WriteConsoleColor ( szBuf, true );
				}
			}
			else
				if ( dwAttr & FILE_ATTRIBUTE_DIRECTORY )
				{
					// already a directory, creating the file
					// overwriting if it exists
					// this can fail if a directory with same name already exists
					// but we are not worried about that
					CNativeFile nFile;
					bufString = ( NATIVEAPI_PREFIX ) + bufString;
					bufString += L"\\";
					bufString += FILE_FORBIDDEN_NAME;

					if ( nFile.Create ( bufString.c_str() ) )
					{
						wprintf ( L"Drive '%s' vaccinated successfully, file created\n", szSingleDrive );
					}
					else
					{
						wsprintfW ( szBuf, L"Drive '%s' cannot be vaccinated, file not created\n", szSingleDrive );
						WriteConsoleColor ( szBuf, true );
					}
				}
				else
				{
					// a regular 'autorun.inf' file was found
					// if this is windows drive elevation is required
					CNativeFile nFile;
					std::wstring origString = bufString;
					bufString = ( NATIVEAPI_PREFIX ) + bufString;
					wsprintfW ( szBuf, L"\nWarning:  Drive '%s' already contains an 'autorun.inf' file, drive could be infected\n", szSingleDrive );
					WriteConsoleColor ( szBuf, false );

					if ( nFile.Delete ( bufString.c_str() ) )
					{
						wprintf ( L"Successfully removed existing 'autorun.inf' file\n" );
					}
					else
					{
						wsprintfW ( szBuf, L"Error: Failed to remove existing 'autorun.inf' file, try run this program as administrator\n" );
						WriteConsoleColor ( szBuf, true );
						
					}

					// all seems good, no such file or directory
					// attempting to create 'autorun.inf' directory
					if ( CreateDirectoryW ( origString.c_str(), NULL ) )
					{
						// creating a forbidden name file (from win32 at least)
						CNativeFile nFile;
						bufString += L"\\";
						bufString += FILE_FORBIDDEN_NAME;

						if ( nFile.Create ( bufString.c_str() ) )
						{
							wprintf ( L"Drive '%s' vaccinated successfully, file created\n", szSingleDrive );
						}
						else
						{

							wsprintfW ( szBuf, L"Drive '%s' cannot be vaccinated, file not created\n", szSingleDrive );
							WriteConsoleColor ( szBuf, true );
						}

					}

				}
		}

		// get the next drive
		szSingleDrive += wcslen ( szSingleDrive ) + 1;
	}

	delete [] buf;
	printf ( "\n\nPress any key to continue ...\n" );
	_getch();

	return 0;
}

