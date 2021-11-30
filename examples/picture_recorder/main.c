//
//  main.c
//
//  Created by Daniel Owsiański on 20/11/2021.
//

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif

// GLFW_INCLUDE_GLCOREARB makes the GLFW header include the modern OpenGL
#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>  // EXIT_SUCCESS, EXIT_FAILURE

#include "sk_capi.h"

//#define USES_CIRCLES
//#define USES_SHADOW
#define NUMBER_OF_SHAPES 6000


#ifdef USES_SHADOW
#define WITH_SHADOW(drawCall) drawCall
#else
#define WITH_SHADOW(drawCall)
#endif

#if NUMBER_OF_SHAPES > 100 && defined(USES_SHADOW)
#warning !?!? Using shadows with a large number of shapes will push the FPS to an incredibly low level

#endif

#define TARGET_FPS 60.0

int winWidth = 512;
int winHeight = 512;

typedef struct {
    sk_picture_t* picture;
    sk_point_t position;
    int size;
    float speed;
} shape;

shape shapes[NUMBER_OF_SHAPES];

int randomNumber(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

void setup() {
    sk_paint_t* strokePaint = sk_paint_new();
    sk_paint_set_style(strokePaint, SK_PAINT_STYLE_STROKE);
    sk_paint_set_color(strokePaint, 0x40000000);
    sk_paint_set_stroke_width(strokePaint, 1);
    sk_paint_set_antialias(strokePaint, true);

    sk_paint_t* fillPaint = sk_paint_new();
    sk_paint_set_style(fillPaint, SK_PAINT_STYLE_FILL);
    sk_paint_set_color(fillPaint, 0xc0FFFFFF);
    sk_paint_set_antialias(fillPaint, true);

#ifdef USES_SHADOW
    sk_paint_t* blurPaint = sk_paint_clone(fillPaint);
    sk_paint_set_color(blurPaint, 0x80000000);
    sk_mask_filter_t* filter = sk_maskfilter_new_blur_with_flags(SK_BLUR_STYLE_OUTER, 2 * 3.4, false);
    sk_paint_set_maskfilter(blurPaint, filter);
#endif

    sk_picture_recorder_t* recorder = sk_picture_recorder_new();

    for (int i = 0; i < NUMBER_OF_SHAPES; i++) {
        int size = randomNumber(5, 50);
        shapes[i].size = size;
        shapes[i].position.x = winWidth + randomNumber(0, winWidth) - size / 2;
        shapes[i].position.y = randomNumber(0, winHeight) - size / 2;

        shapes[i].speed = 1 / (size * 1.0) + (randomNumber(1, 200) / 100.0);

        sk_rect_t strokeRect = {.left = -1, .top = -1, .right = -1 + size + 2, .bottom = -1 + size + 2};
        sk_rect_t fillRect = {.left = 0, .top = 0, .right = 0 + size, .bottom = 0 + size};

        sk_canvas_t* canvas = sk_picture_recorder_begin_recording(recorder, &strokeRect);
#ifdef USES_CIRCLES
        sk_canvas_draw_oval(canvas, &strokeRect, strokePaint);
        sk_canvas_draw_oval(canvas, &fillRect, fillPaint);
        WITH_SHADOW(sk_canvas_draw_oval(canvas, &strokeRect, blurPaint));

#else
        sk_canvas_draw_rect(canvas, &strokeRect, strokePaint);
        sk_canvas_draw_rect(canvas, &fillRect, fillPaint);
        WITH_SHADOW(sk_canvas_draw_rect(canvas, &strokeRect, blurPaint));
#endif

        shapes[i].picture = sk_picture_recorder_end_recording(recorder);
    }

#ifdef USES_SHADOW
    sk_paint_delete(blurPaint);
#endif

    sk_paint_delete(fillPaint);
    sk_paint_delete(strokePaint);
    sk_picture_recorder_delete(recorder);
}

void draw(sk_canvas_t* canvas) {
    sk_canvas_draw_color(canvas, 0xFFFFFFFF, SK_BLEND_MODE_SRCOVER);

    for (int i = 0; i < NUMBER_OF_SHAPES; i++) {
        shapes[i].position.x -= shapes[i].speed;
        sk_canvas_save(canvas);
        sk_canvas_translate(canvas, shapes[i].position.x, shapes[i].position.y);
        sk_canvas_draw_picture(canvas, shapes[i].picture, NULL, NULL);
        sk_canvas_restore(canvas);

        if (shapes[i].position.x + shapes[i].size < 0) {
            shapes[i].position.x = winWidth + shapes[i].size;
            shapes[i].speed = 0.5 + (randomNumber(1, 200) / 100.0);
        }
    }
}

static gr_direct_context_t* makeSkiaContext(void);
static sk_surface_t* newSurface(gr_direct_context_t* context, const int w, const int h);

int main(int argc, char** argv) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = glfwCreateWindow(winWidth, winHeight, "Canvas Paint", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to open GLFW window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    int frames = 0;
    double t, t0, fps;
    char titleBuffer[10];
    t = t0 = fps = 0.0;

    glfwMakeContextCurrent(window);
    gr_direct_context_t* context = makeSkiaContext();
    glfwSwapInterval(1);

    setup();

    while (!glfwWindowShouldClose(window)) {
        double start = glfwGetTime();

        float contentScaleX, contentScaleY;
        double canvasWidth, canvasHeight;
        glfwGetWindowSize(window, &winWidth, &winHeight);
        glfwGetWindowContentScale(window, &contentScaleX, &contentScaleY);

        canvasWidth = winWidth * contentScaleX;
        canvasHeight = winHeight * contentScaleY;

        // Surface is cheap(ish?) to create src: https://groups.google.com/g/skia-discuss/c/3c10MvyaSug
        sk_surface_t* surface = newSurface(context, canvasWidth, canvasHeight);

        sk_canvas_t* canvas = sk_surface_get_canvas(surface);
        sk_canvas_scale(canvas, contentScaleX, contentScaleY);

        draw(canvas);

        sk_canvas_flush(canvas);
        glfwSwapBuffers(window);
        sk_surface_unref(surface);


        glfwPollEvents();
        // Alternative: use wait with timeout instead of glfwPollEvents() to lower load on CPU
        // double timeout = 1 / TARGET_FPS - (glfwGetTime() - start);
        // if (timeout < 0.0) {
        //     timeout = 1 / TARGET_FPS;
        // }
        // glfwWaitEventsTimeout(timeout);

        // FPS
        t = glfwGetTime();
        if ((t - t0) > 0.5 || frames == 0) {
            fps = (double)frames / (t - t0);
            snprintf(titleBuffer, sizeof(titleBuffer), "FPS: %.2f", fps);
            glfwSetWindowTitle(window, titleBuffer);
            t0 = t;
            frames = 0;
        }
        frames++;
    }

    if (context) {
        gr_direct_context_abandon_context(context);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

static gr_direct_context_t* makeSkiaContext() {
    const gr_glinterface_t* interface = gr_glinterface_create_native_interface();
    gr_direct_context_t* context = gr_direct_context_make_gl(interface);
    assert(context != NULL);
    return context;
}

static sk_surface_t* newSurface(gr_direct_context_t* context, const int w, const int h) {
    static gr_gl_framebufferinfo_t fbInfo = {
        .fFBOID = 0,
        .fFormat = GL_RGBA8};

    gr_backendrendertarget_t* target = gr_backendrendertarget_new_gl(w, h, 0, 0, &fbInfo);
    sk_color_type_t colorType = SK_COLOR_TYPE_RGBA_8888;

    sk_surface_t* surface = sk_surface_new_backend_render_target(context, target,
                                                                 GR_SURFACE_ORIGIN_BOTTOM_LEFT,
                                                                 colorType, NULL, NULL);
    gr_backendrendertarget_delete(target);
    assert(surface != NULL);
    return surface;
}
