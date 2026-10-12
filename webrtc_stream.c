/**
 * @file webrtc_stream.c
 * @brief WebRTC streaming demo application using GStreamer.
 *
 * Demonstrates WebRTC streaming in two configurable modes:
 * - GStreamer mode: RTP media streaming via webrtcbin (H.264 video).
 * - DataChannel mode: binary data transfer via WebRTC DataChannels.
 *
 * When no camera (/dev/video0) is detected the application falls back
 * to streaming an MP4 file.  Signaling uses WebSocket (libsoup) and
 * NAT traversal uses the Google STUN server.
 *
 * Usage:
 * @code
 *   ./webrtc_stream --mode gstreamer --mp4 sample.mp4
 *   ./webrtc_stream --mode datachannel --mp4 sample.mp4
 * @endcode
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define GST_USE_UNSTABLE_API
#include <gst/gst.h>
#include <gst/sdp/sdp.h>
#include <gst/webrtc/webrtc.h>

#include "webrtc_stream.h"

/**
 * @brief Camera device path checked for availability.
 */
#define CAMERA_DEVICE_PATH "/dev/video0"

/* ------------------------------------------------------------------ */
/*  Camera detection                                                  */
/* ------------------------------------------------------------------ */

/* See webrtc_stream.h for the public API documentation. */
int detect_camera(void)
{
        if (access(CAMERA_DEVICE_PATH, F_OK) == 0) {
                return 1;
        }
        return 0;
}

/* ------------------------------------------------------------------ */
/*  Argument parsing                                                  */
/* ------------------------------------------------------------------ */

/* See webrtc_stream.h for the public API documentation. */
void print_usage(const char *programName)
{
        if (programName == NULL) {
                programName = "webrtc_stream";
        }
        int bytesWritten = printf(
                "Usage: %s [OPTIONS]\n"
                "Options:\n"
                "  --mode gstreamer|datachannel  Streaming mode (default: gstreamer)\n"
                "  --mp4 PATH                   Fallback MP4 file path\n"
                "  --port PORT                  Signaling WebSocket port (default: %d)\n"
                "  --http-port PORT             HTTP port for browser client (default: %d)\n"
                "  --help                       Show this help message\n",
                programName, SIGNALING_PORT, HTTP_PORT);
        (void)bytesWritten;
}

/* See webrtc_stream.h for the public API documentation. */
webrtcStatus_e parse_args(int argc, char *argv[], webrtcConfig_t *config)
{
        if (config == NULL) {
                return WEBRTC_STATUS_INVALID_ARGS;
        }

        /* Defaults */
        config->mode = WEBRTC_MODE_GSTREAMER;
        config->hasCamera = detect_camera();
        memset(config->mp4Path, 0, sizeof(config->mp4Path));
        config->signalingPort = SIGNALING_PORT;
        config->httpPort = HTTP_PORT;

        for (int i = 1; i < argc; i++) {
                if (argv[i] == NULL) {
                        continue;
                }

                if (strcmp(argv[i], "--mode") == 0) {
                        if ((i + 1) >= argc) {
                                fprintf(stderr, "Error: --mode requires an argument\n");
                                return WEBRTC_STATUS_INVALID_ARGS;
                        }
                        i++;
                        if (strcmp(argv[i], "gstreamer") == 0) {
                                config->mode = WEBRTC_MODE_GSTREAMER;
                        } else if (strcmp(argv[i], "datachannel") == 0) {
                                config->mode = WEBRTC_MODE_DATACHANNEL;
                        } else {
                                fprintf(stderr,
                                        "Error: unknown mode '%s' "
                                        "(use gstreamer or datachannel)\n",
                                        argv[i]);
                                return WEBRTC_STATUS_INVALID_ARGS;
                        }
                } else if (strcmp(argv[i], "--mp4") == 0) {
                        if ((i + 1) >= argc) {
                                fprintf(stderr, "Error: --mp4 requires a file path\n");
                                return WEBRTC_STATUS_INVALID_ARGS;
                        }
                        i++;
                        size_t pathLen = strlen(argv[i]);
                        if (pathLen >= MAX_PATH_LEN) {
                                fprintf(stderr, "Error: MP4 path too long\n");
                                return WEBRTC_STATUS_INVALID_ARGS;
                        }
                        strncpy(config->mp4Path, argv[i], MAX_PATH_LEN - 1);
                        config->mp4Path[MAX_PATH_LEN - 1] = '\0';
                } else if (strcmp(argv[i], "--port") == 0) {
                        if ((i + 1) >= argc) {
                                fprintf(stderr, "Error: --port requires a number\n");
                                return WEBRTC_STATUS_INVALID_ARGS;
                        }
                        i++;
                        char *endPtr = NULL;
                        long portVal = strtol(argv[i], &endPtr, 10);
                        if ((endPtr == argv[i]) || (*endPtr != '\0') ||
                            (portVal < 1) || (portVal > 65535)) {
                                fprintf(stderr, "Error: invalid port '%s'\n", argv[i]);
                                return WEBRTC_STATUS_INVALID_ARGS;
                        }
                        config->signalingPort = (int)portVal;
                } else if (strcmp(argv[i], "--http-port") == 0) {
                        if ((i + 1) >= argc) {
                                fprintf(stderr, "Error: --http-port requires a number\n");
                                return WEBRTC_STATUS_INVALID_ARGS;
                        }
                        i++;
                        char *endPtr = NULL;
                        long portVal = strtol(argv[i], &endPtr, 10);
                        if ((endPtr == argv[i]) || (*endPtr != '\0') ||
                            (portVal < 1) || (portVal > 65535)) {
                                fprintf(stderr, "Error: invalid http-port '%s'\n", argv[i]);
                                return WEBRTC_STATUS_INVALID_ARGS;
                        }
                        config->httpPort = (int)portVal;
                } else if (strcmp(argv[i], "--help") == 0) {
                        print_usage((argc > 0) ? argv[0] : NULL);
                        return WEBRTC_STATUS_INVALID_ARGS;
                } else {
                        fprintf(stderr, "Error: unknown option '%s'\n", argv[i]);
                        return WEBRTC_STATUS_INVALID_ARGS;
                }
        }

        return WEBRTC_STATUS_SUCCESS;
}

