/**
  ******************************************************************************
  * @file    uf4_mem.h
  * @brief   Common placement and inline attributes for hot application paths.
  ******************************************************************************
  */
#ifndef UF4_MEM_H
#define UF4_MEM_H

#ifdef __GNUC__
#define UF4_FORCE_INLINE static inline __attribute__((always_inline))
#define UF4_RAM_FUNC     __attribute__((section(".RamFunc")))
#define UF4_FAST_CONST   __attribute__((section(".data.fast_const")))
#define UF4_FAST_DATA    __attribute__((section(".data.fast")))
#else
#define UF4_FORCE_INLINE static inline
#define UF4_RAM_FUNC
#define UF4_FAST_CONST
#define UF4_FAST_DATA
#endif

#endif /* UF4_MEM_H */
