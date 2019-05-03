//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// transport.h: defines functions provided by the transport layer.
//

#pragma once

#include <stdbool.h>

#include "tokenstore.h"
#include "dnscache.h"
#include "uspcommon.h"
#include "metrics.h"
#include "audio_chunk.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KEYWORD_PATH "Path"

typedef struct _TransportRequest* TransportHandle;

typedef struct HTTP_HEADERS_HANDLE_DATA_TAG* HTTP_HEADERS_HANDLE;

/**
 * Creates a new transport request.
 * @param host The host name.
 * @param proxyConfig The proxy configuration info.
 * @param context The application defined context that will be passed back during callback.
 * @param telemetry Telemetry handle to record various transport events.
 * @param connectionHeaders A handle to headers that will be used to establish a connection.
 * @param connectionId An identifier of a connection, used for diagnostics of errors on the server side.
 * @param disable_default_verify_paths OpenSSL only: disable the default verify paths
 * @param trustedCert OpenSSL only: single trusted cert
 * @param disable_crl_check OpenSSL only: if true, disable CRL if using single trusted cert.
 * @return A new transport handle.
 */
TransportHandle TransportRequestCreate(const char* host, void* context, Microsoft::CognitiveServices::Speech::USP::Telemetry* telemetry, HTTP_HEADERS_HANDLE connectionHeaders, const char* connectionId, const ProxyServerInfo* proxyInfo, const bool disable_default_verify_paths, const char *trustedCert, const bool disable_crl_check);

/**
 * Destroys a transport request.
 * @param transportHandle The transport handle representing the request to be destroyed.
 */
void TransportRequestDestroy(TransportHandle transportHandle);

/**
 * Prepares the start of a new transport request.
 * @param transportHandle The request to prepare.
 * @return A return code or zero if successful.
 */
int TransportRequestPrepare(TransportHandle transportHandle);

/**
 * Prepares the start of a new transport stream.
 * @param transportHandle The request to prepare.
 * @return A return code or zero if successful.
 */
int TransportStreamPrepare(TransportHandle transportHandle);

/**
 * Write a text message to the websocket.
 * @param transportHandle The request to prepare.
 * @param path The path to use for message
 * @param buffer The buffer to write to the websocket.
 * @param bufferSize The byte size of the buffer.
 * @param requestId The requestId for the given message.
 * @return A return code or zero if successful.
 */
int TransportMessageWrite(TransportHandle transportHandle, const std::string& path, const uint8_t* buffer, size_t bufferSize, const char* requestId);

/**
 * Writes to the transport stream.
 * @param transportHandle The request to prepare.
 * @param path The path to use for message.
 * @param buffer The audio chunk to be sent.
 * @param requestId The requestId for the current stream.
 * @return A return code or zero if successful.
 */
int TransportStreamWrite(TransportHandle transportHandle, const std::string& path, const Microsoft::CognitiveServices::Speech::Impl::DataChunkPtr& audioChunk, const char* requestId);

/**
 * Flushes any outstanding I/O on the transport stream.
 * @param transportHandle The request to prepare.
 * @param path The path to use for message.
 * @param requestId The requestId for the current stream.
 * @return A return code or zero if successful.
 */
int TransportStreamFlush(TransportHandle transportHandle, const std::string& path, const char* requestId);

/**
 * Processes any outstanding operations that need attention.
 * @param transportHandle The request to process.
 */
void TransportDoWork(TransportHandle transportHandle);

/**
 * The TransportOpenedCallback type represents an application-defined
 * status callback function used for signaling when the transport has been opened.
 * @param context A pointer to the application-defined callback context.
 */
typedef void(*TransportOpenedCallback)(void* context);

/**
 * The TransportClosedCallback type represents an application-defined
 * status callback function used for signaling when the transport has been closed.
 * @param context A pointer to the application-defined callback context.
 */
typedef void(*TransportClosedCallback)(void* context);

typedef enum _TransportError
{
    TRANSPORT_ERROR_UNKNOWN,
    TRANSPORT_ERROR_REMOTE_CLOSED,
    TRANSPORT_ERROR_CONNECTION_FAILURE,
    TRANSPORT_ERROR_WEBSOCKET_UPGRADE,
    TRANSPORT_ERROR_WEBSOCKET_SEND_FRAME,
    TRANSPORT_ERROR_WEBSOCKET_ERROR,
    TRANSPORT_ERROR_DNS_FAILURE
} TransportError;

