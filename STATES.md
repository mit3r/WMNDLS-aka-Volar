# Volar - working states

| State  | Description                 | Animation            | onPress               | Transitions To                                    |
| ------ | --------------------------- | -------------------- | --------------------- | ------------------------------------------------- |
| `BOOT` | A start up of the device.   | Blink green on edges | -                     | After 3 seconds `IDLE`                            |
| `RECV` | Receiving data from server. | Blink white on edges | Blink white on edges  | After `hold1` to `ADDR`, after `hold2` to `IDLE`  |
| `ADDR` | Setting device address.     | Blink blue on edges  | -                     | After `hold1` to `RECV`, after `hold2` to `IDLE`  |
| `IDLE` | Standby state.              | Leds off             | Blink yellow on edges | After `hold1` to `RECV` , after `hold2` to `RECV` |
