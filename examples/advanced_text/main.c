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

#include "sk_capi.h"

#define TARGET_FPS 60.0

int winWidth = 920;
int winHeight = 1075;

static const char* helloWorld1 =
    "Afrikaans:    Hello Wêreld! \n"
    "Albanian:     Përshendetje Botë! \n"
    "Amharic:      ሰላም ልዑል! \n"
    "Arabic:       مرحبا بالعالم! \n"
    "Armenian:     Բարեւ աշխարհ! \n"
    "Basque:       Kaixo Mundua! \n"
    "Belarussian:  Прывітанне Сусвет! \n"
    "Bengali:      ওহে বিশ্ব! \n"
    "Bulgarian:    Здравей свят! \n"
    "Catalan:      Hola món! \n"
    "Chichewa:     Moni Dziko Lapansi! \n"
    "Chinese:      你好世界！ \n"
    "Croatian:     Pozdrav svijete! \n"
    "Czech:        Ahoj světe! \n"
    "Danish:       Hej Verden! \n"
    "Dutch:        Hallo Wereld! \n"
    "English:      Hello World! \n"
    "Estonian:     Tere maailm! \n"
    "Finnish:      Hei maailma! \n"
    "French:       Bonjour monde! \n"
    "Frisian:      Hallo wrâld! \n"
    "Georgian:     გამარჯობა მსოფლიო! \n"
    "German:       Hallo Welt! \n"
    "Greek:        Γειά σου Κόσμε! \n"
    "Hausa:        Sannu Duniya! \n"
    "Hebrew:       שלום עולם! \n"
    "Hindi:        नमस्ते दुनिया! \n"
    "Hungarian:    Helló Világ! \n"
    "Icelandic:    Halló heimur! \n"
    "Igbo:         Ndewo Ụwa! \n"
    "Indonesian:   Halo Dunia! \n"
    "Italian:      Ciao mondo! \n"
    "Japanese:     こんにちは世界！ \n"
    "Kazakh:       Сәлем Әлем! \n"
    "Khmer:        សួស្តី​ពិភពលោក! \n"
    "Kyrgyz:       Салам дүйнө! \n"
    "Lao:          ສະ​ບາຍ​ດີ​ຊາວ​ໂລກ! \n"
    "Latvian:      Sveika pasaule! \n";

static const char* helloWorld2 =
    "Lithuanian:   Labas pasauli! \n"
    "Luxemburgish: Moien Welt! \n"
    "Macedonian:   Здраво свету! \n"
    "Malay:        Hai dunia! \n"
    "Malayalam:    ഹലോ വേൾഡ്! \n"
    "Mongolian:    Сайн уу дэлхий! \n"
    "Myanmar:      မင်္ဂလာပါကမ္ဘာလောက! \n"
    "Nepali:       नमस्कार संसार! \n"
    "Norwegian:    Hei Verden! \n"
    "Pashto:       سلام نړی! \n"
    "Persian:      سلام دنیا! \n"
    "Polish:       Witaj świecie! \n"
    "Portuguese:   Olá Mundo! \n"
    "Punjabi:      ਸਤਿ ਸ੍ਰੀ ਅਕਾਲ ਦੁਨਿਆ! \n"
    "Romanian:     Salut Lume! \n"
    "Russian:      Привет мир! \n"
    "Scots Gaelic: Hàlo a Shaoghail! \n"
    "Serbian:      Здраво Свете! \n"
    "Sesotho:      Lefatše Lumela! \n"
    "Sinhala:      හෙලෝ වර්ල්ඩ්! \n"
    "Slovenian:    Pozdravljen svet! \n"
    "Spanish:      ¡Hola Mundo! \n"
    "Sundanese:    Halo Dunya! \n"
    "Swahili:      Salamu Dunia! \n"
    "Swedish:      Hej världen! \n"
    "Tajik:        Салом Ҷаҳон! \n"
    "Thai:         สวัสดีชาวโลก! \n"
    "Turkish:      Selam Dünya! \n"
    "Ukrainian:    Привіт Світ! \n"
    "Uzbek:        Salom Dunyo! \n"
    "Vietnamese:   Chào thế giới! \n"
    "Welsh:        Helo Byd! \n"
    "Xhosa:        Molo Lizwe! \n"
    "Yiddish:      העלא וועלט! \n"
    "Yoruba:       Mo ki O Ile Aiye! \n"
    "Zulu:         Sawubona Mhlaba! \n"
    "Emoji:        👋👋🏻👋🏼👋🏽👋🏾👋🏿🌍";

sk_font_t* font = NULL;
void setup() {
    sk_font_mgr_t* fontMgr = sk_fontmgr_ref_default();

    // List all families
    int families = sk_fontmgr_count_families(fontMgr);
    sk_string_t* familyName = sk_string_new_empty();
    for (int i = 0; i < families; i++) {
        sk_fontmgr_get_family_name(fontMgr, i, familyName);
        printf("%s \n", sk_string_get_c_str(familyName));
    }
    sk_string_delete(familyName);

    // Use font
    const char* name = "SF Compact Display";
    sk_font_style_t* fontStyle = sk_fontstyle_new(SK_FONT_STYLE_WEIGHT_BLACK,
                                                  SK_FONT_STYLE_WIDTH_NORMAL,
                                                  SK_FONT_STYLE_SLANT_ITALIC);
    sk_typeface_t* typeface = sk_fontmgr_match_family_style(fontMgr, name, fontStyle);
    assert(typeface);
    font = sk_font_new_with_values(typeface, 20, 1, 0);
    sk_font_set_subpixel(font, true);

    sk_fontstyle_delete(fontStyle);
}
void draw(sk_canvas_t* canvas) {
    sk_canvas_draw_color(canvas, 0xFFFFFFFF, SK_BLEND_MODE_SRCOVER);

    sk_point_t margin = {.x = 4, .y = 0};
    sk_text_blob_t* text1 = sk_shaper_shape_text_for_width(winWidth / 2, helloWorld1, font, &margin);
    sk_text_blob_t* text2 = sk_shaper_shape_text_for_width(winWidth / 2, helloWorld2, font, &margin);

    sk_paint_t* paint = sk_paint_new();
    sk_paint_set_style(paint, SK_PAINT_STYLE_FILL);
    sk_paint_set_color(paint, 0xFF6AEB00);

    sk_canvas_draw_text_blob(canvas, text1, 25, 25, paint);
    sk_canvas_draw_text_blob(canvas, text2, winWidth / 2 + 50, 25, paint);

    sk_paint_set_style(paint, SK_PAINT_STYLE_STROKE);
    sk_paint_set_stroke_width(paint, 1.0);
    sk_paint_set_color(paint, 0xFF200020);

    sk_canvas_draw_text_blob(canvas, text1, 25, 25, paint);
    sk_canvas_draw_text_blob(canvas, text2, winWidth / 2 + 50, 25, paint);

    sk_textblob_unref(text1);
    sk_textblob_unref(text2);

    sk_paint_delete(paint);
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

    glfwMakeContextCurrent(window);
    gr_direct_context_t* context = makeSkiaContext();
    glfwSwapInterval(1);

    setup();

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
