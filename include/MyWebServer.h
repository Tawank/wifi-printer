#ifndef _MyWebServer_h_
#define _MyWebServer_h_
#include <WebServer.h>

void initServer();

void urlHandleIndex();
void urlHandleIndexPrint();
void urlHandleIndexImagePrint();
void urlHandleFileUploadPrint();
void urlHandleTicket();
void urlHandlePhoto();
void urlHandleNotFound();

#endif // _MyWebServer_h_