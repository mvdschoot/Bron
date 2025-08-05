//
// Created by mathijs on 8/5/25.
//

#ifndef UPDATABLE_H
#define UPDATABLE_H

#include <functional>

namespace Steve {
	/**
	 * Super-class to handle updatable components. E.g. sometimes you need to regenerate buffers, and this class
	 * facilitates checking whether regeneration is needed.
	 *
	 * It consists of a lambda function, given by the subclass, that can check whether something is dirty,
	 * and a variable that can be set by the subclass to indicate whether something is dirty.
	 */
	class Updatable {
	public:
		Updatable(std::function<bool()> updateCheck) : updateCheck(updateCheck) {};

		[[nodiscard]] bool isUpdated() const { return areVariablesUpdated || updateCheck();}
		bool isUpdatedAndReset() {
			bool result = areVariablesUpdated || updateCheck();
			areVariablesUpdated = false;
			return result;
		}
	protected:
		std::function<bool()> updateCheck;
		bool areVariablesUpdated = false;
	};
}

#endif //UPDATABLE_H
