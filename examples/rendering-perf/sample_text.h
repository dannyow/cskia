//
//  sample_text.h
//
//  Created by Daniel Owsiański on 03/12/2021.
//   

#ifndef sample_text_h
#define sample_text_h
#include <stdbool.h>  // bool

const char *textForIndex(int i, bool useOnlyNonLatin);
const char *randomText(bool useOnlyNonLatin);

#endif /* sample_text_h */
