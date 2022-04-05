//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2021                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//



/// @brief Template class implementing a generic array
/// @details This file must be included only by DynArray.h,
/// it includes the implementation of the inline methods of DynArray
/// @file dynarrayimpl.inl
/// @author Giovanni Paolo Vigano'

#ifndef GPVULC_DYNARRAY_H_INCLUDED
#error "This file must be included only by DynArray.h"
#else


template<class itemType>
inline int DynArrayDefCmpFunc(itemType a, itemType b)
{
	if (a > b) return 1;
	if (a < b) return -1;

	return 0;
}


template<class itemType>
inline DynArray<itemType>::DynArray()
{
	Init();
}


template<class itemType>
inline DynArray<itemType>::DynArray(const DynArray<itemType>& a)
{
	Init();
	Assign(a);
}


template<class itemType>
inline DynArray<itemType>::DynArray(int n)
{
	Init();
	Allocate(n);
}


template<class itemType>
inline DynArray<itemType>::DynArray(const std::vector<itemType>& v)
{
	Init();
	Copy(v, v.size());
}


template<class itemType>
inline DynArray<itemType>::~DynArray()
{
	Reset();
}


template<class itemType>
inline void DynArray<itemType>::Reset()
{
	if (Data && !Linked())
	{
		Data->clear();
	}
	else
	{
		Init();
	}
}


template<class itemType>
inline void DynArray<itemType>::Fill(itemType data, int itemCount)
{
	if (itemCount < 0)
	{
		itemCount = Data->capacity();
	}
	Data->assign((size_t)itemCount);
}


template<class itemType>
inline void DynArray<itemType>::Init()
{
	Data = std::make_shared< std::vector<itemType> >();
}


template<class itemType>
inline bool DynArray<itemType>::CheckIndex(int idx)
{
	return (idx >= 0 && idx < Size());
}


template<class itemType>
inline bool DynArray<itemType>::CheckIndex(unsigned idx)
{
	return ((int)idx < Size());
}


template<class itemType>
inline void DynArray<itemType>::Assign(const DynArray<itemType>& a)
{
	if (a.Linked())
	{
		LinkTo(const_cast<DynArray<itemType>&>(a));
	}
	else
	{
		Copy(a.GetData(), a.Size());
	}
}


template<class itemType>
inline void DynArray<itemType>::Merge(const std::vector<itemType>& v, int count)
{
	if (count == 0)
	{
		return;
	}
	auto itr = v.cend();
	if (count > 0 && count < v.size())
	{
		itr = v.cbegin() + count;
	}

	Data->reserve(Size() + count);
	Data->insert(Data->end(), v.cbegin(), itr);
}


template<class itemType>
inline void DynArray<itemType>::Copy(const std::vector<itemType>& v, int count)
{
	if (v.empty())
	{
		Data->clear();
		return;
	}
	if (count <= 0)
	{
		*Data = v;
		return;
	}

	auto endItr = v.end();
	if (count < (int)v.size())
	{
		endItr = v.cbegin() + count;
	}
	Data->clear();
	Data->insert(Data->end(), v.cbegin(), endItr);
}


template<class itemType>
inline void DynArray<itemType>::Copy(const std::vector<itemType>& v, unsigned start, unsigned count)
{
	if (v.empty())
	{
		Data->clear();
		return;
	}
	if (start == 0 && count <= 0)
	{
		*Data = v;
		return;
	}
	if (start == 0 && count <= 0)
	{
		count = (int)v.size()-start;
	}
	auto endItr = v.end();
	if (start + count < v.size())
	{
		endItr = v.cbegin() + count;
	}
	Data->clear();
	Data->insert(Data->end(), v.cbegin() + start, endItr);
}


template<class itemType>
inline int DynArray<itemType>::GetIndex(itemType udata) const
{
	int i;
	for (i = 0; i < Size(); ++i)
	{
		if ((*Data)[i] == udata)
		{
			return i;
		}
	}
	return -1;
}


template<class itemType>
inline itemType DynArray<itemType>::PickFirst()
{
	itemType item = GetFirst();
	Data->pop_front();
	return item;
}


template<class itemType>
inline itemType DynArray<itemType>::PickLast()
{
	itemType item = GetLast();
	Data->pop_back();
	return item;
}


template<class itemType>
inline bool DynArray<itemType>::Insert(unsigned pos, itemType data)
{
	if (pos == 0 && Empty())
	{
		Add(data);
		return true;
	}
	if (!CheckIndex(pos))
	{
		return false;
	}

	Data->insert(Data->begin() + pos, data);
	return true;
}


