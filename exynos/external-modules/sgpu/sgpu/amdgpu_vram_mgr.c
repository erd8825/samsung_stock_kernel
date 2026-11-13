/*
 * Copyright 2016 Advanced Micro Devices, Inc.
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Christian König
 */

#include <linux/dma-mapping.h>
#include <drm/ttm/ttm_range_manager.h>
#include "amdgpu.h"
#include "amdgpu_vm.h"
#include "amdgpu_atomfirmware.h"
#include "atom.h"

static inline struct amdgpu_vram_mgr *to_vram_mgr(struct ttm_resource_manager *man)
{
	return container_of(man, struct amdgpu_vram_mgr, manager);
}

static inline struct amdgpu_device *to_amdgpu_device(struct amdgpu_vram_mgr *mgr)
{
	return container_of(mgr, struct amdgpu_device, mman.vram_mgr);
}

/**
 * DOC: mem_info_vram_total
 *
 * The amdgpu driver provides a sysfs API for reporting current total VRAM
 * available on the device
 * The file mem_info_vram_total is used for this and returns the total
 * amount of VRAM in bytes
 */
static ssize_t amdgpu_mem_info_vram_total_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct drm_device *ddev = dev_get_drvdata(dev);
	struct amdgpu_device *adev = drm_to_adev(ddev);

	return snprintf(buf, PAGE_SIZE, "%llu\n", adev->gmc.real_vram_size);
}

/**
 * DOC: mem_info_vis_vram_total
 *
 * The amdgpu driver provides a sysfs API for reporting current total
 * visible VRAM available on the device
 * The file mem_info_vis_vram_total is used for this and returns the total
 * amount of visible VRAM in bytes
 */
static ssize_t amdgpu_mem_info_vis_vram_total_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct drm_device *ddev = dev_get_drvdata(dev);
	struct amdgpu_device *adev = drm_to_adev(ddev);

	return snprintf(buf, PAGE_SIZE, "%llu\n", adev->gmc.visible_vram_size);
}

/**
 * DOC: mem_info_vram_used
 *
 * The amdgpu driver provides a sysfs API for reporting current total VRAM
 * available on the device
 * The file mem_info_vram_used is used for this and returns the total
 * amount of currently used VRAM in bytes
 */
static ssize_t amdgpu_mem_info_vram_used_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct drm_device *ddev = dev_get_drvdata(dev);
	struct amdgpu_device *adev = drm_to_adev(ddev);
	struct ttm_resource_manager *man = ttm_manager_type(&adev->mman.bdev, TTM_PL_VRAM);

	return snprintf(buf, PAGE_SIZE, "%llu\n",
			amdgpu_vram_mgr_usage(man));
}

/**
 * DOC: mem_info_vis_vram_used
 *
 * The amdgpu driver provides a sysfs API for reporting current total of
 * used visible VRAM
 * The file mem_info_vis_vram_used is used for this and returns the total
 * amount of currently used visible VRAM in bytes
 */
static ssize_t amdgpu_mem_info_vis_vram_used_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct drm_device *ddev = dev_get_drvdata(dev);
	struct amdgpu_device *adev = drm_to_adev(ddev);
	struct ttm_resource_manager *man = ttm_manager_type(&adev->mman.bdev, TTM_PL_VRAM);

	return snprintf(buf, PAGE_SIZE, "%llu\n",
			amdgpu_vram_mgr_vis_usage(man));
}

