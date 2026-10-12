/**
 * @file webrtc_stream.h
 * @brief Public API for the WebRTC streaming demo application.
 *
 * Provides data types and function declarations for a WebRTC streaming
 * application that supports two configurable modes:
 * - GStreamer WebRTC mode (RTP media via webrtcbin)
 * - DataChannel mode (binary data via WebRTC DataChannels)
 *
 * The application detects a camera at runtime and falls back to an MP4
 * file when no camera is available.  Signaling is handled over WebSocket
 * using libsoup, and Google STUN servers are used for NAT traversal.
 */

#ifndef WEBRTC_STREAM_H
#define WEBRTC_STREAM_H

#define GST_USE_UNSTABLE_API
#include <gst/gst.h>

/**
 * @brief Google STUN server URI used for NAT traversal.
 */
#define STUN_SERVER "stun://stun.l.google.com:19302"

/**
 * @brief Default WebSocket signaling port.
 */
#define SIGNALING_PORT 8443

/**
 * @brief Default HTTP port for serving the browser client.
 */
#define HTTP_PORT 8080

/**
 * @brief Maximum length for file path strings.
 */
#define MAX_PATH_LEN 512

/**
 * @brief Maximum length for the pipeline description string.
 */
#define MAX_PIPELINE_DESC_LEN 2048

/**
 * @brief Number of seconds to wait for a pipeline EOS/error before timeout.
 */
#define PIPELINE_TIMEOUT_SEC 30

/**
 * @brief Status codes returned by the WebRTC streaming application.
 */
typedef enum {
        WEBRTC_STATUS_SUCCESS = 0,              /**< Operation completed successfully. */
        WEBRTC_STATUS_INVALID_ARGS = 1,         /**< Invalid command-line arguments. */
        WEBRTC_STATUS_INIT_FAILED = 2,          /**< GStreamer initialisation failed. */
        WEBRTC_STATUS_NO_MEDIA_SOURCE = 3,      /**< No camera and no MP4 fallback available. */
        WEBRTC_STATUS_PIPELINE_FAILED = 4,      /**< Pipeline creation or linking failed. */
        WEBRTC_STATUS_SIGNALING_FAILED = 5,     /**< WebSocket signaling server failed. */
        WEBRTC_STATUS_STREAM_ERROR = 6,         /**< Pipeline reported a streaming error. */
        WEBRTC_STATUS_TIMEOUT = 7               /**< Pipeline did not finish in time. */
} webrtcStatus_e;

/**
 * @brief Selectable streaming mode for the application.
 */
typedef enum {
        WEBRTC_MODE_GSTREAMER = 0,      /**< RTP media streaming via webrtcbin. */
        WEBRTC_MODE_DATACHANNEL = 1     /**< Binary data streaming via WebRTC DataChannels. */
} webrtcMode_e;

/**
 * @brief Configuration for a single WebRTC streaming session.
 */
typedef struct {
        webrtcMode_e mode;                      /**< Selected streaming mode. */
        int hasCamera;                          /**< Non-zero if a V4L2 camera is detected. */
        char mp4Path[MAX_PATH_LEN];             /**< Path to the fallback MP4 file. */
        int signalingPort;                      /**< WebSocket signaling port. */
        int httpPort;                           /**< HTTP port for browser client. */
} webrtcConfig_t;

/**
 * @brief Application runtime state.
 */
typedef struct {
        webrtcConfig_t config;                  /**< Session configuration. */
        GstElement *pipeline;                   /**< GStreamer pipeline (owned). */
        GstElement *webrtcbin;                  /**< The webrtcbin element (not owned separately). */
        GMainLoop *mainLoop;                    /**< GLib main loop (owned). */
} webrtcApp_t;

/**
 * @brief Detect whether a V4L2 camera device is available.
 *
 * Checks for the existence of /dev/video0.
 *
 * @return 1 if a camera is detected, 0 otherwise.
 */
int detect_camera(void);

/**
 * @brief Parse command-line arguments into a configuration structure.
 *
 * Recognised options:
 *   --mode gstreamer|datachannel   Select streaming mode (default: gstreamer).
 *   --mp4 PATH                     Path to fallback MP4 file.
 *   --port PORT                    WebSocket signaling port (default: 8443).
 *   --http-port PORT               HTTP serving port (default: 8080).
 *
 * @param argc  Argument count from main().
 * @param argv  Argument vector from main().
 * @param config Output configuration structure.
 *
 * @return WEBRTC_STATUS_SUCCESS on success, WEBRTC_STATUS_INVALID_ARGS on
 *         failure.
 */
webrtcStatus_e parse_args(int argc, char *argv[], webrtcConfig_t *config);

/**
 * @brief Initialise the application state.
 *
 * Allocates a GMainLoop and zeroes the pipeline pointers inside the
 * provided application structure.  The caller must have already filled
 * in app->config before calling this function.
 *
 * @param app Application state to initialise.
 *
 * @return WEBRTC_STATUS_SUCCESS on success, WEBRTC_STATUS_INIT_FAILED
 *         on failure.
 */
webrtcStatus_e init_app(webrtcApp_t *app);

/**
 * @brief Build the GStreamer WebRTC media-streaming pipeline.
 *
 * Creates a pipeline that reads from a camera (v4l2src) or an MP4 file,
 * encodes to H.264, and sends media through webrtcbin.  The webrtcbin
 * element is configured with the Google STUN server.
 *
 * @param app Application state containing configuration and receiving
 *            the constructed pipeline.
 *
 * @return WEBRTC_STATUS_SUCCESS on success, or a non-zero webrtcStatus_e
 *         value on failure.
 */
webrtcStatus_e build_gstreamer_pipeline(webrtcApp_t *app);

/**
 * @brief Build the WebRTC DataChannel streaming pipeline.
 *
 * Creates a minimal webrtcbin pipeline and opens a DataChannel for
 * binary data transfer.  The webrtcbin element is configured with the
 * Google STUN server.
 *
 * @param app Application state containing configuration and receiving
 *            the constructed pipeline.
 *
 * @return WEBRTC_STATUS_SUCCESS on success, or a non-zero webrtcStatus_e
 *         value on failure.
 */
webrtcStatus_e build_datachannel_pipeline(webrtcApp_t *app);

/**
 * @brief Run the constructed pipeline until completion or error.
 *
 * Sets the pipeline to PLAYING, runs the GLib main loop, and waits for
 * EOS or an error on the bus.  Returns when the pipeline finishes or a
 * timeout occurs.
 *
 * @param app Application state with a built pipeline.
 *
 * @return WEBRTC_STATUS_SUCCESS on clean EOS, or a non-zero
 *         webrtcStatus_e value on error/timeout.
 */
webrtcStatus_e run_pipeline(webrtcApp_t *app);

/**
 * @brief Clean up all resources held by the application.
 *
 * Sets the pipeline to NULL, unrefs GStreamer objects and the main loop.
 *
 * @param app Application state to clean up.
 */
void cleanup_app(webrtcApp_t *app);

/**
 * @brief Print usage information to stdout.
 *
 * @param programName The name of the executable (argv[0]).
 */
void print_usage(const char *programName);

#endif /* WEBRTC_STREAM_H */