template<class itemType>
inline bool DynArray<itemType>::Move(unsigned pos, unsigned newpos)
{
	if (!CheckIndex(pos) || !CheckIndex(newpos))
	{
		return false;
	}
	if (pos == newpos)
	{
		return true;
	}
	itemType data = (*Data)[pos];
	unsigned i;
	if (pos < newpos)
	{
		for (i = pos; i < newpos; ++i)
		{
			(*Data)[i] = (*Data)[i + 1];
		}
	}
	if (pos > newpos)
	{
		for (i = pos; i > newpos; --i)
		{
			(*Data)[i] = (*Data)[i - 1];
		}
	}
	(*Data)[newpos] = data;
	return true;
}


template<class itemType>
inline bool DynArray<itemType>::Swap(unsigned pos1, unsigned pos2)
{
	if (!CheckIndex(pos1) || !CheckIndex(pos2))
	{
		return false;
	}
	std::swap((*Data)[pos1], (*Data)[pos2]);

	return true;
}


template<class itemType>
inline bool DynArray<itemType>::MoveForward(itemType udata)
{
	int pos = GetIndex(udata);
	if (pos < 0)
	{
		return false;
	}
	return Swap(pos + 1, pos);
}


template<class itemType>
inline bool DynArray<itemType>::MoveBackward(itemType udata)
{
	int pos = GetIndex(udata);
	if (pos < 0)
	{
		return false;
	}
	return Swap(pos - 1, pos);
}


template<class itemType>
inline bool DynArray<itemType>::Has(itemType item)
{
	return std::find(Data->cbegin(), Data->cend(), item) != Data->cend();
}


template<class itemType>
inline bool DynArray<itemType>::AddUnique(itemType data)
{
	if (Has(data))
	{
		return false;
	}
	Add(data);
	return true;
}


template<class itemType>
inline void DynArray<itemType>::LinkTo(const DynArray<itemType>& lst)
{
	this->Data = lst.Data;
}


template<class itemType>
inline void DynArray<itemType>::Unlink()
{
	std::shared_ptr< std::vector<itemType> > datasave = Data;
	Init();
	Copy(*datasave);
}


template<class itemType>
inline DynArray<itemType>& DynArray<itemType>::operator =(const DynArray<itemType>& a)
{
	Assign(a);
	return *this;
}


template<class itemType>
inline bool DynArray<itemType>::RemoveIdx(unsigned idx)
{
	if (!CheckIndex(idx))
	{
		return false;
	}
	Data->erase(Data->begin() + idx);
	return true;
}


template<class itemType>
inline bool DynArray<itemType>::RemoveElem(itemType data)
{
	auto itr = std::find(Data->begin(), Data->end(), data);
	if (itr == Data->end())
	{
		return false;
	}
	Data->erase(itr);
	return true;
}


template<class itemType>
inline bool DynArray<itemType>::ReplaceElem(itemType data, itemType new_data)
{
	auto itr = std::find(Data->begin(), Data->end(), data);
	if (itr == Data->end())
	{
		return false;
	}
	*itr = new_data;
	return true;
}


template<class itemType>
inline void DynArray<itemType>::RemoveAll(bool free_memory)
{
	Clear();
	if (free_memory) ShrinkToFit();
}


template<class itemType>
inline bool DynArray<itemType>::Set(unsigned idx, itemType data)
{
	if (!CheckIndex(idx))
	{
		return false;
	}
	(*Data)[idx] = data;
	return true;
}


template<class itemType>
inline bool DynArray<itemType>::SetElementsRange(itemType data, int start, int end)
{
	if (!CheckIndex(start))
	{
		return false;
	}
	if (end < 0) end = Size() - 1;
	if (start > end) std::swap(start, end);
	if (!CheckIndex(end))
	{
		return false;
	}
	if (start < 0 || start >= Dimension())
	{
		return false;
	}
	if (end >= Dimension())
	{
		return false;
	}
	for (int i = start; i <= end; ++i) (*Data)[i] = data;
	return true;
}


template<class itemType>
inline void DynArray<itemType>::Sort(
	int(*comparefunc)(itemType, itemType),
	int from,
	int to)
{
	if (to < 0) to = Size() - 1;
	if (to > from)
	{
		int c = (to + from) / 2;

		Sort(comparefunc, from, c);

		Sort(comparefunc, c + 1, to);

		int a = from;
		int b = c + 1;
		int counter = 0;
		std::vector<itemType> result(to - from + 1);

		while (a <= c || b <= to)
		{
			if (a > c) result[counter++] = (*Data)[b++];
			else if (b > to) result[counter++] = (*Data)[a++];
			else if (comparefunc((*Data)[a], (*Data)[b]) <= 0) result[counter++] = (*Data)[a++];
			else result[counter++] = (*Data)[b++];
		}

		for (int i = 0; i < to - from + 1; ++i) (*Data)[i + from] = result[i];
	}
}


#endif//GPVULC_DYNARRAY_H_INCLUDED

