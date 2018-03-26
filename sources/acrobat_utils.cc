//	
//  acrobat_utils.cc
//  Created by ptrckr on 24.03.18.	
//	

#include "acrobat_utils.h"

#ifndef MAC_PLATFORM
#include "PIHeaders.h"
#endif

namespace repromatic {
namespace acrobat_utils {

AVStatusMonitorProcsRec GetStatusMonitor() {
  AVStatusMonitorProcsRec status_monitor;
  memset(&status_monitor, 0, sizeof(status_monitor));
  status_monitor.size = sizeof(status_monitor);
  status_monitor.cancelProc = AVAppGetCancelProc(&status_monitor.cancelProcClientData);
  status_monitor.progMon = AVAppGetDocProgressMonitor(&status_monitor.progMonClientData);
  status_monitor.reportProc = AVAppGetReportProc(&status_monitor.reportProcClientData);

  if (status_monitor.progMon) {
    if (status_monitor.progMon->beginOperation) {
      status_monitor.progMon->beginOperation(status_monitor.progMonClientData);
    }

    if (status_monitor.progMon->setCurrValue) {
      status_monitor.progMon->setCurrValue(0, status_monitor.progMonClientData);
    }

    if (status_monitor.progMon->setDuration) {
      status_monitor.progMon->setDuration(100, status_monitor.progMonClientData);
    }
  }

  return status_monitor;
}

}  // acrobat_utils
}  // repromatic