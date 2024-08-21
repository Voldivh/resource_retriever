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

#include "resource_retriever/plugins/curl_retriever.hpp"

#include <curl/curl.h>

#include <array>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "resource_retriever/exception.hpp"

namespace
{

class CURLStaticInit
{
public:
  CURLStaticInit()
  {
    CURLcode ret = curl_global_init(CURL_GLOBAL_ALL);
    if (ret != 0) {
      fprintf(stderr, "Error initializing libcurl! retcode = %d", ret);
    } else {
      initialized_ = true;
    }
  }

  ~CURLStaticInit()
  {
    if (initialized_) {
      curl_global_cleanup();
    }
  }

private:
  bool initialized_ {false};
};
CURLStaticInit g_curl_init;
}  // namespace

namespace resource_retriever::plugins
{

struct MemoryBuffer
{
  std::vector<uint8_t> v;
};

size_t curlWriteFunc(void * buffer, size_t size, size_t nmemb, void * userp)
{
  auto * membuf = reinterpret_cast<MemoryBuffer *>(userp);
  size_t prev_size = membuf->v.size();
  membuf->v.resize(prev_size + size * nmemb);
  memcpy(&membuf->v[prev_size], buffer, size * nmemb);
  return size * nmemb;
}

CurlRetriever::CurlRetriever():
  curl_handle_(curl_easy_init())
{

}
CurlRetriever::~CurlRetriever()
{
  if (curl_handle_ != nullptr) {
    curl_easy_cleanup(curl_handle_);
  }
}

CurlRetriever::CurlRetriever(CurlRetriever && other) noexcept
: curl_handle_(std::exchange(other.curl_handle_, nullptr))
{
}

CurlRetriever & CurlRetriever::operator=(CurlRetriever && other) noexcept
{
  std::swap(curl_handle_, other.curl_handle_);
  return *this;
}



std::string CurlRetriever::name() {
  return "resource_retriever::plugins::CurlRetriever";
}

bool CurlRetriever::can_handle(const std::string & url)
{
  return (url.find("package://") == 0 ||
          url.find("file://") == 0 ||
          url.find("http://") == 0 ||
          url.find("https://") == 0);
}

MemoryResourcePtr CurlRetriever::get(const std::string & url)
{
  // Expand package:// url into file://
  auto mod_url = url;
  try {
    mod_url = expand_package_url(mod_url);
  } catch (const resource_retriever::Exception &e) {
    return nullptr;
  }

  // newer versions of curl do not accept spaces in URLs
  mod_url = escape_spaces(mod_url);

  curl_easy_setopt(curl_handle_, CURLOPT_URL, mod_url.c_str());
  curl_easy_setopt(curl_handle_, CURLOPT_WRITEFUNCTION, curlWriteFunc);

  char error_buffer[CURL_ERROR_SIZE];
  curl_easy_setopt(curl_handle_, CURLOPT_ERRORBUFFER, error_buffer);

  MemoryResourcePtr res {nullptr};
  MemoryBuffer buf;
  curl_easy_setopt(curl_handle_, CURLOPT_WRITEDATA, &buf);

  CURLcode ret = curl_easy_perform(curl_handle_);

  if (ret == 0 && !buf.v.empty()) {
    res = std::make_shared<MemoryResource>(url, mod_url, buf.v);
  }

  return res;
}

}  // namespace resource_retriever::plugins
