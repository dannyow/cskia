//
//  text_sample.c
//
//  Created by Daniel Owsiański on 03/12/2021.
//

#include "sample_text.h"
#include <stdio.h>      //printf
#include <stdlib.h>     //rand
#include <string.h>     //strlen
#include <math.h>       //rand

static const char *helloWorld[] = {
    "Afrikaans:    Hello Wêreld",
    "Albanian:     Përshendetje Botë",
    "Amharic:      ሰላም ልዑል",
    "Arabic:       مرحبا بالعالم",
    "Armenian:     Բարեւ աշխարհ",
    "Basque:       Kaixo Mundua",
    "Belarussian:  Прывітанне Сусвет",
    "Bengali:      ওহে বিশ্ব",
    "Bulgarian:    Здравей свят",
    "Catalan:      Hola món",
    "Chichewa:     Moni Dziko Lapansi",
    "Chinese:      你好世界!",
    "Croatian:     Pozdrav svijete",
    "Czech:        Ahoj světe",
    "Danish:       Hej Verden",
    "Dutch:        Hallo Wereld",
    "English:      Hello World",
    "Estonian:     Tere maailm",
    "Finnish:      Hei maailma",
    "French:       Bonjour monde",
    "Frisian:      Hallo wrâld",
    "Georgian:     გამარჯობა მსოფლიო",
    "German:       Hallo Welt",
    "Greek:        Γειά σου Κόσμε",
    "Hausa:        Sannu Duniya",
    "Hebrew:       שלום עולם",
    "Hindi:        नमस्ते दुनिया",
    "Hungarian:    Helló Világ",
    "Icelandic:    Halló heimur",
    "Igbo:         Ndewo Ụwa",
    "Indonesian:   Halo Dunia",
    "Italian:      Ciao mondo",
    "Japanese:     こんにちは世界",
    "Kazakh:       Сәлем Әлем",
    "Khmer:        សួស្តី​ពិភពលោក",
    "Kyrgyz:       Салам дүйнө",
    "Lao:          ສະ​ບາຍ​ດີ​ຊາວ​ໂລກ",
    "Latvian:      Sveika pasaule",
    "Lithuanian:   Labas pasauli",
    "Luxemburgish: Moien Welt",
    "Macedonian:   Здраво свету",
    "Malay:        Hai dunia",
    "Malayalam:    ഹലോ വേൾഡ്",
    "Mongolian:    Сайн уу дэлхий",
    "Myanmar:      မင်္ဂလာပါကမ္ဘာလောက",
    "Nepali:       नमस्कार संसार",
    "Norwegian:    Hei Verden",
    "Pashto:       سلام نړی",
    "Persian:      سلام دنیا",
    "Polish:       Witaj świecie",
    "Portuguese:   Olá Mundo",
    "Punjabi:      ਸਤਿ ਸ੍ਰੀ ਅਕਾਲ ਦੁਨਿਆ",
    "Romanian:     Salut Lume",
    "Russian:      Привет мир",
    "Scots Gaelic: Hàlo a Shaoghail",
    "Serbian:      Здраво Свете",
    "Sesotho:      Lefatše Lumela",
    "Sinhala:      හෙලෝ වර්ල්ඩ්",
    "Slovenian:    Pozdravljen svet",
    "Spanish:      ¡Hola Mundo",
    "Sundanese:    Halo Dunya",
    "Swahili:      Salamu Dunia",
    "Swedish:      Hej världen",
    "Tajik:        Салом Ҷаҳон",
    "Thai:         สวัสดีชาวโลก",
    "Turkish:      Selam Dünya",
    "Ukrainian:    Привіт Світ",
    "Uzbek:        Salom Dunyo",
    "Vietnamese:   Chào thế giới",
    "Welsh:        Helo Byd",
    "Xhosa:        Molo Lizwe",
    "Yiddish:      העלא וועלט",
    "Yoruba:       Mo ki O Ile Aiye",
    "Zulu:         Sawubona Mhlaba",
    "Emoji:        👋👋🏻👋🏼👋🏽👋🏾👋🏿🌍"};
static const int nonLatinIndexes[] = {2, 3, 4, 6, 7, 8, 11, 21, 25, 26, 32, 34, 36, 42, 44, 45, 47, 48, 51, 57, 64, 68, 71, 74};

// return only native script part (move beyond ':' + spaces)
static const char *getNativeScript(const char *s) {
    char *r = strchr(s, ':');
    while (r && (*r == ':' || *r == ' ')) r++;
    return !r ? s : r;
}
static const char *nonLatinTextForIndex(int i) {
    size_t total = sizeof(nonLatinIndexes) / sizeof(nonLatinIndexes[0]);
    int index = nonLatinIndexes[i % total];
    return helloWorld[index];
}
static const char *anyTextForIndex(int i) {
    size_t total = sizeof(helloWorld) / sizeof(helloWorld[0]);
    int index = i % total;
    return helloWorld[index];
}
const char *textForIndex(int i, bool useOnlyNonLatin) {
    const char *s;
    if(useOnlyNonLatin){
        s = nonLatinTextForIndex(i);
    }else{
        s = anyTextForIndex(i);
    }
    return getNativeScript(s);
}

const char *randomText(bool useOnlyNonLatin){
    int randomInt = rand() * 100;
    return textForIndex(randomInt, useOnlyNonLatin);

}
