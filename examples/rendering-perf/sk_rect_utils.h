//
//  sk_rect_utils.h
//
//  Created by Daniel Owsiański on 03/12/2021.
//   

#ifndef sk_rect_utils_h
#define sk_rect_utils_h

#if !defined(INLINE_FUNC)
    #if defined (__GNUC__) && (__GNUC__ == 4)
        #define INLINE_FUNC static __inline__ __attribute__((always_inline))
    #else
        #define INLINE_FUNC static __inline__
    #endif
#endif


//https://github.com/osxfuse/prefpane/blob/8a65eb24605e1e703025fc64816a7c531b5bf5b3/externals/google-toolbox-for-mac/GTMDefines.h
//https://github.com/gnustep/libs-base/blob/master/Headers/Foundation/NSGeometry.h
INLINE_FUNC void sk_rect_normalize(sk_rect_t* rect) {
    if (rect->left != 0.0) {
        float left = rect->left;
        rect->left = 0;
        rect->right -= left;
    }
    if (rect->top != 0.0) {
        float top = rect->top;
        rect->top = 0;
        rect->bottom -= top;
    }
}
INLINE_FUNC void sk_rect_offset(sk_rect_t *rect, float dx, float dy) {
    rect->left += dx;
    rect->top += dy;
    rect->right += dx;
    rect->bottom += dy;
}

#endif /* sk_rect_utils_h */