static ssize_t amdgpu_mem_info_vram_vendor(struct device *dev,
						 struct device_attribute *attr,
						 char *buf)
{
	struct drm_device *ddev = dev_get_drvdata(dev);
	struct amdgpu_device *adev = drm_to_adev(ddev);

	switch (adev->gmc.vram_vendor) {
	case SAMSUNG:
		return snprintf(buf, PAGE_SIZE, "samsung\n");
	case INFINEON:
		return snprintf(buf, PAGE_SIZE, "infineon\n");
	case ELPIDA:
		return snprintf(buf, PAGE_SIZE, "elpida\n");
	case ETRON:
		return snprintf(buf, PAGE_SIZE, "etron\n");
	case NANYA:
		return snprintf(buf, PAGE_SIZE, "nanya\n");
	case HYNIX:
		return snprintf(buf, PAGE_SIZE, "hynix\n");
	case MOSEL:
		return snprintf(buf, PAGE_SIZE, "mosel\n");
	case WINBOND:
		return snprintf(buf, PAGE_SIZE, "winbond\n");
	case ESMT:
		return snprintf(buf, PAGE_SIZE, "esmt\n");
	case MICRON:
		return snprintf(buf, PAGE_SIZE, "micron\n");
	default:
		return snprintf(buf, PAGE_SIZE, "unknown\n");
	}
}

static DEVICE_ATTR(mem_info_vram_total, S_IRUGO,
		   amdgpu_mem_info_vram_total_show, NULL);
static DEVICE_ATTR(mem_info_vis_vram_total, S_IRUGO,
		   amdgpu_mem_info_vis_vram_total_show,NULL);
static DEVICE_ATTR(mem_info_vram_used, S_IRUGO,
		   amdgpu_mem_info_vram_used_show, NULL);
static DEVICE_ATTR(mem_info_vis_vram_used, S_IRUGO,
		   amdgpu_mem_info_vis_vram_used_show, NULL);
static DEVICE_ATTR(mem_info_vram_vendor, S_IRUGO,
		   amdgpu_mem_info_vram_vendor, NULL);

static const struct attribute *amdgpu_vram_mgr_attributes[] = {
	&dev_attr_mem_info_vram_total.attr,
	&dev_attr_mem_info_vis_vram_total.attr,
	&dev_attr_mem_info_vram_used.attr,
	&dev_attr_mem_info_vis_vram_used.attr,
	&dev_attr_mem_info_vram_vendor.attr,
	NULL
};

static const struct ttm_resource_manager_func amdgpu_vram_mgr_func;

/**
 * amdgpu_vram_mgr_init - init VRAM manager and DRM MM
 *
 * @adev: amdgpu_device pointer
 *
 * Allocate and initialize the VRAM manager.
 */
int amdgpu_vram_mgr_init(struct amdgpu_device *adev)
{
	struct amdgpu_vram_mgr *mgr = &adev->mman.vram_mgr;
	struct ttm_resource_manager *man = &mgr->manager;
	int ret;

	ttm_resource_manager_init(man, &adev->mman.bdev, adev->gmc.real_vram_size);

	man->func = &amdgpu_vram_mgr_func;

	drm_mm_init(&mgr->mm, 0, man->size);
	spin_lock_init(&mgr->lock);

	/* Add the two VRAM-related sysfs files */
	ret = sysfs_create_files(&adev->dev->kobj, amdgpu_vram_mgr_attributes);
	if (ret)
		DRM_ERROR("Failed to register sysfs\n");

	ttm_set_driver_manager(&adev->mman.bdev, TTM_PL_VRAM, &mgr->manager);
	ttm_resource_manager_set_used(man, true);
	return 0;
}

/**
 * amdgpu_vram_mgr_fini - free and destroy VRAM manager
 *
 * @adev: amdgpu_device pointer
 *
 * Destroy and free the VRAM manager, returns -EBUSY if ranges are still
 * allocated inside it.
 */
void amdgpu_vram_mgr_fini(struct amdgpu_device *adev)
{
	struct amdgpu_vram_mgr *mgr = &adev->mman.vram_mgr;
	struct ttm_resource_manager *man = &mgr->manager;
	int ret;

	ttm_resource_manager_set_used(man, false);

	ret = ttm_resource_manager_evict_all(&adev->mman.bdev, man);
	if (ret)
		return;

	spin_lock(&mgr->lock);
	drm_mm_takedown(&mgr->mm);
	spin_unlock(&mgr->lock);

	sysfs_remove_files(&adev->dev->kobj, amdgpu_vram_mgr_attributes);

	ttm_resource_manager_cleanup(man);
	ttm_set_driver_manager(&adev->mman.bdev, TTM_PL_VRAM, NULL);
}

