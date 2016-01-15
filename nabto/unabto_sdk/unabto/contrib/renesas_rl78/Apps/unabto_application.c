/**
 * uNabto application logic implementation
 */

#include "unabto/unabto_app.h"
#include "unabto/unabto_util.h"
#include "unabto/unabto_app_adapter.h"

#include <sensors/Accelerometer.h>
#include <sensors/LightSensor.h>
#include <sensors/Temperature.h>
#include <sensors/Potentiometer.h>
#include <system/Switch.h>
#include "led.h"

/***************** The uNabto application logic *****************
 * This is where the user implements his/her own functionality
 * to the device. When a Nabto message is received, this function
 * gets called with the message's request id and parameters.
 * Afterwards a user defined message can be sent back to the
 * requesting browser.
 ****************************************************************/
application_event_result application_event(application_request* request, buffer_read_t* read_buffer, buffer_write_t* write_buffer) {
	switch(request->queryId) {
		case 1: {
			/** Get acceleration data */
			extern int16_t gAccData[3];
			uint16_t acc_x;
			uint16_t acc_y;
			uint16_t acc_z;
			
			// Get accelerometer data and calculate yaw, pitch and roll with offset
			Accelerometer_Get();
			acc_x = gAccData[0] + 0xFF;
			acc_y = gAccData[1] + 0xFF;
			acc_z = gAccData[2] + 0xFF;
			
			// Write back data
			if (!buffer_write_uint16(write_buffer, acc_x)) return AER_REQ_RSP_TOO_LARGE;
			if (!buffer_write_uint16(write_buffer, acc_y)) return AER_REQ_RSP_TOO_LARGE;
			if (!buffer_write_uint16(write_buffer, acc_z)) return AER_REQ_RSP_TOO_LARGE;
			
			return AER_REQ_RESPONSE_READY;
		}
		case 2: {
			/** Get temperature data */
			uint16_t temp;
			
			temp = Temperature_Get();
			
			// Write back data
			if (!buffer_write_uint16(write_buffer, temp)) return AER_REQ_RSP_TOO_LARGE;
			
			return AER_REQ_RESPONSE_READY;
		}
		case 3: {
			/** Get light level */
			uint16_t light;
			light = LightSensor_Get();
			
			// Write back data
			if (!buffer_write_uint16(write_buffer, light)) return AER_REQ_RSP_TOO_LARGE;
			
			return AER_REQ_RESPONSE_READY;
		}
		case 4: {
			/** Get potentiometer data */
			uint32_t pot;
			
			pot = Potentiometer_Get();
			
			// Write back data
			if (!buffer_write_uint32(write_buffer, pot)) return AER_REQ_RSP_TOO_LARGE;
			
			return AER_REQ_RESPONSE_READY;
		}
		case 5: {
			/** Get button status */
			uint8_t button1;
			uint8_t button2;
			uint8_t button3;
			
			button1 = Switch1IsPressed();
			button2 = Switch2IsPressed();
			button3 = Switch3IsPressed();
			
			// Write back data
			if (!buffer_write_uint8(write_buffer, button1)) return AER_REQ_RSP_TOO_LARGE;
			if (!buffer_write_uint8(write_buffer, button2)) return AER_REQ_RSP_TOO_LARGE;
			if (!buffer_write_uint8(write_buffer, button3)) return AER_REQ_RSP_TOO_LARGE;
			
			return AER_REQ_RESPONSE_READY;
		}
		case 6: {
			/** Get sound level */
			uint32_t sound;
			
			sound = Microphone_Get();
                        
			// Write back data
			if (!buffer_write_uint32(write_buffer, sound)) return AER_REQ_RSP_TOO_LARGE;
			
			return AER_REQ_RESPONSE_READY;
		}
		case 7: {
			/** Set LED */
			uint16_t led;
			
			// Read parameters in request
			if (!buffer_read_uint16(read_buffer, &led)) return AER_REQ_TOO_SMALL;
			
			if (led == 1) {
				led_all_on();
			} else {
				led_all_off();
			}
			
			// Write back data
			if (!buffer_write_uint16(write_buffer, led)) return AER_REQ_RSP_TOO_LARGE;
			
			return AER_REQ_RESPONSE_READY;
		}
	}
	return AER_REQ_INV_QUERY_ID;
}