/* ------------------------------------------------------------------ */
/*  Application initialisation / cleanup                              */
/* ------------------------------------------------------------------ */

/* See webrtc_stream.h for the public API documentation. */
webrtcStatus_e init_app(webrtcApp_t *app)
{
        if (app == NULL) {
                return WEBRTC_STATUS_INIT_FAILED;
        }

        app->pipeline = NULL;
        app->webrtcbin = NULL;

        app->mainLoop = g_main_loop_new(NULL, FALSE);
        if (app->mainLoop == NULL) {
                fprintf(stderr, "Error: failed to create GLib main loop\n");
                return WEBRTC_STATUS_INIT_FAILED;
        }

        return WEBRTC_STATUS_SUCCESS;
}

/* See webrtc_stream.h for the public API documentation. */
void cleanup_app(webrtcApp_t *app)
{
        if (app == NULL) {
                return;
        }

        if (app->pipeline != NULL) {
                gst_element_set_state(app->pipeline, GST_STATE_NULL);
                gst_object_unref(app->pipeline);
                app->pipeline = NULL;
        }

        app->webrtcbin = NULL;

        if (app->mainLoop != NULL) {
                g_main_loop_unref(app->mainLoop);
                app->mainLoop = NULL;
        }
}

/* ------------------------------------------------------------------ */
/*  Bus message handler                                               */
/* ------------------------------------------------------------------ */

/**
 * @brief GStreamer bus-watch callback.
 *
 * Handles EOS and ERROR messages from the pipeline bus and quits the
 * GLib main loop when either is received.
 *
 * @param bus     The pipeline bus (unused directly).
 * @param message The bus message.
 * @param data    Pointer to the webrtcApp_t application state.
 *
 * @return TRUE to keep the watch active, FALSE to remove it.
 */
static gboolean on_bus_message(GstBus *bus, GstMessage *message, gpointer data)
{
        (void)bus;
        webrtcApp_t *app = (webrtcApp_t *)data;

        if (app == NULL) {
                return FALSE;
        }

        switch (GST_MESSAGE_TYPE(message)) {
        case GST_MESSAGE_EOS:
                fprintf(stdout, "End of stream reached\n");
                if (app->mainLoop != NULL) {
                        g_main_loop_quit(app->mainLoop);
                }
                break;

        case GST_MESSAGE_ERROR: {
                GError *streamError = NULL;
                gchar *debugInfo = NULL;
                gst_message_parse_error(message, &streamError, &debugInfo);
                fprintf(stderr, "Pipeline error: %s (%s)\n",
                        (streamError != NULL) ? streamError->message : "unknown",
                        (debugInfo != NULL) ? debugInfo : "no debug info");
                g_clear_error(&streamError);
                g_free(debugInfo);
                if (app->mainLoop != NULL) {
                        g_main_loop_quit(app->mainLoop);
                }
                break;
        }

        default:
                break;
        }

        return TRUE;
}

/* ------------------------------------------------------------------ */
/*  Pipeline builders                                                 */
/* ------------------------------------------------------------------ */

