/*
 * Этот файл является частью программы ElfLoader
 * Copyright (C) 2011 by Z.Vova, Ganster
 * Licence: GPLv3
 */

#include "loader.h"

#include <unistd.h>
#include <fcntl.h>

// Загрузка эльфа
Elf32_Exec *loader_elf_open(const char *filename) {
	int fp;
	Elf32_Ehdr ehdr;
	Elf32_Exec *ex;

	if ((fp = open(filename, O_RDONLY)) < 0)
		return 0;

	if (read(fp, &ehdr, sizeof(Elf32_Ehdr)) == sizeof(Elf32_Ehdr)) {
		if (!loader_check_elf(&ehdr)) {
			ex = malloc(sizeof(Elf32_Exec));

			if (ex) {
				memcpy(&ex->ehdr, &ehdr, sizeof(Elf32_Ehdr));
				ex->v_addr = (unsigned int)-1;
				ex->fp = fp;
				ex->body = 0;
				ex->type = EXEC_ELF;
				ex->libs = 0;
				ex->hashtab = 0;
				ex->complete = 0;
				ex->__is_ex_import = 0;
				ex->meloaded = 0;
				ex->switab = (int *)loader_library_impl();
				ex->fname = filename;

				const char *p = strrchr(filename, '\\');
				if (p) {
					++p;
					ex->temp_env = malloc(p - filename + 2);
					memcpy(ex->temp_env, filename, p - filename);
					ex->temp_env[p - filename] = 0;
				} else
					ex->temp_env = 0;

				if (!loader_load_sections(ex)) {
					ex->complete = 1;
					close(fp);
					ex->fname = 0;
					return ex;
				} else {
					loader_elf_close(ex);
				}
			}
		}
	}
	ex->fname = 0;
	close(fp);
	return 0;
}

void *loader_elf_entry(Elf32_Exec *ex) {
	if (!ex)
		return 0;
	return (ex->body->value + ex->ehdr.e_entry - ex->v_addr);
}

int loader_elf_close(Elf32_Exec *ex) {
	if (!ex)
		return E_EMPTY;

	if (ex->complete)
		loader_run_FINI_Array(ex);

	// Закрываем либы
	while (ex->libs) {
		Libs_Queue *lib = ex->libs;
		loader_lib_unref_clients(lib->lib);
		loader_lib_close(lib->lib, 0);
		ex->libs = lib->next;
		free(lib);
	}

	if (ex->hashtab)
		free(ex->hashtab);
	if (ex->body)
		free(ex->body);
	if (ex->temp_env)
		free(ex->temp_env);
	free(ex);
	return E_NO_ERROR;
}

int sub_elfclose(Elf32_Exec *ex) {
	loader_subproc_impl((void *)loader_elf_close, ex);
	return 0;
}
