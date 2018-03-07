#pragma once

class CBSUtil
{
public:
	//formats time given in milliseconds
	static CString FormatTime(double time);

	//formats time given is bytes
	static CString FormatSize(DWORD dwBytes);
};
