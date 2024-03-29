/*
 * File: server.h
 * --------------
 * This file exports a set of functions that implement a simple HTTP server
 * that can listen for connections.
 * 
 * @version 2016/03/16
 * - initial version
 */

#include "private/init.h"   // ensure that Stanford C++ lib is initialized

#ifndef INTERNAL_INCLUDE
#include "private/initstudent.h"   // insert necessary included code by student
#endif // INTERNAL_INCLUDE

#ifndef _server_h
#define _server_h

#include <iostream>
#include <string>

#define INTERNAL_INCLUDE 1
#include "../graphics/gevents.h"
#undef INTERNAL_INCLUDE

namespace HttpServer {
const int DEFAULT_PORT = 8080;
const int HTTP_ERROR_OK = 200;

std::string getContentType(const std::string& extension);
std::string getErrorMessage(int httpErrorCode);
std::string getUrlExtension(const std::string& url);
bool isRunning();
void sendResponse(const GEvent& event, const std::string& responseText,
                  const std::string& contentType = "");
void sendResponseError(const GEvent& event, int httpErrorCode,
                  const std::string& errorMessage = "");
void sendResponseFile(const GEvent& event, const std::string& responseFilePath,
                      const std::string& contentType = "" /* auto */);
void startServer(int port = DEFAULT_PORT);
void stopServer();
} // namespace HttpServer

#endif // _server_h