/**
 * amdgpu_vram_mgr_vis_size - Calculate visible node size
 *
 * @adev: amdgpu_device pointer
 * @node: MM node structure
 *
 * Calculate how many bytes of the MM node are inside visible VRAM
 */
static u64 amdgpu_vram_mgr_vis_size(struct amdgpu_device *adev,
				    struct drm_mm_node *node)
{
	uint64_t start = node->start << PAGE_SHIFT;
	uint64_t end = (node->size + node->start) << PAGE_SHIFT;

	if (start >= adev->gmc.visible_vram_size)
		return 0;

	return (end > adev->gmc.visible_vram_size ?
		adev->gmc.visible_vram_size : end) - start;
}

/**
 * amdgpu_vram_mgr_bo_visible_size - CPU visible BO size
 *
 * @bo: &amdgpu_bo buffer object (must be in VRAM)
 *
 * Returns:
 * How much of the given &amdgpu_bo buffer object lies in CPU visible VRAM.
 */
u64 amdgpu_vram_mgr_bo_visible_size(struct amdgpu_bo *bo)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bo->tbo.bdev);
	struct ttm_resource *mem = bo->tbo.resource;
	struct drm_mm_node *nodes = &container_of(mem, struct ttm_range_mgr_node, base)->mm_nodes[0];
;
	unsigned pages = mem->size / PAGE_SIZE;
	u64 usage;

	if (amdgpu_gmc_vram_full_visible(&adev->gmc))
		return amdgpu_bo_size(bo);

	if (mem->start >= adev->gmc.visible_vram_size >> PAGE_SHIFT)
		return 0;

	for (usage = 0; nodes && pages; pages -= nodes->size, nodes++)
		usage += amdgpu_vram_mgr_vis_size(adev, nodes);

	return usage;
}

/**
 * amdgpu_vram_mgr_virt_start - update virtual start address
 *
 * @mem: ttm_resource to update
 * @node: just allocated node
 *
 * Calculate a virtual BO start address to easily check if everything is CPU
 * accessible.
 */
static void amdgpu_vram_mgr_virt_start(struct ttm_resource *mem,
				       struct drm_mm_node *node)
{
	unsigned long start;

	start = node->start + node->size;
	if (start > mem->size / PAGE_SIZE)
		start -= mem->size / PAGE_SIZE;
	else
		start = 0;
	mem->start = max(mem->start, start);
}

/**
 * amdgpu_vram_mgr_new - allocate new ranges
 *
 * @man: TTM memory type manager
 * @tbo: TTM BO we need this range for
 * @place: placement flags and restrictions
 * @res: the resulting mem object
 *
 * Allocate VRAM for the given BO.
 */
static int amdgpu_vram_mgr_new(struct ttm_resource_manager *man,
			       struct ttm_buffer_object *tbo,
			       const struct ttm_place *place,
			       struct ttm_resource **res)
{
	u64 vis_usage = 0, max_bytes, cur_size, min_block_size;
	struct amdgpu_vram_mgr *mgr = to_vram_mgr(man);
	struct amdgpu_device *adev = to_amdgpu_device(mgr);
	struct amdgpu_vram_mgr_resource *vres;
	u64 size, remaining_size, lpfn, fpfn;
	struct drm_buddy *mm = &mgr->mm;
	struct drm_buddy_block *block;
	unsigned long pages_per_block;
	int r;

	lpfn = (u64)place->lpfn << PAGE_SHIFT;
	if (!lpfn || lpfn > man->size)
		lpfn = man->size;

	fpfn = (u64)place->fpfn << PAGE_SHIFT;

