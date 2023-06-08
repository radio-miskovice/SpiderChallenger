# Winkey2 protocol

## Admin Commands - prefix 0x00

|  command |  expects | function | parameters | return | note |
|----------|----------|----------|------------|--------|------|
| 0x00 | 1 byte  | Calibrate     | const byte EOF  | null   | obsolete, no effect |
| 0x01 | - | Reset | null | null | |
| 0x02 | - | Host Open  | null | revision | |
| 0x03 | - | Host Close | null | null | |
| 0x04 | 1 byte | Echo | byte to echo | byte echoed | |
| 0x05 | - | Paddle A2D | null | const byte 0 | |
| 0x06 | - | Speed A2D  | null | const byte 0 | |
| 0x07 | - | Get Values | null | 15 bytes | diag only, not to be used in host open mode; inverse to Load Defaults |
| 0x08 | - | -reserved- | null | ?? | K1EL internal |
| 0x09 | - | Get Cal | null | const byte 0 | obsolete |
| 0x0A | - | Set WK1 mode | null | ?? | disable pushbutton reporting |
| 0x0B | - | Set WK2 mode | null | ?? | enable pushbutton reporting  |
| 0x0C | - | Dump EEPROM  | null | byte[256] | dumps internal EEPROM  |
| 0x0D | - | Download EEPROM | null | ?? | enable pushbutton reporting  |
| 0x0E | 1 byte | Send standalone message | msg number 1-6 | ?? |   |

## Host commands (immediate effect) - prefix 0x01

|  command |  expects | function | parameters | return | note |
|----------|----------|----------|------------|--------|------|
| 0x01 | 1 byte | Sidetone control | paddle-only (MSB), frequency | null |   |
| 0x02 | 1 byte | Set WPM speed    | wpm (0, 5-99) | null |   |
| 0x03 | 1 byte | Set weighting    | 10-90 (%) | null |   |
| 0x04 | 2 bytes | Set PTT Lead/Tail | lead time (x10ms), tail time (x10 ms) | null |   |
| 0x05 | 3 bytes | Set potentiometer range | min wpm, +range wpm, ignore | null |   |
| 0x06 | 1 byte | Set pause state | 1 = pause, 0 = unpause | null |   |
| 0x07 | - | Get speed pot | null | wpm over minimum, 0-31; MSbits always 10 |   |
| 0x08 | - | Backup input buffer by one char | null | null |   |
| 0x09 | 1 byte | Set pin config (ultimatic) | see manual | null |  ignore in Challenger |
| 0x0A | - | clear buffer | null | null |   |
| 0x0B | 1 byte | Tune (key immediate) | 0 = keyup, 1 = keydown | null |   |
| 0x0C | 1 byte | Set HSCW | LPM / 100 | null |   |
| 0x0D | 1 byte | Set Farnsworth WPM | rate WPM | null |   |
| 0x0E | 1 byte | Set Winkeyer2 Mode (bitwise) | see manual | null |   |
| 0x0F | 15 bytes | Load defaults | Mode, WPM, Sidetone, Weight, Lead-In, Tail, min WPM, WPM range, 1st ext, key compensation, Farnsworth WPM, paddle setpoint, dit/dah ratio, pin config, ignore | null |   |
| 0x10 | 1 byte | Set 1st extension | extension in milliseconds | null |   |
| 0x11 | 1 byte | Set Key Compensation | compensation in milliseconds | null |   |
| 0x12 | 1 byte | Set Paddle Switch Point | 10-90 means % of dit time | null |   |
| 0x13 | - | No op |  | null |   |
| 0x14 | 1 byte | Software paddle | 0 = none, 1 = dit, 2 = dah, 3 = both | null |   |
| 0x15 | - | Request Winkeyer Status | null | status, pushbutton status |   |
| 0x16 | 1 or 2 bytes | Buffer Pointer Command  | 0 = reset, 1 = new position overwrite, 2 = new position append, 3 = add N nulls (N = extra byte) | null |   |
| 0x17 | 1 byte | Set Dit/Dah Ratio | 33-66, 50 = 1:3, 33 = 1:2, 66 = 1:4 | null |   |
| 0x18 | 1 byte | PTT ON/OFF | 0 = PTT off, 1 = PTT on | null |   |
| 0x19 | 1 byte | Key Buffered | key interval in seconds | null |   |
| 0x1A | 1 byte | Wait Buffered | wait interval in seconds | null |   |
| 0x1B | 2 bytes | Merge two letters | first, second letter | null | will send two merged letters as prosign |
| 0x1C | 1 byte  | Change Speed Buffered | WPM in buffer transmission | null |  |
| 0x1D | 1 byte  | HSCW Speed Change Buffered | LPM/100 in buffer transmission | null |  |
| 0x1E | -  | Cancel Speed Buffered | null | null |  |
| 0x1F | -  | No Op Buffered | null | null |  |
