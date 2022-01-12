/* ========================= eCAL LICENSE =================================
 *
 * Copyright (C) 2016 - 2019 Continental Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * ========================= eCAL LICENSE =================================
*/

#include <ecal/ecal.h>
#include <ecal/msg/protobuf/client.h>

#include <iostream>
#include <chrono>
#include <thread>

#include "math.pb.h"

// client state callback
void OnClientState(const eCAL::SClientEventCallbackData* data_, int* refcnt_)
{
  assert(data_);
  assert(refcnt_);

  switch (data_->type)
  {
  case client_event_connected:
    std::cout << "---------------------------------" << std::endl;
    std::cout << "Client connected to service      " << std::endl;
    std::cout << "---------------------------------" << std::endl;
    (*refcnt_)++;
    break;
  case client_event_disconnected:
    std::cout << "---------------------------------" << std::endl;
    std::cout << "Client disconnected from service " << std::endl;
    std::cout << "---------------------------------" << std::endl;
    (*refcnt_)--;
    break;
  default:
    std::cout << "Unknown client event." << std::endl;
    break;
  }

  std::cout << "Server Hostname : " << data_->attr.hname    << std::endl;
  std::cout << "Server Name     : " << data_->attr.sname    << std::endl;
  std::cout << "Server Process  : " << data_->attr.pname    << std::endl;
  std::cout << "Server PID      : " << data_->attr.pid      << std::endl;
  std::cout << "Server TCP Port : " << data_->attr.tcp_port << std::endl;
  std::cout << "---------------------------------"          << std::endl << std::endl;
}

// callback for math service response
void OnMathResponse(const struct eCAL::SServiceInfo& service_info_, const std::string& response_)
{
  switch(service_info_.call_state)
  {
  // service successful executed
  case call_state_executed:
    {
      SFloat response;
      response.ParseFromString(response_);
      std::cout << "Received response MathService / " << service_info_.method_name << " : " << response.out() << " from host " << service_info_.host_name << std::endl;
    }
    break;
  // service execution failed
  case call_state_failed:
    std::cout << "Received error MathService / " << service_info_.method_name << " : " << service_info_.error_msg << " from host " << service_info_.host_name << std::endl;
    break;
  default:
    break;
  }
}

// main entry
int main(int argc, char **argv)
{
  // initialize eCAL API
  eCAL::Initialize(argc, argv, "math client");

  // create math service client
  eCAL::protobuf::CServiceClient<MathService> math_service;

  // add response callback
  math_service.AddResponseCallback(OnMathResponse);

  // add event callbacks
  int refcnt(0);
  math_service.AddEventCallback(client_event_connected,    std::bind(OnClientState, std::placeholders::_2, &refcnt));
  math_service.AddEventCallback(client_event_disconnected, std::bind(OnClientState, std::placeholders::_2, &refcnt));

  // loop variables
  int inp1(0);
  int inp2(0);

  while(eCAL::Ok())
  {
    if (refcnt > 0)
    {
      //////////////////////////////////////
      // Math service (callback variant)
      //////////////////////////////////////
      SFloatTuple math_request;
      math_request.set_inp1(inp1++);
      math_request.set_inp2(inp2++);

      if (math_service.Call("Add", math_request))
      {
        std::cout << std::endl << "MathService::Add method called with      : " << math_request.inp1() << " and " << math_request.inp1() << std::endl;
      }
      else
      {
        std::cout << "MathService::Add method call failed .." << std::endl << std::endl;
      }

      if (math_service.Call("Multiply", math_request))
      {
        std::cout << std::endl << "MathService::Multiply method called with : " << math_request.inp1() << " and " << math_request.inp1() << std::endl;
      }
      else
      {
        std::cout << "MathService::Multiply method call failed .." << std::endl << std::endl;
      }

      if (math_service.Call("Divide", math_request))
      {
        std::cout << std::endl << "MathService::Divide method called with   : " << math_request.inp1() << " and " << math_request.inp1() << std::endl;
      }
      else
      {
        std::cout << "MathService::Divide method call failed .." << std::endl << std::endl;
      }
    }
    else
    {
      std::cout << "Waiting for MathService .." << std::endl << std::endl;
    }

    // sleep a second
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }

  // finalize eCAL API
  eCAL::Finalize();
  
  return(0);
}