	max_bytes = adev->gmc.mc_vram_size;
	if (tbo->type != ttm_bo_type_kernel)
		max_bytes -= AMDGPU_VM_RESERVED_VRAM;

	if (place->flags & TTM_PL_FLAG_CONTIGUOUS) {
		pages_per_block = ~0ul;
	} else {
#ifdef CONFIG_TRANSPARENT_HUGEPAGE
		pages_per_block = HPAGE_PMD_NR;
#else
		/* default to 2MB */
		pages_per_block = 2UL << (20UL - PAGE_SHIFT);
#endif
		pages_per_block = max_t(uint32_t, pages_per_block,
					tbo->page_alignment);
	}

	vres = kzalloc(sizeof(*vres), GFP_KERNEL);
	if (!vres)
		return -ENOMEM;

	ttm_resource_init(tbo, place, &vres->base);

	/* bail out quickly if there's likely not enough VRAM for this BO */
	if (ttm_resource_manager_usage(man) > max_bytes) {
		r = -ENOSPC;
		goto error_fini;
	}

	INIT_LIST_HEAD(&vres->blocks);

	if (place->flags & TTM_PL_FLAG_TOPDOWN)
		vres->flags |= DRM_BUDDY_TOPDOWN_ALLOCATION;

	if (fpfn || lpfn != mgr->mm.size)
		/* Allocate blocks in desired range */
		vres->flags |= DRM_BUDDY_RANGE_ALLOCATION;

	remaining_size = (u64)vres->base.size;

	mutex_lock(&mgr->lock);
	while (remaining_size) {
		if (tbo->page_alignment)
			min_block_size = (u64)tbo->page_alignment << PAGE_SHIFT;
		else
			min_block_size = mgr->default_page_size;

		BUG_ON(min_block_size < mm->chunk_size);

		/* Limit maximum size to 2GiB due to SG table limitations */
		size = min(remaining_size, 2ULL << 30);

		if ((size >= (u64)pages_per_block << PAGE_SHIFT) &&
				!(size & (((u64)pages_per_block << PAGE_SHIFT) - 1)))
			min_block_size = (u64)pages_per_block << PAGE_SHIFT;

		cur_size = size;

		if (fpfn + size != (u64)place->lpfn << PAGE_SHIFT) {
			/*
			 * Except for actual range allocation, modify the size and
			 * min_block_size conforming to continuous flag enablement
			 */
			if (place->flags & TTM_PL_FLAG_CONTIGUOUS) {
				size = roundup_pow_of_two(size);
				min_block_size = size;
			/*
			 * Modify the size value if size is not
			 * aligned with min_block_size
			 */
			} else if (!IS_ALIGNED(size, min_block_size)) {
				size = round_up(size, min_block_size);
			}
		}

		r = drm_buddy_alloc_blocks(mm, fpfn,
					   lpfn,
					   size,
					   min_block_size,
					   &vres->blocks,
					   vres->flags);
		if (unlikely(r))
			goto error_free_blocks;

		if (size > remaining_size)
			remaining_size = 0;
		else
			remaining_size -= size;
	}
	mutex_unlock(&mgr->lock);

	if (cur_size != size) {
		struct drm_buddy_block *block;
		struct list_head *trim_list;
		u64 original_size;
		LIST_HEAD(temp);

		trim_list = &vres->blocks;
		original_size = (u64)vres->base.size;

		/*
		 * If size value is rounded up to min_block_size, trim the last
		 * block to the required size
		 */
		if (!list_is_singular(&vres->blocks)) {
			block = list_last_entry(&vres->blocks, typeof(*block), link);
			list_move_tail(&block->link, &temp);
			trim_list = &temp;
			/*
			 * Compute the original_size value by subtracting the
			 * last block size with (aligned size - original size)
			 */
			original_size = amdgpu_vram_mgr_block_size(block) - (size - cur_size);
		}

		mutex_lock(&mgr->lock);
		drm_buddy_block_trim(mm,
				     original_size,
				     trim_list);
		mutex_unlock(&mgr->lock);

		if (!list_empty(&temp))
			list_splice_tail(trim_list, &vres->blocks);
	}

