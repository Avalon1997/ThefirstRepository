/*! \file ringbuffer.h
 *  \brief \b ���λ�����.
 *  \author wangdh
 *  \version 1.0
 *  \copyright Copyright (c) 2018-2021 �������л����Ƽ����޹�˾
 *  \date 2021-05-28
 *  \details \b ����
 *  -# ���λ�����
 *  -# ��������ݵ����������ͷβ��ӵ�ring buffer�����ݴ�βд�룬��ͷ������
 *  -# ��writeIndex��ǰ�����readIndexʱ��bufferΪ������readIndex��ǰ�����writeIndexʱ��bufferΪ�ա�
 *  -# ���Ծݴ�������flag���������ա�����ʵ��ʱ�����ݸ������ж��ˡ�
 *  -# ѭ�����е�Ԫ�ظ�����(β - ͷ + ��) % ��
 *  -# ʹ��atomicʵ��������������������

 *	\details \b �޸���־
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-05-28  <td>1.0      <td>wangdh    <td>������ʼ�汾 </tr>
 *  </table>
 */

#ifndef _RING_BUFFER_H
#define _RING_BUFFER_H
#include <atomic>
#include <memory>
#include <mutex>

	class SpinLock
	{
		std::atomic_flag flag = ATOMIC_FLAG_INIT;
	public:
		void lock() noexcept {
			while (flag.test_and_set(std::memory_order_acquire));
		}
		void unlock() noexcept { flag.clear(std::memory_order_release); }
		bool try_lock() noexcept { return !flag.test_and_set(std::memory_order_acquire); }
	};

	template < typename T >
	class RingBuffer
	{
		static const unsigned MAX_QUEUE_SIZE = 100;
	public:
		RingBuffer();
		explicit RingBuffer(unsigned maxSize = MAX_QUEUE_SIZE);

		RingBuffer(const RingBuffer &other);
		RingBuffer& operator=(const RingBuffer &) = delete;

		~RingBuffer();

		// pushes an item to Queue tail
		void enqueue(const T& item);

		// pops an item from Queue head
		std::shared_ptr< T > dequeue();

		// try to push an item to Queue tail
		bool try_and_enqueue(const T& item);

		// try to pop and item from Queue head
		std::shared_ptr< T > try_and_dequeue();

		bool full();
		bool empty();
		unsigned capacity() { return CAPACITY; }
		unsigned count();

	protected:
		SpinLock lock;
		const unsigned CAPACITY;  // Queue capacity
		T *data;                // array to store the items
		unsigned cnt;           // Queue count
		unsigned head;          // also the readIndex
		unsigned tail;          // also the writeIndex
	};

	template < typename T >
	RingBuffer< T >::RingBuffer(unsigned maxSize) : CAPACITY(maxSize), cnt(0), head(0), tail(0)
	{
		data = new T[CAPACITY];
	}

	template < typename T >
	RingBuffer< T >::RingBuffer(const RingBuffer &other)
	{
		std::lock_guard< SpinLock > lg(lock);
		CAPACITY = other.CAPACITY;
		cnt = other.cnt;
		head = other.head;
		tail = other.tail;
		data = new T[CAPACITY];
		for (unsigned i = 0; i < CAPACITY; ++i)
			data[i] = other.data[i];
	}

	template < typename T >
	RingBuffer< T >::~RingBuffer()
	{
		delete[] data;
	}

	//void enqueue(const T& item);
	template < typename T >
	void RingBuffer< T >::enqueue(const T &item)
	{
		while (!try_and_enqueue(item));
	}

	template < typename T >
	std::shared_ptr< T > RingBuffer< T >::dequeue()
	{
		thread_local std::shared_ptr< T > ptr;
		while ((ptr = try_and_dequeue()) == nullptr)
			;
		return ptr;
	}

	template < typename T >
	bool RingBuffer< T >::try_and_enqueue(const T &item)
	{
		std::lock_guard< SpinLock > lg(lock);
		if (cnt == CAPACITY)
			return false;    // full
		++cnt;
		data[tail++] = std::move(item);
		if (tail == CAPACITY)
			tail -= CAPACITY;
		return true;
	}

	template < typename T >
	std::shared_ptr< T > RingBuffer< T >::try_and_dequeue()
	{
		std::lock_guard< SpinLock > lg(lock);
		if (cnt == 0)
			return std::shared_ptr< T >();   // empty
		--cnt;
		unsigned idx = head;
		++head;
		if (head == CAPACITY)
			head -= CAPACITY;
		return std::make_shared< T >(std::move(data[idx]));
	}

	template < typename T >
	bool RingBuffer< T >::full()
	{
		std::lock_guard< SpinLock > lg(lock);
		return cnt == CAPACITY;
	}

	template < typename T >
	bool RingBuffer< T >::empty()
	{
		std::lock_guard< SpinLock > lg(lock);
		return cnt == 0;
	}

	template < typename T >
	unsigned RingBuffer< T >::count()
	{
		std::lock_guard< SpinLock > lg(lock);
		return cnt;
	}


#endif // !_RING_BUFFER_H
