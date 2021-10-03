#ifndef _printer_web_server_h_
#define _printer_web_server_h_
#include <WebServer.h>

void initServer();

void urlHandleIndex();
void urlHandleIndexPrint();
void urlHandleIndexImagePrint();
void urlHandleFileUploadPrint();
void urlHandleTicket();
void urlHandlePhoto();
void urlHandleNotFound();

#endif // _printer_web_server_h_