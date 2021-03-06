/*
** LuaJIT VM builder.
** Copyright (C) 2005-2017 Mike Pall. See Copyright Notice in luajit.h
*/

#ifndef _BUILDVM_H
#define _BUILDVM_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ravi_def.h"
#include "ravi_arch.h"

/* Hardcoded limits. Increase as needed. */
#define BUILD_MAX_RELOC		200	/* Max. number of relocations. */

/* Prefixes for generated labels. */
#define LABEL_PREFIX		"ravi_"
#define LABEL_PREFIX_BC		LABEL_PREFIX "BC_"
#define LABEL_PREFIX_FF		LABEL_PREFIX "ff_"
#define LABEL_PREFIX_CF		LABEL_PREFIX "cf_"
#define LABEL_PREFIX_FFH	LABEL_PREFIX "ffh_"
#define LABEL_PREFIX_LIBCF	LABEL_PREFIX "lib_cf_"
#define LABEL_PREFIX_LIBINIT	LABEL_PREFIX "lib_init_"

/* Forward declaration. */
struct dasm_State;

/* Build modes. */
#define BUILDDEF(_) \
  _(elfasm) _(coffasm) _(machasm) _(peobj) _(raw) \
  _(bcdef) 

typedef enum {
#define BUILDENUM(name)		BUILD_##name,
BUILDDEF(BUILDENUM)
#undef BUILDENUM
  BUILD__MAX
} BuildMode;

/* Code relocation. */
typedef struct BuildReloc {
  /* Offset of the relocatable symbol computed as (symbol - code) */
  int32_t RelativeOffset;
  /* index into ExportedSymbolNames array? */
  int sym;
  /* type = 0 means absolute address, type = 1 means relative address */
  int type;
} BuildReloc;

typedef struct BuildSym {
  const char *name;
  int32_t ofs;
} BuildSym;

/* Build context structure. */
typedef struct BuildCtx {
  /* DynASM state pointer. Should be first member. */
  struct dasm_State *D;
  /* Parsed command line. */
  BuildMode mode;
  FILE *fp;
  const char *outname;
  char **args;
  /* Code and symbols generated by DynASM. */
  uint8_t *code;
  size_t CodeSize;
  /* Number of entries in dispatch table */
  int SizeofDispatchTable;
  /* Number of exported functions such as luaV_interp()*/
  int NumberOfExportedSymbols;
  /* Count of all symbols including exported symbols and dispatch table functions - should be NUM_OPCODES+1 VM function */
  int NumberOfSymbols;
  int nrelocsym;
  /* Array of exported symbols excluding the start symbol */
  void **ExportedSymbols;
  /* Array of all symbols including exported symbols, start symbol and dispatch table entries */
  BuildSym *AllSymbols;
  /* Names of relocatable symbols - e.g. ImportedSymbols. Not sure why we need this and ImportedSymbolNames */
  const char **RelocatableSymbolNames;
  /* Offsets of lables in the dispatch table, one of each byte code */
  int32_t *DispatchTableOffsets;
  /* This is the first symbol (ravi_vm_asm_begin) - other symbols are relative to this one in the dispatch table */
  const char *StartSymbol;
  /* Strings generated by DynASM. */
  /* ExportedSymbolNames will point to an array of symbol names corresponding to ExportedSymbols */
  const char *const *ExportedSymbolNames;
  /* ImportedSymbolNames is an array of names of external functions called from the VM - e.g. luaF_close */
  const char *const *ImportedSymbolNames;
  const char *dasm_ident;
  const char *dasm_arch;
  /* Relocatable symbols (addresses) */
  BuildReloc Reloc[BUILD_MAX_RELOC];
  /* Size of above (i.e. used elements) */
  int RelocSize;
} BuildCtx;

extern void owrite(BuildCtx *ctx, const void *ptr, size_t sz);
extern void emit_asm(BuildCtx *ctx);
extern void emit_peobj(BuildCtx *ctx);

extern const char *const bc_names[];

#endif
