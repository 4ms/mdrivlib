#pragma once
// HPDMA circular-buffer setup. The HPDMA has no hardware circular mode, so this
// builds a one-node circular linked-list queue that repeats the same block
// transfer forever. Half-transfer/transfer-complete flags fire per
// block, giving the usual double-buffer scheme.
//
// Currently CA35-only: uses the A35 EL check and AArch64 cache ops from
// system_reg.hh (in target/stm32mp2_ca35)

#include "drivers/system_reg.hh"
#include "stm32mp2xx_hal.h"

namespace mdrivlib
{

struct HPDmaCircular {

	// The caller must set the following fields before calling setup():
	// - hdma.Instance (e.g. HPDMA1_Channel0)
	// - hdma.Init.Request (e.g. HPDMA_REQUEST_SAI2_A)
	// - hdma.Init.Direction
	// - node_conf.SrcAddress
	// - node_conf.DstAddress
	// - node_conf.DataSize (in bytes)
	// node and queue must live as long as the transfer runs (and should be
	// alignas(64) so cache maintenance on them doesn't touch neighbors).
	// Returns false if any HAL call failed.
	static bool
	setup(DMA_HandleTypeDef &hdma, DMA_QListTypeDef &queue, DMA_NodeTypeDef &node, DMA_NodeConfTypeDef &node_conf) {
		bool ok = true;

		hdma.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
		hdma.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
		hdma.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
		hdma.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
		hdma.Init.SrcBurstLength = 1;
		hdma.Init.DestBurstLength = 1;
		hdma.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT1;
		hdma.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
		hdma.Init.Mode = DMA_NORMAL;

		if (hdma.Init.Direction == DMA_PERIPH_TO_MEMORY) {
			hdma.Init.SrcInc = DMA_SINC_FIXED;
			hdma.Init.DestInc = DMA_DINC_INCREMENTED;
		} else if (hdma.Init.Direction == DMA_MEMORY_TO_PERIPH) {
			hdma.Init.SrcInc = DMA_SINC_INCREMENTED;
			hdma.Init.DestInc = DMA_DINC_FIXED;
		} else if (hdma.Init.Direction == DMA_MEMORY_TO_MEMORY) {
			hdma.Init.SrcInc = DMA_SINC_INCREMENTED;
			hdma.Init.DestInc = DMA_DINC_INCREMENTED;
		}

		hdma.InitLinkedList.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
		hdma.InitLinkedList.LinkStepMode = DMA_LSM_FULL_EXECUTION;
		hdma.InitLinkedList.LinkAllocatedPort = DMA_LINK_ALLOCATED_PORT1;
		hdma.InitLinkedList.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
		hdma.InitLinkedList.LinkedListMode = DMA_LINKEDLIST_CIRCULAR;

		queue.State = HAL_DMA_QUEUE_STATE_RESET;
		queue.Type = 0;
		queue.Head = NULL;
		queue.FirstCircularNode = NULL;
		queue.NodeNumber = 0;

		node_conf.NodeType = DMA_HPDMA_LINEAR_NODE;
		node_conf.Init.Request = hdma.Init.Request;
		node_conf.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
		node_conf.Init.Direction = hdma.Init.Direction;
		node_conf.Init.SrcInc = hdma.Init.SrcInc;
		node_conf.Init.DestInc = hdma.Init.DestInc;
		node_conf.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
		node_conf.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
		node_conf.Init.SrcBurstLength = 1;
		node_conf.Init.DestBurstLength = 1;
		node_conf.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT1;
		node_conf.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
		node_conf.Init.Mode = DMA_NORMAL;
		node_conf.RepeatBlockConfig.RepeatCount = 1;
		node_conf.RepeatBlockConfig.SrcAddrOffset = 0;
		node_conf.RepeatBlockConfig.DestAddrOffset = 0;
		node_conf.RepeatBlockConfig.BlkSrcAddrOffset = 0;
		node_conf.RepeatBlockConfig.BlkDestAddrOffset = 0;
		node_conf.TriggerConfig.TriggerPolarity = DMA_TRIG_POLARITY_MASKED;
		node_conf.DataHandlingConfig.DataExchange = DMA_EXCHANGE_NONE;
		node_conf.DataHandlingConfig.DataAlignment = DMA_DATA_RIGHTALIGN_ZEROPADDED;
		node_conf.SrcSecure = DMA_CHANNEL_SRC_SEC;
		node_conf.DestSecure = DMA_CHANNEL_DEST_SEC;

		if (HAL_DMAEx_List_BuildNode(&node_conf, &node) != HAL_OK)
			ok = false;

		if (HAL_DMAEx_List_InsertNode_Tail(&queue, &node) != HAL_OK)
			ok = false;

		if (HAL_DMAEx_List_SetCircularMode(&queue) != HAL_OK)
			ok = false;

		HAL_DMAEx_List_ConvertQToDynamic(&queue);

		if (HAL_DMAEx_List_Init(&hdma) != HAL_OK)
			ok = false;

		if (HAL_DMAEx_List_LinkQ(&hdma, &queue) != HAL_OK)
			ok = false;

		clean_dcache_address((uintptr_t)(&node));
		clean_dcache_address((uintptr_t)(&queue));

		auto attr = (get_current_el() == 3)
					  ? (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC | DMA_CHANNEL_SRC_SEC | DMA_CHANNEL_DEST_SEC)
					  : DMA_CHANNEL_NPRIV;
		if (HAL_DMA_ConfigChannelAttributes(&hdma, attr) != HAL_OK)
			ok = false;

		return ok;
	}
};

} // namespace mdrivlib
