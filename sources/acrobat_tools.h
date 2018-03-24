//	
//  acrobat_tools.h
//  Created by ptrckr on 24.03.18.	
//	

#ifndef REPROMATIC_ACROBAT_TOOLS_H
#define REPROMATIC_ACROBAT_TOOLS_H

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

namespace repromatic {
namespace acrobat_tools {

AVStatusMonitorProcsRec GetStatusMonitor();

}  // acrobat_tools
}  // repromatic

#endif