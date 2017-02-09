#include "CNativeFile.h"

CNativeFile::CNativeFile()
{
	//ctor
}

CNativeFile::~CNativeFile()
{
	//dtor
	FreeLibrary ( hMod );
}

BOOL CNativeFile::Init()
{
	//Dynamically load ntdll.dll and find out the addresses of exported functions
	if ( bInitialized )
		return TRUE;

	hMod = LoadLibraryW ( L"ntdll.dll" );
	pNtCreateFile = ( NtCreateFile* ) GetProcAddress ( hMod, "NtCreateFile" );
	pRtlInitUnicodeString = ( RtlInitUnicodeString* ) GetProcAddress ( hMod, "RtlInitUnicodeString" );
	pNtQueryAttributesFile = ( NtQueryAttributesFile* ) GetProcAddress ( hMod, "NtQueryAttributesFile" );
	pNtSetInformationFile = ( NtSetInformationFile* ) GetProcAddress ( hMod, "NtSetInformationFile" );
	pNtDeleteFile = ( NtDeleteFile* ) GetProcAddress ( hMod, "NtDeleteFile" );
	pNtClose = ( NtClose* ) GetProcAddress ( hMod, "NtClose" );

	if ( hMod && pNtCreateFile && pRtlInitUnicodeString && pNtQueryAttributesFile && pNtSetInformationFile && pNtDeleteFile && pNtClose )
	{
		bInitialized = TRUE;
		return TRUE;
	}

	bInitialized = FALSE;
	return FALSE;
}

BOOL CNativeFile::Create ( LPCWSTR file )
{
	if ( !this->Init() )
		return FALSE;

	//Prepare all arguments for the call to NtCreateFile
	IO_STATUS_BLOCK ioStatus;
	memset ( &ioStatus, 0, sizeof ( ioStatus ) );
	OBJECT_ATTRIBUTES object;
	memset ( &object, 0, sizeof ( object ) );
	object.Length = sizeof ( object );
	object.Attributes = OBJ_CASE_INSENSITIVE;
	UNICODE_STRING fn;
	pRtlInitUnicodeString ( &fn, file );
	object.ObjectName = &fn;
	// determine if file is read-only
	ULONG dwAttrib = this->GetAttributes ( file ) ;

	if ( dwAttrib & FILE_ATTRIBUTE_READONLY )
	{
		//cout << "File is read only " << endl;
		this->SetAttributes ( file, FILE_ATTRIBUTE_NORMAL  ) ;
	}

	//Finally, create the file
	//If the file already exists, it is truncated.
	HANDLE out;
	NTSTATUS status = pNtCreateFile ( &out, GENERIC_WRITE, &object, &ioStatus, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN_IF, 0, NULL, 0 );
	pNtClose ( out );

	if ( !NT_SUCCESS ( status ) )
	{
		return FALSE;
	}

	//cout << "File opened " << endl;
	return TRUE;
}


BOOL CNativeFile::Delete ( LPCWSTR file )
{
	if ( !this->Init() )
		return FALSE;

	OBJECT_ATTRIBUTES object;
	memset ( &object, 0, sizeof ( object ) );
	object.Length = sizeof ( object );
	object.Attributes = OBJ_CASE_INSENSITIVE;
	UNICODE_STRING fn;
	pRtlInitUnicodeString ( &fn, file );
	object.ObjectName = &fn;
	// determine if file is read-only
	ULONG dwAttrib = this->GetAttributes ( file );

	if ( dwAttrib & FILE_ATTRIBUTE_READONLY )
	{
		//cout << "File is read only " << endl;
		this->SetAttributes ( file, FILE_ATTRIBUTE_NORMAL  );
		//cout << "Successfully set file attributes" << endl;
	}

	NTSTATUS status = pNtDeleteFile ( &object );

	if ( NT_SUCCESS ( status ) )
	{
		//cout << " File deleted" << endl;
		return TRUE;
	}

	return FALSE ;
}


ULONG CNativeFile::GetAttributes ( LPCWSTR file )
{
	if ( !this->Init() )
		return FALSE;

	OBJECT_ATTRIBUTES object;
	memset ( &object, 0, sizeof ( object ) );
	object.Length = sizeof ( object );
	object.Attributes = OBJ_CASE_INSENSITIVE;
	UNICODE_STRING fn;
	pRtlInitUnicodeString ( &fn, file );
	object.ObjectName = &fn;
	FILE_BASIC_INFORMATION fInfo = { 0 };
	/*NTSTATUS status = */pNtQueryAttributesFile ( &object, &fInfo ) ;
	return fInfo.FileAttributes;
}

BOOL CNativeFile::SetAttributes ( LPCWSTR file, ULONG dwAttrib )
{
	if ( !this->Init() )
		return FALSE;

	IO_STATUS_BLOCK ioStatus;
	memset ( &ioStatus, 0, sizeof ( ioStatus ) );
	OBJECT_ATTRIBUTES object;
	memset ( &object, 0, sizeof ( object ) );
	object.Length = sizeof ( object );
	object.Attributes = OBJ_CASE_INSENSITIVE;
	UNICODE_STRING fn;
	pRtlInitUnicodeString ( &fn, file );
	object.ObjectName = &fn;
	// open file
	HANDLE out;
	NTSTATUS status = pNtCreateFile ( &out, FILE_WRITE_ATTRIBUTES, &object, &ioStatus, NULL, FILE_ATTRIBUTE_NORMAL, 0, FILE_OPEN, 0, NULL, 0 );

	if ( !NT_SUCCESS ( status ) )
	{
		return FALSE;
	}

	// set attributes
	memset ( &ioStatus, 0, sizeof ( ioStatus ) );
	FILE_BASIC_INFORMATION fInfo = { 0 };
	fInfo.FileAttributes = dwAttrib ;
	status = pNtSetInformationFile ( out, &ioStatus, ( PFILE_BASIC_INFORMATION ) &fInfo, sizeof ( fInfo ), FileBasicInformation ) ;
	pNtClose ( out );

	if ( !NT_SUCCESS ( status ) )
	{
		return FALSE;
	}

	return TRUE;
}
