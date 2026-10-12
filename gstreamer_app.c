#include <stdio.h>

#include <gst/gst.h>

/**
 * @brief Number of buffers the test video source produces before sending EOS.
 */
#define VIDEO_TEST_SOURCE_NUM_BUFFERS 100

/**
 * @brief Maximum time in milliseconds to wait for the pipeline to finish.
 */
#define PIPELINE_WAIT_TIMEOUT_MS 5000

/**
 * @brief Status codes returned by the GStreamer application helpers.
 */
typedef enum {
        GSTREAMER_APP_STATUS_SUCCESS = 0,            /**< Operation completed successfully. */
        GSTREAMER_APP_STATUS_INIT_FAILED = 1,        /**< GStreamer initialisation failed. */
        GSTREAMER_APP_STATUS_ELEMENT_FAILED = 2,     /**< A pipeline element could not be created. */
        GSTREAMER_APP_STATUS_LINK_FAILED = 3,        /**< The two elements could not be linked. */
        GSTREAMER_APP_STATUS_PLAY_FAILED = 4,        /**< The pipeline failed to reach PLAYING. */
        GSTREAMER_APP_STATUS_STREAM_ERROR = 5,       /**< The pipeline reported a stream error. */
        GSTREAMER_APP_STATUS_TIMEOUT = 6             /**< The pipeline did not finish in time. */
} gstreamerAppStatus_e;

/**
 * @brief Build a GStreamer pipeline with two plugins.
 *
 * Creates a videotestsrc element (source plugin) and a fakesink element
 * (sink plugin), places them in a new pipeline and links them together.
 * The videotestsrc element is configured to produce a fixed number of
 * buffers so the pipeline terminates with EOS on its own.
 *
 * @param pipeline        Output parameter receiving the created pipeline.
 *                        Set to NULL on failure.
 * @param videoTestSource Output parameter receiving the videotestsrc element.
 * @param fakeSink        Output parameter receiving the fakesink element.
 *
 * @return GSTREAMER_APP_STATUS_SUCCESS on success, or a non-zero
 *         gstreamerAppStatus_e value on failure.
 */
static gstreamerAppStatus_e create_pipeline(GstElement **pipeline,
                                            GstElement **videoTestSource,
                                            GstElement **fakeSink)
{
        if ((pipeline == NULL) || (videoTestSource == NULL) || (fakeSink == NULL)) {
                return GSTREAMER_APP_STATUS_ELEMENT_FAILED;
        }

        *pipeline = gst_pipeline_new("two-plugin-pipeline");
        *videoTestSource = gst_element_factory_make("videotestsrc", "video-source");
        *fakeSink = gst_element_factory_make("fakesink", "fake-sink");

        if ((*pipeline == NULL) || (*videoTestSource == NULL) || (*fakeSink == NULL)) {
                fprintf(stderr, "Failed to create GStreamer elements\n");
                return GSTREAMER_APP_STATUS_ELEMENT_FAILED;
        }

        g_object_set(G_OBJECT(*videoTestSource),
                     "num-buffers", VIDEO_TEST_SOURCE_NUM_BUFFERS, NULL);

        gst_bin_add_many(GST_BIN(*pipeline), *videoTestSource, *fakeSink, NULL);

        if (gst_element_link(*videoTestSource, *fakeSink) != TRUE) {
                fprintf(stderr, "Failed to link videotestsrc to fakesink\n");
                return GSTREAMER_APP_STATUS_LINK_FAILED;
        }

        return GSTREAMER_APP_STATUS_SUCCESS;
}

/**
 * @brief Run the pipeline until EOS or error is reported on the bus.
 *
 * Sets the pipeline to the PLAYING state and polls the pipeline bus until
 * an end-of-stream or error message arrives, or until the wait timeout
 * expires.
 *
 * @param pipeline The pipeline to run. Must not be NULL.
 *
 * @return GSTREAMER_APP_STATUS_SUCCESS when EOS is reached,
 *         GSTREAMER_APP_STATUS_PLAY_FAILED when the state change fails,
 *         GSTREAMER_APP_STATUS_STREAM_ERROR when the bus reports an error,
 *         or GSTREAMER_APP_STATUS_TIMEOUT when no message arrives in time.
 */
