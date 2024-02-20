#include <windows.h>
#include <string>
#include <map>

#include <iostream>

using namespace std;

map<char, string> disks;

void prepareDisksSet()
{
	for (int i = 0; i < 26; ++i)
	{
		disks['A' + i] = string(1, 'A' + i) + ":\\";
	}
}

void showMenu()
{
}

void showDisks()
{
	DWORD dwDrives = 0, dwMask = 1;
	CHAR chDrive;
	dwDrives = GetLogicalDrives();
	cout << "Currently available Disk Drives" << endl;
	for (UINT i = 0; i < 26; i++)
	{
		if (dwDrives & dwMask)
		{
			chDrive = 'A' + i;
			cout << chDrive << ": " << endl;
		}
		dwMask <<= 1;
	}
}

void getInfo(char diskName)
{
	int type;

	type = GetDriveTypeA(disks[diskName].c_str());

	cout << "Disk type: " << endl;
	if (type == DRIVE_UNKNOWN)
		cout << " UNKNOWN" << endl;
	if (type == DRIVE_NO_ROOT_DIR)
		cout << " DRIVE NO ROOT DIR " << endl;
	if (type == DRIVE_REMOVABLE)
		cout << " REMOVABLE" << endl;
	if (type == DRIVE_FIXED)
		cout << " FIXED" << endl;
	if (type == DRIVE_REMOTE)
		cout << " REMOTE" << endl;
	if (type == DRIVE_CDROM)
		cout << " CDROM" << endl;
	if (type == DRIVE_RAMDISK)
		cout << " RAMDISK" << endl;
	cin.get();

	char VolumeNameBuffer[100];
	char FileSystemNameBuffer[100];
	unsigned long VolumeSerialNumber;

	BOOL GetVolumeInformationFlag = GetVolumeInformationA(
		disks[diskName].c_str(),
		VolumeNameBuffer,
		100,
		&VolumeSerialNumber,
		NULL, //&MaximumComponentLength,
		NULL, //&FileSystemFlags,
		FileSystemNameBuffer,
		100);

	if (GetVolumeInformationFlag != 0)
	{
		cout << "Volume Name is " << VolumeNameBuffer << endl;
		cout << "Volume Serial Number is " << VolumeSerialNumber << endl;
		cout << "File System is " << FileSystemNameBuffer << endl;
	}
	else
		cout << "Not Present (GetVolumeInformation)" << endl;

	DWORD FreeBytesAvailable;
	DWORD TotalNumberOfBytes;
	DWORD TotalNumberOfFreeBytes;

	BOOL GetDiskFreeSpaceFlag = GetDiskFreeSpaceEx(
		disks[diskName].c_str(),				 // directory name
		(PULARGE_INTEGER)&FreeBytesAvailable,	 // bytes available to caller
		(PULARGE_INTEGER)&TotalNumberOfBytes,	 // bytes on disk
		(PULARGE_INTEGER)&TotalNumberOfFreeBytes // free bytes on disk
	);

	if (GetDiskFreeSpaceFlag != 0)
	{
		cout << "Total Number Of Free Bytes = " << (unsigned long)TotalNumberOfFreeBytes
			 << "( " << double((unsigned long)(TotalNumberOfFreeBytes) / 1024) / 1024
			 << " Mb )" << endl;
		cout << "Total Number Of Bytes = " << (unsigned long)TotalNumberOfBytes
			 << "( " << double((unsigned long)(TotalNumberOfBytes) / 1024) / 1024
			 << " Mb )" << endl;
	}
	else
		cout << "	Not Present (GetDiskFreeSpace)" << endl;
}

char selectDisk()
{
	char l;
	cout << "Enter disk LETTER: " << endl;
	cin >> l;
	return l;
}

int main(void)
{
	prepareDisksSet();
	cout << "Avilliable disks: " << endl;
	showDisks();

	char diskName = selectDisk();

	cout << "Info about disk: " << diskName << endl;
	getInfo(diskName);

	return 0;
}
