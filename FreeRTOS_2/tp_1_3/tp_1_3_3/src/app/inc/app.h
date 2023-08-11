/* @file   : app.h de TP_1_3_3
 * @date   : Mar, 2023
 */

#ifndef APP_INC_APP_H_
#define APP_INC_APP_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/********************** macros ***********************************************/

/********************** typedef **********************************************/

/********************** external data declaration ****************************/

extern QueueHandle_t mensajes_a_tunel;
extern QueueHandle_t mensajes_emergencia_a_tunel;

/********************** external functions declaration ***********************/

void app_init(void);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* APP_INC_APP_H_ */
/********************** end of file ******************************************/