typedef enum _HttpStatusCode
{
    HTTP_BADREQUEST = 400,
    HTTP_UNAUTHORIZED = 401,
    HTTP_FORBIDDEN = 403,
    HTTP_TOO_MANY_REQUESTS = 429,
} HttpStatusCode;

/** Transport error with additional information.
 *
 * For internal documentation, here is a rough overview on what errorCode contains:
 *
 * For TRANSPORT_ERROR_REMOTE_CLOSED, it should be a WebSocket close code if
 * there was one or -1 if none was received. See CLOSE_* in uws_client.h or
 * WebSocket RFC.
 *
 * For TRANSPORT_ERROR_CONNECTION_FAILURE, it is a WS_OPEN_RESULT. (The
 * errorString should contain an internal numerical code as well. There are
 * a variety of sources for this one, including native socket operation errors.)
 *
 * For TRANSPORT_ERROR_WEBSOCKET_UPGRADE, it is an HTTP status code != 101,
 * received as a upgrade response.
 *
 * For TRANSPORT_ERROR_WEBSOCKET_SEND_FRAME and an HTTP connection it's -1.
 *
 * For TRANSPORT_ERROR_WEBSOCKET_SEND_FRAME and an WebSocket connection it's 1
 * or a line number where the error originated (e.g., from uws_client.c).
 *
 * For TRANSPORT_ERROR_WEBSOCKET_ERROR, it is a WS_ERROR.
 *
 * For TRANSPORT_ERROR_DNS_FAILURE, which cannot trigger on Windows, is is a
 * getaddrinfo() return value.
 */
typedef struct _TransportErrorInfo
{
    TransportError reason;
    int errorCode;
    const char *errorString;
} TransportErrorInfo;

/**
 * The TransportErrorCallback type represents an application-defined
 * status callback function used for signaling when the transport has failed.
 * @param errorInfo Pointer to struct containing transport error information (or NULL).
 * @param context A pointer to the application-defined callback context.
 */
typedef void(*TransportErrorCallback)(TransportErrorInfo* errorInfo, void* context);

typedef enum _ResponseFrameType
{
    FRAME_TYPE_UNKNOWN = 0,
    FRAME_TYPE_TEXT,
    FRAME_TYPE_BINARY
} ResponseFrameType;

typedef struct _TransportResponse
{
    ResponseFrameType frameType;
    HTTP_HEADERS_HANDLE responseHeader;
    const unsigned char* buffer;
    size_t bufferSize;
} TransportResponse;

/**
 * The TransportReponseCallback type represents an application-defined
 * status callback function used for signaling when data has been received.
 * @param response Pointer to struct containing response information.
 * @param context A pointer to the application-defined callback context.
 */
typedef void(*TransportResponseCallback)(TransportResponse* response, void* context);

/**
 * Registers for events from the transport.
 * @param transportHandle The request to prepare.
 * @param errorCallback The error callback.
 * @param recvCallback The response callback.
 * @param openedCallback The callback indicating that the transport has been opened.
 * @param closedCallback The callback indicating that the transport has been closed.
 * @return A return code or zero if successful.
 */
int TransportSetCallbacks(TransportHandle transportHandle,
    TransportErrorCallback errorCallback,
    TransportResponseCallback recvCallback,
    TransportOpenedCallback openedCallback,
    TransportClosedCallback closedCallback);

/**
 * Enables authorization header on transport.
 * @param transportHandle The request to set.
 * @param token_store The token store to pull tokens from.
 * @return A return code or zero if successful.
 */
int TransportSetTokenStore(TransportHandle transportHandle, TokenStore token_store);

/**
* Sets the DNS cache on transport
* @param transportHandle The transport handle.
* @param dnsCache The DNS cache handle.
*/
void TransportSetDnsCache(TransportHandle transportHandle, DnsCacheHandle dnsCache);

/**
 * Sends the provided buffer content as a telemetry event (using 'telemetry' message path).
 * @param transportHandle Transport handle.
 * @param buffer The buffer to write.
 * @param bytesToWrite Size of the buffer in bytes.
 * @param requestId Request id to tag the telemetry message with.
 * @return A return code or zero if successful.
 */
void TransportWriteTelemetry(TransportHandle transportHandle, const uint8_t* buffer, size_t bytesToWrite, const char *requestId);

#ifdef __cplusplus
}
#endif
