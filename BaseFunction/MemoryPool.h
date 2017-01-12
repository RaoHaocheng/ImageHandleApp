#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include "define.h"

// ��Ҫʹ�õĲ���
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
// ���õ��ڴ��ṹ��  
typedef struct memory_chunk
{
	memory_block* pfree_mem_addr;
	memory_chunk* pre;
	memory_chunk* next;
}memory_chunk;
// �ڴ�ؽṹ��  
typedef struct MEMORYPOOL
{
	void *memory;
	size_t size;
	memory_block* pmem_map;
	memory_chunk* pfree_mem_chunk;
	memory_chunk* pfree_mem_chunk_pool;
	size_t mem_used_size;					// ��¼�ڴ�����Ѿ�������û����ڴ�Ĵ�С  
	size_t mem_map_pool_count;				// ��¼����Ԫ�������ʣ��ĵ�Ԫ�ĸ���������Ϊ0ʱ���ܷ��䵥Ԫ��pfree_mem_chunk  
	size_t free_mem_chunk_count;			// ��¼ pfree_mem_chunk�����еĵ�Ԫ����  
	size_t mem_map_unit_count;   
	size_t mem_block_count;					// һ�� mem_unit ��СΪ MINUNITSIZE  
}MEMORYPOOL, *PMEMORYPOOL;

class DLLS_PORT MemoryPool
{
public:
	MemoryPool(size_t sBufSize);
	~MemoryPool();

	// �ⲿ�ӿں���
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

	// �ڲ�ʹ�ô���
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