
#if !defined (ARDUINO_ARCH_AVR)
  #define strncpy_P strncpy
#endif

bool led_state = 0;

/**
* This page stores the actual HTML code that will be presented.
* The data is stored in program memory as a single long string, and is presented
* below in a manageable format
*/

/***************************************************************/

// The basic beginning of an HTML connection, plus
// a style (CSS) section and header to be used on every page
  static const char begin_html[] PROGMEM =
  "HTTP/1.1 200 OK\n"
  "Content-Type: text/html\n" //40b 
  "Connection: close\n\n" //59
  "<!DOCTYPE HTML>\n" //75 
  "<html><head>"//87
  "<style>\n"
  "body{background-color:linen; text-align: center}"
  "table.center{margin-left:auto;margin-right:auto;}"
  "</style>"
  "</head>";
  
/***************************************************************/

// The main HTML page, broken into 2 parts
// It is broken up so some variables can be printed manually in the middle
static const char main_html_p1[] PROGMEM =
  
  "<body>"
  "<img src='http://arduino.cc/en/uploads/Trademark/ArduinoCommunityLogo.png'"
  "style='width:383px;height:162px'>"
  "<br><b>Hello From Arduino!</b><br>\n"
  "<br><br> LED/Digital Pin Control:"
  "<br><br>\n<table class = 'center'>";

/***************************************************************/

static const char main_html_p2[] PROGMEM =
  
  "<tr><td><a href='/ON'>Turn LED On</a>"
  
  "<br></td><td><a href='/OF'>Turn LED Off</a>"
  
  "<br></td></tr></table><br><a href='/ST'>"
  
  "Stats</a> <a href='/CR'>Credits</a>"
  
  "</body></html>";
  
  /***************************************************************/
  
  // The HTML for the credits page
  static const char credits_html[] PROGMEM =
  "<body>"
  "<img src='http://arduino.cc/en/uploads/Trademark/ArduinoCommunityLogo.png'"
  "style='width:383px;height:162px'>"
  "<br><b>Credits:</b><br><table class='center'><tr>"
  "<td>RF24Ethernet by </td>"
  "<td><a href='https://github.com/tmrh20'> TMRh20</a></td>"
  "</tr><tr>"
  "<td>uIP by</td>"
  "<td><a href='https://github.com/adamdunkels/uip'> Adam Dunkels</a></td>"
  "</tr><tr>"
  "<td>Based on</td>"
  "<td><a href='https://github.com/ntruchsess/arduino_uip'> UIPEthernet</a></td>"
  "</tr><tr>"
  "<td>Documentation:</td><td> <a href='http://tmrh20.github.io/RF24Ethernet/'>github.io</a></td>"
  "</tr>"
  "</tr><tr>"
  "<td>RF24toTUN creator:</td><td> <a href='https://github.com/reixd/'>Reixd</a></td>"
  "</tr>"
  "</table>"
  "<br>And everybody who contributed to RF24 and RF24Network"
  "<br><br><a href='/'>Home</a>"
  "</body>"
  "</html>";
  
/***************************************************************/

/**
* This function reads from a specified program memory buffer, and sends the data to the client
* in chunks equal to the max output buffer size or less
* This allows the HTML code to be modified as desired, with no need to change any other code
*/
void sendPage(EthernetClient& _client, const char* _pointer, size_t size ){
  for(int i=0; i<size;i++){
    char c = pgm_read_byte(_pointer++);
    _client.write(&c,1);
  }
}

/***************************************************************/

// Function to send the main page
void main_page(EthernetClient& _client) {
  
  // Send the connection info and header
  const char* html_pointer = begin_html;
  sendPage(_client,html_pointer,sizeof(begin_html));
  
  //Send the first part of the page
  html_pointer = main_html_p1;
  sendPage(_client,html_pointer,sizeof(main_html_p1));
  
  // Include some variables, print them into the page manually
  const char *lState = led_state ? "ON" : "OFF";
  const char *lColor = led_state ? "darkseagreen 1" : "lightpink";
  
  char bf[OUTPUT_BUFFER_SIZE];
  
  if(!led_state){
    sprintf_P(bf,PSTR("<tr><td bgcolor=%s>\n"),lColor);
    _client.write(bf);
    sprintf_P(bf,PSTR("LED is %s</td></tr>\n"), lState);
  }else{
    sprintf_P(bf,PSTR("<tr><td> </td><td bgcolor=%s>\n"),lColor);
    _client.write(bf);
    sprintf_P(bf,PSTR("LED is %s</td></tr>\n"), lState);  
  }
  _client.write(bf);  
  
  // Send the 2nd half of the page
  static const char* html_pointer2 = main_html_p2;
  sendPage(_client,html_pointer2,sizeof(main_html_p2));
  
}

/***************************************************************/

void credits_page(EthernetClient& _client) {
  //Set the pointer to the HTML connection data + header
  const char* html_pointer = begin_html;
  sendPage(_client,html_pointer,sizeof(begin_html));
  
  //Set the pointer to the HTML page data and send it
  html_pointer = credits_html;
  sendPage(_client,html_pointer,sizeof(credits_html));
}

/***************************************************************/

// The stats page is sent differently, just to demonstrate a different method of handling pages
void stats_page(EthernetClient& _client) {
  
  uint32_t seconds = millis() / 1000UL;
  uint32_t minutes = seconds / 60UL;  
  uint32_t hours = minutes / 60UL;
  uint8_t days = hours / 24UL;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;
  
  char buffer[45];
  
  strncpy_P(buffer, PSTR("HTTP/1.1 200 OK\nContent-Type: text/html\n"),45);
  _client.write( buffer );
  strncpy_P(buffer, PSTR("Connection: close\n\n<!DOCTYPE HTML>\n<html>\n"),45);
  _client.write( buffer );
  strncpy_P(buffer, PSTR("<head><style>body{background-color:linen;}\n"),45);
  _client.write( buffer );
  strncpy_P(buffer, PSTR("td{border: 1px solid black;}</style></head>\n"),45);
  _client.write( buffer );
  strncpy_P(buffer, PSTR("<body><table><tr><td> Uptime</td><td>\n"),45);
  _client.write( buffer );
  sprintf_P(buffer, PSTR("%u days, %lu hours %lu minutes %lu"),days,hours,minutes,seconds);
  _client.write( buffer );
  strncpy_P(buffer, PSTR("seconds</td></tr><tr><td>UIP Buffer Size"),45);
  _client.write( buffer );
  sprintf_P(buffer, PSTR("</td><td>%u bytes</td></tr><tr><td>User "),UIP_BUFSIZE);
  _client.write( buffer );
  sprintf_P(buffer, PSTR("Output<br>Buffer Size</td><td>%u bytes"),OUTPUT_BUFFER_SIZE);
  _client.write( buffer );
  strncpy_P(buffer, PSTR("</td></tr></table><br><br>"),45);
  _client.write( buffer );
  strncpy_P(buffer, PSTR("<a href='/'>Home</a></body></html>"),45);
  _client.write( buffer );
  
}

/***************************************************************/

/**
* An example of a very basic HTML page
*/
  static const char html_page[] PROGMEM = 
    "HTTP/1.1 200 OK\n"
    "Content-Type: text/html\n"
    "Connection: close\n\n"
    "<!DOCTYPE HTML>"
    "<html>"
    "<body>"
    "<b>Hello From Arduino!</b>"
    "</body>"
    "</html>";
    