	vres->base.start = 0;
	list_for_each_entry(block, &vres->blocks, link) {
		unsigned long start;

		start = amdgpu_vram_mgr_block_start(block) +
			amdgpu_vram_mgr_block_size(block);
		start >>= PAGE_SHIFT;

		if (start > PFN_UP(vres->base.size))
			start -= PFN_UP(vres->base.size);
		else
			start = 0;
		vres->base.start = max(vres->base.start, start);

		vis_usage += amdgpu_vram_mgr_vis_size(adev, block);
	}

	if (amdgpu_is_vram_mgr_blocks_contiguous(&vres->blocks))
		vres->base.placement |= TTM_PL_FLAG_CONTIGUOUS;

	if (adev->gmc.xgmi.connected_to_cpu)
		vres->base.bus.caching = ttm_cached;
	else
		vres->base.bus.caching = ttm_write_combined;

	atomic64_add(vis_usage, &mgr->vis_usage);
	*res = &vres->base;
	return 0;

error_free_blocks:
	drm_buddy_free_list(mm, &vres->blocks);
	mutex_unlock(&mgr->lock);
error_fini:
	ttm_resource_fini(man, &vres->base);
	kfree(vres);

	return r;
}

/**
 * amdgpu_vram_mgr_del - free ranges
 *
 * @man: TTM memory type manager
 * @mem: TTM memory object
 *
 * Free the allocated VRAM again.
 */
static void amdgpu_vram_mgr_del(struct ttm_resource_manager *man,
				struct ttm_resource *mem)
{
	struct amdgpu_vram_mgr *mgr = to_vram_mgr(man);
	struct amdgpu_device *adev = to_amdgpu_device(mgr);
	struct drm_mm_node *nodes;
	uint64_t usage = 0, vis_usage = 0;
	unsigned pages = mem->size / PAGE_SIZE;

	/* In newer kernels, we need to use ttm_range_mgr_node */
	/* For compatibility, we need to handle this differently */
	/* This is a placeholder for proper implementation */
	return;

	spin_lock(&mgr->lock);
	while (pages) {
		pages -= nodes->size;
		drm_mm_remove_node(nodes);
		usage += nodes->size << PAGE_SHIFT;
		vis_usage += amdgpu_vram_mgr_vis_size(adev, nodes);
		++nodes;
	}
	spin_unlock(&mgr->lock);

	atomic64_sub(usage, &mgr->usage);
	atomic64_sub(vis_usage, &mgr->vis_usage);

	/* kvfree(nodes); */
	/* mem->mm_node = NULL; */
}

/**
 * amdgpu_vram_mgr_alloc_sgt - allocate and fill a sg table
 *
 * @adev: amdgpu device pointer
 * @mem: TTM memory object
 * @dev: the other device
 * @dir: dma direction
 * @sgt: resulting sg table
 *
 * Allocate and fill a sg table from a VRAM allocation.
 */
int amdgpu_vram_mgr_alloc_sgt(struct amdgpu_device *adev,
			      struct ttm_resource *mem,
			      struct device *dev,
			      enum dma_data_direction dir,
			      struct sg_table **sgt)
{
	struct drm_mm_node *node;
	struct scatterlist *sg;
	int num_entries = 0;
	unsigned int pages;
	int i, r;

	*sgt = kmalloc(sizeof(**sgt), GFP_KERNEL);
	if (!*sgt)
		return -ENOMEM;

	/* In newer kernels, we need to use ttm_range_mgr_node */
	/* For compatibility, this needs to be handled differently */
	for (pages = mem->size / PAGE_SIZE, node = NULL;
	     pages; pages -= 0, ++node)
		++num_entries;

	r = sg_alloc_table(*sgt, num_entries, GFP_KERNEL);
	if (r)
		goto error_free;

	for_each_sgtable_sg((*sgt), sg, i)
		sg->length = 0;

