#include "utils/acro.h"

#include "utils/convert.h"  // wide_to_narrow_str()
#include "utils/win.h"

#include <string>

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

as_path::as_path(std::wstring wstr)
{
        this->path = ASFileSysCreatePathName(
                NULL,
                ASAtomFromString("Cstring"),
                wide_to_narrow_str(wstr).c_str(),
                NULL
        );
}

bool as_path::is_valid()
{
        return this->path != NULL;
}

as_path::~as_path()
{
        ASFileSysReleasePath(NULL, this->path);
}