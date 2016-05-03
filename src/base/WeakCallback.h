#ifndef TINY_WEAKCALLBACK_H
#define TINY_WEAKCALLBACK_H

#include <functional>
#include <memory>

template<typename CLASS, typename... ARGS>
class WeakCallback {
public:

	WeakCallback(const std::weak_ptr<CLASS>& object,
				 const std::function<void(CLASS*, ARGS...)>& function)
		: object_(object), function_(function) 
	{  }

	void operator()(ARGS&&... args) const {
		std::shared_ptr<CLASS> ptr(object_.lock());
		if (ptr) {
			function_(ptr.get(), std::forward<ARGS>(args)...);
		}
	}

private:
	std::weak_ptr<CLASS> object_;
	std::function<void(CLASS*, ARGS...)> function_;

};
#endif

