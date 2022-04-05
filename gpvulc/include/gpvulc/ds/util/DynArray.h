//--------------------------------------------------------------------//
// gpvulc                                                             //
// GPV's Utility Library Collection                                   //
//  by Giovanni Paolo Vigano', 2015-2022                              //
//--------------------------------------------------------------------//
//
// Distributed under the MIT Software License.
// See http://opensource.org/licenses/MIT
//


/// @brief Template class implementing a generic dynamic array
/// @file DynArray.h
/// @author Giovanni Paolo Vigano'

#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include <utility>
#include <new>
#include <string.h>

#ifndef GPVULC_DYNARRAY_H_INCLUDED
#define GPVULC_DYNARRAY_H_INCLUDED



namespace gpvulc
{

	/// @addtogroup DS
	/// @{

	/// @addtogroup Util
	/// @{

	//! Default comparison callback (it uses the >,< operators)
	template<class itemType>
	int DynArrayDefCmpFunc(itemType a, itemType b);



	/*!
	Template array: it can be used as a C++ array.

	The given type must have (implicitly or explicitly) the assignment operator (=) defined.<BR>
	@note Warning! Do not keep references or pointers to an item's memory location,
	because items memory locations change due to array reallocation.
	Using the LinkTo() method more arrays can be linked together:
	in this case each array is referencing to the same
	memory buffer. In this way the vector in memory can be shared between many
	synchronized DynArrays, and when a DynArray is Reset() or deleted its memory
	buffer is deleted only if no other DynArray refers to it.
	Another warning: <b>all methods returning a reference throw a std::out_of_range exception
	if the array is empty or the index is invalid</b>
	*/
	template<class itemType>
	class DynArray
	{
	public:

		//! Default constructor.
		DynArray();

		//! Copy constructor.
		DynArray(const DynArray<itemType>& a);

		//! Constructor: allocate a vector with the given dimension (see Allocate()).
		DynArray(int n);

		//! Constructor: allocate a vector with the given dimension and add given number of (not initializated) elements.
		DynArray(int n, int m);

		//! Constructor: allocate a copy of the given array.
		DynArray(const std::vector<itemType>& v);

		//! Destructor: free allocated memory.
		virtual ~DynArray();

		//! Assignment method: calls LinkTo() or Copy() method depending if the given array is linked or not
		void Assign(const DynArray<itemType>& a);

		///@}

		//! Get the size of the array (initially set to 0)
		int Size() const { return (int)Data->size(); }

		//! Get the number of allocated (even if not used) elements (0 if not yet allocated)
		int Dimension() const { return (int)Data->capacity(); }

		//! Reset all and free memory, stop sharing data with other DynArray instances
		void Reset();

		//! Resize the array to the given dimension (reallocate memory).
		void Resize(unsigned int new_size) { Data->resize((size_t)new_size); }

		//! Resize the array to fit the current used size.
		void ShrinkToFit() { Data->shrink_to_fit(); }

		//! Empty the array
		void Clear() { Data->clear(); }


		/*!
		Allocate the array with the given dimension.
		If the array is not empty (and the given dimension is grater than the current one) its content is copied into the new allocated buffer.
		If the given dimension is less or equal than the current size this method simply returns true.
		This method allocates memory only if needed, so if you call more times this method with the same value nothing is done after the first time.
		This method can slightly increase performances when you want to add many items (see Add()):
		first allocate a number of items, then add your items using the already allocated memory.
		@param dim new dimension for the allocated array (see details)
		*/
		void Allocate(unsigned dim) { Data->reserve((size_t)dim); }

		//! Check if this dynamic array is empty.
		bool Empty() { return Data->empty(); }

		/*!
		Fill the array with the given data copied "count" times.
		If count<0 copies until the end of pre-allocated locations.
		Returns true only if all the data was successfully added
		*/
		void Fill(itemType data, int count = -1);

		//! Merge the first count elements of a vector (at the end of this array)
		void Merge(const std::vector<itemType>& v, int count = -1);

		//! Copy the first count elements of a vector
		void Copy(const std::vector<itemType>& v, int count = -1);

		//! Copy the first count elements of a vector starting from the given index
		void Copy(const std::vector<itemType>& v, unsigned start, unsigned count);

		//! Extend the allocated memory reserving more memory to increase performace.
		bool Extend(unsigned count = 1) { return Allocate(Size() + count); }

		//! Add an element at last position
		void Add(itemType data) { Data->push_back(data); }

		//! Get the index of the given data or -1 if not present
		int GetIndex(itemType udata) const;

		//! Get the last element (<b>warning: the array must not be empty</b>)
		itemType& GetFirst() { return Data->front(); }

