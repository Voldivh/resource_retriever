// Copyright 2009, Willow Garage, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the  Willow Garage, Inc. nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef RESOURCE_RETRIEVER__PLUGINS__CURL_RETRIEVER_HPP_
#define RESOURCE_RETRIEVER__PLUGINS__CURL_RETRIEVER_HPP_

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "resource_retriever/plugins/retriever_plugin.hpp"
#include "resource_retriever/visibility_control.hpp"

using CURL = void;

namespace resource_retriever::plugins
{

class RESOURCE_RETRIEVER_PUBLIC CurlRetriever: public RetrieverPlugin
{
public:
  CurlRetriever();
  ~CurlRetriever() override;

  CurlRetriever(const CurlRetriever & ret) = delete;
  CurlRetriever & operator=(const CurlRetriever & other) = delete;

  CurlRetriever(CurlRetriever && other) noexcept;
  CurlRetriever & operator=(CurlRetriever && other) noexcept;

  bool can_handle(const std::string & url) override;
  std::string name() override;
  MemoryResourcePtr get(const std::string & url) override;

private:
  CURL * curl_handle_ {nullptr};
};

}  //  namespace resource_retriever::plugins

#endif  // RESOURCE_RETRIEVER__PLUGINS__CURL_RETRIEVER_HPP_