static gstreamerAppStatus_e run_pipeline(GstElement *pipeline)
{
        if (pipeline == NULL) {
                return GSTREAMER_APP_STATUS_PLAY_FAILED;
        }

        if (gst_element_set_state(pipeline, GST_STATE_PLAYING) == GST_STATE_CHANGE_FAILURE) {
                fprintf(stderr, "Failed to set pipeline to PLAYING\n");
                return GSTREAMER_APP_STATUS_PLAY_FAILED;
        }

        GstBus *messageBus = gst_element_get_bus(pipeline);
        if (messageBus == NULL) {
                fprintf(stderr, "Failed to get pipeline bus\n");
                return GSTREAMER_APP_STATUS_STREAM_ERROR;
        }

        GstMessage *busMessage = gst_bus_timed_pop_filtered(
                messageBus, (GstClockTime)PIPELINE_WAIT_TIMEOUT_MS * GST_MSECOND,
                GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

        gstreamerAppStatus_e runStatus = GSTREAMER_APP_STATUS_SUCCESS;
        if (busMessage == NULL) {
                fprintf(stderr, "Timed out waiting for pipeline EOS\n");
                runStatus = GSTREAMER_APP_STATUS_TIMEOUT;
        } else if (GST_MESSAGE_TYPE(busMessage) == GST_MESSAGE_ERROR) {
                GError *streamError = NULL;
                gchar *debugInfo = NULL;
                gst_message_parse_error(busMessage, &streamError, &debugInfo);
                fprintf(stderr, "Pipeline error: %s (%s)\n",
                        streamError != NULL ? streamError->message : "unknown",
                        debugInfo != NULL ? debugInfo : "no debug info");
                g_clear_error(&streamError);
                g_free(debugInfo);
                runStatus = GSTREAMER_APP_STATUS_STREAM_ERROR;
        }

        if (busMessage != NULL) {
                gst_message_unref(busMessage);
        }
        gst_object_unref(messageBus);

        return runStatus;
}

/**
 * @brief Entry point for the GStreamer two-plugin application.
 *
 * Initialises GStreamer, builds a pipeline containing the videotestsrc and
 * fakesink plugins, runs it until end-of-stream and shuts it down cleanly.
 *
 * @return 0 on success, non-zero on failure.
 */
int main(void)
{
        GError *initError = NULL;
        if (gst_init_check(NULL, NULL, &initError) != TRUE) {
                fprintf(stderr, "GStreamer init failed: %s\n",
                        initError != NULL ? initError->message : "unknown");
                g_clear_error(&initError);
                return GSTREAMER_APP_STATUS_INIT_FAILED;
        }

        GstElement *pipeline = NULL;
        GstElement *videoTestSource = NULL;
        GstElement *fakeSink = NULL;

        gstreamerAppStatus_e appStatus =
                create_pipeline(&pipeline, &videoTestSource, &fakeSink);

        if (appStatus == GSTREAMER_APP_STATUS_SUCCESS) {
                appStatus = run_pipeline(pipeline);
        }

        if (pipeline != NULL) {
                gst_element_set_state(pipeline, GST_STATE_NULL);
                gst_object_unref(pipeline);
        }

        if (appStatus != GSTREAMER_APP_STATUS_SUCCESS) {
                return appStatus;
        }

        int bytesWritten = printf("GStreamer pipeline completed: videotestsrc -> fakesink\n");
        return (bytesWritten < 0) ? GSTREAMER_APP_STATUS_STREAM_ERROR : GSTREAMER_APP_STATUS_SUCCESS;
}
