/**
* Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
* more contributor license agreements. See the NOTICE file distributed
* with this work for additional information regarding copyright ownership.
* Green Energy Corp licenses this file to you under the Apache License,
* Version 2.0 (the "License"); you may not use this file except in
* compliance with the License.  You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* This project was forked on 01/01/2013 by Automatak, LLC and modifications
* may have been made to this file. Automatak, LLC licenses these modifications
* to you under the terms of the License.
*/

#include "EventScanTask.h"

#include "MasterTasks.h"

#include "opendnp3/app/APDUParser.h"
#include "opendnp3/app/APDUBuilders.h"

#include "opendnp3/master/MeasurementHandler.h"

#include "opendnp3/LogLevels.h"

#include <openpal/logging/LogMacros.h>

using namespace openpal;

namespace opendnp3
{

	EventScanTask::EventScanTask(IMasterApplication& application, ISOEHandler& soeHandler, ClassField classes_, TimeDuration retryPeriod_, openpal::Logger logger) :
		PollTaskBase(application, soeHandler, MonotonicTimestamp::Max(), logger, nullptr, -1),
		classes(classes_.OnlyEventClasses()),
		retryPeriod(retryPeriod_)
	{

	}

	void EventScanTask::BuildRequest(APDURequest& request, uint8_t seq)
	{		
		build::ClassRequest(request, FunctionCode::READ, classes, seq);
	}

	bool EventScanTask::IsEnabled() const
	{
		return classes.HasEventClass();
	}

	void EventScanTask::OnResponseOK(openpal::MonotonicTimestamp)
	{
		expiration = MonotonicTimestamp::Max();
	}

	void EventScanTask::_OnResponseTimeout(openpal::MonotonicTimestamp now)
	{
		expiration = now.Add(retryPeriod);
	}

	void EventScanTask::OnResponseError(openpal::MonotonicTimestamp)
	{
		disabled = true;
		expiration = MonotonicTimestamp::Max();
	}

	void EventScanTask::_OnLowerLayerClose(openpal::MonotonicTimestamp)
	{
		expiration = MonotonicTimestamp::Max();
	}


} //end ns
