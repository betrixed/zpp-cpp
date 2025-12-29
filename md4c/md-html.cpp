/*
 * MD4C: Markdown parser for C
 * (http://github.com/mity/md4c)
 *
 * Copyright (c) 2016-2024 Martin Mitáš
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef MD_HTML_CPP
#define MD_HTML_CPP

#include <stdio.h>
#include <string.h>

#include "md-html.h"
#include "src/entity.h"

#ifndef MARKHTML_H
#include "markhtml.h"
#endif

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199409L
    /* C89/90 or old compilers in general may not understand "inline". */
    #if defined __GNUC__
        #define inline __inline__
    #elif defined _MSC_VER
        #define inline __inline
    #else
        #define inline
    #endif
#endif

#ifdef _WIN32
    #define snprintf _snprintf
#endif

using namespace zpp;

struct MD_HTML {
    // php_obj is userdata for callback
    void (*process_output)(const MD_CHAR*, MD_SIZE, obj_ptr);
    obj_ptr   php_obj;
    unsigned flags;
    int image_nesting_level;
    char escape_map[256];

};

#define NEED_HTML_ESC_FLAG   0x1
#define NEED_URL_ESC_FLAG    0x2


/*****************************************
 ***  HTML rendering helper functions  ***
 *****************************************/

#ifndef CH_MACRO_H
#include "ch_macro.h"
#endif


static inline void
render_verbatim(MD_HTML* r, const MD_CHAR* text, MD_SIZE size)
{
    r->process_output(text, size, r->php_obj);
}

/* Keep this as a macro. Most compiler should then be smart enough to replace
 * the strlen() call with a compile-time constant if the string is a C literal. */
#define RENDER_VERBATIM(r, verbatim)                                    \
        render_verbatim((r), (verbatim), (MD_SIZE) (strlen(verbatim)))


static void
render_html_escaped(MD_HTML* r, const MD_CHAR* data, MD_SIZE size)
{
    MD_OFFSET beg = 0;
    MD_OFFSET off = 0;

    /* Some characters need to be escaped in normal HTML text. */
    #define NEED_HTML_ESC(ch)   (r->escape_map[(unsigned char)(ch)] & NEED_HTML_ESC_FLAG)

    while(1) {
        /* Optimization: Use some loop unrolling. */
        while(off + 3 < size  &&  !NEED_HTML_ESC(data[off+0])  &&  !NEED_HTML_ESC(data[off+1])
                              &&  !NEED_HTML_ESC(data[off+2])  &&  !NEED_HTML_ESC(data[off+3]))
            off += 4;
        while(off < size  &&  !NEED_HTML_ESC(data[off]))
            off++;

        if(off > beg)
            render_verbatim(r, data + beg, off - beg);

        if(off < size) {
            switch(data[off]) {
                case '&':   RENDER_VERBATIM(r, "&amp;"); break;
                case '<':   RENDER_VERBATIM(r, "&lt;"); break;
                case '>':   RENDER_VERBATIM(r, "&gt;"); break;
                case '"':   RENDER_VERBATIM(r, "&quot;"); break;
            }
            off++;
        } else {
            break;
        }
        beg = off;
    }
}

static void
render_url_escaped(MD_HTML* r, const MD_CHAR* data, MD_SIZE size)
{
    static const MD_CHAR hex_chars[] = "0123456789ABCDEF";
    MD_OFFSET beg = 0;
    MD_OFFSET off = 0;

    /* Some characters need to be escaped in URL attributes. */
    #define NEED_URL_ESC(ch)    (r->escape_map[(unsigned char)(ch)] & NEED_URL_ESC_FLAG)

    while(1) {
        while(off < size  &&  !NEED_URL_ESC(data[off]))
            off++;
        if(off > beg)
            render_verbatim(r, data + beg, off - beg);

        if(off < size) {
            char hex[3];

            switch(data[off]) {
                case '&':   RENDER_VERBATIM(r, "&amp;"); break;
                default:
                    hex[0] = '%';
                    hex[1] = hex_chars[((unsigned)data[off] >> 4) & 0xf];
                    hex[2] = hex_chars[((unsigned)data[off] >> 0) & 0xf];
                    render_verbatim(r, hex, 3);
                    break;
            }
            off++;
        } else {
            break;
        }

        beg = off;
    }
}

