#include "depch.h"
#include "FileBrowser.h"
#include <shobjidl.h> 

namespace OpenGLEngine
{
	FileBrowser::FileBrowser()
	{

	}

	bool FileBrowser::OpenFile()
	{
		HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(f_SysHr))
			return FALSE;

		IFileOpenDialog* f_FileSystem;
		f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));
		if (FAILED(f_SysHr)) {
			CoUninitialize();
			return FALSE;
		}

		f_SysHr = f_FileSystem->Show(NULL);
		if (FAILED(f_SysHr)) {
			f_FileSystem->Release();
			CoUninitialize();
			return FALSE;
		}

		IShellItem* f_Files;
		f_SysHr = f_FileSystem->GetResult(&f_Files);
		if (FAILED(f_SysHr)) {
			f_FileSystem->Release();
			CoUninitialize();
			return FALSE;
		}

		PWSTR f_Path;
		f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
		if (FAILED(f_SysHr)) {
			f_Files->Release();
			f_FileSystem->Release();
			CoUninitialize();
			return FALSE;
		}

		std::wstring path(f_Path);
		std::string c(path.begin(), path.end());
		m_FileInfos.m_FilePath = c;

		const size_t slash = m_FileInfos.m_FilePath.find_last_of("/\\");
		m_FileInfos.m_SelectedFile = m_FileInfos.m_FilePath.substr(slash + 1);

		CoTaskMemFree(f_Path);
		f_Files->Release();
		f_FileSystem->Release();
		CoUninitialize();

		m_FileInfos.m_FileExtension = m_FileInfos.m_SelectedFile.substr(m_FileInfos.m_SelectedFile.find_last_of(".") + 1);

		size_t lastindex = m_FileInfos.m_SelectedFile.find_last_of(".");
		m_FileInfos.m_FileName = m_FileInfos.m_SelectedFile.substr(0, lastindex);
		
		return TRUE;
	}
}