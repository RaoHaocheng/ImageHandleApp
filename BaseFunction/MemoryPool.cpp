#include "MemoryPool.h"
#include <memory.h>  

/***************************************************************************
* �������ƣ�   MemoryPool
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  size_t sBufSize
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
MemoryPool::MemoryPool(size_t sBufSize)
	:m_memoryPool(NULL),
	m_pBuf(NULL)
{
	CreateMemoryPool(sBufSize);
}

/***************************************************************************
* �������ƣ�   ~MemoryPool
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
MemoryPool::~MemoryPool()
{
	ReleaseMemory();
}

/***************************************************************************
* �������ƣ�   CreateMemoryPool
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  size_t sBufSize
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
bool MemoryPool::CreateMemoryPool(size_t sBufSize)
{
	if (NULL != m_pBuf)
		ReleaseMemory();

	m_pBuf = new unsigned char[sBufSize];
	if (NULL == m_pBuf)
		return false;

	m_memoryPool = CreateMemoryPool(m_pBuf, sBufSize);
	return true;
}

/***************************************************************************
* �������ƣ�   ReleaseMemory
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
bool MemoryPool::ReleaseMemory()
{
	if (NULL != m_pBuf)
		delete[] m_pBuf;

	m_memoryPool = NULL;
	return true;
}

/***************************************************************************
* �������ƣ�   GetMemory
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  size_t sMemorySize
* ����ֵ����   void*
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
void* MemoryPool::GetMemory(size_t sMemorySize)
{
	if (NULL == m_memoryPool)
		return NULL;

	return GetMemory(sMemorySize, m_memoryPool);
}

/***************************************************************************
* �������ƣ�   FreeMemory
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  void * ptrMemoryBlock
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
void MemoryPool::FreeMemory(void *ptrMemoryBlock)
{
	if (NULL == m_memoryPool)
		return;

	FreeMemory(ptrMemoryBlock, m_memoryPool);
}

/***************************************************************************
* �������ƣ�   CheckAlignAddr
* ժ����Ҫ��   �ڴ����ʼ��ַ���뵽ADDR_ALIGN�ֽ�
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  void * & pBuf
* ����ֵ����   size_t
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
size_t MemoryPool::CheckAlignAddr(void*& pBuf)
{
	size_t align = 0;
	size_t addr = (int)pBuf;
	align = (ADDR_ALIGN - addr % ADDR_ALIGN) % ADDR_ALIGN;
	pBuf = (char*)pBuf + align;
	return align;
}

/***************************************************************************
* �������ƣ�   CheckAlignBlock
* ժ����Ҫ��   �ڴ�block��С���뵽MINUNITSIZE�ֽ�
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  size_t size
* ����ֵ����   size_t
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
size_t MemoryPool::CheckAlignBlock(size_t size)
{
	size_t align = size % MINUNITSIZE;

	return size - align;
}


/***************************************************************************
* �������ƣ�   CheckAlignSize
* ժ����Ҫ��   �����ڴ��С���뵽SIZE_ALIGN�ֽ�
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  size_t size
* ����ֵ����   size_t
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
size_t MemoryPool::CheckAlignSize(size_t size)
{
	size = (size + SIZE_ALIGN - 1) / SIZE_ALIGN * SIZE_ALIGN;
	return size;
}


/***************************************************************************
* �������ƣ�   CreateList
* ժ����Ҫ��   ������������ز���
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  memory_chunk * pool
* �Ρ�������   [in]  size_t count
* ����ֵ����   memory_chunk*
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
memory_chunk* MemoryPool::CreateList(memory_chunk* pool, size_t count)
{
	if (!pool)
	{
		return NULL;
	}
	memory_chunk* head = NULL;
	for (size_t i = 0; i < count; i++)
	{
		pool->pre = NULL;
		pool->next = head;
		if (head != NULL)
		{
			head->pre = pool;
		}
		head = pool;
		pool++;
	}
	return head;
}
/***************************************************************************
* �������ƣ�   FrontPop
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  memory_chunk * & pool
* ����ֵ����   memory_chunk*
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
memory_chunk* MemoryPool::FrontPop(memory_chunk*& pool)
{
	if (!pool)
	{
		return NULL;
	}
	memory_chunk* tmp = pool;
	pool = tmp->next;
	pool->pre = NULL;
	return  tmp;
}
/***************************************************************************
* �������ƣ�   PushBack
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  memory_chunk * & head
* �Ρ�������   [in]  memory_chunk * element
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
void MemoryPool::PushBack(memory_chunk*& head, memory_chunk* element)
{
	if (head == NULL)
	{
		head = element;
		head->pre = element;
		head->next = element;
		return;
	}
	head->pre->next = element;
	element->pre = head->pre;
	head->pre = element;
	element->next = head;
}
/***************************************************************************
* �������ƣ�   PushFront
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  memory_chunk * & head
* �Ρ�������   [in]  memory_chunk * element
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
void MemoryPool::PushFront(memory_chunk*& head, memory_chunk* element)
{
	element->pre = NULL;
	element->next = head;
	if (head != NULL)
	{
		head->pre = element;
	}
	head = element;
}
void MemoryPool::DeleteChunk(memory_chunk*& head, memory_chunk* element)
{
	// ��˫ѭ��������ɾ��Ԫ��  
	if (element == NULL)
	{
		return;
	}
	// elementΪ����ͷ  
	else if (element == head)
	{
		// ����ֻ��һ��Ԫ��  
		if (head->pre == head)
		{
			head = NULL;
		}
		else
		{
			head = element->next;
			head->pre = element->pre;
			head->pre->next = head;
		}
	}
	// elementΪ����β  
	else if (element->next == head)
	{
		head->pre = element->pre;
		element->pre->next = head;
	}
	else
	{
		element->pre->next = element->next;
		element->next->pre = element->pre;
	}
	element->pre = NULL;
	element->next = NULL;
}

/***************************************************************************
* �������ƣ�   Index2Addr
* ժ����Ҫ��   �ڴ�ӳ����е�����ת��Ϊ�ڴ���ʼ��ַ
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  PMEMORYPOOL mem_pool
* �Ρ�������   [in]  size_t index
* ����ֵ����   void*
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
void* MemoryPool::Index2Addr(PMEMORYPOOL mem_pool, size_t index)
{
	char* p = (char*)(mem_pool->memory);
	void* ret = (void*)(p + index *MINUNITSIZE);

	return ret;
}

/***************************************************************************
* �������ƣ�   Addr2Index
* ժ����Ҫ��   �ڴ���ʼ��ַת��Ϊ�ڴ�ӳ����е�����
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  PMEMORYPOOL mem_pool
* �Ρ�������   [in]  void * addr
* ����ֵ����   size_t
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
size_t MemoryPool::Addr2Index(PMEMORYPOOL mem_pool, void* addr)
{
	char* start = (char*)(mem_pool->memory);
	char* p = (char*)addr;
	size_t index = (p - start) / MINUNITSIZE;
	return index;
}

/***************************************************************************
* �������ƣ�   CreateMemoryPool
* ժ����Ҫ��   pBuf: �������ڴ�buffer��ʼ��ַ,sBufSize: �������ڴ�buffer��С
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  void * pBuf
* �Ρ�������   [in]  size_t sBufSize
* ����ֵ����   PMEMORYPOOL
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
PMEMORYPOOL MemoryPool::CreateMemoryPool(void* pBuf, size_t sBufSize)
{
	memset(pBuf, 0, sBufSize);
	PMEMORYPOOL mem_pool = (PMEMORYPOOL)pBuf;
	// ������Ҫ����memory map��Ԫ��  
	size_t mem_pool_struct_size = sizeof(MEMORYPOOL);
	mem_pool->mem_map_pool_count = (sBufSize - mem_pool_struct_size + MINUNITSIZE - 1) / MINUNITSIZE;
	mem_pool->mem_map_unit_count = (sBufSize - mem_pool_struct_size + MINUNITSIZE - 1) / MINUNITSIZE;
	mem_pool->pmem_map = (memory_block*)((char*)pBuf + mem_pool_struct_size);
	mem_pool->pfree_mem_chunk_pool = (memory_chunk*)((char*)pBuf + mem_pool_struct_size + sizeof(memory_block) * mem_pool->mem_map_unit_count);

	mem_pool->memory = (char*)pBuf + mem_pool_struct_size + sizeof(memory_block) * mem_pool->mem_map_unit_count + sizeof(memory_chunk) * mem_pool->mem_map_pool_count;
	mem_pool->size = sBufSize - mem_pool_struct_size - sizeof(memory_block) * mem_pool->mem_map_unit_count - sizeof(memory_chunk) * mem_pool->mem_map_pool_count;
	size_t align = CheckAlignAddr(mem_pool->memory);
	mem_pool->size -= align;
	mem_pool->size = CheckAlignBlock(mem_pool->size);
	mem_pool->mem_block_count = mem_pool->size / MINUNITSIZE;
	// ����  
	mem_pool->pfree_mem_chunk_pool = CreateList(mem_pool->pfree_mem_chunk_pool, mem_pool->mem_map_pool_count);
	// ��ʼ�� pfree_mem_chunk��˫��ѭ������  
	memory_chunk* tmp = FrontPop(mem_pool->pfree_mem_chunk_pool);
	tmp->pre = tmp;
	tmp->next = tmp;
	tmp->pfree_mem_addr = NULL;
	mem_pool->mem_map_pool_count--;

	// ��ʼ�� pmem_map  
	mem_pool->pmem_map[0].count = mem_pool->mem_block_count;
	mem_pool->pmem_map[0].pmem_chunk = tmp;
	mem_pool->pmem_map[mem_pool->mem_block_count - 1].start = 0;

	tmp->pfree_mem_addr = mem_pool->pmem_map;
	PushBack(mem_pool->pfree_mem_chunk, tmp);
	mem_pool->free_mem_chunk_count = 1;
	mem_pool->mem_used_size = 0;
	return mem_pool;
}

/***************************************************************************
* �������ƣ�   ReleaseMemory
* ժ����Ҫ��   ��ʱû��
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  PMEMORYPOOL * ppMem
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
void MemoryPool::ReleaseMemory(PMEMORYPOOL* ppMem)
{
}

/***************************************************************************
* �������ƣ�   GetMemory
* ժ����Ҫ��   ���ڴ���з���ָ����С���ڴ�,pMem: �ڴ�� ָ��,sMemorySize: Ҫ������ڴ��С,
*             �ɹ�ʱ���ط�����ڴ���ʼ��ַ��ʧ�ܷ���NULL
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  size_t sMemorySize
* �Ρ�������   [in]  PMEMORYPOOL pMem
* ����ֵ����   void*
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
void* MemoryPool::GetMemory(size_t sMemorySize, PMEMORYPOOL pMem)
{
	sMemorySize = CheckAlignSize(sMemorySize);
	size_t index = 0;
	memory_chunk* tmp = pMem->pfree_mem_chunk;
	for (index = 0; index < pMem->free_mem_chunk_count; index++)
	{
		if (tmp->pfree_mem_addr->count * MINUNITSIZE >= sMemorySize)
		{
			break;
		}

		tmp = tmp->next;
	}

	if (index == pMem->free_mem_chunk_count)
	{
		return NULL;
	}
	pMem->mem_used_size += sMemorySize;
	if (tmp->pfree_mem_addr->count * MINUNITSIZE == sMemorySize)
	{
		// ��Ҫ������ڴ��С�뵱ǰchunk�е��ڴ��С��ͬʱ����pfree_mem_chunk������ɾ����chunk  
		size_t current_index = (tmp->pfree_mem_addr - pMem->pmem_map);
		DeleteChunk(pMem->pfree_mem_chunk, tmp);
		tmp->pfree_mem_addr->pmem_chunk = NULL;

		PushFront(pMem->pfree_mem_chunk_pool, tmp);
		pMem->free_mem_chunk_count--;
		pMem->mem_map_pool_count++;

		return Index2Addr(pMem, current_index);
	}
	else
	{
		// ��Ҫ������ڴ�С�ڵ�ǰchunk�е��ڴ�ʱ������pfree_mem_chunk����Ӧchunk��pfree_mem_addr  

		// ���Ƶ�ǰmem_map_unit  
		memory_block copy;
		copy.count = tmp->pfree_mem_addr->count;
		copy.pmem_chunk = tmp;
		// ��¼��block����ʼ�ͽ�������  
		memory_block* current_block = tmp->pfree_mem_addr;
		current_block->count = sMemorySize / MINUNITSIZE;
		size_t current_index = (current_block - pMem->pmem_map);
		pMem->pmem_map[current_index + current_block->count - 1].start = current_index;
		current_block->pmem_chunk = NULL; // NULL��ʾ��ǰ�ڴ���ѱ�����  
		// ��ǰblock��һ��Ϊ�������µڶ���block�е�����  
		pMem->pmem_map[current_index + current_block->count].count = copy.count - current_block->count;
		pMem->pmem_map[current_index + current_block->count].pmem_chunk = copy.pmem_chunk;
		// ����ԭ����pfree_mem_addr  
		tmp->pfree_mem_addr = &(pMem->pmem_map[current_index + current_block->count]);

		size_t end_index = current_index + copy.count - 1;
		pMem->pmem_map[end_index].start = current_index + current_block->count;
		return Index2Addr(pMem, current_index);
	}
}

/***************************************************************************
* �������ƣ�   FreeMemory
* ժ����Ҫ��   ���ڴ�����ͷ����뵽���ڴ� pMem���ڴ��ָ��
*             ptrMemoryBlock�����뵽���ڴ���ʼ��ַ
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  void * ptrMemoryBlock
* �Ρ�������   [in]  PMEMORYPOOL pMem
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/11      ���ǲ�        ���
***************************************************************************/
void MemoryPool::FreeMemory(void *ptrMemoryBlock, PMEMORYPOOL pMem)
{
	size_t current_index = Addr2Index(pMem, ptrMemoryBlock);
	size_t size = pMem->pmem_map[current_index].count * MINUNITSIZE;
	// �ж��뵱ǰ�ͷŵ��ڴ�����ڵ��ڴ���Ƿ�����뵱ǰ�ͷŵ��ڴ��ϲ�  
	memory_block* pre_block = NULL;
	memory_block* next_block = NULL;
	memory_block* current_block = &(pMem->pmem_map[current_index]);
	// ��һ��  
	if (current_index == 0)
	{
		if (current_block->count < pMem->mem_block_count)
		{
			next_block = &(pMem->pmem_map[current_index + current_block->count]);
			// �����һ���ڴ���ǿ��еģ��ϲ�  
			if (next_block->pmem_chunk != NULL)
			{
				next_block->pmem_chunk->pfree_mem_addr = current_block;
				pMem->pmem_map[current_index + current_block->count + next_block->count - 1].start = current_index;
				current_block->count += next_block->count;
				current_block->pmem_chunk = next_block->pmem_chunk;
				next_block->pmem_chunk = NULL;
			}
			// �����һ���ڴ治�ǿ��еģ���pfree_mem_chunk������һ��chunk  
			else
			{
				memory_chunk* new_chunk = FrontPop(pMem->pfree_mem_chunk_pool);
				new_chunk->pfree_mem_addr = current_block;
				current_block->pmem_chunk = new_chunk;
				PushBack(pMem->pfree_mem_chunk, new_chunk);
				pMem->mem_map_pool_count--;
				pMem->free_mem_chunk_count++;
			}
		}
		else
		{
			memory_chunk* new_chunk = FrontPop(pMem->pfree_mem_chunk_pool);
			new_chunk->pfree_mem_addr = current_block;
			current_block->pmem_chunk = new_chunk;
			PushBack(pMem->pfree_mem_chunk, new_chunk);
			pMem->mem_map_pool_count--;
			pMem->free_mem_chunk_count++;
		}
	}

	// ���һ��  
	else if (current_index == pMem->mem_block_count - 1)
	{
		if (current_block->count < pMem->mem_block_count)
		{
			pre_block = &(pMem->pmem_map[current_index - 1]);
			size_t index = pre_block->count;
			pre_block = &(pMem->pmem_map[index]);

			// ���ǰһ���ڴ���ǿ��еģ��ϲ�  
			if (pre_block->pmem_chunk != NULL)
			{
				pMem->pmem_map[current_index + current_block->count - 1].start = current_index - pre_block->count;
				pre_block->count += current_block->count;
				current_block->pmem_chunk = NULL;
			}
			// ���ǰһ���ڴ治�ǿ��еģ���pfree_mem_chunk������һ��chunk  
			else
			{
				memory_chunk* new_chunk = FrontPop(pMem->pfree_mem_chunk_pool);
				new_chunk->pfree_mem_addr = current_block;
				current_block->pmem_chunk = new_chunk;
				PushBack(pMem->pfree_mem_chunk, new_chunk);
				pMem->mem_map_pool_count--;
				pMem->free_mem_chunk_count++;
			}
		}
		else
		{
			memory_chunk* new_chunk = FrontPop(pMem->pfree_mem_chunk_pool);
			new_chunk->pfree_mem_addr = current_block;
			current_block->pmem_chunk = new_chunk;
			PushBack(pMem->pfree_mem_chunk, new_chunk);
			pMem->mem_map_pool_count--;
			pMem->free_mem_chunk_count++;
		}
	}
	else
	{
		next_block = &(pMem->pmem_map[current_index + current_block->count]);
		pre_block = &(pMem->pmem_map[current_index - 1]);
		size_t index = pre_block->start;
		pre_block = &(pMem->pmem_map[index]);
		bool is_back_merge = false;
		if (next_block->pmem_chunk == NULL && pre_block->pmem_chunk == NULL)
		{
			memory_chunk* new_chunk = FrontPop(pMem->pfree_mem_chunk_pool);
			new_chunk->pfree_mem_addr = current_block;
			current_block->pmem_chunk = new_chunk;
			PushBack(pMem->pfree_mem_chunk, new_chunk);
			pMem->mem_map_pool_count--;
			pMem->free_mem_chunk_count++;
		}
		// ��һ���ڴ��  
		if (next_block->pmem_chunk != NULL)
		{
			next_block->pmem_chunk->pfree_mem_addr = current_block;
			pMem->pmem_map[current_index + current_block->count + next_block->count - 1].start = current_index;
			current_block->count += next_block->count;
			current_block->pmem_chunk = next_block->pmem_chunk;
			next_block->pmem_chunk = NULL;
			is_back_merge = true;
		}
		// ǰһ���ڴ��  
		if (pre_block->pmem_chunk != NULL)
		{
			pMem->pmem_map[current_index + current_block->count - 1].start = current_index - pre_block->count;
			pre_block->count += current_block->count;
			if (is_back_merge)
			{
				DeleteChunk(pMem->pfree_mem_chunk, current_block->pmem_chunk);
				PushFront(pMem->pfree_mem_chunk_pool, current_block->pmem_chunk);
				pMem->free_mem_chunk_count--;
				pMem->mem_map_pool_count++;
			}
			current_block->pmem_chunk = NULL;
		}
	}
	pMem->mem_used_size -= size;
}