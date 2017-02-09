#ifndef CNATIVEFILE_H
#define CNATIVEFILE_H

#include <windows.h>


//NT Native API declarations
typedef LONG NTSTATUS;

typedef struct _UNICODE_STRING
{
	USHORT Length, MaximumLength;
	PWCH Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _IO_STATUS_BLOCK
{
	union
	{
		NTSTATUS Status;
		PVOID Pointer;
	};

	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef struct _OBJECT_ATTRIBUTES
{
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor;
	PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;

#if _MSC_VER
typedef enum _CREATE_DISPOSITION
{
	FILE_SUPERSEDE = 0x00000000,
	FILE_OPEN = 0x00000001,
	FILE_CREATE = 0x00000002,
	FILE_OPEN_IF = 0x00000003,
	FILE_OVERWRITE = 0x00000004,
	FILE_OVERWRITE_IF = 0x00000005,
	FILE_MAXIMUM_DISPOSITION = 0x00000005
} CREATE_DISPOSITION;
#endif // MSC_VER

typedef struct _FILE_BASIC_INFORMATION
{
	LARGE_INTEGER CreationTime;
	LARGE_INTEGER LastAccessTime;
	LARGE_INTEGER LastWriteTime;
	LARGE_INTEGER ChangeTime;
	ULONG         FileAttributes;
} FILE_BASIC_INFORMATION, *PFILE_BASIC_INFORMATION;

// Valid values for the Attributes field of OBJECT_ATTRIBUTES

#define OBJ_INHERIT             0x00000002L
#define OBJ_PERMANENT           0x00000010L
#define OBJ_EXCLUSIVE           0x00000020L
#define OBJ_CASE_INSENSITIVE    0x00000040L
#define OBJ_OPENIF              0x00000080L
#define OBJ_OPENLINK            0x00000100L
#define OBJ_KERNEL_HANDLE       0x00000200L
#define OBJ_FORCE_ACCESS_CHECK  0x00000400L
#define OBJ_VALID_ATTRIBUTES    0x000007F2L

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

typedef enum _FILE_INFORMATION_CLASS
{

	FileDirectoryInformation = 1,
	FileFullDirectoryInformation,
	FileBothDirectoryInformation,
	FileBasicInformation,
	FileStandardInformation,
	FileInternalInformation,
	FileEaInformation,
	FileAccessInformation,
	FileNameInformation,
	FileRenameInformation,
	FileLinkInformation,
	FileNamesInformation,
	FileDispositionInformation,
	FilePositionInformation,
	FileFullEaInformation,
	FileModeInformation,
	FileAlignmentInformation,
	FileAllInformation,
	FileAllocationInformation,
	FileEndOfFileInformation,
	FileAlternateNameInformation,
	FileStreamInformation,
	FilePipeInformation,
	FilePipeLocalInformation,
	FilePipeRemoteInformation,
	FileMailslotQueryInformation,
	FileMailslotSetInformation,
	FileCompressionInformation,
	FileCopyOnWriteInformation,
	FileCompletionInformation,
	FileMoveClusterInformation,
	FileQuotaInformation,
	FileReparsePointInformation,
	FileNetworkOpenInformation,
	FileObjectIdInformation,
	FileTrackingInformation,
	FileOleDirectoryInformation,
	FileContentIndexInformation,
	FileInheritContentIndexInformation,
	FileOleInformation,
	FileMaximumInformation


} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef NTSTATUS __stdcall NtCreateFile ( PHANDLE FileHandle,
		ACCESS_MASK DesiredAccess,
		POBJECT_ATTRIBUTES ObjectAttributes,
		PIO_STATUS_BLOCK IoStatusBlock,
		PLARGE_INTEGER AllocationSize,
		ULONG FileAttributes,
		ULONG ShareAccess,
		ULONG CreateDisposition,
		ULONG CreateOptions,
		PVOID EaBuffer,
		ULONG EaLength );

typedef void __stdcall RtlInitUnicodeString ( PUNICODE_STRING  DestinationString, PCWSTR  SourceString );

typedef NTSTATUS __stdcall NtQueryAttributesFile ( POBJECT_ATTRIBUTES ObjectAttributes, PFILE_BASIC_INFORMATION FileInformation );

typedef NTSTATUS __stdcall NtSetInformationFile (
	IN HANDLE               FileHandle,
	OUT PIO_STATUS_BLOCK    IoStatusBlock,
	IN PVOID                FileInformation,
	IN ULONG                Length,
	IN FILE_INFORMATION_CLASS FileInformationClass );

typedef NTSTATUS __stdcall NtDeleteFile  ( POBJECT_ATTRIBUTES ObjectAttributes ) ;
typedef NTSTATUS __stdcall NtClose       ( HANDLE handle ) ;

class CNativeFile {
	public:
		CNativeFile();
		virtual ~CNativeFile();
		BOOL Create ( LPCWSTR file );
		BOOL Delete ( LPCWSTR file );
		ULONG GetAttributes ( LPCWSTR file );
		BOOL SetAttributes ( LPCWSTR file, ULONG dwAttrib );
	protected:
		BOOL Init();
		BOOL bInitialized = FALSE;
	private:
		NtCreateFile *pNtCreateFile;
		RtlInitUnicodeString *pRtlInitUnicodeString;
		NtQueryAttributesFile *pNtQueryAttributesFile;
		NtSetInformationFile *pNtSetInformationFile;
		NtDeleteFile *pNtDeleteFile;
		NtClose *pNtClose;
		HMODULE hMod;
};

#endif // CNATIVEFILE_H
