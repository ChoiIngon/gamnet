#ifndef __GAMNET_LIB_EXCEPTION_H_
#define __GAMNET_LIB_EXCEPTION_H_

import Gamnet.Exception;

#define GAMNET_EXCEPTION(error, ...) Gamnet::Exception((int)error, "ERR [", __FILE__, ":", __func__, "@" , __LINE__, "] ", #error, "(", (int)error,") ", ##__VA_ARGS__)

#endif /* EXCEPTION_H_ */
