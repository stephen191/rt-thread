/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-01-30     lizhirui     first version
 * 2023-10-12     Shell        Add permission control API
 */

#ifndef __RISCV_MMU_H__
#define __RISCV_MMU_H__

#include <rtthread.h>
#include <rthw.h>
#include "riscv.h"

#undef PAGE_SIZE

#define PAGE_OFFSET_SHIFT 0
#define PAGE_OFFSET_BIT 12
#define PAGE_SIZE __SIZE(PAGE_OFFSET_BIT)
#define PAGE_OFFSET_MASK __MASK(PAGE_OFFSET_BIT)
#define VPN0_SHIFT (PAGE_OFFSET_SHIFT + PAGE_OFFSET_BIT)
#define VPN0_BIT 9
#define VPN1_SHIFT (VPN0_SHIFT + VPN0_BIT)
#define VPN1_BIT 9
#define VPN2_SHIFT (VPN1_SHIFT + VPN1_BIT)
#define VPN2_BIT 9

#define PPN0_SHIFT (PAGE_OFFSET_SHIFT + PAGE_OFFSET_BIT)
#define PPN0_BIT 9
#define PPN1_SHIFT (PPN0_SHIFT + PPN0_BIT)
#define PPN1_BIT 9
#define PPN2_SHIFT (PPN1_SHIFT + PPN1_BIT)
#define PPN2_BIT 26

#define L1_PAGE_SIZE __SIZE(PAGE_OFFSET_BIT + VPN0_BIT + VPN1_BIT)
#define L2_PAGE_SIZE __SIZE(PAGE_OFFSET_BIT + VPN0_BIT)
#define L3_PAGE_SIZE __SIZE(PAGE_OFFSET_BIT)

#define ARCH_ADDRESS_WIDTH_BITS 64

#define PHYSICAL_ADDRESS_WIDTH_BITS 56

#define PAGE_ATTR_NEXT_LEVEL (0)
#define PAGE_ATTR_RWX (PTE_X | PTE_W | PTE_R)
#define PAGE_ATTR_READONLY (PTE_R)
#define PAGE_ATTR_READEXECUTE (PTE_X | PTE_R)

#define PAGE_ATTR_USER (PTE_U)
#define PAGE_ATTR_SYSTEM (0)

#define PAGE_DEFAULT_ATTR_LEAF (PAGE_ATTR_RWX | PAGE_ATTR_USER | PTE_V | PTE_G)
#define PAGE_DEFAULT_ATTR_NEXT (PAGE_ATTR_NEXT_LEVEL | PTE_V | PTE_G)

#define PAGE_IS_LEAF(pte) __MASKVALUE(pte, PAGE_ATTR_RWX)

#define PTE_USED(pte) __MASKVALUE(pte, PTE_V)

/**
 * encoding of SATP (Supervisor Address Translation and Protection register)
 */
#define SATP_MODE_OFFSET    60
#define SATP_MODE_BARE      0
#define SATP_MODE_SV39      8
#define SATP_MODE_SV48      9
#define SATP_MODE_SV57      10
#define SATP_MODE_SV64      11

#define ARCH_VADDR_WIDTH        39
#define SATP_MODE               SATP_MODE_SV39

#define MMU_MAP_K_DEVICE        (PTE_G | PTE_W | PTE_R | PTE_V)
#define MMU_MAP_K_RWCB          (PTE_G | PTE_X | PTE_W | PTE_R | PTE_V)
#define MMU_MAP_K_RW            (PTE_G | PTE_X | PTE_W | PTE_R | PTE_V)
#define MMU_MAP_U_RWCB          (PTE_U | PTE_X | PTE_W | PTE_R | PTE_V)
#define MMU_MAP_U_RWCB_XN       (PTE_U | PTE_W | PTE_R | PTE_V)
#define MMU_MAP_U_RW            (PTE_U | PTE_X | PTE_W | PTE_R | PTE_V)

#define PTE_XWR_MASK            0xe

#define ARCH_PAGE_SIZE          PAGE_SIZE
#define ARCH_PAGE_MASK          (ARCH_PAGE_SIZE - 1)
#define ARCH_PAGE_SHIFT         PAGE_OFFSET_BIT
#define ARCH_INDEX_WIDTH        9
#define ARCH_INDEX_SIZE         (1ul << ARCH_INDEX_WIDTH)
#define ARCH_INDEX_MASK         (ARCH_INDEX_SIZE - 1)

#define ARCH_MAP_FAILED         ((void *)0x8000000000000000)

void mmu_set_pagetable(rt_ubase_t addr);
void mmu_enable_user_page_access();
void mmu_disable_user_page_access();

enum rt_hw_mmu_prot_t {
    RT_HW_MMU_PROT_READ,
    RT_HW_MMU_PROT_WRITE,
    RT_HW_MMU_PROT_EXECUTE,
    RT_HW_MMU_PROT_KERNEL,
    RT_HW_MMU_PROT_USER,
    RT_HW_MMU_PROT_CACHE,
};

/**
 * @brief Remove permission from attribution
 *
 * @param attr architecture specified mmu attribution
 * @param prot protect that will be removed
 * @return size_t returned attribution
 */
rt_inline size_t rt_hw_mmu_attr_rm_perm(size_t attr, enum rt_hw_mmu_prot_t prot)
{
    switch (prot)
    {
        /* remove write permission for user */
        case RT_HW_MMU_PROT_WRITE | RT_HW_MMU_PROT_USER:
            attr &= ~PTE_W;
        default:
            RT_ASSERT(0);
    }
    return attr;
}

/**
 * @brief Add permission from attribution
 *
 * @param attr architecture specified mmu attribution
 * @param prot protect that will be added
 * @return size_t returned attribution
 */
rt_inline size_t rt_hw_mmu_attr_add_perm(size_t attr, enum rt_hw_mmu_prot_t prot)
{
    switch (prot)
    {
        /* add write permission for user */
        case RT_HW_MMU_PROT_WRITE | RT_HW_MMU_PROT_USER:
            attr |= PTE_W;
        default:
            RT_ASSERT(0);
    }
    return attr;
}

/**
 * @brief Test permission from attribution
 *
 * @param attr architecture specified mmu attribution
 * @param prot protect that will be test
 * @return rt_bool_t RT_TRUE if the prot is allowed, otherwise RT_FALSE
 */
rt_inline rt_bool_t rt_hw_mmu_attr_test_perm(size_t attr, enum rt_hw_mmu_prot_t prot)
{
    rt_bool_t rc = 0;
    switch (prot)
    {
        /* test write permission for user */
        case RT_HW_MMU_PROT_WRITE | RT_HW_MMU_PROT_USER:
            rc = !!(attr & PTE_W);
        default:
            RT_ASSERT(0);
    }
    return rc;
}

#endif