static unsigned
hex_val(char ch)
{
    if('0' <= ch && ch <= '9')
        return ch - '0';
    if('A' <= ch && ch <= 'Z')
        return ch - 'A' + 10;
    else
        return ch - 'a' + 10;
}

static void
render_utf8_codepoint(MD_HTML* r, unsigned codepoint,
                      void (*fn_append)(MD_HTML*, const MD_CHAR*, MD_SIZE))
{
    static const MD_CHAR utf8_replacement_char[] = { (char)0xef, (char)0xbf, (char)0xbd };

    unsigned char utf8[4];
    size_t n;

    if(codepoint <= 0x7f) {
        n = 1;
        utf8[0] = codepoint;
    } else if(codepoint <= 0x7ff) {
        n = 2;
        utf8[0] = 0xc0 | ((codepoint >>  6) & 0x1f);
        utf8[1] = 0x80 + ((codepoint >>  0) & 0x3f);
    } else if(codepoint <= 0xffff) {
        n = 3;
        utf8[0] = 0xe0 | ((codepoint >> 12) & 0xf);
        utf8[1] = 0x80 + ((codepoint >>  6) & 0x3f);
        utf8[2] = 0x80 + ((codepoint >>  0) & 0x3f);
    } else {
        n = 4;
        utf8[0] = 0xf0 | ((codepoint >> 18) & 0x7);
        utf8[1] = 0x80 + ((codepoint >> 12) & 0x3f);
        utf8[2] = 0x80 + ((codepoint >>  6) & 0x3f);
        utf8[3] = 0x80 + ((codepoint >>  0) & 0x3f);
    }

    if(0 < codepoint  &&  codepoint <= 0x10ffff)
        fn_append(r, (char*)utf8, (MD_SIZE)n);
    else
        fn_append(r, utf8_replacement_char, 3);
}

/* Translate entity to its UTF-8 equivalent, or output the verbatim one
 * if such entity is unknown (or if the translation is disabled). */
static void
render_entity(MD_HTML* r, const MD_CHAR* text, MD_SIZE size,
              void (*fn_append)(MD_HTML*, const MD_CHAR*, MD_SIZE))
{
    if(r->flags & MD_HTML_FLAG_VERBATIM_ENTITIES) {
        render_verbatim(r, text, size);
        return;
    }

    /* We assume UTF-8 output is what is desired. */
    if(size > 3 && text[1] == '#') {
        unsigned codepoint = 0;

        if(text[2] == 'x' || text[2] == 'X') {
            /* Hexadecimal entity (e.g. "&#x1234abcd;")). */
            MD_SIZE i;
            for(i = 3; i < size-1; i++)
                codepoint = 16 * codepoint + hex_val(text[i]);
        } else {
            /* Decimal entity (e.g. "&1234;") */
            MD_SIZE i;
            for(i = 2; i < size-1; i++)
                codepoint = 10 * codepoint + (text[i] - '0');
        }

        render_utf8_codepoint(r, codepoint, fn_append);
        return;
    } else {
        /* Named entity (e.g. "&nbsp;"). */
        const ENTITY* ent;

        ent = entity_lookup(text, size);
        if(ent != NULL) {
            render_utf8_codepoint(r, ent->codepoints[0], fn_append);
            if(ent->codepoints[1])
                render_utf8_codepoint(r, ent->codepoints[1], fn_append);
            return;
        }
    }

    fn_append(r, text, size);
}

