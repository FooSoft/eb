/*                                                            -*- C -*-
 * Copyright (c) 2003-2006  Motoyuki Kasahara
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
 *     booklist <remote-access-ideintifier>
 * ��:
 *     booklist ebnet://localhost
 * ����:
 *     <remote-access-ideintifier> �ǻ��ꤷ�� EBNET �����Ф���³��
 *     �ơ������Ф��󶡤�����ҡ�appendix �ΰ�����ɽ�����ޤ���
 */
#include <stdio.h>
#include <stdlib.h>

#include <eb/eb.h>
#include <eb/error.h>
#include <eb/booklist.h>

int
main(int argc, char *argv[])
{
    EB_Error_Code error_code;
    EB_BookList bl;
    int book_count;
    char *name, *title;
    int i;

    /* ���ޥ�ɹ԰���������å���*/
    if (argc != 2) {
        fprintf(stderr, "Usage: %s book-path remote-access-identifier\n",
            argv[0]);
        exit(1);
    }

    /* EB �饤�֥��� `bl' ��������*/
    eb_initialize_library();
    eb_initialize_booklist(&bl);

    /* EBNET �����Ф� `bl' �˷���դ��롣*/
    error_code = eb_bind_booklist(&bl, argv[1]);
    if (error_code != EB_SUCCESS) {
        fprintf(stderr, "%s: failed to bind the EBNET server, %s: %s\n",
            argv[0], eb_error_message(error_code), argv[1]);
        goto die;
    }

    /* �����о�ν��ҡ�appendix �θĿ��������*/
    error_code = eb_booklist_book_count(&bl, &book_count);
    if (error_code != EB_SUCCESS) {
        fprintf(stderr, "%s: failed to get the number of books, %s\n",
            argv[0], eb_error_message(error_code));
        goto die;
    }

    for (i = 0; i < book_count; i++) {
        /* ���ҡ�appendix ��̾�Τ������*/
        error_code = eb_booklist_book_name(&bl, i, &name);
	if (error_code != EB_SUCCESS) {
            fprintf(stderr, "%s: failed to get book name #%d, %s\n",
                argv[0], i, eb_error_message(error_code));
            goto die;
        }

        /* ���ҡ�appendix ����̾�������*/
        error_code = eb_booklist_book_name(&bl, i, &title);
	if (error_code != EB_SUCCESS) {
            fprintf(stderr, "%s: failed to get book title #%d, %s\n",
                argv[0], i, eb_error_message(error_code));
            goto die;
        }

        printf("%-20s  %s\n", name, title);
    }
        
    /* `bl' �� EB �饤�֥������Ѥ�λ��*/
    eb_finalize_booklist(&bl);
    eb_finalize_library();
    exit(0);

    /* ���顼ȯ���ǽ�λ����Ȥ��ν�����*/
  die:
    eb_finalize_booklist(&bl);
    eb_finalize_library();
    exit(1);
}
