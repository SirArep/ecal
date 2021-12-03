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

/**
 * @file   client.h
 * @brief  eCAL Client interface based on protobuf service description
**/

#pragma once

#include <ecal/ecal_client.h>
#include <ecal/protobuf/ecal_proto_hlp.h>

// protobuf includes
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4100 4127 4146 4505 4800 4189 4592) // disable proto warnings
#endif
#include <google/protobuf/descriptor.pb.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

// stl includes
#include <string>

namespace eCAL
{
  namespace protobuf
  {
    /**
     * @brief Google Protobuf Client wrapper class.
    **/
    template <typename T>
    class CServiceClient : public eCAL::CServiceClient
    {
    public:
      /**
       * @brief Constructor.
       *
      **/
      CServiceClient()
      {
        Create(T::descriptor()->full_name());
      }

      /**
       * @brief CServiceClients are non-copyable
      **/
      CServiceClient(const CServiceClient&) = delete;

      /**
       * @brief CServiceClients are non-copyable
      **/
      CServiceClient& operator=(const CServiceClient&) = delete;

      /**
       * @brief Call a method of this service, responses will be returned by callback. 
       *
       * @param method_name_  Method name.
       * @param request_      Request message.
       * @param timeout_      Maximum time before operation returns (in milliseconds, -1 means infinite).
       *
       * @return  True if successful.
      **/
      bool Call(const std::string& method_name_, const google::protobuf::Message& request_, const int timeout_ = -1)
      {
        return Call(method_name_, request_.SerializeAsString(), timeout_);
      }

      /**
       * @brief Call a method of this service, response will be returned in service_response_ and response_ (first matching service only for now).
       *
       * THIS WILL BE CHANGED !
       * 
       * @param       method_name_       Method name.
       * @param       request_           Request message.
       * @param [out] service_response_  Service response struct for detailed informations.
       * @param [out] response_          Response string.
       *
      * @return  True if successful.
      **/
      bool Call(const std::string& method_name_, const google::protobuf::Message& request_, struct SServiceResponse& service_response_, google::protobuf::Message& response_)
      {
        // TODO: Find a solution to get a signature like
        // 
        //  bool Call(const std::string & method_name_, const google::protobuf::Message & request_, const int timeout_, struct SServiceResponsePbVecT& service_response_)
        // 
        //  SServiceResponsePbVecT would look like
        // 
        //    struct SServiceResponsePb
        //    {
        //      std::string                host_name;      //!< [in]  service host name
        //      std::string                service_name;   //!< [in]  name of the service
        //      std::string                method_name;    //!< [in]  name of the service method
        //      std::string                error_msg;      //!< [out] human readable error message
        //      int                        ret_state;      //!< [out] return state of the called service method
        //      eCallState                 call_state;     //!< [out] call state (see eCallState)
        //      google::protobuf::Message  response;       //!< [out] service response as google protobuf message
        //    };
        //    typedef std::vector<SServiceResponsePb> SServiceResponsePbVecT;
        //
        // for now we just call the first matching service and return it's answer, to get all responses please use the callback variant
        ServiceResponseVecT service_response_vec;
        const int timeout_(-1);
        if (Call(method_name_, request_.SerializeAsString(), timeout_, &service_response_vec))
        {
          if (service_response_vec.size() > 0)
          {
            service_response_ = service_response_vec[0];
            response_.ParseFromString(service_response_vec[0].response);
            return true;
          }
        }
        return false;
      }

      /**
       * @brief Call a method of this service asynchronously, responses will be returned by callback. 
       *
       * @param method_name_  Method name.
       * @param request_      Request message.
      **/
      void CallAsync(const std::string& method_name_, const google::protobuf::Message& request_)
      {
        CallAsync(method_name_, request_.SerializeAsString());
      }

      using eCAL::CServiceClient::Call;
      using eCAL::CServiceClient::CallAsync;
    };
  }
}