		//! Get and remove the last element (<b>warning: the array must not be empty</b>)
		itemType PickFirst();

		//! Get the last element (<b>warning: the array must not be empty</b>)
		itemType& GetLast() { return Data->back(); }

		//! Get and remove the last element (<b>warning: the array must not be empty</b>)
		itemType PickLast();

		/*!
		Insert a new element at the given position shifting forward the other elements.
		If the array is empty call Add() to add the new element.
		*/
		bool Insert(unsigned pos, itemType data);

		//! Move an element to the given position shifting the other elements
		bool Move(unsigned pos, unsigned newpos);

		//! Swap two elements
		bool Swap(unsigned pos1, unsigned pos2);

		//! Move an element forward in the list
		bool MoveForwardIdx(unsigned pos) { return Swap(pos + 1, pos); }

		//! Move an element forward in the list
		bool MoveForward(itemType udata);

		//! Move an element backward in the list
		bool MoveBackwardIdx(unsigned pos) { return Swap(pos - 1, pos); }

		//! Move an element backward in the list
		bool MoveBackward(itemType udata);

		//! Move an element at the end shifting the other elements
		bool MoveLast(itemType udata) { return Move(GetIndex(udata), Data->size() - 1); }

		//! Check if the given item belongs to this list
		bool Has(itemType item);

		//! Add an element at last position only if it is not yet present
		bool AddUnique(itemType data);

		//! Return data vector
		std::vector<itemType>& GetData() { return *Data; }

		//! Return data vector (read only)
		const std::vector<itemType>& GetData() const { return *Data; }

		//! Get the i-th element of the array.
		itemType& operator [](int idx) { return (*Data)[idx]; }

		//! Get the i-th element of the array.
		const itemType& operator [](int idx) const { return Data->at(idx); }

		//! Share the array data with other DynArray instances (previous data in this array will be lost)
		void LinkTo(const DynArray<itemType>& lst);

		//! Stop sharing data with other DynArray instances (if any), get a local copy of the data
		void Unlink();

		//! Check if sharing data with other DynArray instances
		bool Linked() const { return !Data.unique(); }

		//! Assignment operator: links to the given array if it is linked or calls Copy() method
		DynArray<itemType>& operator =(const DynArray<itemType>& a);

		//! Comparison operator: compare this with the given array
		bool operator ==(const DynArray<itemType>& a) { return *Data == a.GetData(); }

		//! Remove the i-th element shifting backward the other elements
		bool RemoveIdx(unsigned idx);

		//! Remove all the occurrences of the element (if present) equal to the given data
		bool RemoveElem(itemType data);

		//! Replace all the occurrences of the element (if present) equal to the given data
		bool ReplaceElem(itemType data, itemType new_data);

		//! Remove all the elements, free the allocated memory if requested
		void RemoveAll(bool free_memory);

		//! Set the i-th element to the given value
		bool Set(unsigned idx, itemType data);

		//! Set a range of elements to the given value
		bool SetElementsRange(itemType data, int start = 0, int end = -1);

		/*!
		Sort the array using a custom comparison function in the range { from .. to }, the array is sorted in ascending order.
		Example:<BR>@code
		int compare_TextBuffer( TextBuffer a, TextBuffer b ) {
		return a.Compare(b);
		}

		void test_sort() {
		DynArray<TextBuffer> v;
		v.Add(TextBuffer("zzz"));
		v.Add(TextBuffer("fff"));
		v.Add(TextBuffer("aaa"));
		v.Add(TextBuffer("bbb"));
		v.Add(TextBuffer("ddd"));
		v.Sort(compare_TextBuffer);
		for ( int i=0; i<v.Size(); i++ ) puts(v[i]);
		}
		@endcode
		@param comparefunc The comparison function must return 0 if items are equal, >0 if the_first>the_second and <0 if the_first<the_second. A default function is provided, DynArrayDefCmpFunc().
		@param from starting element to be sorted (by default=0, the first one).
		@param to ending element to be sorted (by default=-1, the last one).
		*/
		void Sort(
			int(*comparefunc)(itemType, itemType) = DynArrayDefCmpFunc<itemType>,
			int from = 0,
			int to = -1);

	protected:

		//! Internal data pointer to a vector of elements
		std::shared_ptr< std::vector<itemType> > Data = nullptr;

		//! Check if the index is valid or out of range
		bool CheckIndex(int idx);

		//! Check if the index is valid or out of range
		bool CheckIndex(unsigned idx);

	private:
		void Init();
	};

	///@}
	///@}

	//@cond SKIP_THIS_IN_DOXYGEN
#include "gpvulc/ds/util/DynArrayImpl.inl"
	//@endcond


}//namespace gpvulc



#endif//GPVULC_DYNARRAY_H_INCLUDED