// Render the text contents of an attribute value
static void
render_attribute(MD_HTML* r, const MD_ATTRIBUTE* attr,
                 void (*fn_append)(MD_HTML*, const MD_CHAR*, MD_SIZE))
{
    size_t tlen = attr->text_.size();
    MD_CHAR* text = (MD_CHAR*) attr->text_.data();

    int     ptype = 0;
    size_t  plen = 0;

    str_rc   segment;

    bool     allow_escapes = (attr->flags_ & MD_BUILD_ATTR_NO_ESCAPES) != 0;

    segment = walk_attribute(text, tlen,  allow_escapes, ptype, plen);
    while(segment.ok())
    {
        switch(ptype) 
        {
        case MD_TEXT_NULLCHAR:
            render_utf8_codepoint(r, 0x0000, render_verbatim); 
            break;
        case MD_TEXT_ENTITY:   
            render_entity(r, segment.data(), segment.size(), fn_append); 
            break;
        default:                
            fn_append(r, segment.data(), segment.size()); 
            break; 
        }
        if (plen < tlen)
        {
            segment = walk_attribute(text+plen, tlen-plen, allow_escapes, ptype, plen);
        }   
        else {
            break;
        }
    }
}


static void
render_open_ol_block(MD_HTML* r, const MD_BLOCK_OL_DETAIL* det)
{
    char buf[64];

    if(det->start == 1) {
        RENDER_VERBATIM(r, "<ol>\n");
        return;
    }

    snprintf(buf, sizeof(buf), "<ol start=\"%u\">\n", det->start);
    RENDER_VERBATIM(r, buf);
}

static void
render_open_li_block(MD_HTML* r, const MD_BLOCK_LI_DETAIL* det)
{
    if(det->is_task) {
        RENDER_VERBATIM(r, "<li class=\"task-list-item\">"
                          "<input type=\"checkbox\" class=\"task-list-item-checkbox\" disabled");
        if(det->task_mark == 'x' || det->task_mark == 'X')
            RENDER_VERBATIM(r, " checked");
        RENDER_VERBATIM(r, ">");
    } else {
        RENDER_VERBATIM(r, "<li>");
    }
}

static void
render_open_code_block(MD_HTML* r, const MD_BLOCK_CODE_DETAIL* det)
{
    RENDER_VERBATIM(r, "<pre><code");

    /* If known, output the HTML 5 attribute class="language-LANGNAME". */
    if(det->lang.text_.ok()) {
        RENDER_VERBATIM(r, " class=\"language-");
        render_attribute(r, &det->lang, render_html_escaped);
        RENDER_VERBATIM(r, "\"");
    }

    RENDER_VERBATIM(r, ">");
}

static void
render_open_td_block(MD_HTML* r, const MD_CHAR* cell_type, const MD_BLOCK_TD_DETAIL* det)
{
    RENDER_VERBATIM(r, "<");
    RENDER_VERBATIM(r, cell_type);

    switch(det->align) {
        case MD_ALIGN_LEFT:     RENDER_VERBATIM(r, " align=\"left\">"); break;
        case MD_ALIGN_CENTER:   RENDER_VERBATIM(r, " align=\"center\">"); break;
        case MD_ALIGN_RIGHT:    RENDER_VERBATIM(r, " align=\"right\">"); break;
        default:                RENDER_VERBATIM(r, ">"); break;
    }
}

static void
render_open_a_span(MD_HTML* r,  MD_SPAN_A_DETAIL* det)
{
    //open a tag, href value first
    RENDER_VERBATIM(r, "<a href=\"");

    val_rc link_title_rc;

    htab_rw all_attr(link_title_rc);

    MD_ATTRIBUTE& attr_href = det->href;
    if (attr_href.text_.ok()) {
        all_attr.set(wcc::MTH.href_attr, attr_href.text_);
    }

    MD_ATTRIBUTE& attr_title = det->title;

    if (attr_title.text_.ok()) {
        all_attr.set(wcc::MTH.title_attr, attr_title.text_);
    }

    val_rc rval = r->php_obj.call(wcc::MTH.cb_link_s, link_title_rc);

    all_attr = rval.zarray();


    if (all_attr.size())
    {
        str_rc href_val = all_attr.get(wcc::MTH.href_attr);
        if (href_val.ok())
        {
            attr_href.text_ = href_val;
            all_attr.unset(wcc::MTH.href_attr);
        }
    }
    // href attr value
    render_attribute(r, &attr_href, render_url_escaped);    

    // all other attributes
    htab_walk wk;
    auto      key = wk.key();
    auto      value = wk.value();
    str_buf   buf;

    for(wk.start(all_attr); wk.ok(); wk.next())
    {
        MD_ATTRIBUTE extra;

        extra.text_ = value.zstr();
        // close previous, render next

        // attribute name
        buf << "\" "  << key << "=\"";
        render_verbatim(r, buf.data(), buf.size());

        render_attribute(r, &extra, render_html_escaped);
        buf.reset();
    }
    // end 'a' tag
    RENDER_VERBATIM(r, "\">");
}