	/* In newer kernels, we need to use ttm_range_mgr_node */
	/* For compatibility, this needs to be handled differently */
	node = NULL;
	for_each_sgtable_sg((*sgt), sg, i) {
		phys_addr_t phys = (node->start << PAGE_SHIFT) +
			adev->gmc.aper_base;
		size_t size = node->size << PAGE_SHIFT;
		dma_addr_t addr;

		++node;
		addr = dma_map_resource(dev, phys, size, dir,
					DMA_ATTR_SKIP_CPU_SYNC);
		r = dma_mapping_error(dev, addr);
		if (r)
			goto error_unmap;

		sg_set_page(sg, NULL, size, 0);
		sg_dma_address(sg) = addr;
		sg_dma_len(sg) = size;
	}
	return 0;

error_unmap:
	for_each_sgtable_sg((*sgt), sg, i) {
		if (!sg->length)
			continue;

		dma_unmap_resource(dev, sg->dma_address,
				   sg->length, dir,
				   DMA_ATTR_SKIP_CPU_SYNC);
	}
	sg_free_table(*sgt);

error_free:
	kfree(*sgt);
	return r;
}

/**
 * amdgpu_vram_mgr_alloc_sgt - allocate and fill a sg table
 *
 * @adev: amdgpu device pointer
 * @sgt: sg table to free
 *
 * Free a previously allocate sg table.
 */
void amdgpu_vram_mgr_free_sgt(struct amdgpu_device *adev,
			      struct device *dev,
			      enum dma_data_direction dir,
			      struct sg_table *sgt)
{
	struct scatterlist *sg;
	int i;

	for_each_sgtable_sg(sgt, sg, i)
		dma_unmap_resource(dev, sg->dma_address,
				   sg->length, dir,
				   DMA_ATTR_SKIP_CPU_SYNC);
	sg_free_table(sgt);
	kfree(sgt);
}

/**
 * amdgpu_vram_mgr_usage - how many bytes are used in this domain
 *
 * @man: TTM memory type manager
 *
 * Returns how many bytes are used in this domain.
 */
uint64_t amdgpu_vram_mgr_usage(struct ttm_resource_manager *man)
{
	struct amdgpu_vram_mgr *mgr = to_vram_mgr(man);

	if (amdgpu_force_gtt)
		return 0;

	return atomic64_read(&mgr->usage);
}

/**
 * amdgpu_vram_mgr_vis_usage - how many bytes are used in the visible part
 *
 * @man: TTM memory type manager
 *
 * Returns how many bytes are used in the visible part of VRAM
 */
uint64_t amdgpu_vram_mgr_vis_usage(struct ttm_resource_manager *man)
{
	struct amdgpu_vram_mgr *mgr = to_vram_mgr(man);

	if (amdgpu_force_gtt)
		return 0;

	return atomic64_read(&mgr->vis_usage);
}

/**
 * amdgpu_vram_mgr_debug - dump VRAM table
 *
 * @man: TTM memory type manager
 * @printer: DRM printer to use
 *
 * Dump the table content using printk.
 */
static void amdgpu_vram_mgr_debug(struct ttm_resource_manager *man,
				  struct drm_printer *printer)
{
	struct amdgpu_vram_mgr *mgr = to_vram_mgr(man);

	spin_lock(&mgr->lock);
	drm_mm_print(&mgr->mm, printer);
	spin_unlock(&mgr->lock);

	drm_printf(printer, "man size:%llu pages, ram usage:%lluMB, vis usage:%lluMB\n",
		   man->size, amdgpu_vram_mgr_usage(man) >> 20,
		   amdgpu_vram_mgr_vis_usage(man) >> 20);
}

static const struct ttm_resource_manager_func amdgpu_vram_mgr_func = {
	.alloc	= amdgpu_vram_mgr_new,
	.free	= amdgpu_vram_mgr_del,
	.debug	= amdgpu_vram_mgr_debug
};
