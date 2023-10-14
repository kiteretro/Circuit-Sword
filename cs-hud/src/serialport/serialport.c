//-----------------------------------------------------------------------------
/*
 * The GPL v3 License
 *
 * Kite's Circuit Sword
 * Copyright (C) 2017 Giles Burgess (Kite's Item Shop)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 //----------------------------------------------------------------------------

/*
  serial_init("/dev/ttyUSB0");

  serial_send("ABCDEF", 6);
  usleep(100000);

  uint8_t i;
  for (i=0; i<10; i++) {

    char rx_buffer[256];
    uint8_t ret = serial_receive(rx_buffer, 255);
    fprintf(stdout, "Got %i bytes read : %s\n", ret, rx_buffer);
    usleep(1000000);
  }

  serial_unload();
*/

/*
  serial_init("/dev/ttyACM0");

  serial_send("V", 1);
  usleep(1000000);

  char rx_buffer[256];
  uint8_t ret = serial_receive(rx_buffer, 255);
  fprintf(stdout, "Got %i bytes read : %s\n", ret, rx_buffer);

  serial_unload();

  exit(0);
*/

//-------------------------------------------------------------------------

#include "serialport.h"

//-------------------------------------------------------------------------

// #define SERIALPORT_DEBUG

//-------------------------------------------------------------------------

static int uart0_filestream = -1;

//-------------------------------------------------------------------------

// Open a serial port, returns 0=fail 1=success
bool serial_init(const char* serport)
{
  fprintf(stdout, "[*] Opening UART [%s]\n", serport);

  // http://www.raspberry-projects.com/pi/programming-in-c/uart-serial-port/using-the-uart
  //-------------------------
  //----- SETUP USART 0 -----
  //-------------------------
  //At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively
  // int uart0_filestream = -1;

  //OPEN THE UART
  //The flags (defined in fcntl.h):
  //  Access modes (use 1 of these):
  //    O_RDONLY - Open for reading only.
  //    O_RDWR - Open for reading and writing.
  //    O_WRONLY - Open for writing only.
  //
  //  O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
  //                      if there is no input immediately available (instead of blocking). Likewise, write requests can also return
  //                      immediately with a failure status if the output can't be written immediately.
  //
  //  O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
  uart0_filestream = open(serport, O_RDWR | O_NOCTTY | O_NDELAY);    //Open in non blocking read/write mode
  //uart0_filestream = open(serport, O_RDWR | O_NOCTTY);    //Open in blocking read/write mode
  if (uart0_filestream == -1) {
    //ERROR - CAN'T OPEN SERIAL PORT
    fprintf(stderr, "[!] ERROR: Unable to open UART [%s]\n", serport);
    return 0;
  }

  //CONFIGURE THE UART
  //The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
  //  Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
  //  CSIZE:- CS5, CS6, CS7, CS8
  //  CLOCAL - Ignore modem status lines
  //  CREAD - Enable receiver
  //  IGNPAR = Ignore characters with parity errors
  //  ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
  //  PARENB - Parity enable
  //  PARODD - Odd parity (else even)
  struct termios options;
  tcgetattr(uart0_filestream, &options);
  options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;    //<Set baud rate
  options.c_iflag = IGNPAR;
  options.c_oflag = 0;
  options.c_lflag = 0;
  tcflush(uart0_filestream, TCIFLUSH);
  tcsetattr(uart0_filestream, TCSANOW, &options);

  fprintf(stdout, "[i] UART opened [%i]\n", uart0_filestream);

  return 1;
}

// Send a serial array of chars, returns 0=fail 1=success
bool serial_send(char* tx_buffera, uint8_t len)
{
  //----- TX BYTES -----
#ifdef SERIALPORT_DEBUG
  fprintf(stdout, "[d] Sending UART [%s]\n", tx_buffera);
#endif

  if (uart0_filestream != -1) {
    int count = write(uart0_filestream, tx_buffera, len); //Filestream, bytes, len
    if (count < 0) {
      fprintf(stderr, "[!] ERROR: UART TX error\n");
      return 0;
    }
  }

  return 1;
}

// Receive an array of chars, returns num of recevied chars -1=fail 0=none >1=num chars
int serial_receive(char* rx_buffer, uint8_t lenmax)
{
  //----- CHECK FOR ANY RX BYTES -----
  if (uart0_filestream != -1) {

    // Read up to 255 characters from the port if they are there
    //unsigned char rx_buffer[256];
    //char *rx_buffer[] = rx_buffer_ptr;
    int rx_length = read(uart0_filestream, (void*)rx_buffer, lenmax);    //Filestream, buffer to store in, number of bytes to read (max)

    if (rx_length < 0) {
      //An error occured (will occur if there are no bytes)
#ifdef SERIALPORT_DEBUG
      fprintf(stdout, "[d] WARNING: No data received from UART (in error case)\n");
#endif
      return 0;
    } else if (rx_length == 0) {
      //No data waiting
#ifdef SERIALPORT_DEBUG
      fprintf(stdout, "[d] No data received from UART\n");
#endif
      return 0;
    } else {
      //Bytes received
      rx_buffer[rx_length] = '\0';
#ifdef SERIALPORT_DEBUG
      fprintf(stdout, "[d] %i bytes read : %s\n", rx_length, (char*)rx_buffer);
#endif
      return rx_length;
    }
  }

  return -1;
}

// Receive a certain number of bytes
int serial_receive_bytes(char *rx_buffer, uint8_t len, uint16_t timeout) {

  bool looping = true;
  uint16_t retries = 0;
  while (looping) {

    // Wait for enough bytes
    uint8_t av = serial_available();

    // Check length
    if (av >= len) {
      // Return the size asked
#ifdef SERIALPORT_DEBUG
      fprintf(stdout, "[d] Buffer is %i or more available\n", len);
#endif
      return serial_receive(rx_buffer, len);
    } else {
      // Increment retry counter
      retries++;
      if (retries > timeout) {
        // Exceeded timeout
        looping = false;
#ifdef SERIALPORT_DEBUG
        fprintf(stdout, "[d] Timeout reached\n");
#endif
      } else {
        // Sleep a little bit
        usleep(10000);
#ifdef SERIALPORT_DEBUG
        fprintf(stdout, "[d] Sleeping..\n");
#endif
      }
    }
  }

  return -1;
}

// Clear buffer
void serial_clear()
{
  if (serial_available()) {
    char rx_buffer[256];
    serial_receive(rx_buffer, 255);
  }
}

// Close serial port
void serial_unload()
{
  close(uart0_filestream);

  fprintf(stdout, "[i] UART closed\n");
}

// Return the open state of port
bool serial_opened()
{
  //TODO make this better
  if (uart0_filestream == -1) {
    return 0;
  } else {
    return 1;
  }
}

// Return serial available state
uint8_t serial_available()
{
  //TODO make this better
  if (uart0_filestream == -1) {
    return 0;
  } else {
    uint8_t bytes_available;
    ioctl(uart0_filestream, FIONREAD, &bytes_available);
    return bytes_available;
  }
}

//-------------------------------------------------------------------------