static void
render_open_img_span(MD_HTML* r, const MD_SPAN_IMG_DETAIL* det)
{
    RENDER_VERBATIM(r, "<img src=\"");
    render_attribute(r, &det->src, render_url_escaped);

    RENDER_VERBATIM(r, "\" alt=\"");
}

static void
render_close_img_span(MD_HTML* r, const MD_SPAN_IMG_DETAIL* det)
{
    if(det->title.text_.ok()) {
        RENDER_VERBATIM(r, "\" title=\"");
        render_attribute(r, &det->title, render_html_escaped);
    }

    RENDER_VERBATIM(r, (r->flags & MD_HTML_FLAG_XHTML) ? "\" />" : "\">");
}

static void
render_open_wikilink_span(MD_HTML* r, const MD_SPAN_WIKILINK_DETAIL* det)
{
    RENDER_VERBATIM(r, "<x-wikilink data-target=\"");
    render_attribute(r, &det->target, render_html_escaped);

    RENDER_VERBATIM(r, "\">");
}


/**************************************
 ***  HTML renderer implementation  ***
 **************************************/

static int
enter_block_callback(MD_BLOCKTYPE type, void* detail, void* userdata)
{
    static const MD_CHAR* head[6] = { "<h1>", "<h2>", "<h3>", "<h4>", "<h5>", "<h6>" };
    MD_HTML* r = (MD_HTML*) userdata;

    switch(type) {
        case MD_BLOCK_DOC:      /* noop */ break;
        case MD_BLOCK_QUOTE:    RENDER_VERBATIM(r, "<blockquote>\n"); break;
        case MD_BLOCK_UL:       RENDER_VERBATIM(r, "<ul>\n"); break;
        case MD_BLOCK_OL:       render_open_ol_block(r, (const MD_BLOCK_OL_DETAIL*)detail); break;
        case MD_BLOCK_LI:       render_open_li_block(r, (const MD_BLOCK_LI_DETAIL*)detail); break;
        case MD_BLOCK_HR:       RENDER_VERBATIM(r, (r->flags & MD_HTML_FLAG_XHTML) ? "<hr />\n" : "<hr>\n"); break;
        case MD_BLOCK_H:        RENDER_VERBATIM(r, head[((MD_BLOCK_H_DETAIL*)detail)->level - 1]); break;
        case MD_BLOCK_CODE:     render_open_code_block(r, (const MD_BLOCK_CODE_DETAIL*) detail); break;
        case MD_BLOCK_HTML:     /* noop */ break;
        case MD_BLOCK_P:        RENDER_VERBATIM(r, "<p>"); break;
        case MD_BLOCK_TABLE:    RENDER_VERBATIM(r, "<table>\n"); break;
        case MD_BLOCK_THEAD:    RENDER_VERBATIM(r, "<thead>\n"); break;
        case MD_BLOCK_TBODY:    RENDER_VERBATIM(r, "<tbody>\n"); break;
        case MD_BLOCK_TR:       RENDER_VERBATIM(r, "<tr>\n"); break;
        case MD_BLOCK_TH:       render_open_td_block(r, "th", (MD_BLOCK_TD_DETAIL*)detail); break;
        case MD_BLOCK_TD:       render_open_td_block(r, "td", (MD_BLOCK_TD_DETAIL*)detail); break;
    }

    return 0;
}

