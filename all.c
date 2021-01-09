/*
 * Copyright (c) 2016 Alex Yatskov <alex@foosoft.net>
 * Author: Alex Yatskov <alex@foosoft.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "build-pre.h"
#include "eb.h"
#include "error.h"
#include "build-post.h"

static int eb_match_all(const char word[], const char pattern[], size_t length) {
    (void) word;
    (void) pattern;
    (void) length;
    return 0;
}

static EB_Error_Code eb_search_all(EB_Book* book, EB_Word_Code word_code) {
    EB_Error_Code error_code = EB_SUCCESS;

    do {
        eb_lock(&book->lock);
        LOG(("in: eb_search_all(book=%d)", book->code));

        /* Current subbook must have been set. */
        if (book->subbook_current == NULL) {
            error_code = EB_ERR_NO_CUR_SUB;
            break;
        }

        /* Initialize search context. */
        eb_reset_search_contexts(book);
        EB_Search_Context* context = book->search_contexts;
        context->code = EB_SEARCH_ALL;
        context->compare_pre = eb_match_all;
        context->compare_single = eb_match_all;
        context->compare_group = eb_match_all;

        /* Get a page number. */
        switch (word_code) {
            case EB_WORD_ALPHABET:
                if (book->subbook_current->word_alphabet.start_page != 0) {
                    context->page = book->subbook_current->word_alphabet.start_page;
                }
                else {
                    error_code = EB_ERR_NO_SUCH_SEARCH;
                }
                break;
            case EB_WORD_KANA:
                if (book->subbook_current->word_kana.start_page != 0) {
                    context->page = book->subbook_current->word_kana.start_page;
                }
                else {
                    error_code = EB_ERR_NO_SUCH_SEARCH;
                }
                break;
            case EB_WORD_OTHER:
                if (book->subbook_current->word_asis.start_page != 0) {
                    context->page = book->subbook_current->word_asis.start_page;
                }
                else {
                    error_code = EB_ERR_NO_SUCH_SEARCH;
                }
                break;
            default:
                error_code = EB_ERR_NO_SUCH_SEARCH;
                break;
        }

        if (error_code != EB_SUCCESS) {
            break;
        }

        /* Pre-search. */
        error_code = eb_presearch_word(book, context);
        if (error_code != EB_SUCCESS) {
            break;
        }
    }
    while (0);

    if (error_code != EB_SUCCESS) {
        eb_reset_search_contexts(book);
    }

    LOG(("out: eb_search_all() = %s", eb_error_string(error_code)));
    eb_unlock(&book->lock);

    return error_code;
}

int eb_have_all_search(EB_Book* book) {
    eb_lock(&book->lock);
    LOG(("in: eb_have_all_search(book=%d)", book->code));

    int result = 0;
    for (;;) {
        const EB_Subbook * sb = book->subbook_current;
        if (sb == NULL) {
            break;
        }

        if (sb->word_alphabet.start_page == 0 && sb->word_asis.start_page == 0 && sb->word_kana.start_page == 0) {
            break;
        }

        result = 1;
        break;
    }

    LOG(("out: eb_have_all_search() = %d", result));
    eb_unlock(&book->lock);
    return result;
}

EB_Error_Code eb_search_all_alphabet(EB_Book* book) {
    return eb_search_all(book, EB_WORD_ALPHABET);
}

EB_Error_Code eb_search_all_kana(EB_Book* book) {
    return eb_search_all(book, EB_WORD_KANA);
}

EB_Error_Code eb_search_all_asis(EB_Book* book) {
    return eb_search_all(book, EB_WORD_OTHER);
}
