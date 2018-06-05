#include "state_machine.h"

volatile STATE_t state = ST_NORMAL_MODE;
volatile EVENT_t event = EV_NONE;

STATE_t state_get(void) {
  return state;
}

void state_set(STATE_t new_state) {
  state = new_state;
}

EVENT_t event_get(void) {
  return event;
}

void event_set(EVENT_t new_event) {
  event = new_event;
}