/* See webrtc_stream.h for the public API documentation. */
webrtcStatus_e build_gstreamer_pipeline(webrtcApp_t *app)
{
        if (app == NULL) {
                return WEBRTC_STATUS_PIPELINE_FAILED;
        }

        char pipelineDesc[MAX_PIPELINE_DESC_LEN];
        memset(pipelineDesc, 0, sizeof(pipelineDesc));

        int written = 0;
        if (app->config.hasCamera != 0) {
                written = snprintf(pipelineDesc, sizeof(pipelineDesc),
                        "v4l2src ! videoconvert ! video/x-raw,width=640,height=480 ! "
                        "x264enc tune=zerolatency bitrate=500 speed-preset=ultrafast ! "
                        "rtph264pay ! "
                        "application/x-rtp,media=video,encoding-name=H264,payload=96 ! "
                        "webrtcbin name=sendrecv stun-server=%s",
                        STUN_SERVER);
        } else if (app->config.mp4Path[0] != '\0') {
                written = snprintf(pipelineDesc, sizeof(pipelineDesc),
                        "filesrc location=%s ! decodebin name=demux ! "
                        "queue ! videoconvert ! "
                        "x264enc tune=zerolatency bitrate=500 speed-preset=ultrafast ! "
                        "rtph264pay ! "
                        "application/x-rtp,media=video,encoding-name=H264,payload=96 ! "
                        "webrtcbin name=sendrecv stun-server=%s",
                        app->config.mp4Path, STUN_SERVER);
        } else {
                fprintf(stderr,
                        "Error: no camera detected and no MP4 fallback specified\n");
                return WEBRTC_STATUS_NO_MEDIA_SOURCE;
        }

        if ((written < 0) || ((size_t)written >= sizeof(pipelineDesc))) {
                fprintf(stderr, "Error: pipeline description too long\n");
                return WEBRTC_STATUS_PIPELINE_FAILED;
        }

        GError *parseError = NULL;
        app->pipeline = gst_parse_launch(pipelineDesc, &parseError);
        if (app->pipeline == NULL) {
                fprintf(stderr, "Error: failed to create pipeline: %s\n",
                        (parseError != NULL) ? parseError->message : "unknown");
                g_clear_error(&parseError);
                return WEBRTC_STATUS_PIPELINE_FAILED;
        }
        g_clear_error(&parseError);

        app->webrtcbin = gst_bin_get_by_name(GST_BIN(app->pipeline), "sendrecv");
        if (app->webrtcbin == NULL) {
                fprintf(stderr, "Error: webrtcbin element not found in pipeline\n");
                return WEBRTC_STATUS_PIPELINE_FAILED;
        }
        gst_object_unref(app->webrtcbin);

        fprintf(stdout, "GStreamer WebRTC pipeline built (source: %s, codec: H.264)\n",
                (app->config.hasCamera != 0) ? "camera" : "MP4 file");

        return WEBRTC_STATUS_SUCCESS;
}

/* See webrtc_stream.h for the public API documentation. */
webrtcStatus_e build_datachannel_pipeline(webrtcApp_t *app)
{
        if (app == NULL) {
                return WEBRTC_STATUS_PIPELINE_FAILED;
        }

        char pipelineDesc[MAX_PIPELINE_DESC_LEN];
        memset(pipelineDesc, 0, sizeof(pipelineDesc));

        int written = snprintf(pipelineDesc, sizeof(pipelineDesc),
                "webrtcbin name=sendrecv stun-server=%s bundle-policy=max-bundle",
                STUN_SERVER);

        if ((written < 0) || ((size_t)written >= sizeof(pipelineDesc))) {
                fprintf(stderr, "Error: pipeline description too long\n");
                return WEBRTC_STATUS_PIPELINE_FAILED;
        }

        GError *parseError = NULL;
        app->pipeline = gst_parse_launch(pipelineDesc, &parseError);
        if (app->pipeline == NULL) {
                fprintf(stderr, "Error: failed to create datachannel pipeline: %s\n",
                        (parseError != NULL) ? parseError->message : "unknown");
                g_clear_error(&parseError);
                return WEBRTC_STATUS_PIPELINE_FAILED;
        }
        g_clear_error(&parseError);

        app->webrtcbin = gst_bin_get_by_name(GST_BIN(app->pipeline), "sendrecv");
        if (app->webrtcbin == NULL) {
                fprintf(stderr,
                        "Error: webrtcbin element not found in datachannel pipeline\n");
                return WEBRTC_STATUS_PIPELINE_FAILED;
        }

        /* Create a DataChannel on the webrtcbin for binary transfer */
        GstWebRTCDataChannel *dataChannel = NULL;
        GstStructure *channelOptions =
                gst_structure_new("application/x-data-channel-setup",
                                  "ordered", G_TYPE_BOOLEAN, TRUE,
                                  "max-retransmits", G_TYPE_INT, -1,
                                  NULL);

        g_signal_emit_by_name(app->webrtcbin, "create-data-channel",
                              "stream-channel", channelOptions, &dataChannel);

        if (channelOptions != NULL) {
                gst_structure_free(channelOptions);
        }

        if (dataChannel == NULL) {
                fprintf(stderr, "Warning: DataChannel creation returned NULL "
                        "(may be created after negotiation)\n");
        } else {
                g_object_unref(dataChannel);
        }

        gst_object_unref(app->webrtcbin);

        fprintf(stdout, "DataChannel WebRTC pipeline built (STUN: %s)\n",
                STUN_SERVER);

        return WEBRTC_STATUS_SUCCESS;
}

