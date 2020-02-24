///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a simple container class.
 *	\file		IceContainer.h
 *	\author		Pierre Terdiman
 *	\date		February, 5, 2000
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __ICECONTAINER_H__
#define __ICECONTAINER_H__

	#define CONTAINER_STATS

	class ICECORE_API Container
	{
		public:
		// Constructor / Destructor
								Container();
								Container(unsigned int size, float growth_factor);
								~Container();
		// Management
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A O(1) method to add a value in the container. The container is automatically resized if needed.
		 *	The method is inline, not the resize. The call overhead happens on resizes only, which is not a problem since the resizing operation
		 *	costs a lot more than the call overhead...
		 *
		 *	\param		entry		[in] a unsigned int to store in the container
		 *	\see		Add(float entry)
		 *	\see		Empty()
		 *	\see		Contains(unsigned int entry)
		 *	\return		Self-Reference
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline	Container&		Add(unsigned int entry)
				{
					// Resize if needed
					if(mCurNbEntries==mMaxNbEntries)	Resize();

					// Add _new_ entry
					mEntries[mCurNbEntries++]	= entry;
					return *this;
				}

		inline	Container&		Add(const unsigned int* entries, unsigned int nb)
				{
					// Resize if needed
					if(mCurNbEntries+nb>mMaxNbEntries)	Resize(nb);

					// Add _new_ entry
					CopyMemory(&mEntries[mCurNbEntries], entries, nb*sizeof(unsigned int));
					mCurNbEntries+=nb;
					return *this;
				}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	A O(1) method to add a value in the container. The container is automatically resized if needed.
		 *	The method is inline, not the resize. The call overhead happens on resizes only, which is not a problem since the resizing operation
		 *	costs a lot more than the call overhead...
		 *
		 *	\param		entry		[in] a float to store in the container
		 *	\see		Add(unsigned int entry)
		 *	\see		Empty()
		 *	\see		Contains(unsigned int entry)
		 *	\return		Self-Reference
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define IR(x)					((unsigned int&)(x))
		
		inline	Container&		Add(float entry)
				{
					// Resize if needed
					if(mCurNbEntries==mMaxNbEntries)	Resize();

					// Add _new_ entry
					mEntries[mCurNbEntries++]	= IR(entry);
					return *this;
				}

		inline	Container&		Add(const float* entries, unsigned int nb)
				{
					// Resize if needed
					if(mCurNbEntries+nb>mMaxNbEntries)	Resize(nb);

					// Add _new_ entry
					CopyMemory(&mEntries[mCurNbEntries], entries, nb*sizeof(float));
					mCurNbEntries+=nb;
					return *this;
				}

		//! Add unique [slow]
				Container&		AddUnique(unsigned int entry)
				{
					if(!Contains(entry))	Add(entry);
					return *this;
				}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Clears the container. All stored values are deleted, and it frees used ram.
		 *	\see		Reset()
		 *	\return		Self-Reference
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline	Container&		Empty()
				{
					#ifdef CONTAINER_STATS
					mUsedRam-=mMaxNbEntries*sizeof(unsigned int);
					#endif
					xr_free			(mEntries);
					mCurNbEntries	= mMaxNbEntries = 0;
					return *this;
				}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Resets the container. Stored values are discarded but the buffer is kept so that further calls don't need resizing again.
		 *	That's a kind of temporal coherence.
		 *	\see		Empty()
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline	void			Reset()
				{
					// Avoid the write if possible
					// ### CMOV
					if(mCurNbEntries)	mCurNbEntries = 0;
				}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Sets the initial size of the container. If it already contains something, it's discarded.
		 *	\param		nb		[in] Number of entries
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				bool			SetSize(unsigned int nb);

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Refits the container and get rid of unused bytes.
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				bool			Refit();

		// Checks whether the container already contains a given value.
				bool			Contains(unsigned int entry, unsigned int* location=nullptr) const;
		// Deletes an entry - doesn't preserve insertion order.
				bool			Delete(unsigned int entry);
		// Deletes an entry - does preserve insertion order.
				bool			DeleteKeepingOrder(unsigned int entry);
		//! Deletes the very last entry.
		inline	void			DeleteLastEntry()						{ if(mCurNbEntries)	mCurNbEntries--;			}
		//! Deletes the entry whose index is given
		inline	void			DeleteIndex(unsigned int index)				{ mEntries[index] = mEntries[--mCurNbEntries];	}

		// Helpers
				Container&		FindNext(unsigned int& entry, bool wrap=false);
				Container&		FindPrev(unsigned int& entry, bool wrap=false);
		// Data access.
		inline	unsigned int			GetNbEntries()					const	{ return mCurNbEntries;		}	//!< Returns the current number of entries.
		inline	unsigned int			GetEntry(unsigned int i)				const	{ return mEntries[i];		}	//!< Returns ith entry
		inline	unsigned int*			GetEntries()					const	{ return mEntries;			}	//!< Returns the list of entries.

		// Growth control
		inline	float			GetGrowthFactor()				const	{ return mGrowthFactor;		}	//!< Returns the growth factor
		inline	void			SetGrowthFactor(float growth)			{ mGrowthFactor = growth;	}	//!< Sets the growth factor

		//! Access as an array
		inline	unsigned int&			operator[](unsigned int i)			const	{ ASSERT(i>=0 && i<mCurNbEntries); return mEntries[i];	}

		// Stats
				unsigned int			GetUsedRam()					const;

		//! Operator for Container A = Container B
				void			operator = (const Container& object)
				{
					SetSize(object.GetNbEntries());
					CopyMemory(mEntries, object.GetEntries(), mMaxNbEntries*sizeof(unsigned int));
					mCurNbEntries = mMaxNbEntries;
				}

#ifdef CONTAINER_STATS
		inline	unsigned int			GetNbContainers()				const	{ return mNbContainers;		}
		inline	unsigned int			GetTotalBytes()					const	{ return mUsedRam;			}
		private:

		static	unsigned int			mNbContainers;		//!< Number of containers around
		static	unsigned int			mUsedRam;			//!< Amount of bytes used by containers in the system
#endif
		private:
		// Resizing
				bool			Resize(unsigned int needed=1);
		// Data
				unsigned int			mMaxNbEntries;		//!< Maximum possible number of entries
				unsigned int			mCurNbEntries;		//!< Current number of entries
				unsigned int*			mEntries;			//!< List of entries
				float			mGrowthFactor;		//!< Resize: _new_ number of entries = old number * mGrowthFactor
	};

	

#endif // __ICECONTAINER_H__
