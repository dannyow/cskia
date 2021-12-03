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
#include <string.h>  // strlen

#define _UNUSED __attribute__((unused))

#include "sk_capi.h"
#include "sk_rect_utils.h"

#include "sample_text.h"

#define MAX_NUMBER_OF_SHAPES 5000
#define FONT_SIZE 25

int winWidth = 512 * 2;
int winHeight = 512 * 2;

typedef struct {
    sk_picture_t* picture;
    sk_point_t position;
    const char* text;
    int size;
    float speed;
} shape;

int numberOfShapes = 100;
shape shapes[MAX_NUMBER_OF_SHAPES];
sk_font_t* font = NULL;

typedef struct {
    int scrollBoxes : 1;
    int drawShadow : 1;
    int drawInitialTextBoxFlag : 1;
    int drawFontMeasuredBoxFlag : 1;
} state;
state appState = {0, 0, 0, 0};

int randomNumber(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

sk_rect_t drawShapeAt(sk_canvas_t* canvas, const char* text, float x, float y) {
    sk_paint_t* paint = sk_paint_new();

    float textWidth = 250.0;
    sk_rect_t textBounds = {.left = x, .top = y, .right = x + textWidth, .bottom = y + FONT_SIZE};

    if (appState.drawInitialTextBoxFlag) {
        sk_paint_set_color(paint, 0xFF0000FF);
        sk_paint_set_style(paint, SK_PAINT_STYLE_STROKE);
        sk_canvas_draw_rect(canvas, &textBounds, paint);
    }

    //Colors: https://colors.muz.li/palette/6AEB00/56a400/00eb18/e2eb00/9ea400

    sk_point_t margin = {.x = 10, .y = 0};
    sk_text_blob_t* textBlob = sk_shaper_shape_text_for_width(textWidth, text, font, &margin);
    sk_textblob_get_bounds(textBlob, &textBounds);
    sk_rect_normalize(&textBounds);

    // Background
    sk_path_t* path = sk_path_new();
    sk_rect_offset(&textBounds, x, y + margin.y);
    float conerRadius = 10.0;
    sk_path_add_rounded_rect(path, &textBounds, conerRadius, conerRadius, SK_PATH_DIRECTION_CW);

    sk_paint_set_style(paint, SK_PAINT_STYLE_FILL);
    sk_paint_set_color(paint, 0xff6AEB00);
    sk_canvas_draw_path(canvas, path, paint);

    sk_paint_set_style(paint, SK_PAINT_STYLE_STROKE);
    sk_paint_set_color(paint, 0xFF56a400);
    sk_paint_set_stroke_width(paint, 2.0);
    sk_canvas_draw_path(canvas, path, paint);

    if (appState.drawShadow) {
        sk_paint_t* blurPaint = sk_paint_clone(paint);
        sk_paint_set_style(blurPaint, SK_PAINT_STYLE_FILL);
        sk_paint_set_color(blurPaint, 0x80000000);
        sk_mask_filter_t* filter = sk_maskfilter_new_blur_with_flags(SK_BLUR_STYLE_OUTER, 2 * 3.4, false);
        sk_paint_set_maskfilter(blurPaint, filter);

        sk_canvas_draw_path(canvas, path, blurPaint);

        sk_paint_delete(blurPaint);
    }

    sk_paint_set_style(paint, SK_PAINT_STYLE_FILL);
    sk_paint_set_color(paint, 0xFF200020);
    sk_paint_set_color(paint, 0xFFFFFFFF);
    sk_canvas_draw_text_blob(canvas, textBlob, x, y, paint);

    // Add extra stroke to text in contrasting color
    //    sk_paint_set_style(paint, SK_PAINT_STYLE_STROKE);
    //    sk_paint_set_stroke_width(paint, 1.0);
    //    sk_paint_set_color(paint, 0xFF5300eb);
    //    sk_canvas_draw_text_blob(canvas, textBlob, x, y, paint);

    if (appState.drawFontMeasuredBoxFlag) {
        sk_paint_set_color(paint, 0xFFFF0000);

        sk_rect_t textMeasure;
        sk_font_measure_text(font, text, strlen(text), SK_TEXT_ENCODING_UTF8, &textMeasure, paint);

        sk_rect_normalize(&textMeasure);
        sk_rect_offset(&textMeasure, x, y);
        sk_canvas_draw_rect(canvas, &textMeasure, paint);
    }

    sk_rect_t bounds;
    sk_path_get_bounds(path, &bounds);

    sk_path_delete(path);
    sk_textblob_unref(textBlob);
    sk_paint_delete(paint);

    return bounds;
}

void setup() {
    sk_font_mgr_t* fontMgr = sk_fontmgr_ref_default();
    //listAllFamiliesAndStyles(fontMgr);

    // Use font
    const char* name = "Garamond";
    sk_font_style_t* fontStyle = sk_fontstyle_new(SK_FONT_STYLE_WEIGHT_NORMAL,
                                                  SK_FONT_STYLE_WIDTH_NORMAL,
                                                  SK_FONT_STYLE_SLANT_UPRIGHT);
    sk_typeface_t* typeface = sk_fontmgr_match_family_style(fontMgr, name, fontStyle);
    assert(typeface);
    font = sk_font_new_with_values(typeface, FONT_SIZE, 1, 0);
    sk_font_set_subpixel(font, true);
    sk_fontstyle_delete(fontStyle);

    sk_picture_recorder_t* recorder = sk_picture_recorder_new();
    for (int i = 0; i < numberOfShapes; i++) {
        sk_rect_t bounds = {.left = 0, .top = 0, .right = 256, .bottom = 128};
        if (shapes[i].picture == NULL) {
            int size = bounds.right;  // randomNumber(5, 50);
            shapes[i].text = textForIndex(randomNumber(0, 100), true);
            shapes[i].size = size;
            shapes[i].position.x = randomNumber(0, winWidth);
            shapes[i].position.y = randomNumber(0, winHeight) - FONT_SIZE * 3;
            shapes[i].speed = 1 / (size * 1.0) + (randomNumber(1, 200) / 100.0);
        } else {
            sk_picture_unref(shapes[i].picture);
        }

        sk_canvas_t* canvas = sk_picture_recorder_begin_recording(recorder, &bounds);

        drawShapeAt(canvas, shapes[i].text, 0.0, 0.0);
        shapes[i].picture = sk_picture_recorder_end_recording(recorder);
    }
    sk_picture_recorder_delete(recorder);
}

void draw(sk_canvas_t* canvas) {
    sk_canvas_draw_color(canvas, 0xFFFFFFFF, SK_BLEND_MODE_SRCOVER);

    for (int i = 0; i < numberOfShapes; i++) {
        if (appState.scrollBoxes) {
            shapes[i].position.x -= shapes[i].speed;
        }
        sk_canvas_save(canvas);
        sk_canvas_translate(canvas, shapes[i].position.x, shapes[i].position.y);

        //if(i %3 ==1){
        //    sk_canvas_scale(canvas, 0.6, 0.6);
        //}
        sk_canvas_draw_picture(canvas, shapes[i].picture, NULL, NULL);
        sk_canvas_restore(canvas);

        if (shapes[i].position.x + shapes[i].size < 0) {
            shapes[i].position.x = winWidth + shapes[i].size;
            shapes[i].speed = 0.5 + (randomNumber(1, 200) / 100.0);
        }
    }

    sk_paint_t* legendPaint = sk_paint_new();
    sk_paint_set_style(legendPaint, SK_PAINT_STYLE_FILL);
    sk_paint_set_color(legendPaint, 0xFF000000);

    char legend[100] = {'\0'};
    snprintf(legend, sizeof(legend) - 1, "Shapes #%4d. Press: 1 to %s scroll, 2 to add 100 shapes, 3 to %s shadow",
             numberOfShapes,
             (appState.scrollBoxes ? "stop" : "start"),
             (appState.drawShadow ? "remove" : "add"));
    sk_canvas_draw_simple_text(canvas, legend, strlen(legend), SK_TEXT_ENCODING_UTF8, 10, winHeight - 10, font, legendPaint);
    sk_paint_delete(legendPaint);
}

// Utility function showing all available fonts and their styles
void listAllFamiliesAndStyles(sk_font_mgr_t* fontMgr) {
    // List all families with styles
    int families = sk_fontmgr_count_families(fontMgr);
    sk_string_t* familyName = sk_string_new_empty();
    for (int i = 0; i < families; i++) {
        sk_fontmgr_get_family_name(fontMgr, i, familyName);

        sk_font_style_set_t* styleSet = sk_fontmgr_match_family(fontMgr, sk_string_get_c_str(familyName));
        int stylesCount = sk_fontstyleset_get_count(styleSet);
        printf("Font: >%s< available styles:>%d< \n", sk_string_get_c_str(familyName), stylesCount);

        sk_string_t* styleNameStore = sk_string_new_empty();
        sk_font_style_t* styleStore = sk_fontstyle_new(SK_FONT_STYLE_WEIGHT_BLACK,
                                                       SK_FONT_STYLE_WIDTH_EXTRA_CONDENSED,
                                                       SK_FONT_STYLE_SLANT_UPRIGHT);
        for (int j = 0; j < stylesCount; j++) {
            sk_fontstyleset_get_style(styleSet, j, styleStore, styleNameStore);
            printf("\t %s ->", sk_string_get_c_str(styleNameStore));
            printf(" slant:%d", sk_fontstyle_get_slant(styleStore));
            printf(" weight:%d", sk_fontstyle_get_weight(styleStore));
            printf(" width:%d\n", sk_fontstyle_get_width(styleStore));
        }
        sk_fontstyle_delete(styleStore);
        sk_string_delete(styleNameStore);
    }
    sk_string_delete(familyName);
}

static gr_direct_context_t* makeSkiaContext(void);
static sk_surface_t* newSurface(gr_direct_context_t* context, const int w, const int h);

void keyCallback(_UNUSED GLFWwindow* window, int key, _UNUSED int scancode, int action, _UNUSED int mods) {
    bool needsSetup = false;

    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        appState.scrollBoxes ^= 1;
    }

    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        if(mods == GLFW_MOD_SHIFT){
            numberOfShapes -= 100;
            if(numberOfShapes <=0){
                numberOfShapes = 100;
            }
        }else{
            numberOfShapes += 100;
            if (numberOfShapes > MAX_NUMBER_OF_SHAPES) {
                numberOfShapes = MAX_NUMBER_OF_SHAPES;
            }
        }
        needsSetup = true;
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        appState.drawShadow ^= 1;
        needsSetup = true;
    }

    if (key == GLFW_KEY_4 && action == GLFW_PRESS) {
        appState.drawFontMeasuredBoxFlag ^= 1;
        needsSetup = true;
    }

    if (key == GLFW_KEY_5 && action == GLFW_PRESS) {
        appState.drawInitialTextBoxFlag ^= 1;
        needsSetup = true;
    }

    if (needsSetup) {
        setup();
    }
}

int main(_UNUSED int argc, _UNUSED char** argv) {
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

    glfwMakeContextCurrent(window);
    gr_direct_context_t* context = makeSkiaContext();
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, keyCallback);

    setup();

    int frames = 0;
    double t, t0, fps;
    char titleBuffer[10];
    t = t0 = fps = 0.0;

    while (!glfwWindowShouldClose(window)) {
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

        //        sk_canvas_flush(canvas);
        sk_surface_flush_and_submit(surface, false);
        glfwSwapBuffers(window);
        sk_surface_unref(surface);

        //        glfwWaitEvents();
        glfwPollEvents();

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
