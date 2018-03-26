//	
//  acrobat_utils.h
//  Created by ptrckr on 24.03.18.	
//	

#ifndef REPROMATIC_ACROBAT_UTILS_H
#define REPROMATIC_ACROBAT_UTILS_H

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

namespace repromatic {
namespace acrobat_utils {

AVStatusMonitorProcsRec GetStatusMonitor();

}  // acrobat_utils
}  // repromatic

#endif