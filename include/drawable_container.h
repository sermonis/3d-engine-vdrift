#ifndef _DRAWABLE_CONTAINER_H
#define _DRAWABLE_CONTAINER_H

#include "drawable.h"
#include "reseatable_reference.h"

// drawable container helper functions
namespace DRAWABLE_CONTAINER_HELPER
{
struct SetVisibility
{
	SetVisibility(bool newvis) : vis(newvis) {}
	bool vis;
	template <typename T>
	void operator()(T & container)
	{
		for (typename T::iterator i = container.begin(); i != container.end(); i++)
		{
			i->SetDrawEnable(vis);
		}
	}
};
struct SetAlpha
{
	SetAlpha(float newa) : a(newa) {}
	float a;
	template <typename T>
	void operator()(T & container)
	{
		for (typename T::iterator i = container.begin(); i != container.end(); i++)
		{
			i->SetAlpha(a);
		}
	}
};
struct ClearContainer
{
	template <typename T>
	void operator()(T & container)
	{
		container.clear();
	}
};
struct AccumulateSize
{
	AccumulateSize(unsigned int & newcount) : count(newcount) {}
	unsigned int & count;
	template <typename T>
	void operator()(const T & container)
	{
		count += container.size();
	}
};
template <typename F>
struct ApplyFunctor
{
	ApplyFunctor(F newf) : func(newf) {}
	F func;
	template <typename T>
	void operator()(T & container)
	{
		for (typename T::iterator i = container.begin(); i != container.end(); i++)
		{
			func(*i);
		}
	}
};
template <typename DRAWABLE_TYPE, typename CONTAINER_TYPE, bool use_transform>
void AddDrawableToContainer(DRAWABLE_TYPE & drawable, CONTAINER_TYPE & container, const MATRIX4 <float> & transform)
{
	if (drawable.GetDrawEnable())
	{
		if (use_transform)
			drawable.SetTransform(transform);
		container.push_back(&drawable);
	}
}
/// adds elements from the first container to the second
template <typename DRAWABLE_TYPE, typename CONTAINERT, typename U, bool use_transform>
void AddDrawablesToContainer(CONTAINERT & source, U & dest, const MATRIX4 <float> & transform)
{
	for (typename CONTAINERT::iterator i = source.begin(); i != source.end(); i++)
	{
		AddDrawableToContainer<DRAWABLE_TYPE,U,use_transform>(*i, dest, transform);
	}
}
}

/// pointer vector, drawable_container template parameter
template <typename T>
class PTRVECTOR : public std::vector<T*>
{};

template <template <typename U> class CONTAINER>
struct DRAWABLE_CONTAINER
{
	// all of the layers of the scene
	CONTAINER <DRAWABLE> twodim;
	CONTAINER <DRAWABLE> normal_noblend;
	CONTAINER <DRAWABLE> normal_noblend_nolighting;
	CONTAINER <DRAWABLE> car_noblend;
	CONTAINER <DRAWABLE> normal_blend;
	CONTAINER <DRAWABLE> skybox_blend;
	CONTAINER <DRAWABLE> skybox_noblend;
	CONTAINER <DRAWABLE> text;
	CONTAINER <DRAWABLE> particle;
	CONTAINER <DRAWABLE> nocamtrans_blend;
	CONTAINER <DRAWABLE> nocamtrans_noblend;
	CONTAINER <DRAWABLE> lights_emissive;
	CONTAINER <DRAWABLE> lights_omni;
	// don't forget to add new members to the ForEach, AppendTo, and GetByName functions
	
	template <typename T> 
	void ForEach(T func)
	{
		func(twodim);
		func(normal_noblend);
		func(normal_noblend_nolighting);
		func(car_noblend);
		func(normal_blend);
		func(skybox_blend);
		func(skybox_noblend);
		func(text);
		func(particle);
		func(nocamtrans_blend);
		func(nocamtrans_noblend);
		func(lights_emissive);
		func(lights_omni);
	}
	
	/// adds elements from the first drawable container to the second
	template <template <typename UU> class CONTAINERU, bool use_transform>
	void AppendTo(DRAWABLE_CONTAINER <CONTAINERU> & dest, const MATRIX4 <float> & transform)
	{
		#define ADDTOCONTAINER(x) DRAWABLE_CONTAINER_HELPER::AddDrawablesToContainer<DRAWABLE,CONTAINER<DRAWABLE>,CONTAINERU<DRAWABLE>,use_transform> (x, dest.x, transform);
		ADDTOCONTAINER(twodim);
		ADDTOCONTAINER(normal_noblend);
		ADDTOCONTAINER(normal_noblend_nolighting);
		ADDTOCONTAINER(car_noblend);
		ADDTOCONTAINER(normal_blend);
		ADDTOCONTAINER(skybox_blend);
		ADDTOCONTAINER(skybox_noblend);
		ADDTOCONTAINER(text);
		ADDTOCONTAINER(particle);
		ADDTOCONTAINER(nocamtrans_blend);
		ADDTOCONTAINER(nocamtrans_noblend);
		ADDTOCONTAINER(lights_emissive);
		ADDTOCONTAINER(lights_omni);
		#undef ADDTOCONTAINER
	}
	
	/// this is slow, don't do it often
	reseatable_reference <CONTAINER <DRAWABLE> > GetByName(const std::string & name)
	{
		reseatable_reference <CONTAINER <DRAWABLE> > ref;
		#define TEXTIFY(x) else if (name == #x) ref = x
		if (name == "twodim") ref = twodim;
		TEXTIFY(text);
		TEXTIFY(normal_noblend);
		TEXTIFY(normal_noblend_nolighting);
		TEXTIFY(car_noblend);
		TEXTIFY(normal_blend);
		TEXTIFY(skybox_blend);
		TEXTIFY(skybox_noblend);
		TEXTIFY(particle);
		TEXTIFY(nocamtrans_blend);
		TEXTIFY(nocamtrans_noblend);
		TEXTIFY(lights_emissive);
		TEXTIFY(lights_omni);
		#undef TEXTIFY
		return ref;
	}
	
	/// apply this functor to all drawables
	template <typename T>
	void ForEachDrawable(T func)
	{
		ForEach(DRAWABLE_CONTAINER_HELPER::ApplyFunctor<T>(func));
	}
	
	bool empty() const
	{
		return (size() == 0);
	}
	
	unsigned int size() const
	{
		DRAWABLE_CONTAINER <CONTAINER> * me = const_cast<DRAWABLE_CONTAINER <CONTAINER> *>(this); // messy, but avoids more typing. const correctness is enforced in AccumulateSize::operator()
		unsigned int count = 0;
		me->ForEach(DRAWABLE_CONTAINER_HELPER::AccumulateSize(count));
		return count;
	}
	
	void clear()
	{
		ForEach(DRAWABLE_CONTAINER_HELPER::ClearContainer());
	}
	
	void SetVisibility(bool newvis)
	{
		ForEach(DRAWABLE_CONTAINER_HELPER::SetVisibility(newvis));
	}
	
	void SetAlpha(float a)
	{
		ForEach(DRAWABLE_CONTAINER_HELPER::SetAlpha(a));
	}
};

#endif // _DRAWABLE_CONTAINER_H
