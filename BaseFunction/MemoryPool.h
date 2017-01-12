#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include "define.h"

// 需要使用的参数
const int MINUNITSIZE = 512;
const int ADDR_ALIGN = 8;
const int SIZE_ALIGN = MINUNITSIZE;

struct memory_chunk;
typedef struct memory_block
{
	size_t count;
	size_t start;
	memory_chunk* pmem_chunk;
}memory_block;
// 可用的内存块结构体  
typedef struct memory_chunk
{
	memory_block* pfree_mem_addr;
	memory_chunk* pre;
	memory_chunk* next;
}memory_chunk;
// 内存池结构体  
typedef struct MEMORYPOOL
{
	void *memory;
	size_t size;
	memory_block* pmem_map;
	memory_chunk* pfree_mem_chunk;
	memory_chunk* pfree_mem_chunk_pool;
	size_t mem_used_size;					// 记录内存池中已经分配给用户的内存的大小  
	size_t mem_map_pool_count;				// 记录链表单元缓冲池中剩余的单元的个数，个数为0时不能分配单元给pfree_mem_chunk  
	size_t free_mem_chunk_count;			// 记录 pfree_mem_chunk链表中的单元个数  
	size_t mem_map_unit_count;   
	size_t mem_block_count;					// 一个 mem_unit 大小为 MINUNITSIZE  
}MEMORYPOOL, *PMEMORYPOOL;

class DLLS_PORT MemoryPool
{
public:
	MemoryPool(size_t sBufSize);
	~MemoryPool();

	// 外部接口函数
public:
	bool CreateMemoryPool(size_t sBufSize);
	bool ReleaseMemory();
	void* GetMemory(size_t sMemorySize);
	void FreeMemory(void *ptrMemoryBlock);
	inline const PMEMORYPOOL MemoryPoolInfo() { return m_memoryPool; }
protected:
	PMEMORYPOOL CreateMemoryPool(void* pBuf, size_t sBufSize);
	void ReleaseMemory(PMEMORYPOOL* ppMem);
	void* GetMemory(size_t sMemorySize, PMEMORYPOOL pMem);
	void FreeMemory(void *ptrMemoryBlock, PMEMORYPOOL pMem);

	// 内部使用代码
	size_t CheckAlignAddr(void*& pBuf);
	size_t CheckAlignBlock(size_t size);
	size_t CheckAlignSize(size_t size);
	memory_chunk* CreateList(memory_chunk* pool, size_t count);
	memory_chunk* FrontPop(memory_chunk*& pool);
	void PushBack(memory_chunk*& head, memory_chunk* element);
	void PushFront(memory_chunk*& head, memory_chunk* element);
	void DeleteChunk(memory_chunk*& head, memory_chunk* element);
	void* Index2Addr(PMEMORYPOOL mem_pool, size_t index);
	size_t Addr2Index(PMEMORYPOOL mem_pool, void* addr);

private:
	PMEMORYPOOL m_memoryPool;
	void* m_pBuf;
};

#endif