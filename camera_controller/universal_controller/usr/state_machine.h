#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

typedef enum {
  ST_NORMAL_MODE,
  ST_FORWARDING_MODE,
  //ST_MAX,
} STATE_t;

// TODO: do we really need this?
typedef enum {
  EV_NONE,
  EV_USB_MESSAGE_RECEIVED,
  EV_UART_BUFFER_OVERFLOW,
  //EV_MAX,
} EVENT_t;

STATE_t state_get(void);
void state_set(STATE_t new_state);

EVENT_t event_get(void);

void event_set(EVENT_t new_event);

#endif /* __STATE_MACHINE_H__ */