static int
leave_block_callback(MD_BLOCKTYPE type, void* detail, void* userdata)
{
    static const MD_CHAR* head[6] = { "</h1>\n", "</h2>\n", "</h3>\n", "</h4>\n", "</h5>\n", "</h6>\n" };
    MD_HTML* r = (MD_HTML*) userdata;

    switch(type) {
        case MD_BLOCK_DOC:      /*noop*/ break;
        case MD_BLOCK_QUOTE:    RENDER_VERBATIM(r, "</blockquote>\n"); break;
        case MD_BLOCK_UL:       RENDER_VERBATIM(r, "</ul>\n"); break;
        case MD_BLOCK_OL:       RENDER_VERBATIM(r, "</ol>\n"); break;
        case MD_BLOCK_LI:       RENDER_VERBATIM(r, "</li>\n"); break;
        case MD_BLOCK_HR:       /*noop*/ break;
        case MD_BLOCK_H:        RENDER_VERBATIM(r, head[((MD_BLOCK_H_DETAIL*)detail)->level - 1]); break;
        case MD_BLOCK_CODE:     RENDER_VERBATIM(r, "</code></pre>\n"); break;
        case MD_BLOCK_HTML:     /* noop */ break;
        case MD_BLOCK_P:        RENDER_VERBATIM(r, "</p>\n"); break;
        case MD_BLOCK_TABLE:    RENDER_VERBATIM(r, "</table>\n"); break;
        case MD_BLOCK_THEAD:    RENDER_VERBATIM(r, "</thead>\n"); break;
        case MD_BLOCK_TBODY:    RENDER_VERBATIM(r, "</tbody>\n"); break;
        case MD_BLOCK_TR:       RENDER_VERBATIM(r, "</tr>\n"); break;
        case MD_BLOCK_TH:       RENDER_VERBATIM(r, "</th>\n"); break;
        case MD_BLOCK_TD:       RENDER_VERBATIM(r, "</td>\n"); break;
    }

    return 0;
}

static int
enter_span_callback(MD_SPANTYPE type, void* detail, void* userdata)
{
    MD_HTML* r = (MD_HTML*) userdata;
    int inside_img = (r->image_nesting_level > 0);

    /* We are inside a Markdown image label. Markdown allows to use any emphasis
     * and other rich contents in that context similarly as in any link label.
     *
     * However, unlike in the case of links (where that contents becomescontents
     * of the <a>...</a> tag), in the case of images the contents is supposed to
     * fall into the attribute alt: <img alt="...">.
     *
     * In that context we naturally cannot output nested HTML tags. So lets
     * suppress them and only output the plain text (i.e. what falls into text()
     * callback).
     *
     * CommonMark specification declares this a recommended practice for HTML
     * output.
     */
    if(type == MD_SPAN_IMG)
        r->image_nesting_level++;
    if(inside_img)
        return 0;

    switch(type) {
        case MD_SPAN_EM:                RENDER_VERBATIM(r, "<em>"); break;
        case MD_SPAN_STRONG:            RENDER_VERBATIM(r, "<strong>"); break;
        case MD_SPAN_U:                 RENDER_VERBATIM(r, "<u>"); break;
        case MD_SPAN_A:                 render_open_a_span(r, (MD_SPAN_A_DETAIL*) detail); break;
        case MD_SPAN_IMG:               render_open_img_span(r, (MD_SPAN_IMG_DETAIL*) detail); break;
        case MD_SPAN_CODE:              RENDER_VERBATIM(r, "<code>"); break;
        case MD_SPAN_DEL:               RENDER_VERBATIM(r, "<del>"); break;
        case MD_SPAN_LATEXMATH:         RENDER_VERBATIM(r, "<x-equation>"); break;
        case MD_SPAN_LATEXMATH_DISPLAY: RENDER_VERBATIM(r, "<x-equation type=\"display\">"); break;
        case MD_SPAN_WIKILINK:          render_open_wikilink_span(r, (MD_SPAN_WIKILINK_DETAIL*) detail); break;
    }

    return 0;
}