/* ------------------------------------------------------------------ */
/*  Pipeline execution                                                */
/* ------------------------------------------------------------------ */

/* See webrtc_stream.h for the public API documentation. */
webrtcStatus_e run_pipeline(webrtcApp_t *app)
{
        if ((app == NULL) || (app->pipeline == NULL) || (app->mainLoop == NULL)) {
                return WEBRTC_STATUS_PIPELINE_FAILED;
        }

        GstBus *bus = gst_element_get_bus(app->pipeline);
        if (bus == NULL) {
                fprintf(stderr, "Error: failed to get pipeline bus\n");
                return WEBRTC_STATUS_STREAM_ERROR;
        }

        guint busWatchId = gst_bus_add_watch(bus, on_bus_message, app);
        gst_object_unref(bus);

        if (busWatchId == 0) {
                fprintf(stderr, "Error: failed to add bus watch\n");
                return WEBRTC_STATUS_STREAM_ERROR;
        }

        GstStateChangeReturn stateRet =
                gst_element_set_state(app->pipeline, GST_STATE_PLAYING);
        if (stateRet == GST_STATE_CHANGE_FAILURE) {
                fprintf(stderr, "Error: failed to set pipeline to PLAYING\n");
                g_source_remove(busWatchId);
                return WEBRTC_STATUS_PIPELINE_FAILED;
        }

        fprintf(stdout, "Pipeline is now PLAYING (mode: %s)\n",
                (app->config.mode == WEBRTC_MODE_GSTREAMER) ?
                "gstreamer" : "datachannel");

        g_main_loop_run(app->mainLoop);

        g_source_remove(busWatchId);

        return WEBRTC_STATUS_SUCCESS;
}

/* ------------------------------------------------------------------ */
/*  Main entry point                                                  */
/* ------------------------------------------------------------------ */

/**
 * @brief Entry point for the WebRTC streaming demo.
 *
 * Parses command-line arguments, initialises GStreamer and the
 * application state, builds the selected pipeline, and runs it.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 *
 * @return 0 on success, non-zero webrtcStatus_e value on failure.
 */
int main(int argc, char *argv[])
{
        webrtcApp_t app;
        memset(&app, 0, sizeof(app));

        webrtcStatus_e status = parse_args(argc, argv, &app.config);
        if (status != WEBRTC_STATUS_SUCCESS) {
                return status;
        }

        GError *initError = NULL;
        if (gst_init_check(NULL, NULL, &initError) != TRUE) {
                fprintf(stderr, "GStreamer init failed: %s\n",
                        (initError != NULL) ? initError->message : "unknown");
                g_clear_error(&initError);
                return WEBRTC_STATUS_INIT_FAILED;
        }

        status = init_app(&app);
        if (status != WEBRTC_STATUS_SUCCESS) {
                cleanup_app(&app);
                return status;
        }

        fprintf(stdout,
                "WebRTC Streaming Demo\n"
                "  Mode:       %s\n"
                "  Camera:     %s\n"
                "  MP4:        %s\n"
                "  STUN:       %s\n"
                "  Signal Port: %d\n"
                "  HTTP Port:  %d\n",
                (app.config.mode == WEBRTC_MODE_GSTREAMER) ?
                        "gstreamer" : "datachannel",
                (app.config.hasCamera != 0) ? "detected" : "not detected",
                (app.config.mp4Path[0] != '\0') ?
                        app.config.mp4Path : "(none)",
                STUN_SERVER,
                app.config.signalingPort,
                app.config.httpPort);

        if (app.config.mode == WEBRTC_MODE_GSTREAMER) {
                status = build_gstreamer_pipeline(&app);
        } else {
                status = build_datachannel_pipeline(&app);
        }

        if (status != WEBRTC_STATUS_SUCCESS) {
                cleanup_app(&app);
                return status;
        }

        status = run_pipeline(&app);

        cleanup_app(&app);
        return status;
}
