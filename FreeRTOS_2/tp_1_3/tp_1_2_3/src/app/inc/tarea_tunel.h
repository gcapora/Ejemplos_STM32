/*
 * @file   : tarea_tunel.h
 * @date   :
 * @author : GFC
 */

#ifndef INC_TAREA_TUNEL_H_
#define INC_TAREA_TUNEL_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

#include "app.h"		// Necesito variable global de cola
#include "test_mock.h"	// Necesito el tipo access_t

/********************** macros ***********************************************/

/********************** typedef **********************************************/

typedef struct
{
	access_t	entrada_id;
	uint16_t	cuenta;
} mensaje_a_tunel_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

void oa_tunel(void *argumento);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* INC_TAREA_TUNEL_H_ */
/********************** end of file ******************************************/

