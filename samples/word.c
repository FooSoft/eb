/*                                                            -*- C -*-
 * Copyright (c) 1999-2006  Motoyuki Kasahara
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * ������ˡ:
 *     word <book-path> <subbook-index> <word>
 * ��:
 *     word /cdrom 0 apple
 * ����:
 *     <book-path> �ǻ��ꤷ�� CD-ROM ���Ҥ���������ܤ��椫�� <word>
 *     �Ȥ���ñ��������׸�����õ�����ҥåȤ������٤ƤΥ���ȥ��
 *     ���Ф���ɽ�����ޤ���
 *
 *     <subbook-index> �ˤϡ������оݤ����ܤΥ���ǥå�������ꤷ��
 *     ��������ǥå����ϡ����Ҥκǽ�����ܤ����� 0��1��2 ... ��
 *     �ʤ�ޤ���
 */
#include <stdio.h>
#include <stdlib.h>

#include <eb/eb.h>
#include <eb/error.h>
#include <eb/text.h>

#define MAX_HITS 50
#define MAXLEN_HEADING 127

int
main(int argc, char *argv[])
{
    EB_Error_Code error_code;
    EB_Book book;
    EB_Subbook_Code subbook_list[EB_MAX_SUBBOOKS];
    EB_Hit hits[MAX_HITS];
    char heading[MAXLEN_HEADING + 1];
    int subbook_count;
    int subbook_index;
    int hit_count;
    ssize_t heading_length;
    int i;

    /* ���ޥ�ɹ԰���������å���*/
    if (argc != 4) {
        fprintf(stderr, "Usage: %s book-path subbook-index word\n",
            argv[0]);
        exit(1);
    }

    /* EB �饤�֥��� `book' ��������*/
    eb_initialize_library();
    eb_initialize_book(&book);

    /* ���Ҥ� `book' �˷���դ��롣*/
    error_code = eb_bind(&book, argv[1]);
    if (error_code != EB_SUCCESS) {
        fprintf(stderr, "%s: failed to bind the book, %s: %s\n",
            argv[0], eb_error_message(error_code), argv[1]);
        goto die;
    }

    /* ���ܤΰ����������*/
    error_code = eb_subbook_list(&book, subbook_list, &subbook_count);
    if (error_code != EB_SUCCESS) {
        fprintf(stderr, "%s: failed to get the subbbook list, %s\n",
            argv[0], eb_error_message(error_code));
        goto die;
    }

    /* ���ܤΥ���ǥå����������*/
    subbook_index = atoi(argv[2]);

    /*�ָ��ߤ����� (current subbook)�פ����ꡣ*/
    error_code = eb_set_subbook(&book, subbook_list[subbook_index]);
    if (error_code != EB_SUCCESS) {
        fprintf(stderr, "%s: failed to set the current subbook, %s\n",
            argv[0], eb_error_message(error_code));
        goto die;
    }

    /* ñ�측���Υꥯ�����Ȥ����С�*/
    error_code = eb_search_exactword(&book, argv[3]);
    if (error_code != EB_SUCCESS) {
        fprintf(stderr, "%s: failed to search for the word, %s: %s\n",
            argv[0], eb_error_message(error_code), argv[3]);
        goto die;
    }

    for (;;) {
        /* �ĤäƤ���ҥåȥ���ȥ�������*/
        error_code = eb_hit_list(&book, MAX_HITS, hits, &hit_count);
	if (error_code != EB_SUCCESS) {
            fprintf(stderr, "%s: failed to get hit entries, %s\n",
                argv[0], eb_error_message(error_code));
            goto die;
        }
        if (hit_count == 0)
            break;

        for (i = 0; i < hit_count; i++) {
            /* ���Ф��ΰ��֤ذ�ư��*/
	    error_code = eb_seek_text(&book, &(hits[i].heading));
            if (error_code != EB_SUCCESS) {
                fprintf(stderr, "%s: failed to seek the subbook, %s\n",
                    argv[0], eb_error_message(error_code));
                goto die;
            }

            /* ���Ф����������ɽ����*/
            error_code = eb_read_heading(&book, NULL, NULL, NULL,
		MAXLEN_HEADING, heading, &heading_length);
            if (error_code != EB_SUCCESS) {
                fprintf(stderr, "%s: failed to read the subbook, %s\n",
                    argv[0], eb_error_message(error_code));
                goto die;
            }
            printf("%s\n", heading);
        }
    }
        
    /* ���Ҥ� EB �饤�֥������Ѥ�λ��*/
    eb_finalize_book(&book);
    eb_finalize_library();
    exit(0);

    /* ���顼ȯ���ǽ�λ����Ȥ��ν�����*/
  die:
    eb_finalize_book(&book);
    eb_finalize_library();
    exit(1);
}
