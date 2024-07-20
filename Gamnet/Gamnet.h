#define GAMNET_VERSION 1.0.2

#ifndef GAMNET_GAMNET_H_
#define GAMNET_GAMNET_H_

#include "Network/Network.h"
#include "Database/Database.h"
#include "Library/Atomic.h"
#include "Library/Base64.h"
#include "Library/Buffer.h"
#include "Library/Delegate.h"
#include "Library/Exception.h"
#include "Library/Json/json.h"
#include "Library/MD5.h"
#include "Library/MultiLock.h"
#include "Library/Pool.h"
#include "Library/Random.h"
#include "Library/WeightRandom.h"
#include "Library/Singleton.h"
#include "Library/ThreadPool.h"
#include "Library/Time/Time.h"
#include "Library/Variant.h"
#include "Log/Log.h"
#include "Test/Test.h"

#if BOOST_VERSION < 106600
#error "need boost higher version"
#endif

import Gamnet;

#endif /* GAMNET_H_ */