static int
leave_span_callback(MD_SPANTYPE type, void* detail, void* userdata)
{
    MD_HTML* r = (MD_HTML*) userdata;

    if(type == MD_SPAN_IMG)
        r->image_nesting_level--;
    if(r->image_nesting_level > 0)
        return 0;

    switch(type) {
        case MD_SPAN_EM:                RENDER_VERBATIM(r, "</em>"); break;
        case MD_SPAN_STRONG:            RENDER_VERBATIM(r, "</strong>"); break;
        case MD_SPAN_U:                 RENDER_VERBATIM(r, "</u>"); break;
        case MD_SPAN_A:                 RENDER_VERBATIM(r, "</a>"); break;
        case MD_SPAN_IMG:               render_close_img_span(r, (MD_SPAN_IMG_DETAIL*) detail); break;
        case MD_SPAN_CODE:              RENDER_VERBATIM(r, "</code>"); break;
        case MD_SPAN_DEL:               RENDER_VERBATIM(r, "</del>"); break;
        case MD_SPAN_LATEXMATH:         /*fall through*/
        case MD_SPAN_LATEXMATH_DISPLAY: RENDER_VERBATIM(r, "</x-equation>"); break;
        case MD_SPAN_WIKILINK:          RENDER_VERBATIM(r, "</x-wikilink>"); break;
    }

    return 0;
}

static int
text_callback(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata)
{
    MD_HTML* r = (MD_HTML*) userdata;

    switch(type) {
        case MD_TEXT_NULLCHAR:  render_utf8_codepoint(r, 0x0000, render_verbatim); break;
        case MD_TEXT_BR:        RENDER_VERBATIM(r, (r->image_nesting_level == 0
                                        ? ((r->flags & MD_HTML_FLAG_XHTML) ? "<br />\n" : "<br>\n")
                                        : " "));
                                break;
        case MD_TEXT_SOFTBR:    RENDER_VERBATIM(r, (r->image_nesting_level == 0 ? "\n" : " ")); break;
        case MD_TEXT_HTML:      render_verbatim(r, text, size); break;
        case MD_TEXT_ENTITY:    render_entity(r, text, size, render_html_escaped); break;
        default:                render_html_escaped(r, text, size); break;
    }

    return 0;
}

static void
debug_log_callback(const char* msg, void* userdata)
{
    MD_HTML* r = (MD_HTML*) userdata;
    if(r->flags & MD_HTML_FLAG_DEBUG)
        fprintf(stderr, "MD4C: %s\n", msg);
}

#ifndef CH_MACRO_H
#include "ch_macro.h"
#endif

int
md_html(const MD_CHAR* input, MD_SIZE input_size,
        void (*process_output)(const MD_CHAR*, MD_SIZE, obj_ptr),
        obj_ptr userdata, unsigned parser_flags, unsigned renderer_flags)
{
    MD_HTML render = { process_output, userdata, renderer_flags, 0, { 0 } };
    int i;

    MD_PARSER parser = {
        0,
        parser_flags,
        enter_block_callback,
        leave_block_callback,
        enter_span_callback,
        leave_span_callback,
        text_callback,
        debug_log_callback,
        NULL
    };

    /* Build map of characters which need escaping. */
    for(i = 0; i < 256; i++) {
        unsigned char ch = (unsigned char) i;

        if(strchr("\"&<>", ch) != NULL)
            render.escape_map[i] |= NEED_HTML_ESC_FLAG;

        if(!ISALNUM_(ch)  &&  strchr("~-_.+!*(),%#@?=;:/,+$", ch) == NULL)
            render.escape_map[i] |= NEED_URL_ESC_FLAG;
    }

    /* Consider skipping UTF-8 byte order mark (BOM). */
    if(renderer_flags & MD_HTML_FLAG_SKIP_UTF8_BOM  &&  sizeof(MD_CHAR) == 1) {
        static const MD_CHAR bom[3] = { (char)0xef, (char)0xbb, (char)0xbf };
        if(input_size >= sizeof(bom)  &&  memcmp(input, bom, sizeof(bom)) == 0) {
            input += sizeof(bom);
            input_size -= sizeof(bom);
        }
    }

    return md_parse(input, input_size, &parser, (void*) &render);
}

#endif