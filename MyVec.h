#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

template
<typename T>
class MyVec
{
public:
	MyVec()
	{
	}

	~MyVec()
	{
		Clear();
		free(mRawBuffer);
	}

	size_t Capacity() const { return mCapacity; }
	size_t Size() const { return mSize; }

	void Append(const T& element)
	{
		if (FreeSpace() == 0)
		{
			Grow();
		}

		new (&(mBuffer[mSize++])) T(element);
	}

	void Clear()
	{
		DestroyElements();
		mSize = 0;
	}

	void DestroyElements()
	{
		for (size_t i = 0; i < Size(); ++i)
		{
			mBuffer[i].~T();
		}
	}

	void Reserve(size_t requested)
	{
		if (requested < mCapacity)
		{
			return;
		}

		const size_t requiredAlignment = alignof(T);
		void* tmp = malloc(sizeof(T) * requested + requiredAlignment /*wiggle bytes*/);

		// with help from Game Engine Architecture (5.2.1.3 Aligned Allocations)
		// ---------------------------------------------------------------------
		// alignment will be some power of two, so subtract 1 to get the misalignment mask
		const size_t misalignmentMask = requiredAlignment - 1; 
		// if we AND the actual address with our mask, we'll see how many bytes off we are
		const size_t misalignment     = reinterpret_cast<uintptr_t>(tmp) & misalignmentMask;
		const size_t requiredOffset   = requiredAlignment - misalignment;

		T* typedIntermediateBuffer = reinterpret_cast<T*>(reinterpret_cast<char*>(tmp) + requiredOffset);

		// Copy-Construct into the new buffer
		// TODO do we have to worry about memory overlap?
		for (size_t i = 0; i < Size(); ++i)
		{
			new (&(typedIntermediateBuffer[i])) T(mBuffer[i]);
		}

		// kill off the old
		DestroyElements();		
		free(mRawBuffer);

		mBuffer    = typedIntermediateBuffer;
		mRawBuffer = tmp;
		mCapacity  = requested;
	}

	void Grow()
	{
		if (mCapacity == 0)
		{
			Reserve(1);
		}
		else
		{
			Reserve(mCapacity * 2);
		}
	}

	size_t FreeSpace() const { return mCapacity - mSize; }

	const T& operator[](int idx) const { return mBuffer[idx]; }
	      T& operator[](int idx)       { return mBuffer[idx]; }

private:
	T*     mBuffer    = nullptr;
	void*  mRawBuffer = nullptr;

	size_t mSize      = 0;
	size_t mCapacity  = 0;
};
