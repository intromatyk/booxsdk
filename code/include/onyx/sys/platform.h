// Authors: John

/// Public header of the system configuration library.

#ifndef ONYX_SYSTEM_PLATFORM_H__
#define ONYX_SYSTEM_PLATFORM_H__

#include <QString>

namespace sys
{

QString platform();

bool is166e();

bool isIMX31L();

QString soundModule();
int defaultRotation();

}

#endif  // ONYX_SYSTEM_PLATFORM_H__
