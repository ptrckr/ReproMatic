//
//  menu_methods/overview.h
//  Created by Patrick Rügheimer on 25.06.18.
//

#ifndef OVERVIEW_H
#define OVERVIEW_H

#ifndef MAC_PLATFORM
  #include "PIHeaders.h"
#endif

void Overview();

// Din
ACCB1 void ACCB2 OverviewDinCallback(void *clientData);
ACCB1 ASBool ACCB2 OverviewDinCallbackIsEnabled(void *clientData);

// Scan
ACCB1 void ACCB2 OverviewScanCallback(void *clientData);
ACCB1 ASBool ACCB2 OverviewScanCallbackIsEnabled(void *clientData);

#endif
